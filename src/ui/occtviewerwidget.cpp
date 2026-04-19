#include "occtviewerwidget.h"

OCCTViewerWidget::OCCTViewerWidget(QWidget *parent) : QVTKOpenGLNativeWidget(parent),
                                                      renderer(vtkSmartPointer<vtkRenderer>::New()),
                                                      renderWindow(vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New()),
                                                      camera(vtkSmartPointer<vtkCamera>::New()),
                                                      axesWidget(vtkSmartPointer<vtkOrientationMarkerWidget>::New())
{
    // QVTKOpenGLWidget initialization
    renderWindow->AddRenderer(renderer);
    renderer->SetBackground(0, 0, 0); // Set background color
    this->SetRenderWindow(renderWindow);
    this->setEnableHiDPI(true);

    // Camera initialization
    camera->SetPosition(0.0, 0.0, 0.0);     // Set camera position (x, y, z)
    camera->SetFocalPoint(0.0, 0.0, 800.0); // Set camera focal point (x, y, z)
    camera->SetViewUp(1.0, 0.0, 0.0);       // Set camera's upward direction
    renderer->SetActiveCamera(camera);

    // Create coordinate axes
    vtkSmartPointer<vtkAxesActor> axesActor = vtkSmartPointer<vtkAxesActor>::New();
    axesWidget->SetOrientationMarker(axesActor);                         // Set the coordinate axes to display
    axesWidget->SetInteractor(this->GetRenderWindow()->GetInteractor()); // Set the interactor
    axesWidget->SetViewport(0.0, 0.0, 0.25, 0.25);
    axesWidget->SetEnabled(true); // Enable the small window to display the coordinate axes
    axesWidget->InteractiveOff(); // Disable interaction

    // Update rendering
    renderer->ResetCamera();
    this->GetRenderWindow()->Render();
}

OCCTViewerWidget::~OCCTViewerWidget()
{
    this->GetInteractor()->SetRenderWindow(nullptr);
    this->GetInteractor()->SetInteractorStyle(nullptr);
}

void OCCTViewerWidget::modelShow(const TopoDS_Shape &aShape)
{
    // Clear existing content
    renderer->RemoveAllViewProps();

    // Create PolyDataMapper
    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    if (shape2vtkdata(aShape, mapper))
    {
        // Create Actor
        vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
        actor->SetMapper(mapper);
        actor->GetProperty()->SetDiffuse(0.8);         // Set model diffuse coefficient (0.0-1.0)
        actor->GetProperty()->SetSpecular(0.5);        // Set model specular coefficient (0.0-1.0)
        actor->GetProperty()->SetSpecularPower(60.0);  // Set surface specular highlight properties
        actor->GetProperty()->SetAmbient(0.3);         // Set ambient light reflection
        actor->GetProperty()->SetColor(0.0, 1.0, 0.0); // Set model color
        // Add to renderer
        renderer->AddActor(actor);

        // Adjust camera view
        camera->SetPosition(0.0, 0.0, 0.0);     // Set camera position (x, y, z)
        camera->SetFocalPoint(0.0, 0.0, 800.0); // Set camera focal point (x, y, z)
        camera->SetViewUp(1.0, 0.0, 0.0);       // Set camera's upward direction
        renderer->SetActiveCamera(camera);

        // Start rendering
        renderer->ResetCamera();
        this->GetRenderWindow()->Render();
    }
    else
    {
        QString msg = u8"Model data conversion error!";
        emit sendOCCTViewerMessage(u8"Model Viewer", msg, 0);
    }
}

bool OCCTViewerWidget::shape2vtkdata(const TopoDS_Shape &aShape, vtkSmartPointer<vtkPolyDataMapper> mapper)
{
    if (!aShape.IsNull())
    { // Check for null shape
        try
        {
            // Create VIS wrapper for OCCT shape
            IVtkOCC_Shape::Handle aShapeImpl = new IVtkOCC_Shape(aShape);
            // Create IVtkTools_ShapeDataSource to convert OCCT shape to VTK format
            vtkSmartPointer<IVtkTools_ShapeDataSource> aSource = vtkSmartPointer<IVtkTools_ShapeDataSource>::New();
            aSource->SetShape(aShapeImpl);
            // Create IVtkTools_DisplayModeFilter to hide ISO lines
            vtkSmartPointer<IVtkTools_DisplayModeFilter> filter = vtkSmartPointer<IVtkTools_DisplayModeFilter>::New();
            filter->AddInputConnection(aSource->GetOutputPort());
            filter->SetDisplayMode(DM_Shading);
            // Create PolyDataMapper
            mapper->SetInputConnection(filter->GetOutputPort());

            return true;
        }
        catch (const std::exception &e)
        {
            // Catch standard exceptions
            std::cerr << "Standard exception caught: " << e.what() << std::endl;
            return false;
        }
        catch (...)
        {
            // Catch all other exceptions
            std::cerr << "Unknown exception caught." << std::endl;
            return false;
        }
    }
    else
    {
        QString msg = u8"Model content is empty!";
        emit sendOCCTViewerMessage(u8"Model Viewer", msg, 0);
        return false;
    }
}