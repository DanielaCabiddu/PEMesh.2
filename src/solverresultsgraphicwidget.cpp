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

    update_scene = false;

    curr_mesh_id = static_cast<uint>(index);
}

void SolverResultsGraphicWidget::on_t_slider_valueChanged(int value)
{
    if (value >= 0)
        show_parametric_mesh(value);

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
        break;
    case 2:
        for (uint m=0; m < meshes_canvas1.size(); m++)
        {
            for (uint pid=0; pid < meshes_canvas1.at(m)->num_polys(); pid++)
            {
                cinolib::Color c = cinolib::Color::red_white_blue_ramp_01(errH1s.at(m).data()[pid]);
                meshes_canvas1.at(m)->poly_data(pid).color = c;
                colorGradient.setColorAt(errH1s.at(m).data()[pid], QColor(c.r * 255, c.g * 255, c.b * 255));
            }
        }
        show_poly_color = true;
        break;
    case 3:
        for (uint m=0; m < meshes_canvas1.size(); m++)
        {
            for (uint pid=0; pid < meshes_canvas1.at(m)->num_polys(); pid++)
            {
                cinolib::Color c = cinolib::Color::red_white_blue_ramp_01(errL2s.at(m).data()[pid]);
                meshes_canvas1.at(m)->poly_data(pid).color = c;
                colorGradient.setColorAt(errL2s.at(m).data()[pid], QColor(c.r * 255, c.g * 255, c.b * 255));
            }
        }
        show_poly_color = true;
        break;
    case 4:
        for (uint m=0; m < meshes_canvas1.size(); m++)
        {
            for (uint pid=0; pid < meshes_canvas1.at(m)->num_polys(); pid++)
            {
                cinolib::Color c = cinolib::Color::red_white_blue_ramp_01(Conds.at(m).data()[pid]);
                meshes_canvas1.at(m)->poly_data(pid).color = c;
                colorGradient.setColorAt(Conds.at(m).data()[pid], QColor(c.r * 255, c.g * 255, c.b * 255));
            }
        }
        show_poly_color = true;
        break;
    default:
        break;
    }

    ui->canvas1->clear();

    ui->canvas1->add_mesh(*meshes_canvas1.at(ui->t_slider->value()), show_poly_color);

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
        break;
    case 2:
        for (uint m=0; m < meshes_canvas2.size(); m++)
        {
            for (uint pid=0; pid < meshes_canvas2.at(m)->num_polys(); pid++)
            {
                cinolib::Color c = cinolib::Color::red_white_blue_ramp_01(errH1s.at(m).data()[pid]);
                meshes_canvas2.at(m)->poly_data(pid).color = c;
                colorGradient.setColorAt(errH1s.at(m).data()[pid], QColor(c.r * 255, c.g * 255, c.b * 255));
            }
        }
        show_poly_color = true;
        break;
    case 3:
        for (uint m=0; m < meshes_canvas2.size(); m++)
        {
            for (uint pid=0; pid < meshes_canvas2.at(m)->num_polys(); pid++)
            {
                cinolib::Color c = cinolib::Color::red_white_blue_ramp_01(errL2s.at(m).data()[pid]);
                meshes_canvas2.at(m)->poly_data(pid).color = c;
                colorGradient.setColorAt(errL2s.at(m).data()[pid], QColor(c.r * 255, c.g * 255, c.b * 255));
            }
        }
        show_poly_color = true;
        break;
    case 4:
        for (uint m=0; m < meshes_canvas2.size(); m++)
        {
            for (uint pid=0; pid < meshes_canvas2.at(m)->num_polys(); pid++)
            {
                cinolib::Color c = cinolib::Color::red_white_blue_ramp_01(Conds.at(m).data()[pid]);
                meshes_canvas2.at(m)->poly_data(pid).color = c;
                colorGradient.setColorAt(Conds.at(m).data()[pid], QColor(c.r * 255, c.g * 255, c.b * 255));
            }
        }
        show_poly_color = true;
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
