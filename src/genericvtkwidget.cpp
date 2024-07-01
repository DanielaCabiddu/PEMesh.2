// #include "genericvtkwidget.h"

// #include <vtkActor.h>
// #include <vtkCell.h>
// #include <vtkCellData.h>
// #include <vtkCompositePolyDataMapper.h>
// #include <vtkDataSetAttributes.h>
// #include <vtkDoubleArray.h>
// #include <vtkLookupTable.h>
// #include <vtkMapper.h>
// #include <vtkMultiBlockDataSet.h>
// #include <vtkNew.h>
// #include <vtkPointData.h>
// #include <vtkPolyData.h>
// #include <vtkPolyDataMapper.h>
// #include <vtkPolygon.h>
// #include <vtkProperty.h>
// #include <vtkRegressionTestImage.h>
// #include <vtkRenderWindow.h>
// #include <vtkRenderWindowInteractor.h>
// #include <vtkRenderer.h>
// #include <vtkSmartPointer.h>
// #include <vtkTestUtilities.h>
// #include <vtkTextProperty.h>
// #include <vtkWindowToImageFilter.h>
// #include <vtkPNGWriter.h>

// //cube axes
// #include <vtkCubeAxesActor.h>
// #include <vtkNamedColors.h>


// #include "./ui_genericvtkwidget.h"

// // #ifdef USE_GDAL

// // #include "gdal.h"
// // //#include <vtkGDALVectorReader.h>
// // #include <vtkExtractPoints.h>
// // #include <vtkExtractEdges.h>
// // #include <ogrsf_frmts.h>


// // #endif


// #include <vtkInteractorStyleTrackballCamera.h>
// #include <vtkGenericOpenGLRenderWindow.h>
// // // Add the missing include directive for vtkCellPicker




// genericVTKwidget::genericVTKwidget(QWidget *parent)
//     : QVTKRenderWidget{parent},
//     ui(new Ui::genericVTKwidget)
// {
//     ui->setupUi(this);

// //     min_vertex[0] = DBL_MAX;
// //     min_vertex[1] = DBL_MAX;
// //     min_vertex[2] = DBL_MAX;


//     vtkObject::GlobalWarningDisplayOff();

//     renderer = vtkSmartPointer<vtkRenderer>::New();

// //     // renderer->SetBackground(bgColor[0], bgColor[1], bgColor[2]);
// //     set_bg_gradient(bg_gradient1, bg_gradient2);
// //     renderer->ResetCamera();


//     auto format = QVTKRenderWidget::defaultFormat();
// #if defined(Q_OS_WIN)
//     // On Windows we use a compatibility profile to work around
//     // https://gitlab.kitware.com/vtk/vtk/issues/17572
//     //
//     // See also our Discourse post at
//     // https://discourse.vtk.org/t/problem-in-vtk-8-2-with-defaultformat-and-qvtkopenglwidget-on-windows-10-intel
//     format.setProfile(QSurfaceFormat::CompatibilityProfile);
// #endif
// #if defined(Q_OS_MACOS)
//     this->setAttribute(Qt::WA_AcceptTouchEvents, false);
// #endif
//     setFormat(format);

//     //setFormat(QVTKOpenGLWidget::defaultFormat());   //workaround to fix freezing on hp and dell
//     setEnableHiDPI(true);

//     vtkSmartPointer<vtkGenericOpenGLRenderWindow> win = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();

//     setRenderWindow(win);


//     // Setup the background gradient
//     set_bg_gradient(bg_gradient1, bg_gradient2);


//     renderWindow()->AddRenderer(renderer);

//     // Crea un nuovo interattore di finestra di rendering

//     renderWindow()->GetInteractor()->Initialize();


// //     // myCommand = MyCommand::New();
// //     // myCommand->vProj = this->vProj; // Set the pointer to your class
// //     // myCommand->SetDefaultRenderer(renderer);
// //     // renderWindow()->GetInteractor()->SetInteractorStyle(myCommand);
//     renderWindow()->GetInteractor()->Start();
// }

// void genericVTKwidget::clear ()
// {

//     if (actor.size()>0)
//         for (int a= actor.size()-1; a >= 0; a--)
//             renderer->RemoveActor(renderer->GetActors()->GetLastActor());

//     renderer->RemoveAllViewProps();
//     renderer->RemoveAllObservers();

//     polydata.clear();
//     actor.clear();
//     mapper.clear();

//     renderer->ResetCamera();
//     renderer->GetRenderWindow()->Render();
// }

