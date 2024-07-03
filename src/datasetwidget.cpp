/********************************************************************************
*  This file is part of PEMesh                                                  *
*  Copyright(C) 2022: Daniela Cabiddu                                           *

*                                                                               *
*  Author(s):                                                                   *
*                                                                               *
*     Daniela Cabiddu (daniela.cabiddu@cnr.it)                                  *
*                                                                               *
*     Italian National Research Council (CNR)                                   *
*     Institute for Applied Mathematics and Information Technologies (IMATI)    *
*     Via de Marini, 6                                                          *
*     16149 Genoa,                                                              *
*     Italy                                                                     *
*                                                                               *
*                                                                               *
*                                                                               *
*  This program is free software: you can redistribute it and/or modify it      *
*  under the terms of the GNU General Public License as published by the        *
*  Free Software Foundation, either version 3 of the License, or (at your       *
*  option) any later version.                                                   *
*                                                                               *
*  This program is distributed in the hope that it will be useful, but          *
*  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY   *
*  or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for  *
*  more details.                                                                *
*                                                                               *
*  You should have received a copy of the GNU General Public License along      *
*  with this program. If not, see <https://www.gnu.org/licenses/>.              *
*                                                                               *
*********************************************************************************/

#include "datasetwidget.h"
#include "meshes/vem_elements.h"
#include "ui_datasetwidget.h"

#include "meshes/mirroring.h"
#include "optimization/mesh_agglomeration.h"
#include "optimization/metis_wrap.h"
#include "optimization/hierarchy.h"

#include <cinolib/sampling.h>
#include <cinolib/triangle_wrap.h>
#include <cinolib/tetgen_wrap.h>

#include "optimizedialog.h"
#include "addpolygondialog.h"
#include "parametricdatasetsettingsdialog.h"
#include "aggregatedialog.h"
#include "solversettingsdialog.h"
#include "sortgeometricqualitiesdialog.h"

#include <QFileDialog>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QPushButton>
#include <QString>


DatasetWidget::DatasetWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DatasetWidget)
{
    ui->setupUi(this);

    connect(ui->param_slider, SIGNAL(valueChanged(int)), this, SLOT(show_parametric_mesh(int)));

    ui->param_slider->hide();
    ui->mesh_number_label->hide();

    // ui->canvas->callback_mouse_press =
    //         [this](cinolib::GLcanvas* canvas, QMouseEvent* event){ this->add_polygon(canvas, event); };

    // ui->canvas->skip_default_mouse_press_handler = true;

    std::vector<cinolib::vec3d> verts;
    verts.push_back(cinolib::vec3d(0.0,0.0,0.0));
    verts.push_back(cinolib::vec3d(1.0,0.0,0.0));
    verts.push_back(cinolib::vec3d(1.0,1.0,0.0));
    verts.push_back(cinolib::vec3d(0.0,1.0,0.0));

    std::vector<std::vector<uint>> polys;
    std::vector<uint> poly;
    poly.push_back(0);
    poly.push_back(1);
    poly.push_back(2);
    poly.push_back(3);
    polys.push_back(poly);

    cinolib::Polygonmesh<> polymesh (verts, polys);

    // cinolib::Color c = cinolib::Color(0,0,0,1);
    ui->canvas->add_mesh(polymesh);

    // polymesh->updateGL();
    // ui->canvas->push_obj(polymesh, true);
    // ui->canvas->updateGL();

    // markers.resize(4);

    // for (uint i=0; i < markers.size(); i++)
    //     markers.at(i) = new Marker();

    // markers.at(0)->p3d = cinolib::vec3d(0,0,0);
    // markers.at(0)->label = "0,0";

    // markers.at(1)->p3d = cinolib::vec3d(1,0,0);
    // markers.at(1)->label = "1,0";

    // markers.at(2)->p3d = cinolib::vec3d(0,1,0);
    // markers.at(2)->label = "0,1";

    // markers.at(3)->p3d = cinolib::vec3d(1,1,0);
    // markers.at(3)->label = "1,0";

    // ui->canvas->push_marker(new Marker(*markers.at(0)));
    // ui->canvas->push_marker(new Marker(*markers.at(1)));
    // ui->canvas->push_marker(new Marker(*markers.at(2)));
    // ui->canvas->push_marker(new Marker(*markers.at(3)));

    QSizePolicy spRight(QSizePolicy::Preferred, QSizePolicy::Preferred);
    spRight.setHorizontalStretch(4);
    ui->canvas->setSizePolicy(spRight);

    ui->polygon_list->horizontalHeader()->setStretchLastSection(true);
    ui->polygon_list->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
      
    ui->aggregate_btn->setVisible(false);
    ui->groupBox_3->setVisible(false);
}

DatasetWidget::~DatasetWidget()
{
    delete ui;
}

void DatasetWidget::add_polygon (const SelectedPolyData selected_poly,
                                 const cinolib::vec2d &pos,
                                 const double rotation_angle,
                                 const double scale_factor)
{

    add_polygon(selected_poly, pos, false);

    cinolib::Polygonmesh<> *m = drawable_polys.at(drawable_polys.size()-1);

    std::cout << "BBOX CENTER BEFORE SCALING : " << m->bbox().center() << std::endl;

    //scale
    m->scale(scale_factor);
    elems_scale_factors.at(drawable_polys.size()-1) = scale_factor;

    std::cout << "BBOX CENTER BEFORE ROTATING : " << m->bbox().center() << std::endl;

    rotation_angles.at(drawable_polys.size()-1)->setValue(rotation_angle); // emits signal to rotate the polygon

    m->translate(cinolib::vec3d(pos.x(), pos.y(), 0.0) - m->bbox().center());

    // m->updateGL();

    std::cout << "BBOX CENTER : " << m->bbox().center() << std::endl;

    // ui->canvas->add_mesh(*m);
}

void DatasetWidget::add_polygon (QMouseEvent *event)
{
    if (!enable_add_polygon)
        return;

    // cinolib::vec3d proj;
    // cinolib::vec2i pos (event->pos().x(), event->pos().y());
    // canvas->unproject(pos, proj);
    // std::cout << proj.x() << " -- " << proj.y() << std::endl;

    // if (proj.x() < 1e-6 && proj.y() < 1e-6 ) return;

    // cinolib::vec2d pos2d (proj.x(), proj.y());

    // SelectedPolyData selected_poly = ask_polygon_class();

    // if (selected_poly.class_name.length() > 0)
    // {
    //     add_polygon(selected_poly, pos2d);

    //     ui->load_meshes_btn->setEnabled(false);
    // }
}

const SelectedPolyData DatasetWidget::ask_polygon_class() const
{
    AddPolygonDialog *dialog = new AddPolygonDialog();

    SelectedPolyData selected_poly;

    if (dialog->exec() == 1)
    {
        selected_poly.class_type = dialog->getClassIndex();
        selected_poly.class_name = dialog->getClassName();

        if (selected_poly.class_type == 8) /// Random
        {
            selected_poly.mesh = dialog->getMesh();
            selected_poly.filename = selected_poly.mesh->mesh_data().filename;
        }
        else
        if (selected_poly.class_type > 8)
        {
            QMessageBox *m = new QMessageBox ();
            m->setText("Polygon type not impemented yet."
                       "\n\nPlease choose a class inbetween Comb and Random.");
            m->exec();

            delete m;
            delete dialog;
            return selected_poly;
        }
    }

    delete dialog;
    return selected_poly;
}

