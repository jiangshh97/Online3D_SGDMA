#include "image_common.h"

void reconstruction::pointCloudToImage2(pcl::PointCloud<pcl::PointXYZ>::ConstPtr cloud, cv::Mat &image, PLCS &coordinate, pcl::ModelCoefficients::ConstPtr coefficients)
{
    if (cloud->size() == 0 || coefficients->values.size() != 4)
    {
        std::cerr << "The input argument is empty!" << std::endl;
        return;
    }
    // Spatial plane parameters
    double a = coefficients->values[0];
    double b = coefficients->values[1];
    double c = coefficients->values[2];
    double d = coefficients->values[3];
    // Establish a local plane coordinate system
    coordinate.w = Eigen::Vector3d(a, b, c).normalized(); // Plane normal vector
    Eigen::Vector3d x(1.0, 0.0, 0.0);                     // Project the X-axis onto the plane
    coordinate.u = (x - x.dot(coordinate.w) * coordinate.w).normalized();
    coordinate.v = ((coordinate.w).cross(coordinate.u)).normalized(); // Another orthogonal unit direction
    // Project the point cloud onto the plane
    double min_u = std::numeric_limits<double>::max(), max_u = std::numeric_limits<double>::lowest();
    double min_v = std::numeric_limits<double>::max(), max_v = std::numeric_limits<double>::lowest();
    std::vector<Eigen::Vector3d> projected_coords(cloud->size());
    for (size_t i = 0; i < cloud->size(); ++i)
    {
        Eigen::Vector3d point(cloud->points[i].x, cloud->points[i].y, cloud->points[i].z);
        double u_proj = point.dot(coordinate.u);                                                       // Projection in the u direction of the plane
        double v_proj = point.dot(coordinate.v);                                                       // Projection in the v direction of the plane
        double distance = (a * point.x() + b * point.y() + c * point.z() + d) / (coordinate.w).norm(); // Calculate the distance from the point to the plane (distinguish positive and negative)
        projected_coords[i] = Eigen::Vector3d(u_proj, v_proj, distance);
        // Update range
        min_u = std::min(min_u, u_proj);
        max_u = std::max(max_u, u_proj);
        min_v = std::min(min_v, v_proj);
        max_v = std::max(max_v, v_proj);
    }
    // Calculate image size (expand image boundaries for subsequent contour finding algorithms)
    min_u = std::round(min_u) - 200;
    min_v = std::round(min_v) - 200;
    int img_width = static_cast<int>(std::ceil(max_u - min_u)) + 200;
    int img_height = static_cast<int>(std::ceil(max_v - min_v)) + 200;
    // Calculate the position of the image origin in the point cloud space
    /************************************
     * ux * x + uy * y + uz * z = minu
     * vx * x + vy * y + vz * z = minv
     * a * x + b * y + c *z = -d
     ***********************************/
    Eigen::Matrix3d A;
    Eigen::Vector3d _b;
    A << (coordinate.u).x(), (coordinate.u).y(), (coordinate.u).z(), // First row
        (coordinate.v).x(), (coordinate.v).y(), (coordinate.v).z(),  // Second row
        (coordinate.w).x(), (coordinate.w).y(), (coordinate.w).z();  // Third row
    _b << min_u, min_v, -d;                                          // Vector b
    coordinate.origin = A.colPivHouseholderQr().solve(_b);           // Solve the linear equation A * x = _b
    // Initialize the image to store index values
    image = cv::Mat::zeros(img_height, img_width, CV_8UC1);
    // Fill the image
    for (size_t i = 0; i < cloud->size(); ++i)
    { // Fill the mapping points of the point cloud
        int x = static_cast<int>(std::round(projected_coords[i].x() - min_u));
        int y = static_cast<int>(std::round(projected_coords[i].y() - min_v));
        // Update the binary image
        image.at<uchar>(y, x) = 255;
    }
}

void reconstruction::pixelToPoint(const cv::Point2f &point2d, Eigen::Vector3d &point3d, const PLCS &coordinate)
{
    // Convert to the spatial position of the 3D point cloud
    point3d = coordinate.origin + point2d.x * coordinate.u + point2d.y * coordinate.v + 0.0 * coordinate.w;
}

