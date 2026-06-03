#include "pointcloud_common.h"

void reconstruction::pointcloudFilter_extract(pcl::PointCloud<pcl::PointXYZ>::ConstPtr cloud, pcl::PointCloud<pcl::PointXYZ>::Ptr &cloud_result, pcl::PointIndices::ConstPtr indices, bool negative)
{
    if (cloud->size() <= 0)
    {
        std::cout << "The point cloud is empty!" << std::endl;
        return;
    }

    pcl::ExtractIndices<pcl::PointXYZ> extract;
    extract.setInputCloud(cloud);
    extract.setIndices(indices);
    extract.setNegative(negative);
    extract.filter(*cloud_result);
}

void reconstruction::pointcloudFilter_passThrough(pcl::PointCloud<pcl::PointXYZ>::ConstPtr cloud, pcl::PointCloud<pcl::PointXYZ>::Ptr &cloud_result, std::string field_name, float limit_min, float limit_max, bool negative)
{
    if (cloud->size() <= 0)
    {
        std::cout << "The point cloud is empty!" << std::endl;
        return;
    }

    pcl::PassThrough<pcl::PointXYZ> pass;
    pass.setInputCloud(cloud);
    pass.setFilterFieldName(field_name);
    pass.setFilterLimits(limit_min, limit_max);
    pass.setFilterLimitsNegative(negative);
    pass.filter(*cloud_result);
}

void reconstruction::pointcloudFilter_radiusOutlierRemoval(pcl::PointCloud<pcl::PointXYZ>::ConstPtr cloud, pcl::PointCloud<pcl::PointXYZ>::Ptr &cloud_result, double radius, int min_pts)
{
    if (cloud->size() <= 0)
    {
        std::cout << "The point cloud is empty!" << std::endl;
        return;
    }

    pcl::RadiusOutlierRemoval<pcl::PointXYZ> outrem;
    outrem.setInputCloud(cloud);
    outrem.setRadiusSearch(radius);
    outrem.setMinNeighborsInRadius(min_pts);
    outrem.filter(*cloud_result);
}

void reconstruction::pointcloudFilter_uniformSample(pcl::PointCloud<pcl::PointXYZ>::ConstPtr cloud, pcl::PointCloud<pcl::PointXYZ>::Ptr &cloud_result, float radius)
{
    if (cloud->size() <= 0)
    {
        std::cout << "The point cloud is empty!" << std::endl;
        return;
    }

    pcl::UniformSampling<pcl::PointXYZ> us;
    us.setInputCloud(cloud);
    us.setRadiusSearch(radius);
    us.filter(*cloud_result);
}

void reconstruction::pointcloudFilter_cropBox(pcl::PointCloud<pcl::PointXYZ>::ConstPtr cloud, pcl::PointCloud<pcl::PointXYZ>::Ptr &cloud_result, const Eigen::Vector4f &min_point, const Eigen::Vector4f &max_point, const Eigen::Affine3f &transform, bool negative)
{
    if (cloud->size() <= 0)
    {
        std::cout << "The point cloud is empty!" << std::endl;
        return;
    }

    pcl::CropBox<pcl::PointXYZ> cropbox; 
    cropbox.setInputCloud(cloud);
    cropbox.setTransform(transform.inverse());
    cropbox.setMin(min_point);                
    cropbox.setMax(max_point);               
    cropbox.setNegative(negative);         
    cropbox.filter(*cloud_result);
}

void reconstruction::pointcloudSegmentation_line(pcl::PointCloud<pcl::PointXYZ>::ConstPtr cloud, pcl::PointIndices::Ptr &inliers, pcl::ModelCoefficients::Ptr &coefficients, double threshold, int maxiterations)
{
    if (cloud->size() <= 2)
    {
        std::cout << "The point cloud is empty!" << std::endl;
        return;
    }

    pcl::SACSegmentation<pcl::PointXYZ> seg;
    seg.setInputCloud(cloud);
    seg.setOptimizeCoefficients(true);
    seg.setModelType(pcl::SACMODEL_LINE);
    seg.setMethodType(pcl::SAC_RANSAC);
    seg.setDistanceThreshold(threshold);
    seg.setMaxIterations(maxiterations);
    seg.segment(*inliers, *coefficients);
}

void reconstruction::pointcloudSegmentation_plane(pcl::PointCloud<pcl::PointXYZ>::ConstPtr cloud, pcl::PointIndices::Ptr &inliers, pcl::ModelCoefficients::Ptr &coefficients, double threshold, int maxiterations, bool z)
{
    if (cloud->size() <= 0)
    {
        std::cout << "The point cloud is empty!" << std::endl;
        return;
    }

    pcl::SACSegmentation<pcl::PointXYZ> seg;
    seg.setInputCloud(cloud);
    seg.setOptimizeCoefficients(true);
    seg.setModelType(pcl::SACMODEL_PLANE);
    seg.setMethodType(pcl::SAC_RANSAC);
    seg.setDistanceThreshold(threshold);
    seg.setMaxIterations(maxiterations);
    seg.segment(*inliers, *coefficients);
    if (z)
    {
        if (coefficients->values[2] < 0)
        {
            coefficients->values[0] *= -1;
            coefficients->values[1] *= -1;
            coefficients->values[2] *= -1;
            coefficients->values[3] *= -1;
        }
    }
}

