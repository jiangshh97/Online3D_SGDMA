#ifndef PCLVIEWERWIDGET_H
#define PCLVIEWERWIDGET_H

// Qt
#include <QWidget>
#include <QApplication>

// VTK
#include <QVTKOpenGLNativeWidget.h>
#include <vtkRenderWindow.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkAxesActor.h>
#include <vtkOrientationMarkerWidget.h>

// PCL
#include <pcl/point_cloud.h>
#include <pcl/io/pcd_io.h>
#include <pcl/io/ply_io.h>
#include <pcl/point_types.h>
#include <pcl/visualization/pcl_visualizer.h>

class PCLViewerWidget : public QVTKOpenGLNativeWidget
{
    Q_OBJECT
public:
    explicit PCLViewerWidget(QWidget *parent = nullptr);

    ~PCLViewerWidget();

    void pointcloudShow(pcl::PointCloud<pcl::PointXYZ>::ConstPtr cloud);

signals:
    void sendPLCViewerMessage(QString module, QString msg, int code);

private:
    boost::shared_ptr<pcl::visualization::PCLVisualizer> viewer = nullptr;
    vtkSmartPointer<vtkRenderer> renderer = nullptr;
    vtkSmartPointer<vtkGenericOpenGLRenderWindow> renderWindow = nullptr;
    vtkSmartPointer<vtkOrientationMarkerWidget> axes = nullptr;
};

#endif // PCLVIEWERWIDGET_H