// // /**
// //  * @brief Adds a mesh to the VTK scene.
// //  *
// //  * @param filename The name of the file containing the mesh data.
// //  *
// //  * @return `true` if the mesh was added successfully, `false` otherwise.
// //  */
// bool genericVTKwidget::add_mesh(const cinolib::Polygonmesh<> &m, cinolib::Color &color, bool wireframe)
// {
//     vtkNew<vtkPoints> points;
//     vtkNew<vtkCellArray> polys;

//     for (uint vid=0; vid < m.num_verts(); vid++)
//     {
//         points->InsertNextPoint(m.vert(vid).x(), m.vert(vid).y(), m.vert(vid).z());
//     }

//     for (uint pid=0; pid < m.num_polys(); pid++)
//     {
//         vtkNew<vtkPolygon> polygon;
//         polygon->GetPointIds()->SetNumberOfIds(m.poly_verts(pid).size());

//         uint c=0;
//         for (uint vid : m.poly_verts_id(pid))
//         {
//             polygon->GetPointIds()->SetId(c++, vid);
//         }

//         polys->InsertNextCell(polygon);
//     }

//     // Create a polydata object
//     vtkNew<vtkPolyData> pldata;

//     // Add the geometry and topology to the polydata
//     pldata->SetPoints(points);
//     pldata->SetPolys(polys);

//     pldata->BuildCells();
//     pldata->BuildLinks();

//     polydata.push_back(pldata);

//     // BUILD LOOKUP TABLE

//     // vtkSmartPointer<vtkCellArray> triangles = polydata.at(0)->GetPolys();
//     vtkSmartPointer<vtkLookupTable> lut = vtkSmartPointer<vtkLookupTable>::New();
//     lut->SetNumberOfTableValues(polydata.at(polydata.size()-1)->GetNumberOfCells());
//     lut->Build();

//     // Create a color array
//     vtkSmartPointer<vtkUnsignedCharArray> colors = vtkSmartPointer<vtkUnsignedCharArray>::New();
//     colors->SetNumberOfComponents(3);
//     colors->SetName("Colors");
//     colors->SetNumberOfTuples(polydata.at(polydata.size()-1)->GetNumberOfCells());
//     //  triangles->InitTraversal();
//     //  vtkIdType npts;
//     //  const vtkIdType *pts;
//     for(int i =0; i< colors->GetNumberOfTuples(); i++)
//     {
//         colors->SetTuple3(i, /*m.poly_data(i).*/color.r * 255, /*m.poly_data(i).*/color.g * 255, /*m.poly_data(i).*/color.b * 255);  // Default color for other triangles
//     }

//     // Add the color array to your polydata
//     polydata.at(polydata.size()-1)->GetCellData()->SetScalars(colors);

//     /**
//      * @brief Creates a VTK mapper and actor for the mesh.
//      */
//     mapper.push_back(vtkSmartPointer<vtkPolyDataMapper>::New());
//     mapper.at(mapper.size()-1)->SetInputData(polydata.at(polydata.size()-1));

//     actor.push_back(vtkSmartPointer<vtkActor>::New());
//     actor.at(mapper.size()-1)->SetMapper(mapper.at(mapper.size()-1));

//     // if (wireframe)
//         actor.at(mapper.size()-1)->GetProperty()->SetRepresentationToWireframe();

//     actor.at(mapper.size()-1)->GetProperty()->SetEdgeVisibility(true);
//         actor.at(mapper.size()-1)->GetProperty()->SetEdgeColor(0,0,0);

//     /**
//      * @brief Adds the actor to the scene and sets the background color.
//      */
//     renderer->AddActor(actor.at(mapper.size()-1));
//     set_bg_gradient(bg_gradient1, bg_gradient2);

//     /**
//      * @brief Resets the camera, renders the scene, and shows the rendering window.
//      */
//     renderer->ResetCamera();

//     // vtkSmartPointer<MyInteractorStyle> style = vtkSmartPointer<MyInteractorStyle>::New();
//     // renderWindow()->GetInteractor()->SetInteractorStyle(style);
//     // vtkSmartPointer<vtkInteractorStyleTrackballCamera> trackballCamera =  vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();

//     // renderWindow()->GetInteractor()->SetInteractorStyle(trackballCamera);


//     // renderWindow()->GetInteractor()->Initialize();
//     // renderWindow()->GetInteractor()->SetRotation(0);

//     // renderWindow()->GetInteractor()->Start();

//     renderer->GetRenderWindow()->Render();

//     // points = polydata.at(0)->GetPoints();

//     return true;
// }

// bool genericVTKwidget::update_mesh(const cinolib::Polygonmesh<> &m, cinolib::Color &color, const uint id)
// {
//     vtkNew<vtkPoints> points;
//     vtkNew<vtkCellArray> polys;

