#include "steelanglerecon.h"

using namespace reconstruction;

SteelAngleRecon::SteelAngleRecon(QObject *parent) : AbstractReconAlgorithm(parent)
{
    // Model path
    QDir appDir = QCoreApplication::applicationDirPath();
    appDir.cdUp();
    enginePath_ = appDir.path().toStdString();

    mainPlaneEqu_.reset(new pcl::ModelCoefficients);
}

bool SteelAngleRecon::setParams(std::shared_ptr<const ReconAlgParams> params)
{
    auto p = std::dynamic_pointer_cast<const SteelAngleReconParams>(params);
    if (!p || !p->cfg)
    {
        QString msg = u8"Parameter configuration: input is null.";
        emit sendReconstructMessage(msg, 0); // Emit the runtime message for this class
        return false;
    }
    // Copy the pointer
    params_ = p->cfg;
    // Initialize the YOLO module
    if (yolo_seg == nullptr)
        yolo_seg = std::make_unique<YoLoSegInference>(enginePath_ + params_->enginePath);
    return true;
}

bool SteelAngleRecon::preprocessing()
{
    // Validate the input point cloud
    if (cloud_->size() > 0)
    {
        if (debug_)
        {
            QtConcurrent::run([=]()
                              {
            std::string filepath = filePath_ + "1_cloud_input.pcd";
            pcl::io::savePCDFile(filepath, *cloud_); });
        }
    }
    else
    {
        QString msg = u8"Point cloud preprocessing: source point cloud is empty.";
        emit sendReconstructMessage(msg, 0); // Emit the runtime message for this class
        return false;
    }

    // Remove background points using pass-through filtering
    pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_fitter(new pcl::PointCloud<pcl::PointXYZ>);
    float minz_ = params_->ptParams.valueZ[0];
    float maxz_ = params_->ptParams.valueZ[1];
    pointcloudFilter_passThrough(cloud_, cloud_fitter, "z", minz_, maxz_, false);
    float minx_ = params_->ptParams.valueX[0];
    float maxx_ = params_->ptParams.valueX[1];
    pointcloudFilter_passThrough(cloud_fitter, cloud_fitter, "x", minx_, maxx_, false);

    // Downsample
    float radiusDs = params_->downSample;
//    pointcloudFilter_uniformSample(cloud_fitter, cloud_fitter, radiusDs);

    // Remove outliers
    float radius = params_->outlierParams.radius;
    float min_pts = params_->outlierParams.minPts;
    pointcloudFilter_radiusOutlierRemoval(cloud_fitter, cloud_fitter_, radius, min_pts);

    // Verify the output
    if (cloud_fitter_->size() > 0)
    {
        if (debug_)
        {
            QtConcurrent::run([=]()
                              {
            std::string filepath = filePath_ + "2_cloud_fitter.pcd";
            pcl::io::savePCDFile(filepath, *cloud_fitter_); });
        }
        return true;
    }
    else
    {
        QString msg = u8"Point cloud preprocessing: output point cloud is empty.";
        emit sendReconstructMessage(msg, 0); // Emit the runtime message for this class
        return false;
    }
}

