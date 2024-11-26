#include "solverresultsgraphicwidget.h"
#include "dataset.h"
#include "ui_solverresultsgraphicwidget.h"

SolverResultsGraphicWidget::SolverResultsGraphicWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SolverResultsGraphicWidget)
{
    ui->setupUi(this);
}

SolverResultsGraphicWidget::~SolverResultsGraphicWidget()
{
    delete ui;
}

void SolverResultsGraphicWidget::set_residuals_on()
{
    ui->canvas1_cb->setItemText(4 ,"Residuals (P)");
    ui->canvas2_cb->setItemText(4, "Residuals (P)");
}

void SolverResultsGraphicWidget::set_condition_number_on()
{
    ui->canvas1_cb->setItemText(4, "Condition Number (P)");
    ui->canvas2_cb->setItemText(4, "Condition Number (P)");
}

void SolverResultsGraphicWidget::set_dataset(Dataset *d)
{
    for (uint i = 0; i < d->get_parametric_meshes().size(); i++)
    {
        cinolib::Polygonmesh<> *m = d->get_parametric_mesh(i);
        cinolib::Polygonmesh<> *m_gt =
            new cinolib::Polygonmesh<>(m->vector_verts(), m->vector_polys());

        meshes_canvas1.push_back(m);
        meshes_canvas2.push_back(m_gt);

        for (uint pid=0; pid < m_gt->num_polys(); pid++)
            if (m_gt->adj_p2e(pid).size() > 3)
                for (uint eid : m_gt->adj_p2e(pid))
                    m_gt->edge_data(eid).flags.set(0, true);

        //        m_gt->show_marked_edge(true);
    }

    ui->t_slider->setMaximum(static_cast<int>(d->get_parametric_meshes().size()) - 1);

    if (d->get_parametric_meshes().size() > 0)
        show_parametric_mesh(0);
}

void SolverResultsGraphicWidget::show_mesh_solution_and_groundtruth() const
{
    ui->canvas1_cb->setCurrentIndex(0);
    ui->canvas2_cb->setCurrentIndex(1);
}

void SolverResultsGraphicWidget::show_parametric_mesh(int index)
{
    clean_canvas();

    cinolib::Polygonmesh<> *p_r = meshes_canvas1.at(static_cast<uint>(index));
    cinolib::Polygonmesh<> *p_gt = meshes_canvas2.at(static_cast<uint>(index));

    ui->canvas1->clear();
    ui->canvas2->clear();

    bool canvas1_show_color_on_polys = true;
    bool canvas2_show_color_on_polys = true;

    if (ui->canvas1_cb->currentText().endsWith("(V)"))
    {
        canvas1_show_color_on_polys = false;
    }

    if (ui->canvas2_cb->currentText().endsWith("(V)") )
        canvas2_show_color_on_polys = false;

    ui->canvas1->add_mesh(*p_r, canvas1_show_color_on_polys);
    ui->canvas2->add_mesh(*p_gt, canvas2_show_color_on_polys);


    if (solution_mins.size() >0 )
    {
        /// update min_max labels
        ///
        double min1 = 0.0, min2 = 0.0, max1 = 0.0, max2 = 0.0;

        if (ui->canvas1_cb->currentIndex() == 0)
        {
            min1 = solution_mins.at(ui->t_slider->value());
            max1 = solution_maxs.at(ui->t_slider->value());
        }
        else if (ui->canvas1_cb->currentIndex() == 1)
        {
            min1 = groundtruths_mins.at(ui->t_slider->value());
            max1 = groundtruths_maxs.at(ui->t_slider->value());
        }
        else if (ui->canvas1_cb->currentIndex() == 2)
        {
            min1 = errH1s_mins.at(ui->t_slider->value());
            max1 = errH1s_maxs.at(ui->t_slider->value());
        }
        else if (ui->canvas1_cb->currentIndex() == 3)
        {
            min1 = errL2s_mins.at(ui->t_slider->value());
            max1 = errL2s_maxs.at(ui->t_slider->value());
        }
        else //if (ui->canvas1_cb->currentIndex() == 4)
        {
            min1 = Conds_mins.at(ui->t_slider->value());
            max1 = Conds_maxs.at(ui->t_slider->value());
        }

        if (ui->canvas2_cb->currentIndex() == 0)
        {
            min2 = solution_mins.at(ui->t_slider->value());
            max2 = solution_maxs.at(ui->t_slider->value());
        }
        else if (ui->canvas2_cb->currentIndex() == 1)
        {
            min2 = groundtruths_mins.at(ui->t_slider->value());
            max2 = groundtruths_maxs.at(ui->t_slider->value());
        }
        else if (ui->canvas2_cb->currentIndex() == 2)
        {
            min2 = errH1s_mins.at(ui->t_slider->value());
            max2 = errH1s_maxs.at(ui->t_slider->value());
        }
        else if (ui->canvas2_cb->currentIndex() == 3)
        {
            min2 = errL2s_mins.at(ui->t_slider->value());
            max2 = errL2s_maxs.at(ui->t_slider->value());
        }
        else //if (ui->canvas2_cb->currentIndex() == 4)
        {
            min2 = Conds_mins.at(ui->t_slider->value());
            max2 = Conds_maxs.at(ui->t_slider->value());
        }

        ui->min1_label->setText(QString(std::to_string(min1).c_str()));
        ui->max1_label->setText(QString(std::to_string(max1).c_str()));

        ui->min2_label->setText(QString(std::to_string(min2).c_str()));
        ui->max2_label->setText(QString(std::to_string(max2).c_str()));
    }

    update_scene = false;

    curr_mesh_id = static_cast<uint>(index);
}

