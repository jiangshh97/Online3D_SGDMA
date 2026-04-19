#pragma once

#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>

#include <pcl/point_types.h>
#include <pcl/point_cloud.h>
#include <pcl/ModelCoefficients.h>
#include <pcl/filters/extract_indices.h>

#include <Eigen/Dense>
#include "omp.h"

namespace reconstruction
{
    struct PLCS // Projected Local Coordinate System
    {
        Eigen::Vector3d origin;                   // Origin of the local coordinate system
        Eigen::Vector3d u;                        // Horizontal direction of the local coordinate system
        Eigen::Vector3d v;                        // Vertical direction of the local coordinate system
        Eigen::Vector3d w;                        // Depth direction of the local coordinate system
        PLCS() : origin(Eigen::Vector3d::Zero()), // Initialize to zero vector using Eigen's Zero() function
                 u(Eigen::Vector3d::UnitX()),     // Initialize to unit vector along the x-axis using UnitX()
                 v(Eigen::Vector3d::UnitY()),     // Initialize to unit vector along the y-axis using UnitY()
                 w(Eigen::Vector3d::UnitZ())
        {
        }
    };

    /**
     * @brief pointCloudToImage2    Convert point cloud to image
     * @param cloud                 Input point cloud
     * @param image                 Output binary image
     * @param coordinate            Output projected local coordinate system
     * @param coefficients          Projection plane equation
     */
    void pointCloudToImage2(pcl::PointCloud<pcl::PointXYZ>::ConstPtr cloud, cv::Mat &image, PLCS &coordinate, pcl::ModelCoefficients::ConstPtr coefficients);

    /**
     * @brief pixelToPoint              Convert pixel to 3D point
     * @param point2                    Input pixel position
     * @param point3                    Output 3D point position
     * @param coordinate                Projected local coordinate system
     */
    void pixelToPoint(const cv::Point2f &point2d, Eigen::Vector3d &point3d, const PLCS &coordinate);

    /**
     * @brief imageToPointcloud2            Restore point cloud from image
     * @param image                         Input image
     * @param cloud                         Output point cloud
     * @param coordinate                    Projected local coordinate system
     */
    void imageToPointcloud2(const cv::Mat &image, pcl::PointCloud<pcl::PointXYZ>::Ptr &cloud, const PLCS &coordinate);

    /**
     * @brief ImageFilter_median      Median filtering
     * @param src                     Input image
     * @param dst                     Output image
     * @param size                    Size of the filter kernel, e.g., 3, 5, 7
     */
    void ImageFilter_median(const cv::Mat &src, cv::Mat &dst, int size);

    /**
     * @brief ImageFilter_threshold      Binarization (adaptive thresholding)
     * @param src                        Input image
     * @param dst                        Output image
     */
    void ImageFilter_threshold(const cv::Mat &src, cv::Mat &dst);

    /**
     * @brief ImageFilter_holeFilling2      Hole filling (preserve internal contours)
     * @param src                           Input binary image
     * @param dst                           Output image
     * @param minArea                       Minimum area of internal contours
     */
    void ImageFilter_holeFilling2(const cv::Mat &src, cv::Mat &dst, double minArea);

    /**
     * @brief ImageMorphology_closing    Morphological closing operation
     * @param src                        Input image
     * @param dst                        Output image
     * @param size                       Rectangular structuring element
     */
    void ImageMorphology_closing(const cv::Mat &src, cv::Mat &dst, cv::Size size);

    /**
     * @brief ImageFeature_findExternalContours     Detect external contours
     * @param src                                   Input binary image
     * @param det                                   Output contour image
     */
    void ImageFeature_findExternalContours(const cv::Mat &src, cv::Mat &det);

    /**
     * @brief ImageFeature_rotatedRect     Minimum rotated bounding box
     * @param src                          Input image
     * @param dst                          Output bounding box parameters
     */
    void ImageFeature_rotatedRect(const cv::Mat &src, cv::RotatedRect &rect);

    /**
     * @brief ImageCluster_kmenas         k-means clustering
     * @param data                        Input 1D data cluster
     * @param labels                      Output classification labels
     * @param centers                     Output cluster centers
     * @param k                           Number of clusters
     * @param max_iter                    Maximum number of iterations
     */
    void ImageCluster_kmenas(const std::vector<float> &data, std::vector<int> &labels, std::vector<float> &centers, int k, int max_iter);

    /**
     * @brief ImageProcess_lineRansac     Line fitting using RANSAC
     * @param points                      Input point set
     * @param line                        Output line parameters (vx, vy, x0, y0), where (vx, vy) is the direction vector, and (x0, y0) is a point on the line
     * @param inlierPoints                Output inlier points on the line
     * @param threshold                   Threshold value
     * @param iterations                  Maximum number of iterations
     */
    void ImageProcess_lineRansac(const std::vector<cv::Point> &points, cv::Vec4f &line, std::vector<cv::Point> &inlierPoints, float threshold, int iterations);

    /**
     * @brief ImageProcess_lineskeleton      Extract line skeleton
     * @param src                            Input binary image
     * @param dst                            Output skeleton image
     * @param lines                          Output skeleton line set (endpoints, half-width)
     * @param linesMaxNum                    Maximum number of skeleton lines
     * @param minInPoints                    Minimum number of inlier points for skeleton lines
     */
    void ImageProcess_lineskeleton(const cv::Mat &src, cv::Mat &dst, std::vector<std::pair<cv::Vec4f, float>> &lines, int linesMaxNum = 20, int minInPoints = 15);

    /**
     * @brief ImageVisual_generateRandomColors  Generate random colors
     * @param count                             Number of colors to generate
     */
    std::vector<cv::Scalar> ImageVisual_generateRandomColors(int count);

}