void DatasetWidget::add_polygon (const SelectedPolyData selected_poly, const cinolib::vec2d &pos, const bool scale_to_fit)
{
    cinolib::Polygonmesh<> deformed;

    if (selected_poly.mesh != nullptr)
    {
        deformed = *selected_poly.mesh;

        AbstractVEMelement *elem = create_element(selected_poly.class_type, selected_poly.filename);

        elems.push_back(elem);
        elems_centers.push_back(deformed.bbox().center());
        elems_filename.push_back(deformed.mesh_data().filename);
    }
    else
    {
        uint class_type = className2classId.find(selected_poly.class_name)->second;

        AbstractVEMelement *elem = create_element(class_type);

        elems.push_back(elem);

        elems_centers.push_back(elem->base_elem.bbox().center());
        elems_filename.push_back("NONE");

        deformed = elem->deform(static_cast<float>(0.0));
    }

    elems_class_name.push_back(selected_poly.class_name);
    elems_class_types.push_back(selected_poly.class_type);
    elems_scale_factors.push_back(1.0);
    elems_rotation_angles.push_back(0.0);

    if (scale_to_fit)
    {
//        bool is_in = true;

//        do
//        {
//            is_in = true;

//            std::cout << deformed.bbox() << std::endl;

//            if (deformed.bbox().min.x() < 0.01 ||
//                deformed.bbox().min.y() < 0.01 ||
//                deformed.bbox().max.x() > 0.99 ||
//                deformed.bbox().max.y() > 0.99 )
//            {
//                is_in = false;
//            }

//            if (!is_in)
//            {
//                deformed.scale(0.9);
//                elems_scale_factors.at(elems_scale_factors.size()-1) *= 0.9;
//                std::cout << "Scaled : " << elems_scale_factors.at(elems_scale_factors.size()-1) << std::endl;
//            }
//        }
//        while (!is_in);

//        std::cout << deformed.bbox().center() << std::endl;

        double scale_factor = 1.0/(2*deformed.bbox().diag());

        elems_scale_factors.at(elems_scale_factors.size()-1) *=  scale_factor;
        deformed.scale( scale_factor );
    }


    // move to desired center
    cinolib::vec3d bb_center = deformed.bbox().center();
    cinolib::vec3d desired_center (pos.x(), pos.y(), 0.0);

    std::cout << "Original : " << bb_center << std::endl;
    std::cout << "Desired : " << desired_center << std::endl;

    deformed.translate(desired_center - bb_center);
    std::cout << deformed.bbox().center() << std::endl;

    elems_centers.at(elems_centers.size()-1) = deformed.bbox().center();

    // Generate model to be visualized
    cinolib::Polygonmesh<> * polymesh =  new cinolib::Polygonmesh<>(deformed.vector_verts(), deformed.vector_polys());

    drawable_polys.push_back(polymesh);

    ui->canvas->add_mesh(*polymesh);

    QWidget *scale_btn_box = new QWidget(this);
    QHBoxLayout *l = new QHBoxLayout(scale_btn_box);
    scale_btn_box->setLayout(l);

    QPushButton *plus_btn = new QPushButton(scale_btn_box);
    QPushButton *minus_btn = new QPushButton(scale_btn_box);

    plus_btn->setIcon(QIcon(":/icons/img/plus_icon.png"));
    minus_btn->setIcon(QIcon(":/icons/img/minus_icon.png"));

    plus_btn->setMinimumSize(QSize(17,17));
    minus_btn->setMinimumSize(QSize(17,17));

    connect(plus_btn, SIGNAL(clicked()), this, SLOT (polygon_zoom_in()));
    connect(minus_btn, SIGNAL(clicked()), this, SLOT (polygon_zoom_out()));

    l->addWidget(plus_btn);
    l->addWidget(minus_btn);

    QDoubleSpinBox *rotation_angle_dsb = new QDoubleSpinBox();
    rotation_angle_dsb->setValue(0.0);
    rotation_angle_dsb->setSingleStep(0.01);
    rotation_angle_dsb->setMinimum(-2*M_PI);
    rotation_angle_dsb->setMaximum(2*M_PI);

    connect(rotation_angle_dsb, SIGNAL(valueChanged(double)), this, SLOT(rotate_polygon(double)));

    QDoubleSpinBox *x_dsb = new QDoubleSpinBox();
    x_dsb->setDecimals(6);
    x_dsb->setSingleStep(1e-2);
    x_dsb->setMaximum(1.0);
    x_dsb->setValue(elems_centers.at(elems_centers.size()-1).x());

    QDoubleSpinBox *y_dsb = new QDoubleSpinBox();
    y_dsb->setDecimals(6);
    y_dsb->setSingleStep(1e-2);
    y_dsb->setMaximum(1.0);
    y_dsb->setValue(elems_centers.at(elems_centers.size()-1).y());

    connect(x_dsb, SIGNAL(valueChanged(double)), this, SLOT(move_polygon_on_x(double)));
    connect(y_dsb, SIGNAL(valueChanged(double)), this, SLOT(move_polygon_on_y(double)));

    QWidget *pos_widget = new QWidget (ui->polygon_list);
    QFormLayout *lw = new QFormLayout();
    pos_widget->setLayout(lw);

    lw->addRow(QString("X"), x_dsb);
    lw->addRow(QString("Y"), y_dsb);

    ui->polygon_list->insertRow ( ui->polygon_list->rowCount() );
    ui->polygon_list->setItem   ( ui->polygon_list->rowCount()-1, 0, new QTableWidgetItem(selected_poly.class_name.c_str()));
    ui->polygon_list->setCellWidget( ui->polygon_list->rowCount()-1, 1, pos_widget);
    ui->polygon_list->setCellWidget( ui->polygon_list->rowCount()-1, 2, scale_btn_box);
    ui->polygon_list->setCellWidget( ui->polygon_list->rowCount()-1, 3, rotation_angle_dsb);

    ui->polygon_list->horizontalHeader()->resizeSections(QHeaderView::Stretch);
    ui->polygon_list->resizeRowsToContents();

    plus_buttons.push_back(plus_btn);
    minus_buttons.push_back(minus_btn);
    rotation_angles.push_back(rotation_angle_dsb);

    x_poses.push_back(x_dsb);
    y_poses.push_back(y_dsb);

    ui->generate_dataset_btn->setEnabled(true);

    std::cout << "Enabled" << std::endl;
    //ui->save_btn->setEnabled(true);
    ui->save_polys_btn->setEnabled(true);
}

void DatasetWidget::on_add_btn_clicked()
{
    SelectedPolyData selected_poly = ask_polygon_class();
    std::string class_name = selected_poly.class_name;

    if (class_name.length() == 0) return;

    add_polygon(selected_poly, cinolib::vec2d(0.5, 0.5));

    ui->load_meshes_btn->setEnabled(false);
}

void DatasetWidget::polygon_zoom_in(cinolib::Polygonmesh<> *m)
{
    m->scale(1.0 + (1.0 - minus_scale_factor));
    // m->updateGL();
}

void DatasetWidget::polygon_zoom_out(cinolib::Polygonmesh<> *m)
{
    m->scale(minus_scale_factor);
    // m->updateGL();
}

void DatasetWidget::polygon_zoom_in()
{
    QObject* obj = sender();

    for (uint i=0; i < plus_buttons.size(); i++)
    {
        if (obj == plus_buttons.at(i))
        {
            polygon_zoom_in(drawable_polys.at(i));

            ui->canvas->update_mesh(*drawable_polys.at(i), i+1);

            elems_scale_factors.at(i) /= minus_scale_factor;

            break;
        }
    }
}

void DatasetWidget::polygon_zoom_out()
{
    QObject* obj = sender();

    for (uint i=0; i < minus_buttons.size(); i++)
    {
        if (obj == minus_buttons.at(i))
        {
            polygon_zoom_out(drawable_polys.at(i));

            cinolib::Color c = cinolib::Color (1,0,0,1);
            ui->canvas->update_mesh(*drawable_polys.at(i), i+1);

            elems_scale_factors.at(i) *= minus_scale_factor;

            break;
        }
    }
}