bool SteelAngleRecon::decomposition()
{
    // Validate the input point cloud
    if (cloud_fitter_->size() == 0)
    {
        QString msg = u8"Point cloud decomposition: input point cloud is empty.";
        emit sendReconstructMessage(msg, 0); // Emit the runtime message for this class
        return false;
    }

    // Main-plane fitting
    double threshold = params_->planeFitParams.distanceThreshold;
    int maxiterations = params_->planeFitParams.maxIterations;
    pcl::PointIndices::Ptr mainPlaneInliers(new pcl::PointIndices); // Plane inliers
    pcl::ModelCoefficients::Ptr mainPlaneEqu(new pcl::ModelCoefficients);
    pointcloudSegmentation_plane(cloud_fitter_, mainPlaneInliers, mainPlaneEqu, threshold, maxiterations, true);
    if (mainPlaneInliers->indices.size() >= 3)
    {
        *mainPlaneEqu_ = *mainPlaneEqu;
        if (debug_)
        { // Debug output
            QtConcurrent::run([=]()
                              {
            pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_mainplane(new pcl::PointCloud<pcl::PointXYZ>);
            pointcloudFilter_extract(cloud_fitter_, cloud_mainplane, mainPlaneInliers, false);
            std::string filepath = filePath_ + "3_cloud_mianplane.pcd";
            pcl::io::savePCDFile(filepath, *cloud_mainplane); });
        }
    }
    else
    {
        QString msg = u8"Point cloud decomposition: main-plane fitting failed.";
        emit sendReconstructMessage(msg, 0); // Emit the runtime message for this class
        return false;
    }

    // Project the point cloud onto a 2D image
    cv::Mat binary;  // Projected binary image and projection distance matrix
    PLCS coordinate; // Projected local coordinate system
    pointCloudToImage2(cloud_fitter_, binary, coordinate, mainPlaneEqu_);
    if (!binary.empty())
    {
        if (debug_)
        {
            QtConcurrent::run([=]()
                              {
            std::string filepath = filePath_ + "4_image_project.jpg";
            cv::imwrite(filepath, binary); });
        }
    }
    else
    {
        QString msg = u8"Point cloud decomposition: failed to generate the projected image.";
        emit sendReconstructMessage(msg, 0); // Emit the runtime message for this class
        return false;
    }

    // 2D image preprocessing
    cv::Mat image_fitter;
    ImageMorphology_closing(binary, image_fitter, cv::Size(5, 5)); // Closing operation (fills small holes and smooths edges)
    double minArea = params_->holeArea;
    ImageFilter_holeFilling2(image_fitter, image_fitter, minArea); // Hole filling
    if (!image_fitter.empty())
    {
        if (debug_)
        {
            QtConcurrent::run([=]()
                              {
            std::string filepath = filePath_ + "5_image_filter.jpg";
            cv::imwrite(filepath, image_fitter); });
        }
    }
    else
    {
        QString msg = u8"Point cloud decomposition: projected-image preprocessing failed.";
        emit sendReconstructMessage(msg, 0); // Emit the runtime message for this class
        return false;
    }

    // YOLO instance
    cv::Mat image_seg;
    std::vector<yolo::Object> hook_objs;
    yolo_seg->runInference(image_fitter, image_seg, hook_objs, params_->yoloParams);
    if (!image_seg.empty())
    {
        if (debug_)
        {
            QtConcurrent::run([=]()
                              {
            std::string filepath = filePath_ + "6_image_seg.jpg";
            cv::imwrite(filepath, image_seg); });
        }
    }
    if (hook_objs.empty())
    {
        QString msg = u8"Instance segmentation: hook detection failed.";
        emit sendReconstructMessage(msg, 0); // Emit the runtime message for this class
        return false;
    }

    // Remove the hook portion
    cv::Mat image_wp = image_fitter.clone();
    for (const auto &hook : hook_objs)
        image_wp(hook.rect).setTo(cv::Scalar(0, 0, 0), hook.boxMask);
    ImageFilter_median(image_wp, image_wp, 5); // Median filtering
    ImageFilter_threshold(image_wp, image_wp); // Binarization
    if (!image_wp.empty())
    {
        if (debug_)
        {
            QtConcurrent::run([=]()
                              {
            std::string filepath = filePath_ + "7_image_wp.jpg";
            cv::imwrite(filepath, image_wp); });
        }
    }
    else
    {
        QString msg = u8"Hook removal failed.";
        emit sendReconstructMessage(msg, 0); // Emit the runtime message for this class
        return false;
    }

    // Decompose the image based on the skeleton
    cv::Mat image_skeleton;
    std::vector<std::pair<cv::Vec4f, float>> skeleton;
    ImageProcess_lineskeleton(image_wp, image_skeleton, skeleton);
    if (!image_skeleton.empty())
    {
        cv::Mat image_seg_skeleton = image_seg.clone();
        std::vector<cv::Scalar> colors = ImageVisual_generateRandomColors(skeleton.size());
        for (int i = 0; i < skeleton.size(); ++i)
        {
            const auto &endpoint = skeleton[i].first;
            cv::Point2f pt1 = cv::Point2f(endpoint[0], endpoint[1]);
            cv::Point2f pt2 = cv::Point2f(endpoint[2], endpoint[3]);
            cv::line(image_seg_skeleton, pt1, pt2, colors[i], 2, cv::LINE_AA);
        }
        emit sendSkeletonResult(image_seg_skeleton.clone());

        if (debug_)
        {
            QtConcurrent::run([=]()
                              {
            std::string filepath = filePath_ + "8_image_skeleton.jpg";
            cv::imwrite(filepath, image_seg_skeleton); });
        }
    }
    else if (skeleton.empty())
    {
        QString msg = u8"Skeleton extraction failed.";
        emit sendReconstructMessage(msg, 0); // Emit the runtime message for this class
        return false;
    }

    // Output the OBB of the angle-steel segment
    std::map<int, std::vector<OBB3D>> obbsGroups;
    for (const auto &ske : skeleton)
    {
        // Endpoints
        const auto &endpoint = ske.first;
        cv::Point2f endpoint1 = cv::Point2f(endpoint[0], endpoint[1]);
        cv::Point2f endpoint2 = cv::Point2f(endpoint[2], endpoint[3]);
        // Center point
        cv::Point2f center = 0.5 * (endpoint1 + endpoint2);
        // Length
        double length = cv::norm(endpoint1 - endpoint2);
        // Leg width
        double width = 2.0 * ske.second;
        // Convert to 3D point-cloud space
        Eigen::Vector3d e3(mainPlaneEqu_->values[0], mainPlaneEqu_->values[1], mainPlaneEqu_->values[2]);
        Eigen::Vector3d center3d, e1, e1_0, e1_1, e2;
        pixelToPoint(center, center3d, coordinate);
        pixelToPoint(endpoint1, e1_0, coordinate);
        pixelToPoint(endpoint2, e1_1, coordinate);
        e1 = (e1_1 - e1_0).normalized();
        e3 = e3.normalized();
        e2 = (e1.cross(e3)).normalized();
        // Output the results
        OBB3D box;
        box.center = center3d;
        box.orientation = {e1, e2, e3};
        box.dimension = {length, width, 2.5 * width};
        obbsGroups[0].emplace_back(box);
    }

    // Output detection results
    if (!obbsGroups.empty())
    {
        obbsGroups_ = std::move(obbsGroups);
        QString msg = u8"Point cloud decomposition in progress...";
        emit sendReconstructMessage(msg, -1);
        return true;
    }
    else
    {
        QString msg = u8"Point cloud decomposition: output is empty.";
        emit sendReconstructMessage(msg, 0);
        return false;
    }
}

bool SteelAngleRecon::featureExtraction()
{
    // Validate the input
    if (obbsGroups_.empty())
    {
        QString msg = u8"Feature extraction: input parameters are empty.";
        emit sendReconstructMessage(msg, 0); // Emit the runtime message for this class
        return false;
    }

    // Extract parameters of the angle-steel segment
    const auto &steelangleObbs = obbsGroups_[0];
    std::vector<steelAngleParams3D> steelangle_params;
    this->featureExtraction_Steelangle(steelangleObbs, steelangle_params);

    // Regularize feature parameters
    if (!steelangle_params.empty())
    {
        this->featureRegularization(steelangle_params);
    }
    else
        return false;

    // Convert to model parameters
    std::vector<ModelParameters> modelParams;
    this->featureToModelParams(steelangle_params, modelParams);

    // Verify the output
    if (!modelParams.empty())
    {
        modelParams_ = std::move(modelParams);
        QString msg = u8"Feature extraction in progress...";
        emit sendReconstructMessage(msg, -1);
        return true;
    }
    else
    {
        QString msg = u8"Feature extraction: output is empty.";
        emit sendReconstructMessage(msg, 0);
        return false;
    }
}

