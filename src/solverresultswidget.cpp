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

#include "solverresultswidget.h"
#include "ui_solverresultswidget.h"

#include <QFileDialog>
#include <QRubberBand>

#include <QtCharts/QLineSeries>

SolverResultsWidget::SolverResultsWidget(QWidget *parent)
    : QWidget(parent), ui(new Ui::SolverResultsWidget)
{
  ui->setupUi(this);

#ifndef DEVELOP

  ui->deformation_cb->hide();
  ui->meshsize_cb->hide();

#endif
}

SolverResultsWidget::~SolverResultsWidget() { delete ui; }

void SolverResultsWidget::set_dataset(Dataset *d)
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

  ui->t_slider->setMaximum(static_cast<int>(d->get_parametric_meshes().size()) -
                           1);

  if (d->get_parametric_meshes().size() > 0)
    show_parametric_mesh(0);
}

void SolverResultsWidget::show_mesh_solution_and_groundtruth() const
{
  for (uint i = 0; i < meshes_canvas1.size(); i++) {

    //ui->solver_output->add_mesh(*results.at(i), true);
    //ui->solver_output->add_mesh(*groundtruth.at(i), true);

    // const int tex_type = cinolib::TEXTURE_1D_HSV;
    // results.at(i)->show_texture1D(tex_type);
    // groundtruth.at(i)->show_texture1D(tex_type);

    // results.at(i)->show_vert_color();
    // groundtruth.at(i)->show_vert_color();
  }
}

void SolverResultsWidget::add_chart(CustomizedChartView *chart,
                                    const std::string name)
{
  chart->set_double_click_enabled(true);
  chart_views.push_back(chart);
  chart_views_names.push_back(name);

  QGridLayout *l = ui->gridLayout;

  std::cout << "Adding chart " << last_row << " -- " << last_col << std::endl;

  l->addWidget(chart, static_cast<int>(last_row), static_cast<int>(last_col));

  l->setRowStretch(static_cast<int>(last_row), 1);
  l->setColumnStretch(static_cast<int>(last_col), 1);

  if (last_col == n_cols - 1) {
    last_col = 0;
    last_row++;
  } else {
    last_col++;
  }

  QGraphicsLineItem *line = new QGraphicsLineItem(chart->chart());

  QLineSeries *series =
      static_cast<QLineSeries *>(chart->chart()->series().at(0));

  QValueAxis *axis = static_cast<QValueAxis *>(series->attachedAxes().at(1));

  QPointF a(series->at(0).x(), axis->min());
  QPointF b(series->at(0).x(), axis->max());

  QPointF aa = chart->chart()->mapToPosition(a);
  QPointF bb = chart->chart()->mapToPosition(b);

  line->setLine(aa.x(), aa.y(), bb.x(), bb.y());
  line->setVisible(true);

  track_lines.push_back(line);
}

void SolverResultsWidget::clean_charts() {
  QGridLayout *l = ui->gridLayout;

  for (uint i = 0; i < chart_views.size(); i++) {
    l->removeWidget(chart_views.at(i));
    delete chart_views.at(i);

    //        delete track_lines.at(i);
  }

  last_row = 0;
  last_col = 0;

  chart_views.clear();
  chart_views_names.clear();
  track_lines.clear();
}

void SolverResultsWidget::clean_canvas()
{
  if (curr_mesh_id == cinolib::max_uint)
    return;

  // ui->solver_output->pop(results.at(curr_mesh_id));
  // ui->groundtruth->pop(groundtruth.at(curr_mesh_id));
}

