#ifndef OCCTVIEWERWIDGET_H
#define OCCTVIEWERWIDGET_H
// Qt
#include <QWidget>
#include <QApplication>

// OCCT
#include <TopoDS_Shape.hxx>
#include <StlAPI_Reader.hxx>
#include <StlAPI_Writer.hxx>
#include <STEPControl_Reader.hxx>
#include <STEPControl_Writer.hxx>
#include <IVtkOCC_Shape.hxx>
#include <IVtkVTK_ShapeData.hxx>
#include <IVtkOCC_ShapeMesher.hxx>
#include <IVtkTools_ShapeDataSource.hxx>
#include <IVtkTools_DisplayModeFilter.hxx>

// VTK
#include <QVTKOpenGLNativeWidget.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkType.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkPolyDataMapper.h>
#include <vtkSmartPointer.h>
#include <vtkCamera.h>
#include <vtkAxesActor.h>
#include <vtkOrientationMarkerWidget.h>

class OCCTViewerWidget : public QVTKOpenGLNativeWidget
{
    Q_OBJECT
public:
    explicit OCCTViewerWidget(QWidget *parent = nullptr);
    ~OCCTViewerWidget();

    void modelShow(const TopoDS_Shape &aShape);

signals:
    void sendOCCTViewerMessage(QString module, QString msg, int code);

private:
    bool shape2vtkdata(const TopoDS_Shape &aShape, vtkSmartPointer<vtkPolyDataMapper> mapper);

    bool wire2vtkdata(const TopoDS_Shape &aWire, vtkSmartPointer<vtkPolyDataMapper> mapper);

private:
    vtkSmartPointer<vtkRenderer> renderer = nullptr;
    vtkSmartPointer<vtkGenericOpenGLRenderWindow> renderWindow = nullptr;
    vtkSmartPointer<vtkCamera> camera = nullptr;
    vtkSmartPointer<vtkOrientationMarkerWidget> axesWidget = nullptr;
};

#endif // OCCTVIEWERWIDGET_H