bool SteelAngleRecon::parametricModeling()
{
    // Validate the input
    if (modelParams_.empty())
    {
        QString msg = u8"Parametric modeling: input parameters are empty.";
        emit sendReconstructMessage(msg, 0); // Emit the runtime message for this class
        return false;
    }

    TopoDS_Shape model;
    bool ok = modeling_parametricModel(modelParams_, model);

    if (ok == true)
    {
        BRepBuilderAPI_Copy copier(model); // Deep copy
        shape_ = copier.Shape();
        if (debug_)
        { // Debug output
            QtConcurrent::run([=]()
                              {
            std::string filepath1 = filePath_ + "21_model.stl";
            modelExplorer_stlWriter(shape_, filepath1);
            std::string filepath2 = filePath_ + "22_model.step";
            modelExplorer_stepWriter(shape_, filepath2); });
        }
        QString msg = u8"Parametric modeling in progress...";
        emit sendReconstructMessage(msg, -1);
        return true;
    }
    else
    {
        QString msg = u8"Parametric modeling: model generation failed.";
        emit sendReconstructMessage(msg, 0);
        return false;
    }
}

void SteelAngleRecon::featureExtraction_Steelangle(const std::vector<OBB3D> &steelangle_obb, std::vector<steelAngleParams3D> &steelangle_params)
{
    // Validate the input
    if (steelangle_obb.empty())
    {
        QString msg = u8"Feature extraction: angle-steel input is empty.";
        emit sendReconstructMessage(msg, 0); // Emit the runtime message for this class
        return;
    }

    // PCA local neighborhood radius
    double radiusNeighbor = params_->radiusNeighbor;
    // Preallocate memory for parallel computation
    std::vector<steelAngleParams3D> steelangle_params_(steelangle_obb.size());
    // Parallel computation
#pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < steelangle_obb.size(); ++i)
    {
        // Compute the OBB bounding box
        auto box = steelangle_obb[i];
        Eigen::Affine3f transform;
        Eigen::Vector4f min_point;
        Eigen::Vector4f max_point;
        pointcloudProcess_calOBB(box.center, box.orientation, box.dimension, transform, min_point, max_point);
        // Angle-steel point-cloud decomposition
        pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_anglesteel(new pcl::PointCloud<pcl::PointXYZ>);
        pointcloudFilter_cropBox(cloud_fitter_, cloud_anglesteel, min_point, max_point, transform, false);
        // Validate decomposition results
        if (cloud_anglesteel->size() > 0)
        {
            if (debug_)
            { // Debug output
                QtConcurrent::run([=]()
                                  {
                std::string filepath = filePath_ + "11_cloud_anglesteel_" + std::to_string(i) + ".pcd";
                pcl::io::savePCDFile(filepath, *cloud_anglesteel); });
            }
        }
        else
            continue;

        // Segment leg-tip and leg-plate point clouds
        box.dimension[2] = 0.25 * box.dimension[2]; // Half height
        pointcloudProcess_calOBB(box.center, box.orientation, box.dimension, transform, min_point, max_point);
        // Leg-tip point cloud
        pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_tip(new pcl::PointCloud<pcl::PointXYZ>);
        pointcloudFilter_cropBox(cloud_anglesteel, cloud_tip, min_point, max_point, transform, true);
        // Leg plate point cloud
        pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_plate(new pcl::PointCloud<pcl::PointXYZ>);
        pointcloudFilter_cropBox(cloud_anglesteel, cloud_plate, min_point, max_point, transform, false);
        // Validate decomposition results
        if (cloud_tip->size() > 0 && cloud_plate->size() > 0)
        {
            if (debug_)
            { // Debug output
                QtConcurrent::run([=]()
                                  {
                std::string filepath1 = filePath_ + "12_cloud_tip_" + std::to_string(i) + ".pcd";
                pcl::io::savePCDFile(filepath1, *cloud_tip);
                std::string filepath2 = filePath_ + "14_cloud_plate_" + std::to_string(i) + ".pcd";
                pcl::io::savePCDFile(filepath2, *cloud_plate); });
            }
        }
        else
            continue;

        /***********************************************************************************
         * Leg-tip point cloud: used to determine side-leg direction vector 2 and to distinguish ridge lines from outer edges (on the same side of plane e3-O-e1)
         * ********************************************************************************/
        // Extract leg-tip point clouds parallel to the major axis
        Eigen::Vector3d dir = box.orientation[0]; // OBB major-axis direction vector
        std::vector<double> similaritiesTip;      // Similarity to the OBB major-axis direction
        pointcloudProcess_PCADirSimilar(cloud_tip, similaritiesTip, dir, radiusNeighbor);
        // K-means++ clustering
        int kTip = 2;
        std::vector<float> similaritiesTip_f(similaritiesTip.size());
        for (int j = 0; j < similaritiesTip.size(); j++)
            similaritiesTip_f[j] = static_cast<float>(similaritiesTip[j]);
        std::vector<int> labelsTip;
        std::vector<float> centersTip(kTip, 0.0);
        ImageCluster_kmenas(similaritiesTip_f, labelsTip, centersTip, kTip, 1000);
        // Select parallel point clouds
        int labelTip = std::abs(1 - centersTip[0]) < std::abs(1 - centersTip[1]) ? 0 : 1;
        pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_tip_fitter(new pcl::PointCloud<pcl::PointXYZ>);
        for (size_t i = 0; i < labelsTip.size(); ++i)
        {
            if (labelsTip[i] == labelTip)
                cloud_tip_fitter->push_back(cloud_tip->points[i]);
        }
        pcl::PointIndices::Ptr tipInliers_(new pcl::PointIndices);
        pcl::ModelCoefficients::Ptr tipEqu_(new pcl::ModelCoefficients);
        pointcloudSegmentation_line(cloud_tip_fitter, tipInliers_, tipEqu_, radiusNeighbor, 200);
        pointcloudFilter_extract(cloud_tip_fitter, cloud_tip_fitter, tipInliers_, false);

        // Validate the results
        if (cloud_tip_fitter->size() > 0)
        {
            if (debug_)
            {
                QtConcurrent::run([=]()
                                  {
                std::string filepath = filePath_ + "13_cloud_tip_fitter_" + std::to_string(i) + ".pcd";
                pcl::io::savePCDFile(filepath, *cloud_tip_fitter); });
            }
        }
        else
            continue;

        // Compute feature parameters: determine the ridge-line position relative to plane e3-O-e1 and infer side-leg direction vector 2
        double distance1Sum = 0.0, distance2Sum = 0.0;
        for (const auto &p : cloud_tip_fitter->points)
        {
            Eigen::Vector3d point(p.x, p.y, p.z);
            distance1Sum += pointcloudProcess_calP2Plane(point, box.orientation[1], box.center);
            distance2Sum += pointcloudProcess_calP2Plane(point, box.orientation[2], box.center);
        }

        /***********************************************************************************
         * Leg-plate point cloud: used for ridge-line localization and leg-width estimation
         * ********************************************************************************/
        // Extract contour point clouds
        cv::Mat image_;    // Projected binary image
        PLCS coordinate_;  // Projected local coordinate system
        cv::Mat contours_; // Image contours
        pointCloudToImage2(cloud_plate, image_, coordinate_, mainPlaneEqu_);
        ImageMorphology_closing(image_, image_, cv::Size(5, 5)); // Closing operation (fills small holes and smooths edges)
        ImageFeature_findExternalContours(image_, contours_);    // Extract outer contours
        pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_plate_contour(new pcl::PointCloud<pcl::PointXYZ>);
        imageToPointcloud2(contours_, cloud_plate_contour, coordinate_);
        // Validate the results
        if (cloud_plate_contour->size() > 0)
        {
            if (debug_)
            { // Debug output
                QtConcurrent::run([=]()
                                  {
                std::string filepath = filePath_ + "15_cloud_plate_contour_" + std::to_string(i) + ".pcd";
                pcl::io::savePCDFile(filepath, *cloud_plate_contour); });
            }
        }
        else
            continue;

        // Extract contours parallel to the major axis
        std::vector<double> similaritiesPlate; // Similarity to the OBB major-axis direction
        pointcloudProcess_PCADirSimilar(cloud_plate_contour, similaritiesPlate, dir, radiusNeighbor);
        // K-means++ clustering
        int kPlate = 2;
        std::vector<float> similaritiesPlate_f(similaritiesPlate.size());
        for (int k = 0; k < similaritiesPlate.size(); k++)
            similaritiesPlate_f[k] = static_cast<float>(similaritiesPlate[k]);
        std::vector<int> labelsPlate;
        std::vector<float> centersPlate(kPlate, 0.0);
        ImageCluster_kmenas(similaritiesPlate_f, labelsPlate, centersPlate, kPlate, 1000);
        // Select parallel point clouds
        int labelPlate = std::abs(1 - centersPlate[0]) < std::abs(1 - centersPlate[1]) ? 0 : 1;
        pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_plate_paraedge(new pcl::PointCloud<pcl::PointXYZ>);
        for (size_t i = 0; i < labelsPlate.size(); ++i)
        {
            if (labelsPlate[i] == labelPlate)
                cloud_plate_paraedge->push_back(cloud_plate_contour->points[i]);
        }
        // Validate the results
        if (cloud_plate_paraedge->size() > 0)
        {
            if (debug_)
            { // Debug output
                QtConcurrent::run([=]()
                                  {
                std::string filepath = filePath_ + "16_cloud_plate_paraedge_" + std::to_string(i) + ".pcd";
                pcl::io::savePCDFile(filepath, *cloud_plate_paraedge); });
            }
        }
        else
            continue;

        // Segment contour lines parallel to the major axis
        double threshold = params_->lineFitParams.distanceThreshold;
        int maxiterations = params_->lineFitParams.maxIterations;
        pcl::PointIndices::Ptr line1Inliers(new pcl::PointIndices);       // Inliers of line 1
        pcl::PointIndices::Ptr line2Inliers(new pcl::PointIndices);       // Inliers of line 2
        pcl::ModelCoefficients::Ptr line1Equ(new pcl::ModelCoefficients); // Line 1 parameters (point-direction form)
        pcl::ModelCoefficients::Ptr line2Equ(new pcl::ModelCoefficients); // Line 2 parameters
        pointcloudSegmentation_line(cloud_plate_paraedge, line1Inliers, line1Equ, threshold, maxiterations);
        pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_plate_paraedge_(new pcl::PointCloud<pcl::PointXYZ>);
        pointcloudFilter_extract(cloud_plate_paraedge, cloud_plate_paraedge_, line1Inliers, true);
        if (cloud_plate_paraedge_->size() > 0)
            pointcloudSegmentation_line(cloud_plate_paraedge_, line2Inliers, line2Equ, threshold, maxiterations);
        else
            continue;

        // Separate ridge lines and outer edges
        pcl::ModelCoefficients::Ptr interEqu(new pcl::ModelCoefficients);
        pcl::ModelCoefficients::Ptr outerEqu(new pcl::ModelCoefficients);
        pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_plate_inter(new pcl::PointCloud<pcl::PointXYZ>); // Ridge-line points
        pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_plate_outer(new pcl::PointCloud<pcl::PointXYZ>); // Rolled-edge line points
        Eigen::Vector3d line1P, line2P;
        if (line1Equ->values.size() >= 6 && line2Equ->values.size() >= 6)
        {
            line1P = Eigen::Vector3d(line1Equ->values[0], line1Equ->values[1], line1Equ->values[2]);
            line2P = Eigen::Vector3d(line2Equ->values[0], line2Equ->values[1], line2Equ->values[2]);
        }
        else
            continue;

        // Determine based on the distance from the line points to plane e3-O-e1
        double line1Dis = pointcloudProcess_calP2Plane(line1P, box.orientation[1], box.center);
        double line2Dis = pointcloudProcess_calP2Plane(line2P, box.orientation[1], box.center);
        if (line1Dis * distance1Sum > 0.0)
        {
            // Ridge-line parameters
            *interEqu = *line1Equ;
            *outerEqu = *line2Equ;
            // Extract point clouds
            pointcloudFilter_extract(cloud_plate_paraedge, cloud_plate_inter, line1Inliers, false);  // Ridge line
            pointcloudFilter_extract(cloud_plate_paraedge_, cloud_plate_outer, line2Inliers, false); // Rolled edge
        }
        else if (line2Dis * distance1Sum > 0.0)
        {
            // Ridge-line parameters
            *interEqu = *line2Equ;
            *outerEqu = *line1Equ;
            // Extract point clouds
            pointcloudFilter_extract(cloud_plate_paraedge_, cloud_plate_inter, line2Inliers, false); // Ridge line
            pointcloudFilter_extract(cloud_plate_paraedge, cloud_plate_outer, line1Inliers, false);  // Rolled edge
        }
        else
            continue;

        // Validate the results
        if (cloud_plate_inter->size() > 0 && cloud_plate_outer->size() > 0)
        {
            if (debug_)
            {
                QtConcurrent::run([=]()
                                  {
                std::string filepath1 = filePath_ + "17_cloud_plate_inter_" + std::to_string(i) + ".pcd";
                pcl::io::savePCDFile(filepath1, *cloud_plate_inter);
                std::string filepath2 = filePath_ + "18_cloud_plate_outer_" + std::to_string(i) + ".pcd";
                pcl::io::savePCDFile(filepath2, *cloud_plate_outer); });
            }
        }
        else
            continue;

        /***********************************************************************************
         * Feature-parameter extraction
         * ********************************************************************************/

        // Compute ridge-line endpoints
        Eigen::Vector3d interA, interB, interM;
        pointcloudProcess_calProjEndPoints(cloud_tip_fitter, interEqu, interA, interB, interM);

        // Compute direction vector 1
        Eigen::Vector3d leg1Vec = distance1Sum > 0 ? (-1.0 * box.orientation[1]).normalized() : (box.orientation[1]).normalized();

        // Compute direction vector 2
        Eigen::Vector3d leg2Vec = distance2Sum > 0 ? (box.orientation[2]).normalized() : (-1.0 * box.orientation[2]).normalized();

        // Compute the leg width
        Eigen::Vector3d outerP(outerEqu->values[0], outerEqu->values[1], outerEqu->values[2]);
        double width_ = pointcloudProcess_calP2Line(outerP, interEqu);

        // Compute the thickness
        Eigen::Vector3d tipA, tipB, tipM;
        pointcloudProcess_calProjEndPoints(cloud_tip_fitter, tipEqu_, tipA, tipB, tipM);
        Eigen::Vector3d e1 = Eigen::Vector3d(tipEqu_->values[0], tipEqu_->values[1], tipEqu_->values[2]).normalized();
        Eigen::Vector3d e3 = Eigen::Vector3d(mainPlaneEqu_->values[0], mainPlaneEqu_->values[1], mainPlaneEqu_->values[2]).normalized();
        Eigen::Vector3d e2 = (e1.cross(e3)).normalized();
        OBB3D tipObb = {tipM, {e1, e2, e3}, {0.5 * (tipA - tipB).norm(), radiusNeighbor, radiusNeighbor}};
        Eigen::Affine3f transformTip;
        Eigen::Vector4f min_pointTip;
        Eigen::Vector4f max_pointTip;
        pointcloudProcess_calOBB(tipObb.center, tipObb.orientation, tipObb.dimension, transformTip, min_pointTip, max_pointTip);
        pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_tip_part(new pcl::PointCloud<pcl::PointXYZ>);
        pointcloudFilter_cropBox(cloud_tip_fitter, cloud_tip_part, min_pointTip, max_pointTip, transformTip, false);
        cv::Mat imageTip;
        PLCS coordinateTip;
        pointCloudToImage2(cloud_tip_part, imageTip, coordinateTip, mainPlaneEqu_);
        cv::RotatedRect rectTip;
        ImageFeature_rotatedRect(imageTip, rectTip);
        double thickness_ = std::min(rectTip.size.width, rectTip.size.height);

        // Feature-parameter results
        steelAngleParams3D steelangle_params3d;
        steelangle_params3d.ridge[0] = interA;
        steelangle_params3d.ridge[1] = interB;
        steelangle_params3d.leg1Vec = leg1Vec;
        steelangle_params3d.leg2Vec = leg2Vec;
        steelangle_params3d.width = width_;
        steelangle_params3d.thickness = thickness_;

        // Output result parameters
        steelangle_params_[i] = steelangle_params3d;
    }

    // Filter unrecognized angle-steel parameters
    steelangle_params_.erase(
        std::remove_if(
            steelangle_params_.begin(),
            steelangle_params_.end(),
            [](const steelAngleParams3D &p)
            {
                return !std::isfinite(p.width) || std::abs(p.width) < 1e-6;
            }),
        steelangle_params_.end());

    // Output the results
    if (steelangle_obb.size() == steelangle_params_.size())
    {
        steelangle_params = std::move(steelangle_params_);
    }
    else
    {
        QString msg = u8"Feature extraction: angle-steel data is incomplete.";
        emit sendReconstructMessage(msg, 0); // Emit the runtime message for this class
    }
}

