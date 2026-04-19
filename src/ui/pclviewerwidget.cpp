#include "pclviewerwidget.h"
#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkRenderingOpenGL2)
VTK_MODULE_INIT(vtkInteractionStyle)
VTK_MODULE_INIT(vtkRenderingVolumeOpenGL2)
VTK_MODULE_INIT(vtkRenderingFreeType)

PCLViewerWidget::PCLViewerWidget(QWidget *parent) : QVTKOpenGLNativeWidget(parent),
                                                    renderer(vtkSmartPointer<vtkRenderer>::New()),
                                                    renderWindow(vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New()),
                                                    axes(vtkSmartPointer<vtkOrientationMarkerWidget>::New())
{
    renderWindow->AddRenderer(renderer);
    viewer.reset(new pcl::visualization::PCLVisualizer(renderer, renderWindow, "pcl viewer", false));
    viewer->setupInteractor(this->GetInteractor(), this->GetRenderWindow());
    this->SetRenderWindow(viewer->getRenderWindow());
    this->setEnableHiDPI(true);
    // Set background color
    viewer->setBackgroundColor(0, 0, 0);
    // Set camera view
    viewer->setCameraPosition(0, 0, 0,   // Camera position on the Z-axis (x, y, z)
                              0, 0, 800, // Focus position at the origin (x, y, z)
                              1, 0, 0);  // View direction along the positive X-axis (x, y, z)
    viewer->resetCamera();               // Reset the camera and fit the current scene
    // Add coordinate system
    vtkSmartPointer<vtkAxesActor> actor = vtkSmartPointer<vtkAxesActor>::New();
    // Display the OrientationMarkerWidget for the coordinate system
    axes->SetOrientationMarker(actor);                               // Set the coordinate axes to display
    axes->SetInteractor(viewer->getRenderWindow()->GetInteractor()); // Set the interactor
    axes->SetViewport(0.0, 0.0, 0.25, 0.25);
    axes->SetEnabled(true); // Enable the small window to display the coordinate axes
    axes->InteractiveOff(); // Disable interaction
    // Start rendering
    this->GetRenderWindow()->Render();
}

PCLViewerWidget::~PCLViewerWidget()
{
    this->GetInteractor()->SetRenderWindow(nullptr);
    this->GetInteractor()->SetInteractorStyle(nullptr);
}

void PCLViewerWidget::pointcloudShow(pcl::PointCloud<pcl::PointXYZ>::ConstPtr cloud)
{
    viewer->removeAllPointClouds();
    if (cloud->size() > 0)
    {
        // Deep copy of the point cloud
        pcl::PointCloud<pcl::PointXYZ>::Ptr copied(new pcl::PointCloud<pcl::PointXYZ>);
        *copied = *cloud;
        // Visualization
        pcl::visualization::PointCloudColorHandlerCustom<pcl::PointXYZ> cloud_color(copied, 0, 255, 0); // Custom point cloud color
        viewer->addPointCloud<pcl::PointXYZ>(copied, cloud_color, "sample cloud");
        viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 1, "sample cloud"); // Size of individual points in the point cloud
        viewer->resetCamera();
        this->GetRenderWindow()->Render();
    }
    else
    {
        QString msg = u8"Point cloud file is empty!";
        emit sendPLCViewerMessage(u8"Point Cloud Viewer", msg, 0); // Pass runtime information of this class (code == -1 for notification / code != -1 for error code)
    }
}