void reconstruction::imageToPointcloud2(const cv::Mat &image, pcl::PointCloud<pcl::PointXYZ>::Ptr &cloud, const PLCS &coordinate)
{
    if (image.empty())
    {
        std::cout << "Input image is empty!" << std::endl;
        return;
    }
    // Clear the indices to be restored
    pcl::PointCloud<pcl::PointXYZ>::Ptr pointcloud(new pcl::PointCloud<pcl::PointXYZ>);
    // Traverse the image and restore the point cloud
    for (int y = 0; y < image.rows; ++y)
    {
        for (int x = 0; x < image.cols; ++x)
        {
            if (image.at<uchar>(y, x) == 255)
            { // White pixels in the binary image represent valid points
                Eigen::Vector3d point3D = coordinate.origin + x * coordinate.u + y * coordinate.v + 0.0 * coordinate.w;
                pointcloud->push_back(pcl::PointXYZ(point3D.x(), point3D.y(), point3D.z()));
            }
        }
    }
    // Output the result
    *cloud = *pointcloud; // Deep copy
}
void reconstruction::ImageFilter_median(const cv::Mat &src, cv::Mat &dst, int size)
{
    if (src.empty())
    {
        std::cout << "Input image is empty!" << std::endl;
        return;
    }
    cv::medianBlur(src, dst, size);
}

void reconstruction::ImageFilter_threshold(const cv::Mat &src, cv::Mat &dst)
{
    if (src.empty())
    {
        std::cout << "Input image is empty!" << std::endl;
        return;
    }
    cv::threshold(src, dst, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
}
void reconstruction::ImageFilter_holeFilling2(const cv::Mat &src, cv::Mat &dst, double minArea)
{
    if (src.empty())
    {
        std::cout << "Input image is empty!" << std::endl;
        return;
    }
    // Find all contours
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(src, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);
    // Create a mask
    cv::Mat mask = cv::Mat::zeros(src.size(), CV_8UC1);
    // Iterate through contours and fill small holes
    for (size_t i = 0; i < contours.size(); i++)
    {
        double area = cv::contourArea(contours[i]);
        if (area < minArea)
        {
            // Fill small holes
            cv::drawContours(mask, contours, static_cast<int>(i), 255, cv::FILLED);
        }
        else
        {
            // Retain large holes
            cv::drawContours(mask, contours, static_cast<int>(i), 0, cv::FILLED);
        }
    }
    // Apply the mask
    cv::bitwise_or(src, mask, dst);
}

void reconstruction::ImageMorphology_closing(const cv::Mat &src, cv::Mat &dst, cv::Size size)
{
    if (src.empty())
    {
        std::cout << "Input image is empty!" << std::endl;
        return;
    }
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, size);
    cv::morphologyEx(src, dst, cv::MORPH_CLOSE, kernel);
}