void SteelAngleRecon::featureRegularization(std::vector<steelAngleParams3D> &steelangle_params)
{
    // Validate the input
    if (steelangle_params.empty())
    {
        QString msg = u8"Parameter regularization: input feature parameters are empty.";
        emit sendReconstructMessage(msg, 0); // Emit the runtime message for this class
        return;
    }

    // Correction error threshold
    double angThreshold = params_->corrAngleTh;
    double disThreshold = params_->corrdisTh;

#pragma region "Global Cross-Section Dimension Averaging" {
    // Compute the mean width of the angle steel
    double width_ = 0.0, thickness_ = 0.0;
    for (const auto &angle : steelangle_params)
    {
        width_ += angle.width;
        thickness_ += angle.thickness;
    }
    double width_steelangle = !steelangle_params.empty() ? width_ / steelangle_params.size() : 0.0;
    double thickness_steelangle = !steelangle_params.empty() ? thickness_ / steelangle_params.size() : 0.0;
    // Specify the leg width and thickness of the angle steel
    for (size_t i = 0; i < steelangle_params.size(); ++i)
    {
        steelangle_params[i].width = width_steelangle;
        steelangle_params[i].thickness = thickness_steelangle;
    }
#pragma endregion }

#pragma region "Global Correction" {
    // Project onto the main plane and correct to the plane (line fitting may place ridge lines slightly off the plane)
    for (size_t i = 0; i < steelangle_params.size(); ++i)
    {
        pointcloudProcess_projectPointOntoPlane(steelangle_params[i].ridge[0], steelangle_params[i].ridge[0], mainPlaneEqu_);
        pointcloudProcess_projectPointOntoPlane(steelangle_params[i].ridge[1], steelangle_params[i].ridge[1], mainPlaneEqu_);
    }
    // Correct angle-steel direction vector 1
    Eigen::Vector3d n(mainPlaneEqu_->values[0], mainPlaneEqu_->values[1], mainPlaneEqu_->values[2]);
    for (size_t i = 0; i < steelangle_params.size(); ++i)
    {
        Eigen::Vector3d p1 = steelangle_params[i].ridge[0];
        Eigen::Vector3d p2 = steelangle_params[i].ridge[1];

        Eigen::Vector3d dir1 = (n.cross(p2 - p1)).normalized();
        steelangle_params[i].leg1Vec = dir1.dot(steelangle_params[i].leg1Vec) < 0 ? -dir1 : dir1;
    }
#pragma endregion }

#pragma region "Endpoint Correction at Angle-Steel Joints" {
    if (steelangle_params.size() >= 2)
    {
        double distanceThreshold = 0.25 * width_steelangle; // The distance between the corner point and the endpoint is less than 0.25 times the angle-steel width
        for (size_t i = 0; i < steelangle_params.size(); ++i)
        {
            for (size_t j = i + 1; j < steelangle_params.size(); ++j)
            {
                // Ridge line
                Eigen::Vector3d p1 = steelangle_params[i].ridge[0];
                Eigen::Vector3d p2 = steelangle_params[i].ridge[1];
                Eigen::Vector3d p3 = steelangle_params[j].ridge[0];
                Eigen::Vector3d p4 = steelangle_params[j].ridge[1];
                // Compute the angle between ridge lines
                Eigen::Vector3d lv1 = (p2 - p1).normalized();
                Eigen::Vector3d lv2 = (p4 - p3).normalized();
                double angle = pointcloudProcess_calAngle(lv1, lv2);
                angle = angle * 180.0 / M_PI;
                angle = angle > 90 ? 180 - angle : angle;
                if (angle <= angThreshold) // This also ensures that the two lines are not parallel
                    continue;
                // Compute intersections of ridge lines
                Eigen::Vector3d intersection;
                bool ok = pointcloudProcess_calIntersection(p1, p2, p3, p4, intersection);
                if (!ok)
                    continue;
                // Compute distances
                double distance1 = pointcloudProcess_calDistance(intersection, p1);
                double distance2 = pointcloudProcess_calDistance(intersection, p2);
                double distance3 = pointcloudProcess_calDistance(intersection, p3);
                double distance4 = pointcloudProcess_calDistance(intersection, p4);
                // Check whether the intersection lies on the line
                bool online1 = pointcloudProcess_isPointOnLine(p1, p2, intersection);
                bool online2 = pointcloudProcess_isPointOnLine(p3, p4, intersection);
                // Check whether the normals of adjacent angle steels point in the same direction
                Eigen::Vector3d d1 = steelangle_params[i].leg2Vec;
                Eigen::Vector3d d2 = steelangle_params[j].leg2Vec;
                double dirSign = d1.dot(d2);
                // Find the endpoints that need to be adjusted
                std::vector<int> pointId;
                if (distance1 < distanceThreshold)
                    pointId.push_back(1);
                if (distance2 < distanceThreshold)
                    pointId.push_back(2);
                if (distance3 < distanceThreshold)
                    pointId.push_back(3);
                if (distance4 < distanceThreshold)
                    pointId.push_back(4);
                // Filter the targets: "L" type, "T" type, and cross type
                if (pointId.size() == 2)
                {                        // "L" type: for same-normal bends or opposite-normal lap joints, replace the original endpoint with the intersection
                    if (pointId[0] == 1) // Point p1
                        steelangle_params[i].ridge[0] = intersection;
                    if (pointId[0] == 2) // Point p2
                        steelangle_params[i].ridge[1] = intersection;
                    if (pointId[1] == 3) // Point p3
                        steelangle_params[j].ridge[0] = intersection;
                    if (pointId[1] == 4) // Point p4
                        steelangle_params[j].ridge[1] = intersection;
                }
                else if ((pointId.size() == 1 && online1 == true && online2 == true)                                                   // "T" type: replace with the shifted intersection point, where the shift direction is determined by the adjacent edge direction vector
                         || (pointId.size() == 1 && online1 == true && distance1 > distanceThreshold && distance2 > distanceThreshold) // "T" type
                         || (pointId.size() == 1 && online2 == true && distance3 > distanceThreshold && distance4 > distanceThreshold))
                { // "T" type
                    Eigen::Vector3d v1(0, 0, 0), v2(0, 0, 0), v3(0, 0, 0), v4(0, 0, 0), projPt(0, 0, 0);
                    switch (pointId[0])
                    {
                    case 1: // Point p1：
                        steelangle_params[i].ridge[0] = intersection;
                        pointcloudProcess_calPerpendicularFoot(p3, p4, p2, projPt);
                        v3 = (p3 - p4).normalized();
                        v4 = (p4 - p3).normalized();
                        if (distance3 - width_steelangle < disThreshold && v3.dot(steelangle_params[i].leg1Vec) > 0.0)
                            steelangle_params[j].ridge[0] = projPt + width_steelangle * v3;
                        if (distance4 - width_steelangle < disThreshold && v4.dot(steelangle_params[i].leg1Vec) > 0.0)
                            steelangle_params[j].ridge[1] = projPt + width_steelangle * v4;
                        break;
                    case 2: // Point p2：
                        steelangle_params[i].ridge[1] = intersection;
                        pointcloudProcess_calPerpendicularFoot(p3, p4, p1, projPt);
                        v3 = (p3 - p4).normalized();
                        v4 = (p4 - p3).normalized();
                        if (distance3 - width_steelangle < disThreshold && v3.dot(steelangle_params[i].leg1Vec) > 0.0)
                            steelangle_params[j].ridge[0] = projPt + width_steelangle * v3;
                        if (distance4 - width_steelangle < disThreshold && v4.dot(steelangle_params[i].leg1Vec) > 0.0)
                            steelangle_params[j].ridge[1] = projPt + width_steelangle * v4;
                        break;
                    case 3: // Point p3：
                        steelangle_params[j].ridge[0] = intersection;
                        pointcloudProcess_calPerpendicularFoot(p1, p2, p4, projPt);
                        v1 = (p1 - p2).normalized();
                        v2 = (p2 - p1).normalized();
                        if (distance1 - width_steelangle < disThreshold && v1.dot(steelangle_params[j].leg1Vec) > 0.0)
                            steelangle_params[i].ridge[0] = projPt + width_steelangle * v1;
                        if (distance2 - width_steelangle < disThreshold && v2.dot(steelangle_params[j].leg1Vec) > 0.0)
                            steelangle_params[i].ridge[1] = projPt + width_steelangle * v2;
                        break;
                    case 4: // Point p4：
                        steelangle_params[j].ridge[1] = intersection;
                        pointcloudProcess_calPerpendicularFoot(p1, p2, p3, projPt);
                        v1 = (p1 - p2).normalized();
                        v2 = (p2 - p1).normalized();
                        if (distance1 - width_steelangle < disThreshold && v1.dot(steelangle_params[j].leg1Vec) > 0.0)
                            steelangle_params[i].ridge[0] = projPt + width_steelangle * v1;
                        if (distance2 - width_steelangle < disThreshold && v2.dot(steelangle_params[j].leg1Vec) > 0.0)
                            steelangle_params[i].ridge[1] = projPt + width_steelangle * v2;
                        break;
                    default:
                        std::cerr << u8"parameter Correct Error" << std::endl;
                        break;
                    }
                }
                else if (pointId.size() == 0 && online1 == true && online2 == true && dirSign < 0.0)
                { // Cross type
                    // Mark patch: increase the length of the angle steel
                    // Compute the line direction
                    Eigen::Vector3d v1 = (p2 - p1).normalized();
                    Eigen::Vector3d v2 = (p4 - p3).normalized();
                    // Extend from the intersection along the line direction by one angle-steel width to obtain four new endpoints
                    Eigen::Vector3d p1_ = intersection - v1 * width_steelangle;
                    Eigen::Vector3d p2_ = intersection + v1 * width_steelangle;
                    Eigen::Vector3d p3_ = intersection - v2 * width_steelangle;
                    Eigen::Vector3d p4_ = intersection + v2 * width_steelangle;
                    // Replace the original endpoint if it is close to the new endpoint
                    if (pointcloudProcess_calDistance(p1_, p1) < distanceThreshold)
                        steelangle_params[i].ridge[0] = p1_;
                    if (pointcloudProcess_calDistance(p2_, p2) < distanceThreshold)
                        steelangle_params[i].ridge[1] = p2_;
                    if (pointcloudProcess_calDistance(p3_, p3) < distanceThreshold)
                        steelangle_params[j].ridge[0] = p3_;
                    if (pointcloudProcess_calDistance(p4_, p4) < distanceThreshold)
                        steelangle_params[j].ridge[1] = p4_;
                }
                else
                { // Handle any missed cases
                }
            }
        }
    }
#pragma endregion }

}