void DatasetWidget::rotate_polygon(double angle)
{
    QObject* obj = sender();

    for (uint i=0; i < rotation_angles.size(); i++)
    {
        if (obj == rotation_angles.at(i))
        {
            double a = - (elems_rotation_angles.at(i) - angle);

            drawable_polys.at(i)->rotate(cinolib::vec3d(0,0,1), a);

            ui->canvas->update_mesh(*drawable_polys.at(i), i+1);

            elems_rotation_angles.at(i) += a;

            break;
        }
    }
}

AbstractVEMelement * DatasetWidget::create_element(const uint elem_type, const std::string fname) const
{
    AbstractVEMelement *elem = nullptr;

    switch (elem_type)
    {
    case 0:
        elem = new CombTester();
        break;

    case 1:
        elem = new ConvexityTester();
        break;

    case 2:
        elem = new IsotropyTester();
        break;

    case 3:
        elem = new MazeTester();
        break;

    case 4:
        elem = new NumSidesTester();
        break;

    case 5:
        elem = new StarTester();
        break;

    case 6:
        elem = new UTester();
        break;

    case 7:
        elem = new ZetaTester();
        break;

    case 8:
        elem = new RandomTester(fname);
        break;

    default:
        break;

    }

    return  elem;
}

void DatasetWidget::clean_canvas()
{
    ui->canvas->clear();
    drawable_polys.clear();
}

void DatasetWidget::compute_geometric_metrics ()
{
    if (dataset->get_parametric_meshes().size() == 0)
        return;

    for (uint i=0; i < dataset->get_parametric_meshes().size(); i++)
    {
        cinolib::Polygonmesh<> m = *(dataset->get_parametric_mesh(i));

        // This is fundamental because many metrics are scale dependent.
        // With area normalization we fix this degree of freedom.
        //m.normalize_area();

        QApplication::setOverrideCursor(Qt::WaitCursor);

        std::string message = "\nMesh " + std::to_string(i);
        message += " : Computing polygon geometry metrics ... Please wait ...";

        ui->log_label->append(message.c_str());

        MeshMetrics m_metrics;
        compute_mesh_metrics(m, m_metrics);

        std::string message2 = " ... DONE";

        QCoreApplication::processEvents();

        ui->log_label->append(message2.c_str());

        QApplication::restoreOverrideCursor();

        dataset->add_parametric_mesh_metrics(m_metrics);
    }

    ui->add_btn->setEnabled(false);

    emit (computed_mesh_metrics());
}

void DatasetWidget::on_geom_qualities_btn_clicked()
{
    ui->geom_qualities_btn->setEnabled(false);

    compute_geometric_metrics();
}

void DatasetWidget::on_generate_dataset_btn_clicked()
{
    for (cinolib::Polygonmesh<> *m : drawable_polys)
    {
        if (m->bbox().min.x() < 0.0 ||
            m->bbox().min.y() < 0.0 ||
            m->bbox().max.x() > 1.0 ||
            m->bbox().max.y() > 1.0 )
        {
            QMessageBox *mb = new QMessageBox (this);

            mb->setText("ERROR: One or more polygons are out of the canvas. \n\nPlease edit and try again.");
            mb->exec();

            return;
        }
    }

    bool is_parametric = false;

    for (uint class_type : elems_class_types)
        if (class_type < 8)
        {
            is_parametric = true;
            break;
        }

    ParametricDatasetSettingsDialog *dialog = new ParametricDatasetSettingsDialog();

    dialog->set_is_parametric (is_parametric);

    if (dialog->exec() == 1)
    {
        QRect frame_rect = ui->frame->geometry();
        QRect frame2_rect = ui->frame_2->geometry();

//        std::string flags = "cq" + std::to_string(dialog->get_min_angle_value()) + "a";

        std::vector<float> sample_interval;
        uint n_samples = dialog->get_num_meshes();

        if (is_parametric && n_samples >= 2)
        {
            const float int_begin = 0.0;
            const float int_end = dialog->get_max_deformation_value();
            sample_interval = cinolib::sample_within_interval(int_begin, int_end, n_samples);
        }
        else
        {
            sample_interval.push_back(0.0);
            n_samples = 1;
        }

        std::vector<cinolib::Polygonmesh<> *> meshes_with_canvas;
        std::vector<float> t_values;

        for(float t : sample_interval)
        {
            std::vector<cinolib::Polygonmesh<> *> polys;

            for (uint e=0; e < elems.size(); e++)
            {
                AbstractVEMelement *elem = elems.at(e);

                cinolib::Polygonmesh<> deformed = elem->deform(t);

                deformed.scale(elems_scale_factors.at(e));
                deformed.rotate(cinolib::vec3d(0,0,1), elems_rotation_angles.at(e));

                cinolib::vec3d bb_center = deformed.bbox().center();
                cinolib::vec3d desired_center (elems_centers.at(e));

                for (uint vid=0; vid < deformed.num_verts(); vid++)
                {
                    deformed.vert(vid).x() = deformed.vert(vid).x() + (desired_center - bb_center).x();
                    deformed.vert(vid).y() = deformed.vert(vid).y() + (desired_center - bb_center).y();
                }

                deformed.update_bbox();

                cinolib::Polygonmesh<> * polymesh =  new cinolib::Polygonmesh<>(deformed.vector_verts(), deformed.vector_polys());

                std::cout << "Scaling : " << elems_scale_factors.at(e) << std::endl;
                std::cout << "Center : " << polymesh->bbox().center() << std::endl;

                //ui->canvas->push_marker(polymesh->bbox().center(), "c_" + std::to_string(t));

                polys.push_back(polymesh);
            }

            std::string triangle_flags;

            MAX_AREA_TYPES max_area_type = dialog->get_max_area_type();
            double max_area_value = dialog->get_max_area_value();

            if (max_area_type == BASED_ON_MIN_EDGE)
            {
                double area_b = cinolib::inf_double, min_e = cinolib::inf_double, min_angle = cinolib::inf_double;
                for (cinolib::Polygonmesh<> *m : polys)
                {
                    for(uint pid=0; pid<m->num_polys(); ++pid)
                        area_b = std::min(area_b, m->poly_area(pid));   //min area

                    for(uint eid=0; eid<m->num_edges(); ++eid)
                        min_e = std::min(min_e, m->edge_length(eid));   //min edge

                    for(uint pid=0; pid<m->num_polys(); ++pid)
                        for (uint vid : m->adj_p2v(pid))
                            min_angle = std::min(min_angle, m->poly_angle_at_vert(pid, vid, cinolib::RAD));    //min angle
                }
                double edge_b = min_e * min_e * sqrt(3) / 4;  //area of an equilateral triangle with edge min_e
                double angle_b = min_e * min_e * std::sin(min_angle);     //area of an isoscele triangle with angle min_angle

                double bound = std::min(edge_b, std::min(angle_b, area_b)); //area bound for triangles

                triangle_flags += "a" + std::to_string(bound);
            }
            else
            if (max_area_type == BASED_ON_AVG_POLY_DIAGONAL)
            {
                double delta = 0.0;
                for (cinolib::Polygonmesh<> *m : polys)
                    delta += m->bbox().diag();
                delta /= polys.size();

                triangle_flags += "a" + std::to_string(delta*delta*0.25);
            }
            else
            if (max_area_type == CONSTANT_USER_DEFINED)
            {
                triangle_flags += "a" + std::to_string(max_area_value);
            }

            double min_angle = dialog->get_min_angle_value();

            if (min_angle > 0.0)
                triangle_flags += "q" + std::to_string(min_angle);

            cinolib::Polygonmesh<> m = deform_with_canvas(polys, triangle_flags);
            cinolib::Polygonmesh<> *dm = new cinolib::Polygonmesh<> (m.vector_verts(), m.vector_polys());

            for (uint pid=dm->num_polys()-elems.size(); pid < dm->num_polys(); pid++)
            {
                for (uint eid : dm->adj_p2e(pid))
                    dm->edge_data(eid).flags.set(0, true);

                // dm->poly_data(pid).color = cinolib::Color::RED();
            }

            // dm->show_marked_edge(true);

            std::stringstream ss;
            ss << dm->bbox();

            meshes_with_canvas.push_back(dm);
            t_values.push_back(t);

            std::string message = "[" + std::to_string(dataset->get_parametric_meshes().size()) +
                                  "] T value: <b>" + std::to_string(t) + "</b> : " + ss.str() + "<br>";
            ui->log_label->append(message.c_str());
        }

        dataset->clean();

//        for (uint i : elems_class_types)
//            dataset->add_class_name(classPrefix.at(i));

        uint id = 0;

        for (cinolib::Polygonmesh<>* m : meshes_with_canvas)
        {
            dataset->add_parametric_mesh(m, static_cast<double>(t_values.at(id)), UINT_MAX);
            id++;
        }

        ui->param_slider->setMaximum(static_cast<int>(n_samples-1));

        if (n_samples == 1)
            ui->param_slider->setVisible(false);

        ui->log_label->append("Dataset completed.");
        ui->param_slider->show();
        ui->mesh_number_label->show();

        ui->add_btn->setEnabled(false);
        ui->generate_dataset_btn->setEnabled(false);

        ui->geom_qualities_btn->setEnabled(true);
        ui->optimize_btn->setEnabled(true);
        ui->save_btn->setEnabled(true);

        ui->load_polys_btn->setEnabled(false);

        show_parametric_mesh(0);

        ui->frame->setGeometry(frame_rect);
        ui->frame_2->setGeometry(frame2_rect);

        ui->polygon_list->setEnabled(false);

        enable_add_polygon = false;

        ui->aggregate_btn->setEnabled(true);
        ui->mirroring_btn->setEnabled(true);

        ui->highlight_polys_cb->setEnabled(true);

        // ui->canvas->callback_mouse_press =
        //         [](cinolib::GLcanvas* canvas, QMouseEvent* event){ };

        emit (computed_parametric_dataset());
    }

    delete dialog;
}