//     for (uint vid=0; vid < m.num_verts(); vid++)
//     {
//         points->InsertNextPoint(m.vert(vid).x(), m.vert(vid).y(), m.vert(vid).z());
//     }

//     for (uint pid=0; pid < m.num_polys(); pid++)
//     {
//         polys->InsertNextCell(m.poly_verts(pid).size());

//         for (uint vid : m.poly_verts_id(pid))
//             polys->InsertCellPoint(vid);
//     }

//     // Create a polydata object
//     vtkNew<vtkPolyData> pldata;

//     // Add the geometry and topology to the polydata
//     pldata->SetPoints(points);
//     pldata->SetPolys(polys);

//     pldata->BuildCells();
//     pldata->BuildLinks();

//     polydata.at(id) = pldata;

//     // BUILD LOOKUP TABLE

//     // vtkSmartPointer<vtkCellArray> triangles = polydata.at(0)->GetPolys();
//     vtkSmartPointer<vtkLookupTable> lut = vtkSmartPointer<vtkLookupTable>::New();
//     lut->SetNumberOfTableValues(polydata.at(id)->GetNumberOfCells());
//     lut->Build();

//     // Create a color array
//     vtkSmartPointer<vtkUnsignedCharArray> colors = vtkSmartPointer<vtkUnsignedCharArray>::New();
//     colors->SetNumberOfComponents(3);
//     colors->SetName("Colors");
//     colors->SetNumberOfTuples(polydata.at(id)->GetNumberOfCells());
//     //  triangles->InitTraversal();
//     //  vtkIdType npts;
//     //  const vtkIdType *pts;
//     for(int i =0; i< colors->GetNumberOfTuples(); i++)
//     {
//         colors->SetTuple3(i, color.r * 255, color.g * 255, color.b * 255);  // Default color for other triangles
//     }

//     // Add the color array to your polydata
//     polydata.at(id)->GetCellData()->SetScalars(colors);

//     /**
//      * @brief Creates a VTK mapper and actor for the mesh.
//      */
//     // mapper.push_back(vtkSmartPointer<vtkPolyDataMapper>::New());
//     mapper.at(id)->SetInputData(polydata.at(id));

//     // actor.push_back(vtkSmartPointer<vtkActor>::New());
//     actor.at(id)->SetMapper(mapper.at(id));
//     actor.at(id)->GetProperty()->SetRepresentationToWireframe();

//     /**
//      * @brief Adds the actor to the scene and sets the background color.
//      */
//     // renderer->AddActor(actor.at(mapper.size()-1));
//     // renderer->SetBackground(.5, .9, .5); // Background color green
//     set_bg_gradient(bg_gradient1, bg_gradient2);

//     /**
//      * @brief Resets the camera, renders the scene, and shows the rendering window.
//      */
//     renderer->ResetCamera();

//     // vtkSmartPointer<MyInteractorStyle> style = vtkSmartPointer<MyInteractorStyle>::New();
//     // renderWindow()->GetInteractor()->SetInteractorStyle(style);
//     // vtkSmartPointer<vtkInteractorStyleTrackballCamera> trackballCamera =  vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();

//     // renderWindow()->GetInteractor()->SetInteractorStyle(trackballCamera);


//     // renderWindow()->GetInteractor()->Initialize();
//     // renderWindow()->GetInteractor()->SetRotation(0);

//     // renderWindow()->GetInteractor()->Start();

//     renderer->GetRenderWindow()->Render();

//     // points = polydata.at(0)->GetPoints();

//     return true;
// }

// bool genericVTKwidget::add_polyline(std::vector<cinolib::vec3d> vertex)
// {

//     // Create the geometry of a point (the coordinate)
//     vtkNew<vtkPoints> points;

//     // Create the topology of the point (a vertex)
//     vtkNew<vtkCellArray> vertices;
//     // We need an an array of point id's for InsertNextCell.
//     vtkIdType pid[4];

//     uint counter=0;

//     for(cinolib::vec3d pp : vertex)
//     {
//         const double p[3] = {pp.x(), pp.y(), pp.z()};
//         pid[counter] = points->InsertNextPoint(p);
//         vertices->InsertNextCell(counter, pid);
//         counter++;
//     }

//     // Create a polydata object
//     vtkNew<vtkPolyData> point;

//     // Set the points and vertices we created as the geometry and topology of the
//     // polydata
//     point->SetPoints(points);
//     point->SetVerts(vertices);