void reconstruction::pointcloudProcess_projectPointOntoPlane(const Eigen::Vector3d &point, Eigen::Vector3d &pointProject, pcl::ModelCoefficients::ConstPtr coefficients)
{
    if (coefficients->values.size() < 4)
    {
        std::cout << "The input argument is error!" << std::endl;
        return;
    }

    Eigen::Vector3d n(coefficients->values[0], coefficients->values[1], coefficients->values[2]);
    double d = coefficients->values[3];
    double norm2 = n.dot(n);
    double projCoeff = (n.dot(point) + d) / norm2;
    pointProject = point - projCoeff * n;
}

double reconstruction::pointcloudProcess_calDistance(const Eigen::Vector3d &point1, const Eigen::Vector3d &point2)
{
    Eigen::Vector3d d(point1 - point2);
    return d.norm();
}

double reconstruction::pointcloudProcess_calAngle(const Eigen::Vector3d &d1, const Eigen::Vector3d &d2)
{
    double dot_product = d1.dot(d2);
    double norm_d1 = d1.norm();
    double norm_d2 = d2.norm();
    if (norm_d1 < 1e-6 || norm_d2 < 1e-6)
    {
        std::cerr << "Zero-length vector detected" << std::endl;
        return 0.0;
    }
    double cos_theta = dot_product / (norm_d1 * norm_d2);
    cos_theta = cos_theta < -1.0 ? -1.0 : cos_theta;
    cos_theta = cos_theta > +1.0 ? +1.0 : cos_theta;
    return std::acos(cos_theta); 
}

bool reconstruction::pointcloudProcess_calIntersection(const Eigen::Vector3d &p1, const Eigen::Vector3d &p2, const Eigen::Vector3d &p3, const Eigen::Vector3d &p4, Eigen::Vector3d &intersection)
{
    // Compute direction vectors
    Eigen::Vector3d dir1 = p2 - p1; // Direction vector of line 1
    Eigen::Vector3d dir2 = p4 - p3; // Direction vector of line 2
    // Compute the intersection of the two lines
    Eigen::Vector3d cross_product = dir1.cross(dir2); // Cross product of the two direction vectors
    // Check whether the cross product is zero; if so, the two lines are parallel or collinear
    if (cross_product.isZero())
    {
        intersection = Eigen::Vector3d(0.0, 0.0, 0.0);
        std::cout << "The lines are parallel or collinear!" << std::endl;
        return false; // No intersection: the lines are parallel or collinear
    }
    // Construct the constant vector
    Eigen::Vector3d b = p3 - p1;
    // Compute t and s, the parameters of the intersection point
    double t = b.cross(dir2).dot(cross_product) / cross_product.squaredNorm();
    intersection = p1 + t * dir1; // The intersection point is P1 + t * dir1
    return true;
}

bool reconstruction::pointcloudProcess_isPointOnLine(const Eigen::Vector3d &p1, const Eigen::Vector3d &p2, const Eigen::Vector3d &p)
{
    // Compute vectors AB and AP
    Eigen::Vector3d ab = p2 - p1;
    Eigen::Vector3d ap = p - p1;
    // 1. Check whether the cross product is zero, i.e., whether P lies on line AB
    if (ab.cross(ap).norm() > 1e-6)
    {
        return false; // The cross product is nonzero, so point P is not on line AB
    }
    // 2. Check whether point P lies on segment AB, i.e., whether it falls within the extent of AB
    // Use the dot product to determine whether P projects onto the segment interval
    double t = ap.dot(ab) / ab.dot(ab);
    if (t >= 0.0 && t <= 1.0)
        return true;
    else
        return false;
}

void reconstruction::pointcloudProcess_calPerpendicularFoot(const Eigen::Vector3d &p1, const Eigen::Vector3d &p2, const Eigen::Vector3d &p, Eigen::Vector3d &res)
{
    Eigen::Vector3d ab = p2 - p1;
    Eigen::Vector3d ap = p - p1;

    // Projection coefficient
    double t = ap.dot(ab) / ab.squaredNorm();

    // Foot of the perpendicular: F = A + t * AB
    res = p1 + t * ab;
}