void DatasetWidget::show_parametric_mesh(int index)
{
    clean_canvas();

    cinolib::Polygonmesh<> *m = dataset->get_parametric_mesh(static_cast<uint>(index));

    ui->canvas->add_mesh(*m);

    reset_canvas = false;

    ui->mesh_number_label->setText(std::to_string(index).c_str());
}

void DatasetWidget::on_save_btn_clicked()
{
    QString dir;
    QDir d;

    do
    {
        dir = QFileDialog::getExistingDirectory(this, tr("Select Directory"),
                                                        "/tmp",
                                                        QFileDialog::ShowDirsOnly
                                                        | QFileDialog::DontResolveSymlinks);
        if (dir.isNull())
            return;

        d.setPath(dir);

        if (!d.isEmpty())
        {
            QMessageBox *m = new QMessageBox (this);

            m->setText("The folder is not empty."
                       "\n\nPlease select an empty folder.");

            m->exec();
        }
    }
    while (!d.isEmpty());

    dataset->save_on_disk(dir.toStdString());

    dataset_folder = dir.toStdString();

    std::string message = "Saved in " + dir.toStdString();
    ui->log_label->append(message.c_str());

    ui->add_btn->setEnabled(false);
    ui->generate_dataset_btn->setEnabled(false);
    ui->aggregate_btn->setEnabled(false);
    ui->mirroring_btn->setEnabled(false);

    emit (saved_in (dir.toStdString()));
}

//void DatasetWidget::on_sort_geom_qualities_btn_clicked()
//{
//    SortGeometricQualitiesDialog *dialog = new SortGeometricQualitiesDialog();

//    if (dialog->exec() == 1)
//    {
//        uint selected_id = static_cast<uint>(dialog->get_selected());

//        emit (sort_geometric_qualities(selected_id));
//    }

//    delete dialog;
//}

cinolib::Polygonmesh<> DatasetWidget::deform_with_canvas(const std::vector<cinolib::Polygonmesh<> *> &elems_polys, const std::string triangle_flags)
{
    std::vector<cinolib::vec2d> verts_in;
    std::vector<uint>  segs;

    uint vid_off = 0;
    std::vector<uint> vert_holes;

    for (uint i=0; i < elems_polys.size(); i++)
    {
        cinolib::Polygonmesh<> m = *elems_polys.at(i);

        for(uint vid=0; vid<m.num_verts(); ++vid)
        {
            verts_in.push_back(cinolib::vec2d(m.vert(vid).x(), m.vert(vid).y()));
            if (vid==0) vert_holes.push_back(vid_off);
        }

        for(uint eid=0; eid<m.num_edges(); ++eid)
        {
            segs.push_back(vid_off + m.edge_vert_id(eid,0));
            segs.push_back(vid_off + m.edge_vert_id(eid,1));
        }

        vid_off += m.num_verts();
    }

    // create a quad around the base element bounding box
    //
//    vec2d  bot_l = vec2d(m.bbox().min);
//    vec2d  top_r = vec2d(m.bbox().max);

    verts_in.push_back(cinolib::vec2d(0.0, 0.0));
    verts_in.push_back(cinolib::vec2d(1.0, 0.0));
    verts_in.push_back(cinolib::vec2d(1.0, 1.0));
    verts_in.push_back(cinolib::vec2d(0.0, 1.0));

    // create a hole in correspondence of the deformed element
    // (I'm taking as hole seed the centroid of one of the
    // triangles in the triangulation of the polygon)
    //
    std::vector<cinolib::vec2d> holes;

    for (uint i=0; i < elems_polys.size(); i++)
    {
        cinolib::Polygonmesh<> m = *elems_polys.at(i);
        std::vector<uint>  tmp = m.poly_tessellation(0);
        cinolib::vec3d sum = (m.vert(tmp.at(0)) + m.vert(tmp.at(1)) + m.vert(tmp.at(2)));
        cinolib::vec2d hole = cinolib::vec2d(sum.x(), sum.y())/3.0;
        holes.push_back(hole);

        std::stringstream ss;
        ss << hole;
        // ui->canvas->updateGL();
    }

    std::vector<cinolib::vec3d> verts_out;
    std::vector<uint> tris;
    //std::string flags("cq20.0a");
//    std::string t_flags = "cq20.0a" + std::to_string(delta*delta*0.25);
    std::string t_flags = "cq" + triangle_flags;
    triangle_wrap(verts_in, segs, holes, 0, t_flags.c_str(), verts_out, tris);

    cinolib::Polygonmesh<> m_with_canvas(verts_out, cinolib::polys_from_serialized_vids(tris,3));

    std::vector<uint> new_polys;

    std::cout << "elems_polys.size() : "<< elems_polys.size() << std::endl;
    std::cout << "holes : "<< holes.size() << std::endl;

    for (uint i=0; i < elems_polys.size(); i++)
    {
        // walk along boundary to fill holes with PEM elements
        uint start_v = vert_holes.at(i);
        uint curr_v  = vert_holes.at(i);
        std::vector<uint> poly;
        std::set<uint> visited;
        do
        {
            poly.push_back(curr_v);
            bool found = false;
            uint next_v = 0;
            for(uint eid : m_with_canvas.adj_v2e(curr_v))
            {
                if (found || !m_with_canvas.edge_is_boundary(eid)) continue;
                if(cinolib::DOES_NOT_CONTAIN(visited,eid))
                {
                    visited.insert(eid);
                    assert(found==false);
                    found=true;
                    next_v = m_with_canvas.vert_opposite_to(eid, curr_v);
                }
            }

            //assert(found==true);
            if (!found)
            {
                ui->log_label->append("<b><font color=\"red\"> ERROR: Polygons intersecting each other. </font></b>");
                return cinolib::Polygonmesh<>();
            }


            curr_v = next_v;
        }
        while(curr_v!=start_v);

        for (uint vid : poly)
            std::cout << m_with_canvas.vert(vid) << std::endl;

        uint pid = m_with_canvas.poly_add(poly);
        if(m_with_canvas.poly_verts_are_CCW(pid, m_with_canvas.poly_vert_id(pid, 0), m_with_canvas.poly_vert_id(pid, 1)))
        {
            m_with_canvas.poly_remove(pid);
            std::reverse(poly.begin(),poly.end());
            m_with_canvas.poly_add(poly);
        }

        // cinolib::Polygonmesh<> m_with_canvas__(verts_out, cinolib::polys_from_serialized_vids(poly,3));
        // m_with_canvas = m_with_canvas__;
        // new_polys.push_back(pid);
    }



    /////////

    // Aggregate triangles so that max point to point distance becomes stable

//    if (aggregate)
//    {
//        double max_pd = -inf_double;
//        //uint count_polys = new_polys.size();

//        if (!new_polys.empty())
//        {
//            for (uint pid=0; pid < new_polys.size(); pid++)
//            {
//                std::vector<cinolib::vec3d> points = m_with_canvas.poly_verts(new_polys.at(pid));

//                for(uint i=0;   i<points.size()-1; ++i)
//                for(uint j=i+1; j<points.size();   ++j)
//                {
//                    max_pd = std::max(max_pd, points.at(i).dist(points.at(j)));
//                }

//                m_with_canvas.poly_data(new_polys.at(pid)).flags.set(0,true);
//            }
//            aggregate_triangles(m_with_canvas, max_pd, 0);
//        }

//        //////// check

//        for (uint pid=0; pid < m_with_canvas.num_polys(); pid++)
//        {
//            std::vector<cinolib::vec3d> points = m_with_canvas.poly_verts(pid);
//            if (points.size() == 3) continue;

//            double d = -inf_double;

//            for(uint i=0;   i<points.size()-1; ++i)
//                for(uint j=i+1; j<points.size();   ++j)
//                    d = std::max(d, points.at(i).dist(points.at(j)));

//            if (d > max_pd) assert (false);
//        }

//    }

    return m_with_canvas;
}

