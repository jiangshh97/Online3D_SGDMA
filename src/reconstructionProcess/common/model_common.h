#pragma once

// Header files
#include <iostream>
#include <algorithm>

// OCCT
#include <TopoDS.hxx>
#include <TopoDS_Builder.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_ListOfShape.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GC_MakeArcOfCircle.hxx>
#include <GC_MakeSegment.hxx>
#include <GC_MakeCircle.hxx>
#include <BRepLib.hxx>
#include <BRepLib_PointCloudShape.hxx>
#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <BRepBuilderAPI_Copy.hxx>
#include <BRepBuilderAPI_MakePolygon.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeShape.hxx>
#include <BRepBuilderAPI_Sewing.hxx>
#include <BRepBuilderAPI_MakeSolid.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <BRepPrimAPI_MakeSweep.hxx>
#include <BRepOffsetAPI_MakePipeShell.hxx>
#include <BRepOffsetAPI_MakePipe.hxx>
#include <BRepOffsetAPI_ThruSections.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <BRepAlgoAPI_Common.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <Bnd_OBB.hxx>
#include <BRepBndLib.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepLProp_SurfaceTool.hxx>
#include <Geom_Plane.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <BRepOffsetAPI_MakeThickSolid.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <GCE2d_MakeSegment.hxx>
#include <TopExp_Explorer.hxx>
#include <Law_Function.hxx>
#include <STEPControl_Reader.hxx>
#include <STEPControl_Writer.hxx>
#include <StlAPI.hxx>
#include <StlAPI_Reader.hxx>
#include <StlAPI_Writer.hxx>
#include <gp_Cylinder.hxx>
#include <ShapeAnalysis.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <gp_Vec.hxx>
#include <TCollection_AsciiString.hxx>
#include <Standard_CString.hxx>
#include <ShapeUpgrade_UnifySameDomain.hxx>
#include <ShapeUpgrade_RemoveInternalWires.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <Poly_Triangulation.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <BRepClass3d_SolidClassifier.hxx>
#include <Precision.hxx>
#include <Interface_Static.hxx>
#include <BOPAlgo_BOP.hxx>
#include <BOPAlgo_MakeConnected.hxx>
#include <BOPAlgo_PaveFiller.hxx>
#include <BOPAlgo_Builder.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <gp_Quaternion.hxx>
#include <ShapeFix_Shape.hxx>
#include <IVtkOCC_Shape.hxx>
#include <IVtkVTK_ShapeData.hxx>
#include <IVtkOCC_ShapeMesher.hxx>
#include <IVtkTools_ShapeDataSource.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <Interface_Static.hxx>
#include <XSControl_WorkSession.hxx>
#include <Message.hxx>
#include <Message_PrinterOStream.hxx>
#include <TopAbs_ShapeEnum.hxx>

// PCL
#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>
#include <pcl/common/transforms.h>

// VTK
#include <vtkSmartPointer.h>
#include <vtkPolyData.h>

namespace reconstruction
{
    /**
     * @brief Export the model to a STEP file
     * @param ashape     Input CAD shape
     * @param filename   Output file path
     */
    void modelExplorer_stepWriter(const TopoDS_Shape &ashape, const std::string &filename);

    /**
     * @brief Export the model to an STL file
     * @param ashape     Input CAD shape
     * @param filename   Output file path
     */
    void modelExplorer_stlWriter(const TopoDS_Shape &ashape, const std::string &filename);

    /************************************ Model parameter structure ************************************/
    struct ModelParameters
    {
        std::vector<std::array<gp_Pnt, 2>> leaderLine; // Sweep path (a single segment or a chain of connected segments)
        std::vector<gp_Pnt> sketchPoints;              // Cross-section sketch vertices
    };

    /**
     * @brief Parametric model generation
     * @param para       Model parameters
     * @param ashape     Output CAD shape
     */
    bool modeling_parametricModel(const std::vector<ModelParameters> &para, TopoDS_Shape &ashape);

    /************************************ Model to point cloud ************************************/
    class PointCloudPnt : public BRepLib_PointCloudShape
    {
    public:
        PointCloudPnt(const TopoDS_Shape &shape) : BRepLib_PointCloudShape(shape) {}
        std::vector<gp_Pnt> pointCloud; // Generated point cloud data
    protected:
        virtual void addPoint(const gp_Pnt &thePoint,
                              const gp_Vec &theNorm,
                              const gp_Pnt2d &theUV,
                              const TopoDS_Shape &theFace) Standard_OVERRIDE
        {
            pointCloud.push_back(thePoint);
        }
    };

    /**
     * @brief Sample point cloud from CAD model
     * @param ashape     Input CAD shape
     * @param cloud      Output point cloud
     * @param density    Sampling density
     */
    bool modelHandler_samplePoints(const TopoDS_Shape &ashape,
                                   pcl::PointCloud<pcl::PointXYZ>::Ptr &cloud,
                                   double density = 1.0);
}