void reconstruction::ImageFeature_findExternalContours(const cv::Mat &src, cv::Mat &det)
{
    if (src.empty())
    {
        std::cout << "Input image is empty!" << std::endl;
        return;
    }
    // Contour retrieval
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(src, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
    // Output contours
    cv::Mat result = cv::Mat::zeros(src.size(), CV_8UC1);
    cv::drawContours(result, contours, -1, 255, 1);
    // Deep copy
    det = result.clone();
}

void reconstruction::ImageFeature_rotatedRect(const cv::Mat &src, cv::RotatedRect &rect)
{
    if (src.empty())
    {
        std::cout << "Input image is empty!" << std::endl;
        return;
    }
    std::vector<cv::Point> points;
    cv::findNonZero(src, points);
    rect = cv::minAreaRect(points);
}

void reconstruction::ImageCluster_kmenas(const std::vector<float> &data, std::vector<int> &labels, std::vector<float> &centers, int k, int max_iter)
{
    if (data.empty() || k <= 0) // Validate the input
        return;
    // Convert the data to OpenCV `cv::Mat` format
    cv::Mat samples(data.size(), 1, CV_32F);
    for (size_t i = 0; i < data.size(); ++i)
        samples.at<float>(i, 0) = data[i];
    // Termination criterion: maximum of 100 iterations
    cv::TermCriteria criteria(cv::TermCriteria::EPS + cv::TermCriteria::MAX_ITER, max_iter, 0.01);
    // Output variables
    cv::Mat centersMat;
    cv::Mat labelsMat;
    // Run K-Means++
    cv::kmeans(samples, k, labelsMat, criteria, 3, cv::KMEANS_PP_CENTERS, centersMat);
    // Copy the results
    labels.assign(labelsMat.begin<int>(), labelsMat.end<int>());
    centers.assign(centersMat.begin<float>(), centersMat.end<float>());
}

void reconstruction::ImageProcess_lineRansac(const std::vector<cv::Point> &points, cv::Vec4f &line, std::vector<cv::Point> &inlierPoints, float threshold, int iterations)
{
    if (points.size() < 2)
    {
        std::cerr << "Input points is empty!" << std::endl;
        return;
    }

    cv::RNG rng; // Create the random number generator
    float bestScore = -1.;
    int n = points.size(); // Get the number of points
    for (int iter = 0; iter < iterations; iter++)
    {
        // Randomly select two distinct points
        int i1 = rng.uniform(0, n - 1);
        int i2 = rng.uniform(0, n - 1);
        if (i1 == i2)
            continue;

        // Direction vector of the line
        const cv::Point &p1 = points[i1];
        const cv::Point &p2 = points[i2];
        cv::Point2f dp = p2 - p1;
        dp *= 1.0 / cv::norm(dp);

        // Compute inliers
        float score = 0;
        std::vector<cv::Point> inliers;
        for (int i = 0; i < n; i++)
        {
            cv::Point2f v = points[i] - p1;
            float d = v.y * dp.x - v.x * dp.y; // Cross product magnitude of vectors a and b divided by |b|, where ||b|| = 1 / norm(dp)
            // Check whether the point-to-line distance is below the threshold
            if (std::fabs(d) < threshold)
            {
                score += 1;
                inliers.push_back(points[i]); // Store the inlier
            }
        }

        // Update the best result if the current fitting score is higher
        if (score > bestScore)
        {
            line = cv::Vec4f(dp.x, dp.y, p1.x, p1.y);
            bestScore = score;
            inlierPoints = inliers; // Update the inlier set
        }
    }
}

void reconstruction::ImageProcess_lineskeleton(const cv::Mat &src, cv::Mat &dst, std::vector<std::pair<cv::Vec4f, float>> &lines, int linesMaxNum, int minInPoints)
{
    if (src.empty())
    {
        std::cout << "Input image is empty!" << std::endl;
        return;
    }

    // Distance transform
    cv::Mat imageDistance; // Distance transform image
    cv::distanceTransform(src, imageDistance, cv::DIST_L2, 3, CV_32F);

    // Local maxima (candidate skeleton points)
    cv::Mat dilated, localMaxima;
    cv::dilate(imageDistance, dilated, cv::Mat());                  // Use dilation to locate neighborhood maxima
    localMaxima = (imageDistance == dilated) & (imageDistance > 0); // Compare the original image with the dilated result
    localMaxima.convertTo(localMaxima, CV_8UC1, 255);               // Convert to a binary image
    std::vector<cv::Point> skeletonPoints;
    cv::findNonZero(localMaxima, skeletonPoints); // Candidate skeleton points

    // Multi-line fitting
    std::vector<std::pair<cv::Vec4f, std::vector<cv::Point>>> linesR; // Fitting results
    for (int i = 0; i < linesMaxNum; ++i)
    {
        cv::Vec4f line;                      // Line parameters
        std::vector<cv::Point> inlierPoints; // Inlier points of the line
        reconstruction::ImageProcess_lineRansac(skeletonPoints, line, inlierPoints, 1.5f, 200);
        if (inlierPoints.size() > minInPoints)
        {
            linesR.push_back(std::make_pair(line, inlierPoints));
            // Remove inliers
            auto newEnd = std::remove_if(skeletonPoints.begin(), skeletonPoints.end(), [&inlierPoints](const cv::Point &element)
                                         {
                                             return std::find(inlierPoints.begin(), inlierPoints.end(), element) != inlierPoints.end(); // Find the element in the inlierPoints container
                                         });
            skeletonPoints.erase(newEnd, skeletonPoints.end()); // Remove the matched elements from the skeletonPoints container
        }
        // Check the remaining points
        if (skeletonPoints.size() < minInPoints)
            break;
    }

    // Rasterize skeleton pixels along the line: (x, y) = (x0, y0) + (vx, vy) * t
    std::vector<std::pair<cv::Vec4f, float>> skeletonLines; // Skeleton line parameters (endpoints, width)
    cv::Mat skeleton = cv::Mat::zeros(src.size(), CV_8UC1); // Skeleton image
    for (auto &line : linesR)
    {
        const cv::Vec4f lineParameters = line.first;
        const std::vector<cv::Point> inlierPoints = line.second;
        // Compute the width
        std::vector<float> skeletonDistance;
        for (auto &point : inlierPoints)
            skeletonDistance.push_back(imageDistance.at<float>(point.y, point.x));
        float sum = std::accumulate(skeletonDistance.begin(), skeletonDistance.end(), 0.0f);
        float distance = skeletonDistance.empty() ? 0.0 : sum / skeletonDistance.size();
        // Line parameters
        float vx = lineParameters[0];
        float vy = lineParameters[1];
        int x0 = std::floor(lineParameters[2]);
        int y0 = std::floor(lineParameters[3]);
        // Traverse in different directions according to the slope
        std::vector<cv::Point> linePoints; // Skeleton pixels on the line
        cv::Vec4f endpoint;                // Skeleton endpoints
        if (vx == 0.0f && vy == 0.0f)
        {
            std::cerr << "The linear parameters are Error!" << std::endl;
            return;
        }
        else if (vx == 0.0f && vy != 0.0f)
        {
            for (int y = y0; y >= 0; y--)
            { // Traverse in the negative direction
                if (src.at<uchar>(y, x0) != 0)
                {
                    skeleton.at<uchar>(y, x0) = 255;
                    linePoints.push_back(cv::Point(x0, y));
                    endpoint[0] = x0;
                    endpoint[1] = y;
                }
                else
                    break;
            }
            for (int y = y0; y < skeleton.rows; y++)
            { // Traverse in the positive direction
                if (src.at<uchar>(y, x0) != 0)
                {
                    skeleton.at<uchar>(y, x0) = 255;
                    linePoints.push_back(cv::Point(x0, y));
                    endpoint[2] = x0;
                    endpoint[3] = y;
                }
                else
                    break;
            }
        }
        else if (vy == 0.0f && vx != 0.0f)
        {
            for (int x = x0; x >= 0; x--)
            { // Traverse in the negative direction
                if (src.at<uchar>(y0, x) != 0)
                {
                    skeleton.at<uchar>(y0, x) = 255;
                    linePoints.push_back(cv::Point(x, y0));
                    endpoint[0] = x;
                    endpoint[1] = y0;
                }
                else
                    break;
            }
            for (int x = x0; x < skeleton.cols; x++)
            { // Traverse in the positive direction
                if (src.at<uchar>(y0, x) != 0)
                {
                    skeleton.at<uchar>(y0, x) = 255;
                    linePoints.push_back(cv::Point(x, y0));
                    endpoint[2] = x;
                    endpoint[3] = y0;
                }
                else
                    break;
            }
        }
        else if (std::fabs(vx) >= std::fabs(vy))
        {
            for (int x = x0, y; x >= 0; x--)
            { // Traverse in the negative direction
                // Compute the corresponding y-coordinate
                float t = (x - x0) / vx;
                y = std::round(y0 + vy * t);
                if (y >= 0 && y < skeleton.rows)
                { // Check whether y is within the image bounds
                    if (src.at<uchar>(y, x) != 0)
                    {
                        skeleton.at<uchar>(y, x) = 255;
                        linePoints.push_back(cv::Point(x, y));
                        endpoint[0] = x;
                        endpoint[1] = y;
                    }
                    else
                        break;
                }
            }
            for (int x = x0, y; x < skeleton.cols; x++)
            { // Traverse in the positive direction
                // Compute the corresponding y-coordinate
                float t = (x - x0) / vx;
                y = std::round(y0 + vy * t);
                if (y >= 0 && y < skeleton.rows)
                { // Check whether y is within the image bounds
                    if (src.at<uchar>(y, x) != 0)
                    {
                        skeleton.at<uchar>(y, x) = 255;
                        linePoints.push_back(cv::Point(x, y));
                        endpoint[2] = x;
                        endpoint[3] = y;
                    }
                    else
                        break;
                }
            }
        }
        else
        {
            for (int x, y = y0; y >= 0; y--)
            { // Traverse in the negative direction
                // Compute the corresponding x-coordinate
                float t = (y - y0) / vy;
                x = std::round(x0 + vx * t);
                if (x >= 0 && x < skeleton.cols)
                { // Check whether x is within the image bounds
                    if (src.at<uchar>(y, x) != 0)
                    {
                        skeleton.at<uchar>(y, x) = 255;
                        linePoints.push_back(cv::Point(x, y));
                        endpoint[0] = x;
                        endpoint[1] = y;
                    }
                    else
                        break;
                }
            }
            for (int x, y = y0; y < skeleton.rows; y++)
            { // Traverse in the positive direction
                // Compute the corresponding x-coordinate
                float t = (y - y0) / vy;
                x = std::round(x0 + vx * t);
                if (x >= 0 && x < skeleton.cols)
                { // Check whether y is within the image bounds
                    if (src.at<uchar>(y, x) != 0)
                    {
                        skeleton.at<uchar>(y, x) = 255;
                        linePoints.push_back(cv::Point(x, y));
                        endpoint[2] = x;
                        endpoint[3] = y;
                    }
                    else
                        break;
                }
            }
        }
        // Output data
        skeletonLines.push_back(std::make_pair(endpoint, distance));
    }
    // Output the skeleton image
    dst = skeleton.clone();
    // Output the line parameters
    lines = skeletonLines;
}

std::vector<cv::Scalar> reconstruction::ImageVisual_generateRandomColors(int count)
{
    std::vector<cv::Scalar> colors;

    if (count == 0)
        return colors;

    for (int i = 0; i < count; ++i)
    {
        int h = (i * 180 / count) % 180; // Evenly spaced hue
        int s = 255;                     // Maximum saturation
        int v = 255;                     // Maximum value
        cv::Mat hsv(1, 1, CV_8UC3, cv::Scalar(h, s, v));
        cv::Mat bgr;
        cv::cvtColor(hsv, bgr, cv::COLOR_HSV2BGR);
        colors.push_back(bgr.at<cv::Vec3b>(0, 0));
    }
    return colors;
}



