#ifndef GENERICVTKWIDGET_H
#define GENERICVTKWIDGET_H

#include <QVTKRenderWidget.h>

#include <vtkOBJReader.h>
#include <vtkPLYReader.h>
#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkLookupTable.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkCellData.h>
#include <vtkPolyLine.h>
#include <vtkProperty.h>
#include <vtkPropAssembly.h>
#include <vtkScalarBarActor.h>

#include <vtkAxesActor.h>
#include <vtkCamera.h>
#include <vtkCaptionActor2D.h>
#include <vtkNamedColors.h>
#include <vtkTransform.h>

#include <vtkCallbackCommand.h>
#include <vtkRendererCollection.h>

#include <vtkCellPicker.h>

// #include <mycommand.h>
#include <vtkGenericDataObjectReader.h>

#include <cinolib/geometry/vec_mat.h>
#include <cinolib/meshes/polygonmesh.h>


class vtkMyCallback : public vtkCommand
{
public:
    static vtkMyCallback *New()
    {
        return new vtkMyCallback;
    }
    virtual void Execute(vtkObject *caller, unsigned long, void*)
    {
        vtkRenderWindow *renWin = reinterpret_cast<vtkRenderWindow*>(caller);
        int *size = renWin->GetSize();
        if(scalarBar != nullptr)
        {
            scalarBar->SetMaximumWidthInPixels(size[0] * 0.1); // Set the maximum width to 10% of the window width
            scalarBar->SetMaximumHeightInPixels(size[1] * 0.85); // Set the maximum height to 80% of the window height
        }
    }
    vtkScalarBarActor *scalarBar;
};

namespace Ui {

class genericVTKwidget;
}

class genericVTKwidget : public QVTKRenderWidget
{
    Q_OBJECT
    typedef QVTKRenderWidget Superclass;

public:
    explicit genericVTKwidget(QWidget *parent = nullptr);

    bool add_mesh(const cinolib::Polygonmesh<> &m, cinolib::Color &color, bool wireframe = true);
    bool update_mesh(const cinolib::Polygonmesh<> &m, cinolib::Color &color, const uint id);
#ifdef USE_GDAL
    bool add_gpkg(const std::string &filename);
#endif
    bool add_polyline(std::vector<cinolib::vec3d> vertex );
    vtkSmartPointer<vtkPropAssembly> create_prop_assembly();
    void init_prop_assembly();
    void add_prop_assembly();
    bool add_tris(const std::vector<unsigned int> &vi);
    vtkSmartPointer<vtkPolyData> get_polydata(size_t i){return polydata.at(i);}

    bool set_cell_color(std::vector<unsigned int> vi);

    void render_view();

    void render_axes();

    void set_show_axes(bool b) {show_axes = b;}

    void set_show_colorbar(bool b) {show_colorbar = b;}

    void set_bg_color(double r,double g, double b) { bgColor = {r,g,b};}
    void set_bg_gradient (const std::string color1, const std::string color2);

    void save_screenshot(const std::string filename, double scale);

    void draw_selected_objects_by_ids(std::vector<unsigned int> id);

    void clear ();

    vtkSmartPointer<vtkRenderer> renderer = nullptr;

public slots:
    void update_selection(std::vector<unsigned int> ids);

private:
    bool show_axes = false;
    bool show_colorbar = false;

    Ui::genericVTKwidget *ui;

    std::vector<vtkSmartPointer<vtkPolyDataMapper>> mapper;
    std::vector<vtkSmartPointer<vtkActor>> actor;
    std::vector<vtkSmartPointer<vtkPolyData>> polydata;  //vettore di mesh.
    // vtkSmartPointer<vtkPoints> points;  //TODO trasformarlo in vettore per associare più mesh. Non inizializzarlo fino a quando non servono i punti per disegnare le annotazioni.
    vtkSmartPointer<vtkPropAssembly> propAssembly;   //TODO trasformarlo in vettore per gestire più annotazioni, o annotazioni di tipo diverso.

    vtkLookupTable* lut;

    vtkMyCallback *myCallback ;

    double min_vertex[3];

    std::vector<double> bgColor = {.5, .5, .9};

    std::string bg_gradient1 = "antique_white";
    std::string bg_gradient2 = "light_grey";

    // vtkSmartPointer<vtkDataSetMapper> selectedMapper = nullptr;
    vtkSmartPointer<vtkActor> selectedActor = nullptr;

    //vtkSmartPointer<vtkCubeAxesActor> axes = nullptr;

signals:

};




#endif // GENERICVTKWIDGET_H