void SolverResultsGraphicWidget::on_t_slider_valueChanged(int value)
{
    if (value >= 0)
        show_parametric_mesh(value);

    ui->mesh_id_label->setText(QString(std::to_string(value).c_str()));

    // for (uint c = 0; c < chart_views.size(); c++) {
    //     QGraphicsLineItem *line = track_lines.at(c);

    //     QLineSeries *series =
    //         static_cast<QLineSeries *>(chart_views.at(c)->chart()->series().at(0));

    //     QValueAxis *axis = static_cast<QValueAxis *>(series->attachedAxes().at(1));

    //     QPointF a(series->at(value).x(), axis->min());
    //     QPointF b(series->at(value).x(), axis->max());

    //     QPointF aa = chart_views.at(c)->chart()->mapToPosition(a);
    //     QPointF bb = chart_views.at(c)->chart()->mapToPosition(b);

    //     line->setLine(aa.x(), aa.y(), bb.x(), bb.y());
    // }
}

void SolverResultsGraphicWidget::on_canvas1_cb_currentIndexChanged(int index)
{
    bool show_poly_color= true;

    QRectF rect = QRectF (ui->colorbar1->rect());
    QLinearGradient colorGradient = QLinearGradient(rect.topLeft(), rect.topRight());

    uint n_polys = meshes_canvas1.at(0)->num_polys();
    uint n_colors = n_polys < 100 ? n_polys : 100;

    switch (index)
    {
    case 0:
        for (uint m=0; m < meshes_canvas1.size(); m++)
        {
            for (uint vid=0; vid < meshes_canvas1.at(m)->num_verts(); vid++)
            {
                cinolib::Color c = cinolib::Color::red_white_blue_ramp_01(solutions.at(m).data()[vid]);
                meshes_canvas1.at(m)->vert_data(vid).color = c;
                colorGradient.setColorAt(solutions.at(m).data()[vid], QColor(c.r * 255, c.g * 255, c.b * 255));
            }
        }
        show_poly_color=false;

        ui->min1_label->setText(QString(std::to_string(solution_mins.at(ui->t_slider->value())).c_str()));
        ui->max1_label->setText(QString(std::to_string(solution_maxs.at(ui->t_slider->value())).c_str()));

        break;
    case 1:
        for (uint m=0; m < meshes_canvas1.size(); m++)
        {
            for (uint vid=0; vid < meshes_canvas1.at(m)->num_verts(); vid++)
            {
                cinolib::Color c = cinolib::Color::red_white_blue_ramp_01(groundtruths.at(m).data()[vid]);
                meshes_canvas1.at(m)->vert_data(vid).color = c;
                colorGradient.setColorAt(groundtruths.at(m).data()[vid], QColor(c.r * 255, c.g * 255, c.b * 255));
            }
        }
        show_poly_color=false;

        ui->min1_label->setText(QString(std::to_string(groundtruths_mins.at(ui->t_slider->value())).c_str()));
        ui->max1_label->setText(QString(std::to_string(groundtruths_maxs.at(ui->t_slider->value())).c_str()));

        break;
    case 2:
        for (uint m=0; m < meshes_canvas1.size(); m++)
        {
            for (uint pid=0; pid < meshes_canvas1.at(m)->num_polys(); pid++)
            {
                // cinolib::Color c = cinolib::Color::red_white_blue_ramp_01(errH1s.at(m).data()[pid]);
                cinolib::Color c = cinolib::Color::parula_ramp(n_colors+1, (int)(errH1s.at(m).data()[pid] * (n_colors-1)));
                meshes_canvas1.at(m)->poly_data(pid).color = c;
                colorGradient.setColorAt(errH1s.at(m).data()[pid], QColor(c.r * 255, c.g * 255, c.b * 255));
            }
        }
        show_poly_color = true;

        ui->min1_label->setText(QString(std::to_string(errH1s_mins.at(ui->t_slider->value())).c_str()));
        ui->max1_label->setText(QString(std::to_string(errH1s_maxs.at(ui->t_slider->value())).c_str()));

        break;
    case 3:
        for (uint m=0; m < meshes_canvas1.size(); m++)
        {
            for (uint pid=0; pid < meshes_canvas1.at(m)->num_polys(); pid++)
            {
                // cinolib::Color c = cinolib::Color::red_white_blue_ramp_01(errL2s.at(m).data()[pid]);
                cinolib::Color c = cinolib::Color::parula_ramp(n_colors+1, (int)(errL2s.at(m).data()[pid] * (n_colors-1)));
                meshes_canvas1.at(m)->poly_data(pid).color = c;
                colorGradient.setColorAt(errL2s.at(m).data()[pid], QColor(c.r * 255, c.g * 255, c.b * 255));
            }
        }
        show_poly_color = true;

        ui->min1_label->setText(QString(std::to_string(errL2s_mins.at(ui->t_slider->value())).c_str()));
        ui->max1_label->setText(QString(std::to_string(errL2s_maxs.at(ui->t_slider->value())).c_str()));

        break;
    case 4:
        for (uint m=0; m < meshes_canvas1.size(); m++)
        {
            for (uint pid=0; pid < meshes_canvas1.at(m)->num_polys(); pid++)
            {
                // cinolib::Color c = cinolib::Color::red_white_blue_ramp_01(Conds.at(m).data()[pid]);
                cinolib::Color c = cinolib::Color::parula_ramp(n_colors+1, (int)(Conds.at(m).data()[pid] * (n_colors-1)));
                meshes_canvas1.at(m)->poly_data(pid).color = c;
                colorGradient.setColorAt(Conds.at(m).data()[pid], QColor(c.r * 255, c.g * 255, c.b * 255));
            }
        }
        show_poly_color = true;

        ui->min1_label->setText(QString(std::to_string(Conds_mins.at(ui->t_slider->value())).c_str()));
        ui->max1_label->setText(QString(std::to_string(Conds_maxs.at(ui->t_slider->value())).c_str()));

        break;
    default:
        break;
    }

    ui->canvas1->clear();

    uint t_index = ui->t_slider->value();

    ui->canvas1->add_mesh(*meshes_canvas1.at(t_index), show_poly_color);

    ui->canvas1->updateGL();

    QBrush colorGradiantBrush = QBrush(colorGradient);

    QPalette palette = ui->colorbar1->palette();
    palette.setBrush(QPalette::Base, colorGradiantBrush);
    ui->colorbar1->setPalette(palette);
}