//     vtkSmartPointer<vtkPolyLine> polyLine = vtkSmartPointer<vtkPolyLine>::New();
//     polyLine->GetPointIds()->SetNumberOfIds(points->GetNumberOfPoints());
//     for(unsigned int i = 0; i<points->GetNumberOfPoints(); i++)
//     {
//         polyLine->GetPointIds()->SetId(i,i);
//     }

//     vtkSmartPointer<vtkCellArray> cells = vtkSmartPointer<vtkCellArray>::New();
//     cells->InsertNextCell(polyLine);

//     vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
//     polyData->SetPoints(points);
//     polyData->SetLines(cells);
//     vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
//     mapper->SetInputData(polyData);

//     vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
//     actor->SetMapper(mapper);

//     actor->GetProperty()->SetColor(0.0, 0.0, 1.0);
//     renderer->AddActor(actor);
//     //    if(propAssembly != nullptr)
//     //        propAssembly->AddPart(actor);

//     return true;
// }

// // vtkSmartPointer<vtkPropAssembly> genericVTKwidget::create_prop_assembly()
// // {
// //     return vtkSmartPointer<vtkPropAssembly>::New();
// // }

// // void genericVTKwidget::init_prop_assembly()
// // {
// //     propAssembly = vtkSmartPointer<vtkPropAssembly>::New();
// // }

// // void genericVTKwidget::add_prop_assembly()
// // {
// //     renderer->AddViewProp(propAssembly);
// // }

// // bool genericVTKwidget::add_tris(const std::vector<unsigned int> &vi)
// // {
// //     vtkSmartPointer<vtkCellArray> cells = vtkSmartPointer<vtkCellArray>::New();

// //     vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
// //     // Iterare su tutte le celle in polyData
// //     for(uint i = 0; i < vi.size(); i++)
// //     {
// //         vtkCell* cell = polydata.at(0)->GetCell(vi.at(i));

// //         cells->InsertNextCell(cell);

// //     }

// //     polyData->SetPoints(polydata.at(0)->GetPoints());
// //     polyData->SetPolys(cells);

// //     vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
// //     mapper->SetInputData(polyData);

// //     vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
// //     actor->SetMapper(mapper);

// //     actor->GetProperty()->SetColor(0.0, 1.0, 0.0);

// //     if(propAssembly != nullptr)
// //         propAssembly->AddPart(actor);
// //     return true;
// // }

// // bool genericVTKwidget::set_cell_color(std::vector<unsigned int> vi)
// // {

// //     for(unsigned int i :vi)
// //         colors->SetTuple3(i,255, 0, 0);  // Default color for other triangles



// //     // Add the color array to your polydata
// //     polydata.at(0)->GetCellData()->SetScalars(colors); // TODO gestire mesh multiple
// //     renderer->ResetCamera();
// //     return true;
// // }

// // void genericVTKwidget::render_view()
// // {
// //     renderer->RemoveAllViewProps();

// //     set_bg_gradient(bg_gradient1, bg_gradient2);

// //     // renderer->SetBackground(bgColor[0], bgColor[1], bgColor[2]); // Background color green

// //     if(vProj != nullptr)
// //         myCommand->vProj = this->vProj;
// //     for(uint i =0; i < vProj->vList_size();i++)
// //     {
// //         viewer_project::view_element *v_el = vProj->get_vElement(i);
// //         if(v_el->mesh_is_visible)
// //         {


// //             vtkSmartPointer<vtkUnsignedCharArray> colors = vtkSmartPointer<vtkUnsignedCharArray>::New();
// //             colors->SetNumberOfComponents(3);
// //             colors->SetName("Colors");
// //             colors->SetNumberOfTuples(v_el->polydata->GetNumberOfCells());
// //             //  triangles->InitTraversal();
// //             //  vtkIdType npts;
// //             //  const vtkIdType *pts;
// //             for(size_t in =0; in< colors->GetNumberOfTuples(); in++)
// //             {
// //                 colors->SetTuple3(in,v_el->color.at(0),v_el->color.at(1), v_el->color.at(2));  // Default color for other triangles

// //             }
// //             bool no_shown = true;
// //             std::string title="";
// //             for (const auto& el : v_el->v_list)
// //             {
// //                 if(el->is_visible)
// //                 {
// //                     if (typeid(*el) == typeid( scalar_element ))
// //                     {
// //                         scalar_element *element =  dynamic_cast<scalar_element*>(el);
// //                         vtkDoubleArray* scalars = vtkDoubleArray::New();

// //                         if (element->scalars.size() == v_el->polydata->GetNumberOfPoints())
// //                             colors->SetNumberOfTuples(v_el->polydata->GetNumberOfPoints());

