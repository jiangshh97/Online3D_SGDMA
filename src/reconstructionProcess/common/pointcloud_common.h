#pragma once

// Header files
#include <iostream>
#include <algorithm>
#include <omp.h>

// PCL
#include <pcl/io/io.h>
#include <pcl/io/pcd_io.h>
#include <pcl/io/ply_io.h>
#include <pcl/point_types.h>
#include <pcl/point_cloud.h>

#include <pcl/common/common.h>
#include <pcl/common/centroid.h>
#include <pcl/common/time.h>
#include <pcl/common/transforms.h>
#include <pcl/common/concatenate.h>
#include <pcl/common/pca.h>

#include <pcl/kdtree/kdtree.h>
#include <pcl/kdtree/impl/kdtree_flann.hpp>
#include <pcl/octree/octree_search.h>
#include <pcl/octree/octree_pointcloud.h>
#include <pcl/search/kdtree.h>

#include <pcl/filters/filter.h>
#include <pcl/filters/passthrough.h>
#include <pcl/filters/voxel_grid.h>
#include <pcl/octree/octree_pointcloud_voxelcentroid.h>
#include <pcl/filters/statistical_outlier_removal.h>
#include <pcl/filters/radius_outlier_removal.h>
#include <pcl/filters/conditional_removal.h>
#include <pcl/filters/shadowpoints.h>
#include <pcl/filters/extract_indices.h>
#include <pcl/filters/project_inliers.h>
#include <pcl/filters/plane_clipper3D.h>
#include <pcl/filters/crop_box.h>
#include <pcl/filters/random_sample.h>
#include <pcl/filters/uniform_sampling.h>
#include <pcl/filters/normal_space.h>
#include <pcl/filters/normal_refinement.h>

#include <pcl/features/normal_3d.h>
#include <pcl/features/normal_3d_omp.h>
#include <pcl/features/integral_image_normal.h>
#include <pcl/features/moment_of_inertia_estimation.h>

#include <pcl/sample_consensus/method_types.h>
#include <pcl/sample_consensus/model_types.h>
#include <pcl/sample_consensus/ransac.h>
#include <pcl/sample_consensus/sac_model_plane.h>
#include <pcl/sample_consensus/sac_model_sphere.h>

#include <pcl/segmentation/sac_segmentation.h>
#include <pcl/segmentation/extract_clusters.h>
#include <pcl/segmentation/conditional_euclidean_clustering.h>
#include <pcl/segmentation/region_growing.h>
#include <pcl/segmentation/min_cut_segmentation.h>
#include <pcl/segmentation/supervoxel_clustering.h>
#include <pcl/segmentation/lccp_segmentation.h>

#include <pcl/surface/concave_hull.h>
#include <pcl/surface/mls.h>

#include <pcl/registration/icp.h>
#include <pcl/registration/gicp.h>
#include <pcl/registration/ndt.h>

#include <Eigen/Dense>

namespace reconstruction
{
  /**
   * @brief Extract points from a point cloud by indices
   * @param cloud         Input point cloud
   * @param cloud_result  Output point cloud
   * @param indices       Point indices
   * @param negative      Extraction mode (false: keep indexed points; true: remove indexed points)
   */
  void pointcloudFilter_extract(pcl::PointCloud<pcl::PointXYZ>::ConstPtr cloud, pcl::PointCloud<pcl::PointXYZ>::Ptr &cloud_result, pcl::PointIndices::ConstPtr indices, bool negative = false);

  /**
   * @brief Apply pass-through filtering to a point cloud
   * @param cloud         Input point cloud
   * @param cloud_result  Output point cloud
   * @param field_name    Filtering field ("x", "y", or "z")
   * @param limit_min     Minimum filter limit
   * @param limit_max     Maximum filter limit
   * @param negative      Filtering mode (false: keep points within the range; true: remove points within the range)
   */
  void pointcloudFilter_passThrough(pcl::PointCloud<pcl::PointXYZ>::ConstPtr cloud, pcl::PointCloud<pcl::PointXYZ>::Ptr &cloud_result, std::string field_name, float limit_min, float limit_max, bool negative = false);

  /**
   * @brief Remove radius-based outliers from a point cloud
   * @param cloud         Input point cloud
   * @param cloud_result  Output point cloud
   * @param radius        Neighborhood radius
   * @param min_pts       Minimum number of neighboring points
   */
  void pointcloudFilter_radiusOutlierRemoval(pcl::PointCloud<pcl::PointXYZ>::ConstPtr cloud, pcl::PointCloud<pcl::PointXYZ>::Ptr &cloud_result, double radius, int min_pts);

  /**
   * @brief Perform uniform sampling on a point cloud
   * @param cloud         Input point cloud
   * @param cloud_result  Output point cloud
   * @param radius        Sampling radius
   */
  void pointcloudFilter_uniformSample(pcl::PointCloud<pcl::PointXYZ>::ConstPtr cloud, pcl::PointCloud<pcl::PointXYZ>::Ptr &cloud_result, float radius);

  /**
   * @brief Crop a point cloud using a box filter
   * @param cloud         Input point cloud
   * @param cloud_result  Output point cloud
   * @param min_point     Minimum corner of the crop box
   * @param max_point     Maximum corner of the crop box
   * @param transform     Pose transformation matrix
   * @param negative      Cropping mode (false: keep points inside the box; true: remove points inside the box)
   */
  void pointcloudFilter_cropBox(pcl::PointCloud<pcl::PointXYZ>::ConstPtr cloud, pcl::PointCloud<pcl::PointXYZ>::Ptr &cloud_result, const Eigen::Vector4f &min_point, const Eigen::Vector4f &max_point, const Eigen::Affine3f &transform, bool negative = false);