void DatasetWidget::move_polygon_on_x(double new_pos)
{
    QObject* obj = sender();

    for (uint i=0; i < x_poses.size(); i++)
    {
        if (obj == x_poses.at(i))
        {
            cinolib::Polygonmesh<> *m = drawable_polys.at(i);
            cinolib::vec3d bb_center = m->bbox().center();

            for (uint vid=0; vid < m->num_verts(); vid++)
            {
                m->vert(vid).x() = m->vert(vid).x() + (new_pos - bb_center.x());
            }

            m->update_bbox();

            std::cout << m->bbox().center() << std::endl;

            elems_centers.at(i).x() = new_pos;

            ui->canvas->update_mesh(*m, i+1);

            break;
        }
    }
}

void DatasetWidget::move_polygon_on_y(double new_pos)
{
    QObject* obj = sender();

    for (uint i=0; i < y_poses.size(); i++)
    {
        if (obj == y_poses.at(i))
        {
            cinolib::Polygonmesh<> *m = drawable_polys.at(i);
            cinolib::vec3d bb_center = m->bbox().center();

            for (uint vid=0; vid < m->num_verts(); vid++)
            {
                m->vert(vid).y() = m->vert(vid).y() + (new_pos - bb_center.y());
            }

            m->update_bbox();

            std::cout << m->bbox().center() << std::endl;

            elems_centers.at(i).y() = new_pos;

            ui->canvas->update_mesh(*m, i+1);

            break;
        }
    }
}

void DatasetWidget::on_save_polys_btn_clicked()
{
    QString filters("Polygon Set (*.csv)");
    QString filename = QFileDialog::getSaveFileName(this, tr("Select CSV File"), "/tmp", filters);
    if (filename.isNull() || filename.isEmpty()) return;

    if (!filename.endsWith(".csv"))
        filename += ".csv";

    std::ofstream outfile;
    outfile.open(filename.toStdString().c_str());

    if (!outfile.is_open())
    {
        std::cerr << "ERROR" << std::endl;
        return;
    }

    for (uint r=0; r < static_cast<uint>(ui->polygon_list->rowCount()); r++)
    {
        outfile << elems_class_name.at(r) << " ";
        outfile << elems_filename.at(r) << " ";
        outfile << elems_centers.at(r).x() << " ";
        outfile << elems_centers.at(r).y() << " ";
        outfile << elems_rotation_angles.at(r) << " ";
        outfile << elems_scale_factors.at(r) << std::endl;
    }

    outfile.close();
}

void DatasetWidget::on_load_polys_btn_clicked()
{
    QString filters("Polygon Set (*.csv)");
    QString filename = QFileDialog::getOpenFileName(this, tr("Select CSV File"), "/tmp", filters);
    if (filename.isNull() || filename.isEmpty()) return;

    std::ifstream infile;
    infile.open(filename.toStdString().c_str());

    if (!infile.is_open())
    {
        std::cerr << "ERROR" << std::endl;
        return;
    }

    std::string class_name, fname;
    double x, y, rot, fact;

    while (infile >> class_name >> fname >> x >> y >> rot >> fact)
    {
        SelectedPolyData selected_poly;

        selected_poly.class_name = class_name;
        selected_poly.filename = fname;

        if (fname.compare("NONE") != 0)
        {
            selected_poly.mesh = new cinolib::Polygonmesh<> (fname.c_str());
            selected_poly.class_type = 8;
        }

        add_polygon(selected_poly, cinolib::vec2d(x,y), rot, fact);
    }

    infile.close();

    ui->load_meshes_btn->setEnabled(false);
}