// //                         // Calcolo del valore locale per tutti i punti
// //                         double min=DBL_MAX, max = -min;
// //                         for (size_t index = 0; index < element->scalars.size(); index++)
// //                         {
// //                             if(element->scalars.at(index) < DBL_MAX && element->scalars.at(index) < min) min=element->scalars.at(index);
// //                             if(element->scalars.at(index) < DBL_MAX && element->scalars.at(index) > max) max=element->scalars.at(index);
// //                         }

// //                         for (size_t index = 0; index < element->scalars.size(); index++)
// //                         {
// //                             if(element->scalars.at(index) < DBL_MAX)
// //                                 scalars->InsertTuple1(index, (element->scalars.at(index)));
// //                             else
// //                                 scalars->InsertTuple1(index, nan(""));
// //                         }
// //                         lut = vtkLookupTable::New();

// //                         lut->SetHueRange(0, 0);
// //                         lut->SetSaturationRange(0, 0);
// //                         lut->SetValueRange(max, min);
// //                         lut->SetNumberOfColors(256);
// //                         lut->SetHueRange(0.0, 0.667);

// //                         lut->Build();

// //                         // Aggiunta degli scalari
// //                         if (element->scalars.size() == v_el->polydata->GetNumberOfCells())
// //                             v_el->polydata->GetCellData()->SetScalars(scalars);
// //                         else
// //                             v_el->polydata->GetPointData()->SetScalars(scalars);

// //                         v_el->polydata->Modified();


// //                         v_el->mapper->SetInputData(v_el->polydata);

// //                         if (element->scalars.size() == v_el->polydata->GetNumberOfCells())
// //                             v_el->mapper->SetScalarModeToUseCellData();
// //                         else
// //                             v_el->mapper->SetScalarModeToUsePointData();

// //                         v_el->mapper->SetColorModeToMapScalars();
// //                         v_el->mapper->SetLookupTable(lut);
// //                         v_el->mapper->SetScalarRange(min,max);

// //                         //show the color bar according to luptable
// //                         //title = v_el->scalar_name.at(j);
// //                         no_shown = false;

// //                     }
// //                     else if (typeid(*el) == typeid( annot_element ))
// //                     {
// //                         annot_element *element =  dynamic_cast<annot_element*>(el);

// //                         uint j = 0;
// //                         //for(annot_element::annotations_vtk &a: element->ann_vtk)
// //                         annot_element::annotations_vtk a = element->ann_vtk;
// //                         if(element->annotations_are_visible && element->has_area)
// //                             colors = element->colors;
// //                         for(vtkSmartPointer<vtkActor> act: a.actor)
// //                         {
// //                             if(v_el->annotations_are_visible.at(j))
// //                             {
// //                                 act->GetProperty()->SetPointSize(5);
// //                                 renderer->AddActor(act);
// //                             }
// //                             else
// //                                 renderer->RemoveActor(act);
// //                         }
// //                         j++;

// //                         j = 0;

// //                         if(!element->color_mesh_by_scalar && element->annotations_are_visible)
// //                         {
// //                             v_el->polydata->GetCellData()->SetScalars(element->colors);

// //                             v_el->mapper->SetColorModeToDefault();
// //                             v_el->mapper->SetScalarModeToDefault();

// //                             v_el->polydata->Modified();

// //                             v_el->mapper->SetInputData(v_el->polydata);
// //                             no_shown = false;

// //                         }
// //                         else if(element->annotations_are_visible && element->color_mesh_by_scalar)
// //                         {
// //                             vtkDoubleArray* scalars = vtkDoubleArray::New();

// //                             // Calcolo del valore locale per tutti i punti
// //                             double min=DBL_MAX, max = -min;
// //                             for (size_t index = 0; index<v_el->scalars.at(j).size(); index++)
// //                             {
// //                                 if(element->scalars.at(index) < DBL_MAX && element->scalars.at(index) < min) min=element->scalars.at(index);
// //                                 if(element->scalars.at(index) < DBL_MAX && element->scalars.at(index) > max) max=element->scalars.at(index);
// //                             }

// //                             for (size_t index = 0; index < element->scalars.size(); index++)
// //                             {
// //                                 if(element->scalars.at(index) < DBL_MAX)
// //                                     scalars->InsertTuple1(index, (element->scalars.at(index)));
// //                                 else
// //                                     scalars->InsertTuple1(index, nan(""));

// //                             }
// //                             lut = vtkLookupTable::New();

// //                             lut->SetHueRange(0.2,1);
// //                             lut->SetRange(min,max);
// //                             lut->SetTableRange(min,max);

// //                             lut->SetAlphaRange(1,1);
// //                             lut->SetSaturationRange(1,1);
// //                             lut->SetNanColor(v_el->color[0],v_el->color[1],v_el->color[2], 1);