void SolverResultsWidget::show_parametric_mesh(int index)
{
  clean_canvas();

  cinolib::Polygonmesh<> *p_r = meshes_canvas1.at(static_cast<uint>(index));
  cinolib::Polygonmesh<> *p_gt = meshes_canvas2.at(static_cast<uint>(index));

  // p_r->updateGL();
  // p_gt->updateGL();

  // ui->solver_output->push_obj(p_r, update_scene);
  // ui->solver_output->updateGL();

  // ui->groundtruth->push_obj(p_gt, update_scene);
  // ui->groundtruth->updateGL();

  ui->canvas1->clear();
  ui->canvas2->clear();

  ui->canvas1->add_mesh(*p_r, false);
  ui->canvas2->add_mesh(*p_gt, false);

  update_scene = false;

  curr_mesh_id = static_cast<uint>(index);
}

void SolverResultsWidget::on_t_slider_valueChanged(int value)
{
  if (value >= 0)
    show_parametric_mesh(value);

  for (uint c = 0; c < chart_views.size(); c++) {
    QGraphicsLineItem *line = track_lines.at(c);

    QLineSeries *series =
        static_cast<QLineSeries *>(chart_views.at(c)->chart()->series().at(0));

    QValueAxis *axis = static_cast<QValueAxis *>(series->attachedAxes().at(1));

    QPointF a(series->at(value).x(), axis->min());
    QPointF b(series->at(value).x(), axis->max());

    QPointF aa = chart_views.at(c)->chart()->mapToPosition(a);
    QPointF bb = chart_views.at(c)->chart()->mapToPosition(b);

    line->setLine(aa.x(), aa.y(), bb.x(), bb.y());
  }
}