void DatasetWidget::on_load_meshes_btn_clicked()
{
    QString dir;
    QDir d;

    do
    {
        dir = QFileDialog::getExistingDirectory(this, tr("Select Directory"),
                                                        "/tmp",
                                                        QFileDialog::ShowDirsOnly
                                                        | QFileDialog::DontResolveSymlinks);
        if (dir.isNull())
            return;

        d.setPath(dir);

        if (d.isEmpty())
        {
            QMessageBox *m = new QMessageBox (this);

            m->setText("The folder is empty."
                       "\n\nPlease select another folder.");

            m->exec();
        }
    }
    while (d.isEmpty());

    //// Load files in the selected folder

    QStringList mesh_files = d.entryList(QStringList() << "*.obj" << "*.OBJ" << "*.off" << "*.OFF", QDir::Files);
    QStringList node_ele_files = d.entryList(QStringList() << "*.node" << "*.ele", QDir::Files);


    if (mesh_files.empty())
    {
        if (d.isEmpty())
        {
            QMessageBox *m = new QMessageBox (this);

            m->setText("The folder has no .obj file."
                       "\n\nPlease select another folder.");

            m->exec();
            return;
        }
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);

    uint id = 1;
    for (QString &f : mesh_files)
    {
        const std::string filename = dir.toStdString() + QString(QDir::separator()).toStdString() + f.toStdString();

        std::string message = "[" + std::to_string(id) + "/" + std::to_string(mesh_files.size()) + "] ";
                    message += "Loading " + filename + " ... ";
        ui->log_label->append(message.c_str());

        // check class
        QString subString = f;

        if (f.length() > 0 && f.at(0).isDigit())
        {
            // subString = QStringRef(&f, f.indexOf("_")+1, f.length() - f.indexOf("_") - 1);
        }

        std:: cout << subString.toStdString() << std::endl;

        uint class_id = UINT_MAX;

        for (uint i=0; i < classPrefix.size(); i++)
            if (subString.startsWith(classPrefix.at(i).c_str()) )
            {
                bool contains_other = false;

                for (uint j=i+1; j < classPrefix.size(); j++)
                    if (subString.contains(classPrefix.at(j).c_str()))
                        contains_other = true;

                if (contains_other) continue;

                class_id = i;
                break;
            }

        cinolib::Polygonmesh<> *m = new cinolib::Polygonmesh<> (filename.c_str());

        m->mesh_data().filename = f.toStdString();

        message = std::to_string(m->num_verts()) + "V / " + std::to_string(m->num_polys()) + "P ";
        ui->log_label->append(message.c_str());

        for (uint pid=0; pid < m->num_polys(); pid++)
        {
            if (m->poly_verts(pid).size() > 3)
            {
                for (uint eid : m->adj_p2e(pid))
                    m->edge_data(eid).flags.set(0, true);

                m->poly_data(pid).flags.set(1, true);
            }

            // m->show_marked_edge(true);
        }

        // save node/ele if not present - to enable pde solver
        QFileInfo fileInfo(QString(filename.c_str()));
        QString basename = fileInfo.completeBaseName();

        QString node_filename = dir + QString(QDir::separator()) + basename + ".node";
        QString ele_filename = dir + QString(QDir::separator()) + basename + ".ele";

        if (!QFile(node_filename).exists() || !QFile(ele_filename).exists())
        {
            const std::string node_ele_filename = dir.toStdString() + QString(QDir::separator()).toStdString() + basename.toStdString();

            write_NODE_ELE_2D(node_ele_filename.c_str(), m->vector_verts(), m->vector_polys());
            message = "Saved NODE/ELE " + node_ele_filename;
            ui->log_label->append(message.c_str());
        }

        dataset->add_parametric_mesh(m, DBL_MAX, class_id);

        ui->log_label->append("\n");

        id++;

        QCoreApplication::processEvents();
    }

    QApplication::restoreOverrideCursor();

    ui->param_slider->setMaximum(static_cast<int>(mesh_files.size()-1));

    ui->log_label->append("Dataset completed.");
    ui->param_slider->show();
    ui->mesh_number_label->show();

    ui->highlight_polys_cb->setEnabled(true);

    show_parametric_mesh(0);

    //// Disable/enable buttons
    ui->load_polys_btn->setEnabled(false);
    ui->load_meshes_btn->setEnabled(false);
    ui->add_btn->setEnabled(false);
    ui->generate_dataset_btn->setEnabled(false);

    ui->geom_qualities_btn->setEnabled(true);
    ui->optimize_btn->setEnabled(true);

    // ui->canvas->callback_mouse_press =
    //         [](cinolib::GLcanvas* canvas, QMouseEvent* event){ };

    ui->aggregate_btn->setEnabled(true);

    emit (saved_in (dir.toStdString()));
}

void DatasetWidget::on_aggregate_btn_clicked()
{
    AggregateDialog *dialog = new AggregateDialog();

    uint aggregation_type = 10;
    if (dialog->exec() == 1)
        aggregation_type = dialog->getAggregationIndex();
    if (aggregation_type >3)     return;
    delete dialog;

    if (dataset->get_parametric_meshes().empty()) return;
    uint count_mesh = 0;

    QApplication::setOverrideCursor(Qt::WaitCursor);

    for (cinolib::Polygonmesh<> *m : dataset->get_parametric_meshes())
    {
        double diameter = -cinolib::inf_double, rho = -cinolib::inf_double, rho_a = -cinolib::inf_double;

        for (uint pid = 0; pid < m->num_polys(); pid++)
        {
            double max_pd = -cinolib::inf_double;
            std::vector<cinolib::vec3d> points = m->poly_verts(pid);
            for(uint i=0; i<points.size()-1; ++i)
                for(uint j=i+1; j<points.size(); ++j)
                {
                    max_pd = std::max(max_pd, points.at(i).dist(points.at(j)));

                    if (m->poly_data(pid).flags.test(1))
                    {
                        diameter = std::max(diameter, max_pd);
                    }
                }

            /*if (aggregation_type == 0)
            {
                diameter = std::max(diameter, max_pd);
            }
            else */if (aggregation_type == 1)
            {
                double min_e = cinolib::inf_double;
                for (auto eid : m->adj_p2e(pid))
                    min_e = std::min(min_e, m->edge_length(eid));

                double area = m->poly_area(pid);

                rho = std::max(rho, max_pd / std::min(sqrt(area), min_e));
            }
            else if (aggregation_type == 2)
            {
                double min_e = cinolib::inf_double;
                for (auto eid : m->adj_p2e(pid))
                    min_e = std::min(min_e, m->edge_length(eid));

                double area = m->poly_area(pid);

                rho = max_pd / std::min(sqrt(area), min_e);
                rho_a = std::max(rho_a, rho * rho * area);
            }
        }

        for (uint e=0; e < elems.size(); e++)
            m->poly_data(m->num_polys()-(e+1)).flags.set(1, true);

        std::cout << count_mesh <<  " - aggregating ... " << std::endl;
        double value;
        if      (aggregation_type == 0)     value = diameter;
        else if (aggregation_type == 1)     value = 1000;
        else if (aggregation_type == 2)     value = rho;
        else if (aggregation_type == 3)     value = rho_a;
        else return;

        ui->log_label->append(("Aggregating Triangles on Mesh " + std::to_string(count_mesh) + " ...").c_str());
        aggregate_triangles(*m, value, aggregation_type);

        // std::string filename = m->mesh_data().filename.substr(m->mesh_data().filename.find_last_of("/")+1);

        // filename = "/home/danielacabiddu/Downloads/5k_polygons_canvas_aggregated_2/" + filename;
        // m->save(filename.c_str());

        // std::cout << "saved " << filename << std::endl;

        ui->log_label->append("DONE");

        // m->updateGL();

        QCoreApplication::processEvents();


        count_mesh++;
    }
    // ui->canvas->updateGL();
    QApplication::restoreOverrideCursor();

    ui->aggregate_btn->setEnabled(false);
    ui->save_btn->setEnabled(true);
}