// //                             lut->Build();

// //                             // Aggiunta degli scalari
// //                             v_el->polydata->GetCellData()->SetScalars(scalars);
// //                             v_el->polydata->Modified();


// //                             v_el->mapper->SetInputData(v_el->polydata);
// //                             v_el->mapper->SetScalarModeToUseCellData();
// //                             v_el->mapper->SetColorModeToMapScalars();
// //                             v_el->mapper->SetLookupTable(lut);
// //                             v_el->mapper->SetScalarRange(min,max);


// //                             //show the color bar according to luptable
// //                             title = element->scalar_name;
// //                             no_shown = false;

// //                         }



// //                         no_shown = false;
// //                     }
// //                 }
// //             }


// //             if(no_shown)
// //             {
// //                 v_el->polydata->GetCellData()->SetScalars(colors);
// //                 v_el->polydata->Modified();
// //                 v_el->mapper->SetInputData(v_el->polydata);
// //                 v_el->mapper->SetColorModeToDefault();
// //                 v_el->mapper->SetScalarModeToDefault();
// //                 lut = nullptr;
// //             }

// //             if(show_colorbar && lut != nullptr)
// //             {
// //                 vtkNew<vtkScalarBarActor> scalarBar;
// //                 scalarBar->SetLookupTable(v_el->mapper->GetLookupTable());
// //                 scalarBar->GetLabelTextProperty()->SetColor(0.0, 0.0, 0.0);
// //                 scalarBar->GetLabelTextProperty()->SetFontSize(11);
// //                 scalarBar->SetTitle(title.c_str());
// //                 scalarBar->GetTitleTextProperty()->SetFontSize(12);
// //                 int *size = renderWindow()->GetSize();
// //                 scalarBar->SetMaximumWidthInPixels(size[0] * 0.1); // Set the maximum width to 10% of the window width
// //                 scalarBar->SetMaximumHeightInPixels(size[1] * 0.85); // Set the maximum height to 80% of the window height

// //                 //myCallback->scalarBar =scalarBar;
// //                 renderer->AddActor2D(scalarBar);
// //             }

// //             v_el->actor->SetMapper(v_el->mapper);
// //             v_el->actor->Modified();
// //             renderer->AddActor(v_el->actor);



// //             /**
// //      * @brief Adds the actor to the scene and sets the background color.
// //      */



// //             //add_prop_assembly();

// //             /**
// //      *
// //      * @brief Resets the camera, renders the scene, and shows the rendering window.
// //      */

// //         }

// //         if(show_axes)
// //             render_axes();

// //         const auto actors = renderer->GetActors();
// //         actors->InitTraversal();
// //         for (vtkIdType i = 0; i < actors->GetNumberOfItems(); ++i)
// //         {
// //             actors->GetNextActor()->Modified();
// //         }




// //         // Render the current window
// //         renderer->Render();


// //         //renderer->ResetCamera();
// //         this->show();


// //     }
// // }

// // void genericVTKwidget::render_axes()
// // {
// //     vtkNew<vtkNamedColors> colors;


// //     vtkColor3d axis1Color = colors->GetColor3d("slate_grey_dark");
// //     vtkColor3d axis2Color = colors->GetColor3d("slate_grey_dark");
// //     vtkColor3d axis3Color = colors->GetColor3d("slate_grey_dark");


// //     vtkActorCollection* actors = renderer->GetActors();
// //     actors->InitTraversal();

// //     double globalBounds[6] = {VTK_DOUBLE_MAX, VTK_DOUBLE_MIN, VTK_DOUBLE_MAX, VTK_DOUBLE_MIN, VTK_DOUBLE_MAX, VTK_DOUBLE_MIN};

// //     for(vtkIdType i = 0; i < actors->GetNumberOfItems(); i++)
// //     {
// //         vtkActor* actor = actors->GetNextActor();
// //         double bounds[6];
// //         actor->GetBounds(bounds);

// //         for (int j = 0; j < 3; ++j)
// //         {
// //             globalBounds[2*j]   = std::min(globalBounds[2*j], bounds[2*j]);   // minima
// //             globalBounds[2*j+1] = std::max(globalBounds[2*j+1], bounds[2*j+1]); // maxima
// //         }
// //     }


// //     vtkNew<vtkCubeAxesActor> cubeAxesActor;
// //     cubeAxesActor->SetUseTextActor3D(1);
// //     cubeAxesActor->SetBounds(globalBounds);
// //     cubeAxesActor->SetCamera(renderer->GetActiveCamera());
// //     cubeAxesActor->GetTitleTextProperty(0)->SetColor(axis1Color.GetData());
// //     cubeAxesActor->GetTitleTextProperty(0)->SetFontSize(48);
// //     cubeAxesActor->GetLabelTextProperty(0)->SetColor(axis1Color.GetData());

