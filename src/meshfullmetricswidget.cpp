#include "meshfullmetricswidget.h"
#include "ui_meshfullmetricswidget.h"

#include "meshes/mesh_metrics.h"
#include "quality_metrics.h"
#include "sortgeometricqualitiesdialog.h"

#include <QColorDialog>
#include <QFileDialog>
#include <QtCharts/QLineSeries>

#include <iomanip>
#include <iostream>
#include <sstream>

MeshFullMetricsWidget::MeshFullMetricsWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MeshFullMetricsWidget)
{
    ui->setupUi(this);

    ui->splitter->setStretchFactor(0, 4);
}
MeshFullMetricsWidget::~MeshFullMetricsWidget() { delete ui; }

void MeshFullMetricsWidget::reset()
{
    last_row = 0;
    last_col = 0;

    for (CustomizedChartView *c : chart_views)
        delete c;

    chart_views.clear();
}

void MeshFullMetricsWidget::add_chart(CustomizedChartView *chart)
{
    chart->set_double_click_enabled(true);

    chart_views.push_back(chart);

    QGridLayout *l = ui->chart_grid_layout;

    std::cout << "Adding chart " << last_row << " -- " << last_col << std::endl;

    l->addWidget(chart, static_cast<int>(last_row), static_cast<int>(last_col));

    l->setRowStretch(static_cast<int>(last_row), 1);
    l->setColumnStretch(static_cast<int>(last_col),1);

    if (last_col == n_cols-1)
    {
        last_col = 0;
        last_row++;
    }
    else
    {
        last_col++;
    }

    // show_series(0,  ui->inr_cb->checkState());
    // show_series(1,  ui->our_cb->checkState());
    // show_series(2,  ui->cir_cb->checkState());
    // show_series(3,  ui->krr_cb->checkState());
    // show_series(4,  ui->kar_cb->checkState());
    // show_series(5,  ui->apr_cb->checkState());
    // show_series(6,  ui->mia_cb->checkState());
    // show_series(7,  ui->maa_cb->checkState());
    // show_series(8,  ui->anr_cb->checkState());
    // show_series(9,  ui->vem_cb->checkState());
    // show_series(10, ui->jac_cb->checkState());
    // show_series(11, ui->fro_cb->checkState());
}

void MeshFullMetricsWidget::on_inr_cb_stateChanged(int checked) { show_series(0, checked); }
void MeshFullMetricsWidget::on_our_cb_stateChanged(int checked) { show_series(1, checked); }
void MeshFullMetricsWidget::on_cir_cb_stateChanged(int checked) { show_series(2, checked); }
void MeshFullMetricsWidget::on_krr_cb_stateChanged(int checked) { show_series(3, checked); }
void MeshFullMetricsWidget::on_kar_cb_stateChanged(int checked) { show_series(4, checked); }
void MeshFullMetricsWidget::on_apr_cb_stateChanged(int checked) { show_series(5, checked); }
void MeshFullMetricsWidget::on_mia_cb_stateChanged(int checked) { show_series(6, checked); }
void MeshFullMetricsWidget::on_maa_cb_stateChanged(int checked) { show_series(7, checked); }
void MeshFullMetricsWidget::on_anr_cb_stateChanged(int checked) { show_series(8, checked); }
void MeshFullMetricsWidget::on_vem_cb_stateChanged(int checked) { show_series(9, checked); }
void MeshFullMetricsWidget::on_jac_cb_stateChanged(int checked) { show_series(10, checked); }
void MeshFullMetricsWidget::on_fro_cb_stateChanged(int checked) { show_series(11, checked); }

void MeshFullMetricsWidget::on_inr_color_cb_clicked() { change_series_color(0); }
void MeshFullMetricsWidget::on_our_color_cb_clicked() { change_series_color(1); }
void MeshFullMetricsWidget::on_cir_color_cb_clicked() { change_series_color(2); }
void MeshFullMetricsWidget::on_krr_color_cb_clicked() { change_series_color(3); }
void MeshFullMetricsWidget::on_kar_color_cb_clicked() { change_series_color(4); }
void MeshFullMetricsWidget::on_apr_color_cb_clicked() { change_series_color(5); }
void MeshFullMetricsWidget::on_mia_color_cb_clicked() { change_series_color(6); }
void MeshFullMetricsWidget::on_maa_color_cb_clicked() { change_series_color(7); }
void MeshFullMetricsWidget::on_anr_color_cb_clicked() { change_series_color(8); }
void MeshFullMetricsWidget::on_vem_color_cb_clicked() { change_series_color(9); }
void MeshFullMetricsWidget::on_jac_color_cb_clicked() { change_series_color(10); }
void MeshFullMetricsWidget::on_fro_color_cb_clicked() { change_series_color(11); }