void DatasetWidget::aggregate_triangles (cinolib::Polygonmesh<> &dm, const double value_b, const int aggregation_type)
{
    bool ok = false;
    while (!ok)
    {
        std::map<double, std::pair<uint,uint>> value2pid; // = (value, (pid1,pid2))
        std::set<std::pair<uint,uint>> already;

//        if (aggregation_type == 1)
//        {
//            uint pid1 = rand() % dm.num_polys();
//            uint pid2 = dm.adj_p2p(pid1).at(0);

//            double val = 10;
//            if (dm.num_polys() <= 50)
//                val = 1000 + val;

//            value2pid.insert(std::pair<double, std::pair<uint,uint>> (val, std::pair<uint,uint> (pid1, pid2)));
//        }
//        else
        {
            for (uint pid = 0; pid < dm.num_polys(); pid++)
            {
                if (dm.poly_data(pid).flags.test(1)) continue;

                std::vector<uint> points_pid = dm.poly_verts_id(pid); // id dei vertici di pid

                for (uint adj_pid : dm.adj_p2p(pid))    // poligoni adiacenti a pid
                {
                    if (dm.poly_data(adj_pid).flags.test(1)) continue;

                    if (already.find(std::pair<uint,uint>(adj_pid, pid)) != already.end() ) continue;

                    already.insert(std::pair<uint,uint> (adj_pid, pid));
                    already.insert(std::pair<uint,uint> (pid, adj_pid));

                    std::vector<uint> points_adj = dm.poly_verts_id(adj_pid); // id dei vertici di adj_pid
                    std::set<uint> points_ids;
                    std::vector<cinolib::vec3d> points;
                    for (uint p : points_pid)   points_ids.insert(p);
                    for (uint p : points_adj)   points_ids.insert(p);
                    for (uint p : points_ids)   points.push_back(dm.vert(p));

                    double diameter = -cinolib::inf_double;
                    for(uint i=0; i<points.size()-1; ++i)
                        for(uint j=i+1; j<points.size(); ++j)
                            diameter = std::max(diameter, points.at(i).dist(points.at(j)));

                    if (aggregation_type == 0)
                    {
                        value2pid.insert(std::pair<double, std::pair<uint,uint>> (diameter, std::pair<uint,uint> (adj_pid, pid)));
                    }
                    else
                    if (aggregation_type == 1)
                    {
                        double val = rand() % 1000;

                        //if (dm.poly_verts(pid).size()>3 && dm.poly_verts(adj_pid).size()>3 )
                        if (dm.num_polys() <= 50)
                            val = 1000 + val;

                        value2pid.insert(std::pair<double, std::pair<uint,uint>> (val, std::pair<uint,uint> (adj_pid, pid)));

                        break;
                    }
                    else
                    if (aggregation_type == 2)
                    {
                        double min_e = cinolib::inf_double;
                        for (auto eid : dm.adj_p2e(pid))
                            if(!(dm.poly_contains_edge(adj_pid,eid)))
                                min_e = std::min(min_e, dm.edge_length(eid));

                        for (auto eid : dm.adj_p2e(adj_pid))
                            if(!(dm.poly_contains_edge(pid,eid)))
                                min_e = std::min(min_e, dm.edge_length(eid));

                        double area = dm.poly_area(pid) + dm.poly_area(adj_pid);
                        double rho = diameter / std::min(sqrt(area), min_e);

                        value2pid.insert(std::pair<double, std::pair<uint,uint>> (rho, std::pair<uint,uint> (adj_pid, pid)));
                    }
                    else if (aggregation_type == 3)
                    {
                        double min_e = cinolib::inf_double;
                        for (auto eid : dm.adj_p2e(pid))
                            if(!(dm.poly_contains_edge(adj_pid,eid)))
                                min_e = std::min(min_e, dm.edge_length(eid));

                        for (auto eid : dm.adj_p2e(adj_pid))
                            if(!(dm.poly_contains_edge(pid,eid)))
                                min_e = std::min(min_e, dm.edge_length(eid));

                        double area = dm.poly_area(pid) + dm.poly_area(adj_pid);
                        double rho = diameter / std::min(sqrt(area), min_e);
                        double rho_a = rho * rho * area;

                        value2pid.insert(std::pair<double, std::pair<uint,uint>> (rho_a, std::pair<uint,uint> (adj_pid, pid)));
                    }
                    else return;
                }
            }
        }

        if (value2pid.empty())
        {
            ok = true;
            break;
        }


        uint pid_tb_merged = (*value2pid.begin()).second.first;
        uint pid_tb_merged_2 = (*value2pid.begin()).second.second;
        double value = (*value2pid.begin()).first;

        std::cout << value << std::endl;

        if (value > value_b )    ok = true;
        else        //se il valore è < di quello di bound, unisco
        {
                cinolib::Polygonmesh<> *m = new cinolib::Polygonmesh<> (dm.vector_verts(), {});

                m->poly_add(dm.adj_p2v(pid_tb_merged_2));
                m->poly_add(dm.adj_p2v(pid_tb_merged));

//                m->save("prova.obj");

                bool has_no_manifold = false;
                bool has_hole = false;
                std::vector<cinolib::ipair> be = m->get_boundary_edges();
                std::vector<bool> visited (be.size(), false);
                std::queue<cinolib::ipair> queue;
                queue.push(be.at(0));
                visited.at(0) =true;

                while (!queue.empty())
                {
                    cinolib::ipair e = queue.front();
                    queue.pop();

                    for (uint i=0; i < be.size(); i++)
                    {
                        if (visited.at(i)) continue;

                        if (be.at(i).first == e.first || be.at(i).first == e.second ||
                                be.at(i).second == e.first || be.at(i).second == e.second )
                        {
                            queue.push(be.at(i));
                            visited.at(i) =true;

                        }
                    }
                }

                uint count_visisted = 0;

                for (bool b : visited)
                    if (b) count_visisted++;

                has_hole = (! (count_visisted == be.size()));


                for (uint i=0; i < be.size()-1; i++)
                {
                    uint count_first = 0;
                    uint count_sec = 0;

                    for (uint j=i+1; j < be.size(); j++)
                    {
                        if (be.at(i).first == be.at(j).first || be.at(i).first == be.at(j).second)
                            count_first++;

                        if (be.at(i).second == be.at(j).first || be.at(i).second == be.at(j).second)
                            count_sec++;

                        if (count_first > 1 || count_sec > 1 )
                        {
                            has_no_manifold = true;
                            break;
                            //std::cout << "[" << be.at(i).first << ", " << be.at(i).second << "] edge is not manifold" << std::endl;
                        }
                    }

                    if (has_no_manifold) break;
                }


                if (!has_no_manifold && !has_hole)
                {
                    std::vector<uint> verts = m->get_ordered_boundary_vertices();
                    std::vector<uint> bverts = verts;

                    bool has_dupl = false;
                    std::sort(bverts.begin(), bverts.end());
                    for (uint i=0; i < bverts.size()-1; i++)
                    {
                        if (bverts.at(i) == bverts.at(i+1))
                            has_dupl = true;
                    }

                    if (!has_dupl)
                    {

                        dm.poly_add(verts);

                        delete  m;
                        dm.poly_remove(std::max (pid_tb_merged_2, pid_tb_merged));
                        dm.poly_remove(std::min (pid_tb_merged_2, pid_tb_merged));
                    }
                }

        }
    }
}

void DatasetWidget::on_mirroring_btn_clicked()
{
    ui->mirroring_btn->setEnabled(false);

    QApplication::setOverrideCursor(Qt::WaitCursor);

    uint index=0;

    for (cinolib::Polygonmesh<> *mesh : dataset->get_parametric_meshes())
    {
        std::string message = "Mirroring mesh " + std::to_string(index) + ": " +
                                                  std::to_string(mesh->num_verts()) + "V|" +
                                                  std::to_string(mesh->num_polys()) + "P" ;
        ui->log_label->append(message.c_str());

        apply_mirroring(*mesh);

        //mesh->edge_unmark_all();
        mesh->edge_mark_boundaries();

        // mesh->updateGL();

        message = "--> Mirrired mesh " + std::to_string(index) + ": " +
                                         std::to_string(mesh->num_verts()) + "V|" +
                                         std::to_string(mesh->num_polys()) + "P \n";

        ui->log_label->append(message.c_str());

        index++;
    }

    show_parametric_mesh(ui->param_slider->value());

    QApplication::restoreOverrideCursor();

    ui->mirroring_btn->setEnabled(true);
}

void DatasetWidget::on_show_coords_cb_stateChanged(int checked)
{
    // if (!checked)
    //     ui->canvas->pop_all_markers();
    // else
    // {
    //     for (const Marker * m : markers)
    //         ui->canvas->push_marker(new Marker(*m));
    // }

    // ui->canvas->updateGL();
}

void DatasetWidget::on_highlight_polys_cb_stateChanged(int checked)
{
    #pragma omp parallel for
    for (uint i=0; i < dataset->get_num_parametric_meshes(); i++)
    {
        cinolib::Polygonmesh<> *m = dataset->get_parametric_mesh(i);

        // m->show_marked_edge(checked);
        // m->updateGL();
    }

    // ui->canvas->updateGL();
}