// //     cubeAxesActor->GetTitleTextProperty(1)->SetColor(axis2Color.GetData());
// //     cubeAxesActor->GetLabelTextProperty(1)->SetColor(axis2Color.GetData());

// //     cubeAxesActor->GetTitleTextProperty(2)->SetColor(axis3Color.GetData());
// //     cubeAxesActor->GetLabelTextProperty(2)->SetColor(axis3Color.GetData());

// //     cubeAxesActor->DrawXGridlinesOn();
// //     cubeAxesActor->DrawYGridlinesOn();
// //     cubeAxesActor->DrawZGridlinesOn();
// // #if VTK_MAJOR_VERSION == 6
// //     cubeAxesActor->SetGridLineLocation(VTK_GRID_LINES_FURTHEST);
// // #endif
// // #if VTK_MAJOR_VERSION > 6
// //     cubeAxesActor->SetGridLineLocation(cubeAxesActor->VTK_GRID_LINES_FURTHEST);
// // #endif

// //     cubeAxesActor->XAxisMinorTickVisibilityOff();
// //     cubeAxesActor->YAxisMinorTickVisibilityOff();
// //     cubeAxesActor->ZAxisMinorTickVisibilityOff();

// //     cubeAxesActor->SetFlyModeToStaticEdges();


// //     // The axes are positioned with a user transform
// //     // axes->SetUserTransform(transform);

// //     // properties of the axes labels can be set as follows
// //     // this sets the x axis label to red
// //     // axes->GetXAxisCaptionActor2D()->GetCaptionTextProperty()->SetColor(
// //     //  colors->GetColor3d("Red").GetData());

// //     // the actual text of the axis label can be changed:
// //     // axes->SetXAxisLabelText("test");



// //     renderer->AddActor(cubeAxesActor);
// //     renderer->Render();
// // }

// void genericVTKwidget::set_bg_gradient(const std::string color1, const std::string color2)
// {
//     vtkNew<vtkNamedColors> colors;

//     renderer->GradientBackgroundOn();
//     renderer->SetBackground(colors->GetColor3d(color1.c_str()).GetData());
//     renderer->SetBackground2(colors->GetColor3d(color2.c_str()).GetData());

//     bg_gradient1 = color1;
//     bg_gradient2 = color2;
// }

// // void genericVTKwidget::save_screenshot(const std::string filename, double scale)
// // {
// //     vtkSmartPointer<vtkWindowToImageFilter> windowToImageFilter =
// //         vtkSmartPointer<vtkWindowToImageFilter>::New();

// //     //render_view();
// //     //renderer->SetBackground(1,1,1);
// //     windowToImageFilter->SetInput(renderWindow());
// //     windowToImageFilter->SetScale(scale); //set the resolution of the output image (3 times the current resolution of vtk render window)
// //     windowToImageFilter->SetInputBufferTypeToRGB(); //also record the alpha (transparency) channel
// //     windowToImageFilter->ReadFrontBufferOn(); // read from the back buffer
// //     windowToImageFilter->Update();

// //     vtkSmartPointer<vtkPNGWriter> writer =
// //         vtkSmartPointer<vtkPNGWriter>::New();
// //     writer->SetFileName(filename.c_str());
// //     writer->SetInputConnection(windowToImageFilter->GetOutputPort());
// //     writer->Write();

// //     renderWindow()->Render();
// //     //  renderer->ResetCamera();
// //     //GetRenderWindow()->ResetCamera();
// //     // renderWindowInteractor->Start();

// // }

// // void genericVTKwidget::draw_selected_objects_by_ids(std::vector<unsigned int> id)
// // {
// //     selectedMapper = vtkSmartPointer<vtkDataSetMapper>::New();
// //     selectedActor = vtkSmartPointer<vtkActor>::New();
// //     vtkNew<vtkNamedColors> colors;

// //     vtkNew<vtkIdTypeArray> ids;
// //     ids->SetNumberOfComponents(1);
// //     for (auto i : id)
// //     {
// //         ids->InsertNextValue(i);
// //     }


// //     vtkNew<vtkSelectionNode> selectionNode;
// //     selectionNode->SetFieldType(vtkSelectionNode::CELL);
// //     selectionNode->SetContentType(vtkSelectionNode::INDICES);
// //     selectionNode->SetSelectionList(ids);

// //     vtkNew<vtkSelection> selection;
// //     selection->AddNode(selectionNode);