void reconstruction::pointcloudProcess_calOBB(const Eigen::Vector3d &center, const std::array<Eigen::Vector3d, 3> &orientation, const std::array<double, 3> &dimension,
                                              Eigen::Affine3f &transform, Eigen::Vector4f &minPoint, Eigen::Vector4f &maxPoint)
{
    // OBB transformation matrix
    transform = Eigen::Affine3f::Identity();
    transform.linear().col(0) = orientation[0].cast<float>(); // Set the rotation component
    transform.linear().col(1) = orientation[1].cast<float>(); // Set the rotation component
    transform.linear().col(2) = orientation[2].cast<float>(); // Set the rotation component
    transform.translation() = center.cast<float>();           // Set the translation component
    // Extremal points in the OBB coordinate system
    minPoint = Eigen::Vector4f(-0.5 * dimension[0], -0.5 * dimension[1], -0.5 * dimension[2], 1.0f);
    maxPoint = Eigen::Vector4f(0.5 * dimension[0], 0.5 * dimension[1], 0.5 * dimension[2], 1.0f);
}

double reconstruction::pointcloudProcess_calP2Line(const Eigen::Vector3d &point, pcl::ModelCoefficients::ConstPtr coefficients)
{
    if (coefficients->values.size() < 6)
        return 0.0;

    Eigen::Vector3d lineP(coefficients->values[0], coefficients->values[1], coefficients->values[2]);
    Eigen::Vector3d lineDir(coefficients->values[3], coefficients->values[4], coefficients->values[5]);
    return ((point - lineP).cross(lineDir)).norm() / lineDir.norm();
}

double reconstruction::pointcloudProcess_calP2Plane(const Eigen::Vector3d &point, const Eigen::Vector3d &planeDir, const Eigen::Vector3d &planeP)
{
    return planeDir.dot(point - planeP) / planeDir.norm();
}

void reconstruction::pointcloudProcess_calProjEndPoints(pcl::PointCloud<pcl::PointXYZ>::ConstPtr cloud, pcl::ModelCoefficients::ConstPtr coefficients,
                                                        Eigen::Vector3d &A, Eigen::Vector3d &B, Eigen::Vector3d &M)
{
    if (cloud->size() == 0 || coefficients->values.size() == 0)
    {
        std::cout << "The input argument is empty!" << std::endl;
        return;
    }
    // Line parameters
    Eigen::Vector3d lineP(coefficients->values[0], coefficients->values[1], coefficients->values[2]);
    Eigen::Vector3d lineVec(coefficients->values[3], coefficients->values[4], coefficients->values[5]);
    lineVec = lineVec.normalized();
    // Compute the projected point
    double min_t = std::numeric_limits<double>::max(), max_t = std::numeric_limits<double>::lowest();
    Eigen::Vector3d min_pt, max_pt;
    for (const auto &point : cloud->points)
    {
        Eigen::Vector3d p(point.x, point.y, point.z);
        double t = (p - lineP).dot(lineVec);     // Projection length
        Eigen::Vector3d q = lineP + t * lineVec; // Projected point
        if (t <= min_t)
        {
            min_t = t;
            min_pt = q;
        }
        if (t >= max_t)
        {
            max_t = t;
            max_pt = q;
        }
    }
    // Output the computed results
    A = min_pt;
    B = max_pt;
    M = 0.5 * (A + B);
}

void reconstruction::pointcloudProcess_PCADirSimilar(pcl::PointCloud<pcl::PointXYZ>::ConstPtr cloud, std::vector<double> &similarities,
                                                     const Eigen::Vector3d &dir, double radius)
{
    // Build the KdTree
    pcl::search::KdTree<pcl::PointXYZ>::Ptr kdtree(new pcl::search::KdTree<pcl::PointXYZ>);
    // Set the search structure to a Kd-tree
    kdtree->setInputCloud(cloud);
    // Iterate through the point cloud
    for (size_t i = 0; i < cloud->size(); ++i)
    {
        // Search for neighborhood points
        std::vector<int> point_indices;
        std::vector<float> point_squared_distances;
        if (kdtree->radiusSearch(cloud->points[i], radius, point_indices, point_squared_distances) < 3)
        {
            similarities.push_back(0.0);
            continue;
        }
        // Extract neighborhood points
        pcl::PointCloud<pcl::PointXYZ>::Ptr neighborhood(new pcl::PointCloud<pcl::PointXYZ>);
        for (int idx : point_indices)
        {
            neighborhood->push_back(cloud->points[idx]);
        }
        // Perform PCA analysis
        pcl::PCA<pcl::PointXYZ> pca;
        pca.setInputCloud(neighborhood);
        Eigen::Vector3f eigen_vectorsMax = pca.getEigenVectors().col(0); // Principal direction
        Eigen::Vector3f eigen_value = pca.getEigenValues();              // Eigenvalues
        // Output the results
        double alpha = eigen_value[0] != 0 ? (eigen_value[0] - eigen_value[1]) / eigen_value[0] : 0.0;
        Eigen::Vector3d eigen_vectorsMax_(eigen_vectorsMax.x(), eigen_vectorsMax.y(), eigen_vectorsMax.z()); // Principal direction
        double similarity = alpha * std::fabs(dir.dot(eigen_vectorsMax_) / (dir.norm() * eigen_vectorsMax_.norm()));
        similarities.push_back(similarity);
    }
}