void DatasetWidget::on_optimize_btn_clicked()
{
    ui->optimize_btn->setEnabled(false);

    OptimizeDialog *dialog = new OptimizeDialog();
    double (*indicator)(const std::vector<cinolib::vec3d>&);
    bool   node_weights = false, arc_weights = false;
    double parameter = 100.;
    bool   overwrite = false;

    if (dialog->exec() == 1)
    {
        // dialog->get_indicator(indicator);
        switch (dialog->get_indicator()) {
        case 0 : { indicator = compute_metric_INR; break; }
        case 1 : { indicator = compute_metric_OUR; break; }
        case 2 : { indicator = compute_metric_CIR; break; }
        case 3 : { indicator = compute_metric_KRR; break; }
        case 4 : { indicator = compute_metric_KAR; break; }
        case 5 : { indicator = compute_metric_APR; break; }
        case 6 : { indicator = compute_metric_MIA; break; }
        case 7 : { indicator = compute_metric_MAA; break; }
        case 8 : { indicator = compute_metric_ANR; break; }
        case 9 : { indicator = compute_metric_VEM; break; }
        case 10: { indicator = compute_metric_JAC; break; }
        case 11: { indicator = compute_metric_FRO; break; }
        default: break;
        }
        dialog->get_weights(node_weights, arc_weights);
        dialog->get_parameter(parameter);
        dialog->get_overwrite(overwrite);
    }
    else
    {
        ui->optimize_btn->setEnabled(true);
        return;
    }
    delete dialog;

    QApplication::setOverrideCursor(Qt::WaitCursor);

    ui->canvas->clear();

    uint index=0;
    for (cinolib::Polygonmesh<> *mesh : dataset->get_parametric_meshes())
    {
        std::string message = "Optimizing mesh " + std::to_string(index) + ": " +
                              std::to_string(mesh->num_verts()) + "V|" +
                              std::to_string(mesh->num_polys()) + "P" ;
        ui->log_label->append(message.c_str());

        message = "--> Metis Labelling, required: " + std::to_string((uint)(mesh->num_polys() * parameter / 100.));
        int n_labels = metis_wrap_dual(*mesh, parameter, indicator, node_weights, arc_weights);
        message += ", found: " + std::to_string(n_labels);
        ui->log_label->append(message.c_str());

        cinolib::Polygonmesh<> mesh_new = *mesh;
        mesh_agglomerate_wrt_labels(mesh_new);
        message = "--> Mesh Agglomeration, n_polys: " + std::to_string(mesh_new.num_polys());
        ui->log_label->append(message.c_str());

        cinolib::Hierarchy agglomeration_hierarchy;
        agglomeration_hierarchy.compute(*mesh, mesh_new);
        assert(agglomeration_hierarchy.check(*mesh, mesh_new) && "ERROR: hierarchy check failed");
        std::string output_h  = "_hierarchy.txt";
        agglomeration_hierarchy.print(output_h);
        message = "Computed Hierarchy";
        ui->log_label->append(message.c_str());

        message = "Optimization done: " +
                  std::to_string(mesh->num_verts() - mesh_new.num_verts()) + " removed verts, " +
                  std::to_string(mesh->num_polys() - mesh_new.num_polys()) + " merged polys.\n" ;
        ui->log_label->append(message.c_str());

        *mesh = mesh_new;

        // if (overwrite) {
        // } else {
        //     ui->canvas->add_mesh(*mesh, c);
        //     // dataset->add_parametric_mesh(mesh_new, 1., index);
        // }
        ui->canvas->add_mesh(*mesh);

        index++;
    }

    ui->canvas->updateGL();

    QApplication::restoreOverrideCursor();

    ui->geom_qualities_btn->setEnabled(true);
    ui->optimize_btn->setEnabled(true);
}


void DatasetWidget::on_load_mesh_btn_clicked()
{
    QString filters("Meshes (*.off *.obj *.stl)");
    QString filename;

    do
    {
        filename = QFileDialog::getOpenFileName(this, tr("Select Mesh File"), "/tmp", filters);

        if (filename.isNull() || filename.isEmpty())
            return;
    }
    while (filename.isNull() || filename.isEmpty());

    QApplication::setOverrideCursor(Qt::WaitCursor);

    std::string message = "Loading " + filename.toStdString() + " ... ";
    ui->log_label->append(message.c_str());

    // check class
    QString subString = filename;

    if (filename.length() > 0 && filename.at(0).isDigit())
    {
        // subString = QStringRef(&f, f.indexOf("_")+1, f.length() - f.indexOf("_") - 1);
    }

    std:: cout << subString.toStdString() << std::endl;

    uint class_id = UINT_MAX;

    for (uint i=0; i < classPrefix.size(); i++)
        if (subString.startsWith(classPrefix.at(i).c_str()) )
        {
            bool contains_other = false;

            for (uint j=i+1; j < classPrefix.size(); j++)
                if (subString.contains(classPrefix.at(j).c_str()))
                    contains_other = true;

            if (contains_other) continue;

            class_id = i;
            break;
        }

    cinolib::Polygonmesh<> *m = new cinolib::Polygonmesh<> (filename.toStdString().c_str());

    m->mesh_data().filename = filename.toStdString();

    message = std::to_string(m->num_verts()) + "V / " + std::to_string(m->num_polys()) + "P ";
    ui->log_label->append(message.c_str());

    for (uint pid=0; pid < m->num_polys(); pid++)
    {
        if (m->poly_verts(pid).size() > 3)
        {
            for (uint eid : m->adj_p2e(pid))
                m->edge_data(eid).flags.set(0, true);

            m->poly_data(pid).flags.set(1, true);
        }

        // m->show_marked_edge(true);
    }

    // save node/ele if not present - to enable pde solver
    QFileInfo fileInfo(QString(filename.toStdString().c_str()));
    QString basename = fileInfo.completeBaseName();

    QString node_filename = fileInfo.absoluteDir().absolutePath() + QString(QDir::separator()) + basename + ".node";
    QString ele_filename = fileInfo.absoluteDir().absolutePath() + QString(QDir::separator()) + basename + ".ele";

    if (!QFile(node_filename).exists() || !QFile(ele_filename).exists())
    {
        const std::string node_ele_filename = fileInfo.absoluteDir().absolutePath().toStdString() + QString(QDir::separator()).toStdString() + basename.toStdString();

        write_NODE_ELE_2D(node_ele_filename.c_str(), m->vector_verts(), m->vector_polys());
        message = "Saved NODE/ELE " + node_ele_filename;
        ui->log_label->append(message.c_str());
    }

    dataset->add_parametric_mesh(m, DBL_MAX, class_id);

    ui->log_label->append("\n");

    QCoreApplication::processEvents();


    QApplication::restoreOverrideCursor();

    if (dataset->get_num_parametric_meshes() > 1)
    {
        ui->param_slider->setMaximum(static_cast<int>(dataset->get_num_parametric_meshes()-1));
        ui->param_slider->setVisible(true);
    }
    else
    {
        ui->param_slider->setVisible(false);
    }

    ui->log_label->append("Dataset completed.");
    ui->param_slider->show();
    ui->mesh_number_label->show();

    ui->highlight_polys_cb->setEnabled(true);

    show_parametric_mesh(0);

    //// Disable/enable buttons
    ui->load_polys_btn->setEnabled(false);
    ui->load_meshes_btn->setEnabled(false);
    ui->add_btn->setEnabled(false);
    ui->generate_dataset_btn->setEnabled(false);

    ui->geom_qualities_btn->setEnabled(true);
    ui->optimize_btn->setEnabled(true);

    // ui->canvas->callback_mouse_press =
    //         [](cinolib::GLcanvas* canvas, QMouseEvent* event){ };

    ui->aggregate_btn->setEnabled(true);

    ui->load_mesh_btn->setEnabled(false);

    // emit (saved_in (dir.toStdString()));
}