void SteelAngleRecon::featureToModelParams(const std::vector<steelAngleParams3D> &steelangle_params, std::vector<ModelParameters> &modelParams)
{
    // Validate the input
    if (steelangle_params.empty())
    {
        QString msg = u8"Parameter conversion: input feature parameters are empty.";
        emit sendReconstructMessage(msg, 0); // Emit the runtime message for this class
        return;
    }

#pragma region "Angle-Steel Section" {
    std::vector<steelAngleParams3D> steelangleParams = steelangle_params;
    // Check for identical endpoints
    auto endPointisEqual = [](const std::array<Eigen::Vector3d, 2> &line1, const std::array<Eigen::Vector3d, 2> &line2)
    {
        Eigen::Vector3d p1(line1[0]), p2(line1[1]), p3(line2[0]), p4(line2[1]);
        int order = -1;
        if ((p1 - p3).isApprox(Eigen::Vector3d::Zero(), 1e-6)) // A shared point exists; line 1 must follow line 2
            order = -1;
        else if ((p1 - p4).isApprox(Eigen::Vector3d::Zero(), 1e-6)) // A shared point exists; line 1 must follow line 2
            order = -2;
        else if ((p2 - p3).isApprox(Eigen::Vector3d::Zero(), 1e-6)) // A shared point exists; line 2 must follow line 1
            order = +1;
        else if ((p2 - p4).isApprox(Eigen::Vector3d::Zero(), 1e-6)) // A shared point exists; line 2 must follow line 1
            order = +2;
        else
            order = 0;
        return order;
    };

    // Based on depth-first search
    std::vector<std::deque<int>> indexQueue;                     // Store angle-steel feature parameters
    std::vector<bool> isVisited(steelangleParams.size(), false); // Mark whether the item has been processed
    // Define the recursive DFS lambda
    std::function<void(int, std::deque<int> &)> dfs =
        [&](int current, std::deque<int> &index)
    {
        isVisited[current] = true;
        for (int j = 0; j < steelangleParams.size(); ++j)
        {
            if (!isVisited[j])
            {

                int order = endPointisEqual(steelangleParams[current].ridge, steelangleParams[j].ridge); // Check whether identical endpoints exist
                switch (order)
                {
                case 0: // No identical endpoints
                    break;
                case -1:
                    std::swap(steelangleParams[j].ridge[0], steelangleParams[j].ridge[1]); // Adjust the endpoint order
                    index.push_front(j);                                                   // Insert at the front of the queue
                    dfs(j, index);                                                         // Recursively process adjacent line segments
                    break;
                case -2:
                    index.push_front(j); // Insert at the front of the queue
                    dfs(j, index);       // Recursively process adjacent line segments
                    break;
                case +1:
                    index.push_back(j); // Insert at the back of the queue
                    dfs(j, index);      // Recursively process adjacent line segments
                    break;
                case +2:
                    std::swap(steelangleParams[j].ridge[0], steelangleParams[j].ridge[1]); // Adjust the endpoint order
                    index.push_back(j);                                                    // Insert at the back of the queue
                    dfs(j, index);                                                         // Recursively process adjacent line segments
                    break;
                default:
                    std::cerr << "Unknown Error" << std::endl;
                    break;
                }
            }
        }
    };

    // Perform DFS traversal
    for (int i = 0; i < steelangleParams.size(); ++i)
    {
        if (isVisited[i])
            continue;
        std::deque<int> index;
        index.push_back(i);
        dfs(i, index);
        indexQueue.push_back(index);
    }

    // Model parameters
    if (indexQueue.size() <= 0)
        return;

    std::vector<ModelParameters> steelanglemodelParam;
    for (const auto &idx : indexQueue)
    {
        ModelParameters para;
        /************************ Sweep Guide Line ****************************/
        std::vector<std::array<gp_Pnt, 2>> leaderLine;
        for (size_t i = 0; i < idx.size(); ++i)
        {
            Eigen::Vector3d p1 = steelangleParams[idx[i]].ridge[0];
            Eigen::Vector3d p2 = steelangleParams[idx[i]].ridge[1];
            std::array<gp_Pnt, 2> edge = {gp_Pnt(p1.x(), p1.y(), p1.z()), gp_Pnt(p2.x(), p2.y(), p2.z())};
            leaderLine.push_back(edge);
        }

        /************************ Sketch Corners ****************************/
        // Step 1: Sketch cross-section reference frame
        Eigen::Vector3d _o = steelangleParams[idx[0]].ridge[0];
        Eigen::Vector3d _u = steelangleParams[idx[0]].leg1Vec;
        Eigen::Vector3d _v = steelangleParams[idx[0]].leg2Vec;
        gp_Pnt o(_o.x(), _o.y(), _o.z());
        gp_Vec u = gp_Vec(_u.x(), _u.y(), _u.z());
        gp_Vec v = gp_Vec(_v.x(), _v.y(), _v.z());
        // Step 2: Define the cross-section vertices
        gp_Pnt p0 = o;
        gp_Pnt p1 = p0.Translated(steelangleParams[idx[0]].width * u);
        gp_Pnt p2 = p1.Translated(steelangleParams[idx[0]].thickness * v);
        gp_Pnt p3 = p0.Translated(steelangleParams[idx[0]].thickness * u + steelangleParams[idx[0]].thickness * v);
        gp_Pnt p5 = p0.Translated(steelangleParams[idx[0]].width * v);
        gp_Pnt p4 = p5.Translated(steelangleParams[idx[0]].thickness * u);
        // Step 3: Output the cross-section corner points
        std::vector<gp_Pnt> sketchPoints = {p0, p1, p2, p3, p4, p5};

        /************************ Output Results ****************************/
        para.leaderLine = leaderLine;
        para.sketchPoints = sketchPoints;
        steelanglemodelParam.push_back(para);
    }
#pragma endregion }

    // Output the results
    modelParams.insert(modelParams.end(), steelanglemodelParam.begin(), steelanglemodelParam.end());
}