void SolverResultsGraphicWidget::on_canvas2_cb_currentIndexChanged(int index)
{
    bool show_poly_color= true;

    QRectF rect = QRectF (ui->colorbar2->rect());
    QLinearGradient colorGradient = QLinearGradient(rect.topLeft(), rect.topRight());

    uint n_polys = meshes_canvas2.at(0)->num_polys();
    uint n_colors = n_polys < 100 ? n_polys : 100;

    switch (index)
    {
    case 0:
        for (uint m=0; m < meshes_canvas2.size(); m++)
        {
            for (uint vid=0; vid < meshes_canvas2.at(m)->num_verts(); vid++)
            {
                cinolib::Color c = cinolib::Color::red_white_blue_ramp_01(solutions.at(m).data()[vid]);
                meshes_canvas2.at(m)->vert_data(vid).color = c;
                colorGradient.setColorAt(solutions.at(m).data()[vid], QColor(c.r * 255, c.g * 255, c.b * 255));
            }
        }
        show_poly_color = false;

        ui->min2_label->setText(QString(std::to_string(solution_mins.at(ui->t_slider->value())).c_str()));
        ui->max2_label->setText(QString(std::to_string(solution_maxs.at(ui->t_slider->value())).c_str()));

        break;
    case 1:
        for (uint m=0; m < meshes_canvas2.size(); m++)
        {
            for (uint vid=0; vid < meshes_canvas2.at(m)->num_verts(); vid++)
            {
                cinolib::Color c = cinolib::Color::red_white_blue_ramp_01(groundtruths.at(m).data()[vid]);
                meshes_canvas2.at(m)->vert_data(vid).color = c;
                colorGradient.setColorAt(groundtruths.at(m).data()[vid], QColor(c.r * 255, c.g * 255, c.b * 255));
            }
        }
        show_poly_color = false;

        ui->min2_label->setText(QString(std::to_string(groundtruths_mins.at(ui->t_slider->value())).c_str()));
        ui->max2_label->setText(QString(std::to_string(groundtruths_maxs.at(ui->t_slider->value())).c_str()));

        break;
    case 2:
        for (uint m=0; m < meshes_canvas2.size(); m++)
        {
            for (uint pid=0; pid < meshes_canvas2.at(m)->num_polys(); pid++)
            {
                // cinolib::Color c = cinolib::Color::red_white_blue_ramp_01(errH1s.at(m).data()[pid]);
                cinolib::Color c = cinolib::Color::parula_ramp(n_colors+1, (int)(errH1s.at(m).data()[pid] * (n_colors-1)));
                meshes_canvas2.at(m)->poly_data(pid).color = c;
                colorGradient.setColorAt(errH1s.at(m).data()[pid], QColor(c.r * 255, c.g * 255, c.b * 255));
            }
        }
        show_poly_color = true;

        ui->min2_label->setText(QString(std::to_string(errH1s_mins.at(ui->t_slider->value())).c_str()));
        ui->max2_label->setText(QString(std::to_string(errH1s_maxs.at(ui->t_slider->value())).c_str()));

        break;
    case 3:
        for (uint m=0; m < meshes_canvas2.size(); m++)
        {
            for (uint pid=0; pid < meshes_canvas2.at(m)->num_polys(); pid++)
            {
                // cinolib::Color c = cinolib::Color::red_white_blue_ramp_01(errL2s.at(m).data()[pid]);
                cinolib::Color c = cinolib::Color::parula_ramp(n_colors+1, (int)(errL2s.at(m).data()[pid] * (n_colors-1)));
                meshes_canvas2.at(m)->poly_data(pid).color = c;
                colorGradient.setColorAt(errL2s.at(m).data()[pid], QColor(c.r * 255, c.g * 255, c.b * 255));
            }
        }
        show_poly_color = true;

        ui->min2_label->setText(QString(std::to_string(errL2s_mins.at(ui->t_slider->value())).c_str()));
        ui->max2_label->setText(QString(std::to_string(errL2s_maxs.at(ui->t_slider->value())).c_str()));

        break;
    case 4:
        for (uint m=0; m < meshes_canvas2.size(); m++)
        {
            for (uint pid=0; pid < meshes_canvas2.at(m)->num_polys(); pid++)
            {
                // cinolib::Color c = cinolib::Color::red_white_blue_ramp_01(Conds.at(m).data()[pid]);
                cinolib::Color c = cinolib::Color::parula_ramp(n_colors+1, (int)(Conds.at(m).data()[pid] * (n_colors-1)));
                meshes_canvas2.at(m)->poly_data(pid).color = c;
                colorGradient.setColorAt(Conds.at(m).data()[pid], QColor(c.r * 255, c.g * 255, c.b * 255));
            }
        }
        show_poly_color = true;

        ui->min2_label->setText(QString(std::to_string(Conds_mins.at(ui->t_slider->value())).c_str()));
        ui->max2_label->setText(QString(std::to_string(Conds_maxs.at(ui->t_slider->value())).c_str()));

        break;
    default:
        break;
    }

    QBrush colorGradiantBrush = QBrush(colorGradient);

    QPalette palette = ui->colorbar2->palette();
    palette.setBrush(QPalette::Base, colorGradiantBrush);
    ui->colorbar2->setPalette(palette);

    ui->canvas2->clear();

    ui->canvas2->add_mesh(*meshes_canvas2.at(ui->t_slider->value()), show_poly_color);

    ui->canvas2->updateGL();
}

void SolverResultsGraphicWidget::clean_canvas()
{
    if (curr_mesh_id == cinolib::max_uint)
        return;

    // ui->solver_output->pop(results.at(curr_mesh_id));
    // ui->groundtruth->pop(groundtruth.at(curr_mesh_id));
}

cinolib::Polygonmesh<> *SolverResultsGraphicWidget::get_gt_mesh(const uint i)
{
    return meshes_canvas2.at(i);
}

void SolverResultsGraphicWidget::set_t_slider_value (const uint val)
{
    ui->t_slider->setValue(val);
}

void SolverResultsGraphicWidget::set_t_slider_min (const uint val)
{
    ui->t_slider->setMinimum(val);
}
void SolverResultsGraphicWidget::set_t_slider_max (const uint val)
{
    ui->t_slider->setMaximum(val);
}
