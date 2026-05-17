#include "model_common.h"

void reconstruction::modelExplorer_stepWriter(const TopoDS_Shape &ashape, const std::string &filename)
{
    Message::DefaultMessenger()->RemovePrinters(STANDARD_TYPE(Message_PrinterOStream));
    TCollection_AsciiString stepname(filename.c_str());
    STEPControl_Writer writer;
    writer.Transfer(ashape, STEPControl_AsIs);
    writer.Write(stepname.ToCString());
}

void reconstruction::modelExplorer_stlWriter(const TopoDS_Shape &ashape, const std::string &filename)
{
    TCollection_AsciiString stlname(filename.c_str());
    const Standard_Real deflection = 0.5;
    const Standard_Real angle = 0.5;
    BRepMesh_IncrementalMesh(ashape, deflection, Standard_False, angle, Standard_True);
    StlAPI::Write(ashape, stlname.ToCString());
}

bool reconstruction::modeling_parametricModel(const std::vector<ModelParameters> &para, TopoDS_Shape &ashape)
{
    if (para.empty())
        return false;

    try
    {
        // Independent shape construction
        std::vector<TopoDS_Shape> shapes;
        for (const auto &p : para)
        {
            // Step 1: Use BRepBuilderAPI_MakeWire to construct the sweep spine
            BRepBuilderAPI_MakeWire spineWire;
            for (const auto &point : p.leaderLine)
            {
                spineWire.Add(BRepBuilderAPI_MakeEdge(point[0], point[1]).Edge());
            }
            TopoDS_Wire spine = spineWire.Wire();
            // Step 2: Use BRepBuilderAPI_MakePolygon to construct the sweep profile
            BRepBuilderAPI_MakePolygon polygonBuilder;
            for (const auto &point : p.sketchPoints)
            {
                polygonBuilder.Add(point);
            }
            polygonBuilder.Close();
            TopoDS_Wire profile = polygonBuilder.Wire();
            // Step 3: Use BRepOffsetAPI_MakePipeShell for sweeping
            BRepOffsetAPI_MakePipeShell sweep(spine);
            sweep.SetMode(Standard_False);
            sweep.SetTransitionMode(BRepBuilderAPI_RightCorner);
            sweep.Add(profile);
            sweep.Build();
            if (sweep.MakeSolid())
                shapes.push_back(sweep.Shape());
        }

        // Compute the union of multiple shapes
        TopoDS_Shape fusedShape;
        if (shapes.size() == 1)
        {
            fusedShape = shapes[0];
        }
        else if (shapes.size() > 1)
        {
            BOPAlgo_BOP bop;
            bop.SetOperation(BOPAlgo_Operation::BOPAlgo_FUSE);
            bop.SetRunParallel(Standard_True);
            bop.SetNonDestructive(Standard_False);
            bop.SetGlue(BOPAlgo_GlueEnum::BOPAlgo_GlueOff);
            bop.AddArgument(shapes[0]);
            for (int i = 1; i < shapes.size(); ++i)
                bop.AddTool(shapes[i]);
            bop.Perform();
            fusedShape = bop.Shape();
        }
        else
            return false;

        // Merge coplanar faces and simplify the topology
        ShapeUpgrade_UnifySameDomain unify(fusedShape, Standard_True, Standard_True, Standard_False);
        unify.AllowInternalEdges(Standard_False);
        unify.SetLinearTolerance(Precision::Confusion());
        unify.SetAngularTolerance(Precision::Confusion());
        unify.Build();
        TopoDS_Shape unified = unify.Shape();

        // Output the result
        if (unified.IsNull())
            return false;
        else
        {
            BRepBuilderAPI_Copy copier(unified); // Deep copy
            ashape = copier.Shape();
            return true;
        }
    }
    catch (const std::exception &e)
    {
        // Catch standard exceptions and output the exception message
        std::cerr << "Standard exception caught: " << e.what() << std::endl;
        return false;
    }
    catch (...)
    {
        // Catch any unknown exceptions
        std::cerr << "Unknown exception caught!" << std::endl;
        return false;
    }
}

bool reconstruction::modelHandler_samplePoints(const TopoDS_Shape &ashape, pcl::PointCloud<pcl::PointXYZ>::Ptr &cloud, double density)
{
    if (ashape.IsNull())
        return false;
    try
    {
        // Step 1: Create a BRepLib_PointCloudShape object
        PointCloudPnt pointCloudGenerate(ashape);
        // Step 2: Generate point cloud based on density
        if (!pointCloudGenerate.GeneratePointsByDensity(density))
        {
            std::cerr << "Failed to generate point cloud!" << std::endl;
            return false;
        }
        // Step 3: Retrieve the generated point cloud data
        pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_(new pcl::PointCloud<pcl::PointXYZ>);
        for (const auto &p : pointCloudGenerate.pointCloud)
        {
            gp_Pnt point = p;
            cloud_->push_back(pcl::PointXYZ(point.X(), point.Y(), point.Z()));
        }
        // Step 4: Output the point cloud
        if (cloud_->size() > 0)
        {
            *cloud = *cloud_;
            return true;
        }
        else
            return false;
    }
    catch (const std::exception &e)
    {
        // Catch standard exceptions and output the exception message
        std::cerr << "Standard exception caught: " << e.what() << std::endl;
        return false;
    }
    catch (...)
    {
        // Catch any unknown exceptions
        std::cerr << "Unknown exception caught!" << std::endl;
        return false;
    }
}
