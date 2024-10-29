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

#ifndef MESHMETRICSWIDGET_H
#define MESHMETRICSWIDGET_H

#include "customizedchartview.h"
#include "dataset.h"

#include <QWidget>

#include <QtCharts/QChartView>

namespace Ui {
class MeshMetricsWidget;
}

class MeshMetricsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MeshMetricsWidget(QWidget *parent = nullptr);
    ~MeshMetricsWidget();

    void add_chart (CustomizedChartView *);

    void set_dataset (Dataset *d) { dataset = d; }
    void set_metrics (std::vector<MeshMetrics> *m) { metrics = m;}

    void reset ();

Q_SIGNALS:

    void sort_geometric_qualities (unsigned int id);
    void show_unsorted_metrics ();

private slots:

    void on_sort_btn_clicked();
    void on_original_btn_clicked();

    void on_show_legend_cb_stateChanged(int checked);
    void on_show_ranges_cb_stateChanged(int arg1);

    void on_min_color_btn_clicked();
    void on_max_color_btn_clicked();
    void on_avg_color_btn_clicked();
    void on_poly_min_color_btn_clicked();
    void on_poly_max_color_btn_clicked();
    void on_poly_avg_color_btn_clicked();
    void on_mesh_color_btn_clicked();

    void on_save_txt_btn_clicked();
    void on_save_plots_btn_clicked();

    void on_scale_dependent_cb_stateChanged(int arg1);

    void on_min_cb_stateChanged(int arg1);
    void on_max_cb_stateChanged(int arg1);
    void on_avg_cb_stateChanged(int arg1);
    void on_poly_min_cb_stateChanged(int arg1);
    void on_poly_max_cb_stateChanged(int arg1);
    void on_poly_avg_cb_stateChanged(int arg1);
    void on_mesh_cb_stateChanged(int arg1);
    void update_cb_stateChanged();

    void on_all_btn_clicked();
    void on_none_btn_clicked();

private:
    Ui::MeshMetricsWidget *ui;

    std::vector<CustomizedChartView *> chart_views;

    Dataset *dataset = nullptr;
    std::vector<MeshMetrics> *metrics = nullptr;

    unsigned int last_row = 0;
    unsigned int last_col = 0;
    const unsigned int n_cols = 3;

    void change_series_color (const int series_id);
    void show_series(const unsigned int series_id, const bool check);
};

#endif // MESHMETRICSWIDGET_H