// //     vtkNew<vtkExtractSelection> extractSelection;
// //     extractSelection->SetInputData(0, vProj->get_selected_polydata());
// //     extractSelection->SetInputData(1, selection);
// //     extractSelection->Update();

// //     // In selection
// //     vtkNew<vtkUnstructuredGrid> selected;
// //     selected->ShallowCopy(extractSelection->GetOutput());

// //     std::cout << "Number of points in the selection: "
// //               << selected->GetNumberOfPoints() << std::endl;
// //     std::cout << "Number of cells in the selection : "
// //               << selected->GetNumberOfCells() << std::endl;
// //     selectedMapper->SetInputData(selected);
// //     selectedActor->SetMapper(selectedMapper);
// //     selectedActor->GetProperty()->EdgeVisibilityOn();
// //     selectedActor->GetProperty()->SetColor(
// //         colors->GetColor3d("Tomato").GetData());

// //     selectedActor->GetProperty()->SetLineWidth(3);

// //     renderer->AddActor(selectedActor);
// //     renderer->Render();

// // }

// void genericVTKwidget::update_selection(std::vector<unsigned int> ids)
// {
//     // draw_selected_objects_by_ids(ids);

//     renderWindow()->Render();
//     this->show();

// }
// // #ifdef USE_GDAL

// // /**
// //  * @brief Adds a mesh to the VTK scene.
// //  *
// //  * @param filename The name of the file containing the mesh data.
// //  *
// //  * @return `true` if the mesh was added successfully, `false` otherwise.
// //  */
// // bool genericVTKwidget::add_gpkg(const std::string &filename)
// // {

// //     // std::cout << "With GDAL ...." << std::endl;
// //     // // 1. Register all the format drivers that are desired
// //     // GDALAllRegister();

// //     // // 2. Open file
// //     // GDALDataset *poDS;
// //     // poDS = static_cast<GDALDataset*> (GDALOpenEx(filename.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL ));
// //     // if( poDS == NULL )
// //     // {
// //     //     std::cerr << "Error while loading GeoPackage file " << filename << std::endl;
// //     //     return false;
// //     // }

// //     // int numLayers = GDALDatasetGetLayerCount(poDS);
// //     // std::cout << "Number of layers: " << numLayers << std::endl;

// //     // ///////////////////////////

// //     // std::cout << "With VTK ...." << std::endl;

// //     // vtkSmartPointer<vtkPolyData> pldata;

// //     // std::cout << "Loading: " << filename << std::endl;

// //     // vtkSmartPointer<vtkGDALVectorReader> reader = vtkSmartPointer<vtkGDALVectorReader>::New();
// //     // reader->SetFileName(filename.c_str());
// //     // reader->AddFeatureIdsOn();

// //     // // Test layer information helpers
// //     // reader->UpdateInformation();

// //     // std::cout << "Loaded: " << filename << std::endl;

// //     // // Get the data

// //     // int nl = reader->GetNumberOfLayers();
// //     // std::cout << "# Layers: " << nl << std::endl;

// //     // // for (int i = 0; i < nl; ++i)
// //     // // {
// //     // //     reader->SetActiveLayer(i);
// //     // //     std::cout   << "Layer " << i << " Type " << reader->GetActiveLayerType()
// //     // //                 << " FeatureCount " << reader->GetActiveLayerFeatureCount() << "\n";
// //     // // }

// //     // reader->SetActiveLayer(0);

// //     // vtkSmartPointer<vtkMultiBlockDataSet> mbds = reader->GetOutput();
// //     // reader->Update();

// //     // std::cout << mbds->GetNumberOfBlocks() << std::endl;

// //     // vtkNew<vtkExtractPoints> points;
// //     // points->SetInputData(mbds);

// //     // points->Update();
// //     // // cerr << points->GetOutputDataObject(0)->GetClassName() << endl;
// //     // // points->GetOutputDataObject(0)->PrintSelf(std::cerr, vtkIndent(0));

// //     // vtkNew<vtkCompositePolyDataMapper> mapper;
// //     // mapper->SetInputDataObject(mbds);

// //     // vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
// //     // actor->SetMapper(mapper);
// //     // actor->GetProperty()->SetPointSize(10);

// //     // /**
// //     //  * @brief Adds the actor to the scene and sets the background color.
// //     //  */
// //     // renderer->AddActor(actor);
// //     // // renderer->SetBackground(.5, .9, .5); // Background color green


// //     // /**
// //     //  *
// //     //  * @brief Resets the camera, renders the scene, and shows the rendering window.
// //     //  */
// //     // renderer->ResetCamera();

// //     return true;
// // }

// // #endif