void MeshFullMetricsWidget::on_save_txt_btn_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select Directory"),
                                                    "/tmp",
                                                    QFileDialog::ShowDirsOnly
                                                        | QFileDialog::DontResolveSymlinks);

    if (dir.isNull()) return;

    for (uint i=0; i < dataset->get_parametric_meshes().size(); i++)
    {
        std::string filename ;

        std::cout << dataset->get_parametric_mesh(i)->mesh_data().filename << std::endl;

        if (dataset->is_on_disk())
        {
            std::string fn = dataset->get_parametric_mesh(i)->mesh_data().filename;
            std::string fname = fn.substr(fn.find_last_of(QDir::separator().toLatin1()) +1);

            std::cout << fname << std::endl;

            std::string filename_ne = fname.substr(0, fname.find_last_of("."));

            std::cout << filename_ne << std::endl;

            filename = dir.toStdString() + QString(QDir::separator()).toStdString() +
                       filename_ne + ".txt";
        }
        else
        {
            std::stringstream ss;
            ss << std::setw((dataset->get_parametric_meshes().size() / 10) + 1)
               << std::setfill('0') << i;
            std::string s = ss.str();

            std::string filename_prefix = ss.str() + "_";

            filename = dir.toStdString() + QString(QDir::separator()).toStdString() +
                       filename_prefix + ".txt";
        }

        std::cout << "Saving " << filename << std::endl;

        save_to_file(filename.c_str(), metrics->at(i));
    }
}

void MeshFullMetricsWidget::on_save_plots_btn_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select Directory"),
                                                    "/tmp",
                                                    QFileDialog::ShowDirsOnly
                                                        | QFileDialog::DontResolveSymlinks);

    if (dir.isNull()) return;
    std::string filename = dir.toStdString() + QString(QDir::separator()).toStdString()
                                             + "all_metrics.png";

    QPixmap p = chart_views.at(0)->grab();
    p.save(filename.c_str());
}

void MeshFullMetricsWidget::change_series_color(const int series_id)
{
    QLineSeries *series = static_cast<QLineSeries *> (chart_views.at(0)->chart()->series().at(series_id));
    QPen pen = series->pen();
    QColor prev_color = pen.brush().color();

    QColorDialog *colorDialog = new QColorDialog(this);
    QColor color = colorDialog->getColor(prev_color, this, "Effect Color", QColorDialog::DontUseNativeDialog);

    if (color == nullptr) return;

    for (CustomizedChartView *chart : chart_views)
    {
        QLineSeries *series = static_cast<QLineSeries *> (chart->chart()->series().at(series_id));

        QPen pen = series->pen();
        pen.setWidth(1);
        pen.setBrush(QBrush(color)); // or just pen.setColor("red");
        series->setPen(pen);
    }
}

void MeshFullMetricsWidget::show_series(const uint series_id, const bool checked)
{
    for (CustomizedChartView *chart : chart_views)
    {
        QLineSeries *series = static_cast<QLineSeries *> (chart->chart()->series().at(static_cast<int>(series_id)));

        if (checked) series->show();
        else series->hide();
    }
}

void MeshFullMetricsWidget::on_all_btn_clicked()
{
    ui->inr_cb->setCheckState(Qt::Checked);
    ui->our_cb->setCheckState(Qt::Checked);
    ui->cir_cb->setCheckState(Qt::Checked);
    ui->krr_cb->setCheckState(Qt::Checked);
    ui->kar_cb->setCheckState(Qt::Checked);
    ui->apr_cb->setCheckState(Qt::Checked);
    ui->mia_cb->setCheckState(Qt::Checked);
    ui->maa_cb->setCheckState(Qt::Checked);
    ui->anr_cb->setCheckState(Qt::Checked);
    ui->vem_cb->setCheckState(Qt::Checked);
    ui->jac_cb->setCheckState(Qt::Checked);
    ui->fro_cb->setCheckState(Qt::Checked);
}


void MeshFullMetricsWidget::on_none_btn_clicked()
{
    ui->inr_cb->setCheckState(Qt::Unchecked);
    ui->our_cb->setCheckState(Qt::Unchecked);
    ui->cir_cb->setCheckState(Qt::Unchecked);
    ui->krr_cb->setCheckState(Qt::Unchecked);
    ui->kar_cb->setCheckState(Qt::Unchecked);
    ui->apr_cb->setCheckState(Qt::Unchecked);
    ui->mia_cb->setCheckState(Qt::Unchecked);
    ui->maa_cb->setCheckState(Qt::Unchecked);
    ui->anr_cb->setCheckState(Qt::Unchecked);
    ui->vem_cb->setCheckState(Qt::Unchecked);
    ui->jac_cb->setCheckState(Qt::Unchecked);
    ui->fro_cb->setCheckState(Qt::Unchecked);
}