void SolverResultsWidget::on_save_plots_btn_clicked()
{
  QString dir = QFileDialog::getExistingDirectory(
      this, tr("Select Directory"), "/tmp",
      QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

  if (dir.isNull())
    return;

  for (uint i = 0; i < chart_views.size(); i++) {
    std::string filename = dir.toStdString() +
                           QString(QDir::separator()).toStdString() +
                           chart_views_names.at(i) + ".png";

    QPixmap p = chart_views.at(i)->grab();
    p.save(filename.c_str());
  }
}

void SolverResultsWidget::change_series_color(const uint series_id)
{
  QLineSeries *series = static_cast<QLineSeries *>(
      chart_views.at(series_id)->chart()->series().at(0));
  QPen pen = series->pen();
  QColor prev_color = pen.brush().color();

  // QColorDialog *colorDialog = new QColorDialog(this);
  // QColor color = colorDialog->getColor(prev_color, this, "Effect Color",
  //                                      QColorDialog::DontUseNativeDialog);

  // if (color == nullptr)
  //   return;

  // pen.setBrush(QBrush(color)); // or just pen.setColor("red");
  series->setPen(pen);
}

void SolverResultsWidget::on_errS_color_btn_clicked()
{
  change_series_color(0);
}

void SolverResultsWidget::on_errInf_color_btn_clicked()
{
  change_series_color(1);
}

void SolverResultsWidget::on_errL2_color_btn_clicked()
{
  change_series_color(2);
}

void SolverResultsWidget::on_hEmax_color_btn_clicked() {
  change_series_color(3);
}

void SolverResultsWidget::on_condVect_color_btn_clicked()
{
  change_series_color(4);
}

void SolverResultsWidget::on_all_color_btn_clicked()
{
  for (uint i = 0; i < chart_views.size(); i++)
    change_series_color(i);
}

void SolverResultsWidget::on_track_t_cb_stateChanged(int checked)
{
  for (uint c = 0; c < track_lines.size(); c++) {
    track_lines.at(c)->setVisible(checked);
  }
}

void SolverResultsWidget::on_loglog_cb_stateChanged(int checked)
{
  for (uint series_id = 0; series_id < 5; series_id++) {
    QLineSeries *series = static_cast<QLineSeries *>(
        chart_views.at(series_id)->chart()->series().at(0));

    for (auto s : series->pointsVector()) {
      if (checked) {
        s.setX(log(s.x()));
        s.setY(log(s.y()));
      } else {
        s.setX(exp(s.x()));
        s.setY(exp(s.y()));
      }
    }
  }
}

void SolverResultsWidget::on_deformation_cb_stateChanged(int checked)
{
  //    for (uint series_id=0; series_id<5; series_id++)
  //    {
  //        QLineSeries *series = static_cast<QLineSeries *>
  //        (chart_views.at(series_id)->chart()->series().at(0));

  //    }
  ui->meshsize_cb->setChecked(!checked);
}

void SolverResultsWidget::on_meshsize_cb_stateChanged(int checked)
{
  //    for (uint series_id=0; series_id<5; series_id++)
  //    {
  //        QLineSeries *series = static_cast<QLineSeries *>
  //        (chart_views.at(series_id)->chart()->series().at(0));

  //        std::vector<std::pair<double,double>> pairs;
  //        auto m = dataset.get_parametric_meshes_metrics();
  //        if(m.size()!=0)
  //        {
  //            for (uint v=0; v < errs.at(i).size(); v++)
  //            {
  //                double e = std::min(m.at(v).SE_min, m.at(v).SE_poly_min);
  //                pairs.push_back(std::pair<double,double> (e,
  //                errs.at(i).at(v)));
  //            }
  //            sort(pairs.begin(), pairs.end());

  //            for(uint i=0; i<pairs.size(); i++)
  //                series->append(pairs.at(i).first, pairs.at(i).second);
  //        }

  //    }
  ui->deformation_cb->setChecked(!checked);
}

cinolib::Polygonmesh<> *SolverResultsWidget::get_gt_mesh(const uint i)
{
  return meshes_canvas2.at(i);
}

void SolverResultsWidget::on_canvas1_cb_currentIndexChanged(int index)
{
    bool show_poly_color= true;

    switch (index)
    {
    case 0:
        for (uint m=0; m < meshes_canvas1.size(); m++)
        {
            solutions.at(m).copy_to_mesh(*meshes_canvas1.at(m));
        }
        show_poly_color=false;
        break;
    case 1:
        for (uint m=0; m < meshes_canvas1.size(); m++)
        {
            groundtruths.at(m).copy_to_mesh(*meshes_canvas1.at(m));
        }
        show_poly_color=false;
        break;
    case 2:
        for (uint m=0; m < meshes_canvas1.size(); m++)
        {
            errH1s.at(m).copy_to_mesh(*meshes_canvas1.at(m));
        }
        show_poly_color = true;
        break;
    case 3:
        for (uint m=0; m < meshes_canvas1.size(); m++)
        {
            errL2s.at(m).copy_to_mesh(*meshes_canvas1.at(m));
        }
        show_poly_color = true;
        break;
    default:
        break;
    }

    ui->canvas1->clear();

    ui->canvas1->add_mesh(*meshes_canvas1.at(ui->t_slider->value()), show_poly_color);

    ui->canvas1->updateGL();
}


void SolverResultsWidget::on_canvas2_cb_currentIndexChanged(int index)
{
    bool show_poly_color= true;

    switch (index)
    {
    case 0:
        for (uint m=0; m < meshes_canvas2.size(); m++)
        {
            solutions.at(m).copy_to_mesh(*meshes_canvas2.at(m));
        }
        show_poly_color = false;
        break;
    case 1:
        for (uint m=0; m < meshes_canvas2.size(); m++)
        {
            groundtruths.at(m).copy_to_mesh(*meshes_canvas2.at(m));
        }
        show_poly_color = false;
        break;
    case 2:
        for (uint m=0; m < meshes_canvas2.size(); m++)
        {
            errH1s.at(m).copy_to_mesh(*meshes_canvas2.at(m));
        }
        show_poly_color = true;
        break;
    case 3:
        for (uint m=0; m < meshes_canvas2.size(); m++)
        {
            errL2s.at(m).copy_to_mesh(*meshes_canvas2.at(m));
        }
        show_poly_color = true;
        break;
    default:
        break;
    }

    ui->canvas2->clear();

    ui->canvas2->add_mesh(*meshes_canvas2.at(ui->t_slider->value()), show_poly_color);

    ui->canvas2->updateGL();
}