  /**
   * @brief Fit a line to a point cloud
   * @param cloud         Input point cloud
   * @param inliers       Output inlier indices
   * @param coefficients  Output model coefficients
   * @param threshold     Distance threshold for inlier classification
   * @param maxiterations Maximum number of iterations
   */
  void pointcloudSegmentation_line(pcl::PointCloud<pcl::PointXYZ>::ConstPtr cloud, pcl::PointIndices::Ptr &inliers, pcl::ModelCoefficients::Ptr &coefficients, double threshold, int maxiterations);

  /**
   * @brief Segment a plane from a point cloud
   * @param cloud         Input point cloud
   * @param inliers       Output inlier indices
   * @param coefficients  Output model coefficients
   * @param threshold     Distance threshold for inlier classification
   * @param maxiterations Maximum number of iterations
   * @param z             Whether to enforce Z-axis direction correction
   */
  void pointcloudSegmentation_plane(pcl::PointCloud<pcl::PointXYZ>::ConstPtr cloud, pcl::PointIndices::Ptr &inliers, pcl::ModelCoefficients::Ptr &coefficients, double threshold, int maxiterations, bool z = false);

  /**
   * @brief Project a point onto a plane
   * @param point         Input point
   * @param pointProject  Output projected point
   * @param coefficients  Plane model coefficients
   */
  void pointcloudProcess_projectPointOntoPlane(const Eigen::Vector3d &point, Eigen::Vector3d &pointProject, pcl::ModelCoefficients::ConstPtr coefficients);

  /**
   * @brief Compute the distance between two points
   * @param point1  Input point 1
   * @param point2  Input point 2
   * @param distance Output distance
   */
  double pointcloudProcess_calDistance(const Eigen::Vector3d &point1, const Eigen::Vector3d &point2);

  /**
   * @brief Compute the angle between two direction vectors
   * @param d1  Input direction vector 1
   * @param d2  Input direction vector 2
   * @return Angle in radians
   */
  double pointcloudProcess_calAngle(const Eigen::Vector3d &d1, const Eigen::Vector3d &d2);

  /**
   * @brief Compute the intersection point of two lines
   * @param p1            Input point P1
   * @param p2            Input point P2
   * @param p3            Input point P3
   * @param p4            Input point P4
   * @param intersection  Output intersection point
   */
  bool pointcloudProcess_calIntersection(const Eigen::Vector3d &p1, const Eigen::Vector3d &p2, const Eigen::Vector3d &p3, const Eigen::Vector3d &p4, Eigen::Vector3d &intersection);

  /**
   * @brief Check whether a point lies on a line segment
   * @param p1  Input point P1
   * @param p2  Input point P2
   * @param p   Query point
   * @return True if the point lies on the segment; otherwise false
   */
  bool pointcloudProcess_isPointOnLine(const Eigen::Vector3d &p1, const Eigen::Vector3d &p2, const Eigen::Vector3d &p);

  /**
   * @brief Compute the foot of the perpendicular from a point to a line
   * @param p1   Input line point P1
   * @param p2   Input line point P2
   * @param p    Input spatial point
   * @param res  Output foot point
   */
  void pointcloudProcess_calPerpendicularFoot(const Eigen::Vector3d &p1, const Eigen::Vector3d &p2, const Eigen::Vector3d &p, Eigen::Vector3d &res);

  /**
   * @brief Compute the OBB representation
   * @param center       Input center point
   * @param orientation  Input principal axes
   * @param dimension    Input dimensions
   * @param transform    Output transformation matrix
   * @param minPoint     Output minimum point in the OBB coordinate system
   * @param maxPoint     Output maximum point in the OBB coordinate system
   */
  void pointcloudProcess_calOBB(const Eigen::Vector3d &center, const std::array<Eigen::Vector3d, 3> &orientation, const std::array<double, 3> &dimension,
                                Eigen::Affine3f &transform, Eigen::Vector4f &minPoint, Eigen::Vector4f &maxPoint);

  /**
   * @brief Compute the distance from a point to a line
   * @param point         Spatial point
   * @param coefficients  Line model coefficients
   * @return Signed distance
   */
  double pointcloudProcess_calP2Line(const Eigen::Vector3d &point, pcl::ModelCoefficients::ConstPtr coefficients);

  /**
   * @brief Compute the signed distance from a point to a plane
   * @param point     Spatial point
   * @param planeDir  Plane normal vector
   * @param planeP    A point on the plane
   * @return Signed distance
   */
  double pointcloudProcess_calP2Plane(const Eigen::Vector3d &point, const Eigen::Vector3d &planeDir, const Eigen::Vector3d &planeP);

  /**
   * @brief Compute the projected endpoints and midpoint of a point cloud on a line
   * @param cloud         Input point cloud
   * @param coefficients  Line model coefficients
   * @param A             Endpoint 1
   * @param B             Endpoint 2
   * @param M             Midpoint
   */
  void pointcloudProcess_calProjEndPoints(pcl::PointCloud<pcl::PointXYZ>::ConstPtr cloud, pcl::ModelCoefficients::ConstPtr coefficients,
                                          Eigen::Vector3d &A, Eigen::Vector3d &B, Eigen::Vector3d &M);

  /**
   * @brief Compute the directional similarity between local PCA directions and a reference direction
   * @param cloud         Input point cloud
   * @param similarities  Output directional similarities
   * @param dir           Reference direction vector
   * @param radius        Neighborhood radius
   */
  void pointcloudProcess_PCADirSimilar(pcl::PointCloud<pcl::PointXYZ>::ConstPtr cloud, std::vector<double> &similarities,
                                       const Eigen::Vector3d &dir, double radius);

}
