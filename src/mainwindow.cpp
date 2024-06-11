/********************************************************************************
*  This file is part of PEMesh                                             *
*  Copyright(C) 2020: Daniela Cabiddu                                           *
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
*********************************************************************************/

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <cinolib/scalar_field.h>

#include <QDir>
#include <QFileDialog>
#include <QJsonArray>
#include <QJsonDocument>

#include <QtCharts/QChartView>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QLogValueAxis>
#include <QtCharts/QValueAxis>
QT_CHARTS_USE_NAMESPACE

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowState(Qt::WindowMaximized);

    for (int i=1; i < ui->tab_widgets->count(); i++)
        ui->tab_widgets->setTabEnabled(i, false);

    ui->datasetWidget->set_dataset(&dataset);
    ui->graphicMeshMetricWidget->set_dataset(&dataset);
    ui->solverWidget->set_dataset(&dataset);

    connect(ui->datasetWidget, SIGNAL (computed_mesh_metrics()), this, SLOT(show_mesh_metrics()));
    connect(ui->metricsWidget, SIGNAL (sort_geometric_qualities(const uint)), this, SLOT(show_sorted_mesh_metrics(const uint)));
    connect(ui->metricsWidget, SIGNAL (show_unsorted_metrics()), this, SLOT(show_mesh_metrics()));
    connect(ui->solverWidget, SIGNAL (solver_completed (const uint, const std::string, const std::string)), this, SLOT (show_solver_results (const uint, const std::string, const std::string)));
    connect(ui->datasetWidget, SIGNAL (saved_in(const std::string)), this, SLOT (update_solver_input_folder (const std::string)));

    connect(ui->scatterPlotsGPWidget, SIGNAL(compute_GP_scatterplots()), this, SLOT(compute_GP_scatterplots()));

    setWindowIcon(QIcon(":/logo/img/erc_logo.png"));

    extra_colors.push_back(QColor(255, 192, 203));
    extra_colors.push_back(QColor(211, 211, 211));
    extra_colors.push_back(QColor(0, 0, 139));
    extra_colors.push_back(QColor(34,139,34));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::show_mesh_metrics()
{
    ui->tab_widgets->setCurrentIndex(1);
    ui->tab_widgets->setTabEnabled(1, true);
    ui->metricsWidget->set_dataset(&dataset);
    ui->metricsWidget->set_metrics(&metrics);
    ui->graphicMeshMetricWidget->clean_canvas();

    metrics = ui->datasetWidget->get_parametric_meshes_metrics();
    ui->graphicMeshMetricWidget->set_metrics(&metrics);
    ui->graphicMeshMetricWidget->set_slider_max(dataset.get_parametric_meshes().size()-1);

    ui->graphicMeshMetricWidget->show_mesh(0);
    ui->metricsWidget->reset();

    for (uint i=0; i < n_metrics; i++)
    {
        QLineSeries *series_min = new QLineSeries();
        QLineSeries *series_max = new QLineSeries();
        QLineSeries *series_avg = new QLineSeries();

        QLineSeries *series_poly_min = new QLineSeries();
        QLineSeries *series_poly_max = new QLineSeries();
        QLineSeries *series_poly_avg = new QLineSeries();

        series_poly_min->setColor(extra_colors.at(0));
        series_poly_max->setColor(extra_colors.at(1));
        series_poly_avg->setColor(extra_colors.at(2));

        series_min->setName("Min");
        series_max->setName("Max");
        series_avg->setName("Avg");

        series_poly_min->setName("Poly Min");
        series_poly_max->setName("Poly Max");
        series_poly_avg->setName("Poly Avg");

        QChart *chart = new QChart();

        for (uint m=0; m < metrics.size(); m++)
        {
            double val_min = 0.0;
            double val_max = 0.0;
            double val_avg = 0.0;

            double val_poly_min = 0.0;
            double val_poly_max = 0.0;
            double val_poly_avg = 0.0;

            uint min_id = 0;
            uint max_id = 0;
            uint min_poly_id = 0;
            uint max_poly_id = 0;

            switch (i)
            {
            case 0: val_min = metrics.at(m).IC_min;
                val_max = metrics.at(m).IC_max;
                val_avg = metrics.at(m).IC_avg;
                val_poly_min = metrics.at(m).IC_poly_min;
                val_poly_max = metrics.at(m).IC_poly_max;
                val_poly_avg = metrics.at(m).IC_poly_avg;
                min_id = metrics.at(m).IC_min_id;
                max_id = metrics.at(m).IC_max_id;
                min_poly_id = metrics.at(m).IC_poly_min_id;
                max_poly_id = metrics.at(m).IC_poly_max_id;
                break;
            case 1: val_min = metrics.at(m).CC_min;
                val_max = metrics.at(m).CC_max;
                val_avg = metrics.at(m).CC_avg;
                val_poly_min = metrics.at(m).CC_poly_min;
                val_poly_max = metrics.at(m).CC_poly_max;
                val_poly_avg = metrics.at(m).CC_poly_avg;
                min_id = metrics.at(m).CC_min_id;
                max_id = metrics.at(m).CC_max_id;
                min_poly_id = metrics.at(m).CC_poly_min_id;
                max_poly_id = metrics.at(m).CC_poly_max_id;
                break;
            case 2: val_min = metrics.at(m).CR_min;
                val_max = metrics.at(m).CR_max;
                val_avg = metrics.at(m).CR_avg;
                val_poly_min = metrics.at(m).CR_poly_min;
                val_poly_max = metrics.at(m).CR_poly_max;
                val_poly_avg = metrics.at(m).CR_poly_avg;
                min_id = metrics.at(m).CR_min_id;
                max_id = metrics.at(m).CR_max_id;
                min_poly_id = metrics.at(m).CR_poly_min_id;
                max_poly_id = metrics.at(m).CR_poly_max_id;
                break;
            case 3: val_min = metrics.at(m).AR_min;
                val_max = metrics.at(m).AR_max;
                val_avg = metrics.at(m).AR_avg;
                val_poly_min = metrics.at(m).AR_poly_min;
                val_poly_max = metrics.at(m).AR_poly_max;
                val_poly_avg = metrics.at(m).AR_poly_avg;
                min_id = metrics.at(m).AR_min_id;
                max_id = metrics.at(m).AR_max_id;
                min_poly_id = metrics.at(m).AR_poly_min_id;
                max_poly_id = metrics.at(m).AR_poly_max_id;
                break;
            case 4: val_min = metrics.at(m).KE_min;
                val_max = metrics.at(m).KE_max;
                val_avg = metrics.at(m).KE_avg;
                val_poly_min = metrics.at(m).KE_poly_min;
                val_poly_max = metrics.at(m).KE_poly_max;
                val_poly_avg = metrics.at(m).KE_poly_avg;
                min_id = metrics.at(m).KE_min_id;
                max_id = metrics.at(m).KE_max_id;
                min_poly_id = metrics.at(m).KE_poly_min_id;
                max_poly_id = metrics.at(m).KE_poly_max_id;
                break;
            case 5: val_min = metrics.at(m).KAR_min;
                val_max = metrics.at(m).KAR_max;
                val_avg = metrics.at(m).KAR_avg;
                val_poly_min = metrics.at(m).KAR_poly_min;
                val_poly_max = metrics.at(m).KAR_poly_max;
                val_poly_avg = metrics.at(m).KAR_poly_avg;
                min_id = metrics.at(m).KAR_min_id;
                max_id = metrics.at(m).KAR_max_id;
                min_poly_id = metrics.at(m).KAR_poly_min_id;
                max_poly_id = metrics.at(m).KAR_poly_max_id;
                break;
            case 6: val_min = metrics.at(m).APR_min;
                val_max = metrics.at(m).APR_max;
                val_avg = metrics.at(m).APR_avg;
                val_poly_min = metrics.at(m).APR_poly_min;
                val_poly_max = metrics.at(m).APR_poly_max;
                val_poly_avg = metrics.at(m).APR_poly_avg;
                min_id = metrics.at(m).APR_min_id;
                max_id = metrics.at(m).APR_max_id;
                min_poly_id = metrics.at(m).APR_poly_min_id;
                max_poly_id = metrics.at(m).APR_poly_max_id;
                break;
            case 7: val_min = metrics.at(m).MA_min;
                val_max = metrics.at(m).MA_max;
                val_avg = metrics.at(m).MA_avg;
                val_poly_min = metrics.at(m).MA_poly_min;
                val_poly_max = metrics.at(m).MA_poly_max;
                val_poly_avg = metrics.at(m).MA_poly_avg;
                min_id = metrics.at(m).MA_min_id;
                max_id = metrics.at(m).MA_max_id;
                min_poly_id = metrics.at(m).MA_poly_min_id;
                max_poly_id = metrics.at(m).MA_poly_max_id;
                break;
            case 8: val_min = metrics.at(m).SE_min;
                val_max = metrics.at(m).SE_max;
                val_avg = metrics.at(m).SE_avg;
                val_poly_min = metrics.at(m).SE_poly_min;
                val_poly_max = metrics.at(m).SE_poly_max;
                val_poly_avg = metrics.at(m).SE_poly_avg;
                min_id = metrics.at(m).SE_min_id;
                max_id = metrics.at(m).SE_max_id;
                min_poly_id = metrics.at(m).SE_poly_min_id;
                max_poly_id = metrics.at(m).SE_poly_max_id;
                break;
            case 9: val_min = metrics.at(m).ER_min;
                val_max = metrics.at(m).ER_max;
                val_avg = metrics.at(m).ER_avg;
                val_poly_min = metrics.at(m).ER_poly_min;
                val_poly_max = metrics.at(m).ER_poly_max;
                val_poly_avg = metrics.at(m).ER_poly_avg;
                min_id = metrics.at(m).ER_min_id;
                max_id = metrics.at(m).ER_max_id;
                min_poly_id = metrics.at(m).ER_poly_min_id;
                max_poly_id = metrics.at(m).ER_poly_max_id;
                break;
            case 10: val_min = metrics.at(m).MPD_min;
                val_max = metrics.at(m).MPD_max;
                val_avg = metrics.at(m).MPD_avg;
                val_poly_min = metrics.at(m).MPD_poly_min;
                val_poly_max = metrics.at(m).MPD_poly_max;
                val_poly_avg = metrics.at(m).MPD_poly_avg;
                min_id = metrics.at(m).MPD_min_id;
                max_id = metrics.at(m).MPD_max_id;
                min_poly_id = metrics.at(m).MPD_poly_min_id;
                max_poly_id = metrics.at(m).MPD_poly_max_id;
                break;
            case 11: val_min = metrics.at(m).NS_min;
                val_max = metrics.at(m).NS_max;
                val_avg = metrics.at(m).NS_avg;
                val_poly_min = metrics.at(m).NS_poly_min;
                val_poly_max = metrics.at(m).NS_poly_max;
                val_poly_avg = metrics.at(m).NS_poly_avg;
                min_id = metrics.at(m).NS_min_id;
                max_id = metrics.at(m).NS_max_id;
                min_poly_id = metrics.at(m).NS_poly_min_id;
                max_poly_id = metrics.at(m).NS_poly_max_id;
                break;
            case 12: val_min = metrics.at(m).MXA_min;
                val_max = metrics.at(m).MXA_max;
                val_avg = metrics.at(m).MXA_avg;
                val_poly_min = metrics.at(m).MXA_poly_min;
                val_poly_max = metrics.at(m).MXA_poly_max;
                val_poly_avg = metrics.at(m).MXA_poly_avg;
                min_id = metrics.at(m).MXA_min_id;
                max_id = metrics.at(m).MXA_max_id;
                min_poly_id = metrics.at(m).MXA_poly_min_id;
                max_poly_id = metrics.at(m).MXA_poly_max_id;
                break;
            case 13: val_min = metrics.at(m).SR_min;
                val_max = metrics.at(m).SR_max;
                val_avg = metrics.at(m).SR_avg;
                val_poly_min = metrics.at(m).SR_poly_min;
                val_poly_max = metrics.at(m).SR_poly_max;
                val_poly_avg = metrics.at(m).SR_poly_avg;
                min_id = metrics.at(m).SR_min_id;
                max_id = metrics.at(m).SR_max_id;
                min_poly_id = metrics.at(m).SR_poly_min_id;
                max_poly_id = metrics.at(m).SR_poly_max_id;
                break;
            case 14: val_min = std::max(metrics.at(m).VEM_max, metrics.at(m).VEM_poly_max);
                val_max = std::max(metrics.at(m).VEM_max, metrics.at(m).VEM_poly_max);
                val_avg = std::max(metrics.at(m).VEM_max, metrics.at(m).VEM_poly_max);
                val_poly_min = std::max(metrics.at(m).VEM_max, metrics.at(m).VEM_poly_max);
                val_poly_max = std::max(metrics.at(m).VEM_max, metrics.at(m).VEM_poly_max);
                val_poly_avg = std::max(metrics.at(m).VEM_max, metrics.at(m).VEM_poly_max);
                min_id = metrics.at(m).VEM_min_id;
                max_id = metrics.at(m).VEM_max_id;
                min_poly_id = metrics.at(m).VEM_poly_min_id;
                max_poly_id = metrics.at(m).VEM_poly_max_id;
                break;
            case 15: val_min = sqrt(metrics.at(m).VEMA_sum + metrics.at(m).VEMA_poly_sum);
                val_max = sqrt(metrics.at(m).VEMA_sum + metrics.at(m).VEMA_poly_sum);
                val_avg = sqrt(metrics.at(m).VEMA_sum + metrics.at(m).VEMA_poly_sum);
                val_poly_min = sqrt(metrics.at(m).VEMA_sum + metrics.at(m).VEMA_poly_sum);
                val_poly_max = sqrt(metrics.at(m).VEMA_sum + metrics.at(m).VEMA_poly_sum);
                val_poly_avg = sqrt(metrics.at(m).VEMA_sum + metrics.at(m).VEMA_poly_sum);
                min_id = metrics.at(m).VEMA_min_id;
                max_id = metrics.at(m).VEMA_max_id;
                min_poly_id = metrics.at(m).VEMA_poly_min_id;
                max_poly_id = metrics.at(m).VEMA_poly_max_id;
                break;
            default:
                std::cerr << "Invalid metric index" << std::endl;
            }

            if (min_id < UINT_MAX)
                series_min->append(m, val_min);

            if (max_id < UINT_MAX)
                series_max->append(m, val_max);

            if (min_id < UINT_MAX && max_id < UINT_MAX)
                series_avg->append(m, val_avg);

            if (min_poly_id < UINT_MAX)
                series_poly_min->append(m, val_poly_min);

            if (max_poly_id < UINT_MAX)
                series_poly_max->append(m, val_poly_max);

            if (min_poly_id < UINT_MAX && max_poly_id < UINT_MAX)
                series_poly_avg->append(m, val_poly_avg);
        }

        chart->legend()->hide();

        series_min->setPointsVisible();
        series_max->setPointsVisible();
        series_avg->setPointsVisible();

        series_poly_min->setPointsVisible();
        series_poly_max->setPointsVisible();
        series_poly_avg->setPointsVisible();

        chart->addSeries(series_min);
        chart->addSeries(series_max);
        chart->addSeries(series_avg);

        chart->addSeries(series_poly_min);
        chart->addSeries(series_poly_max);
        chart->addSeries(series_poly_avg);

//        QLogValueAxis *axisXlog = new QLogValueAxis();
//        axisXlog->setLabelFormat("%g");
//        axisXlog->setBase(8.0);
//        axisXlog->setMinorTickCount(-1);
//        chart->addAxis(axisXlog, Qt::AlignBottom);

//        QLogValueAxis *axisYlog = new QLogValueAxis();
//        axisYlog->setLabelFormat("%g");
//        axisYlog->setBase(8.0);
//        axisYlog->setMinorTickCount(-1);
//        chart->addAxis(axisYlog, Qt::AlignLeft);


//        series_min->attachAxis(axisXlog);
//        series_max->attachAxis(axisYlog);
//        series_avg->attachAxis(axisYlog);
//        series_poly_min->attachAxis(axisYlog);
//        series_poly_max->attachAxis(axisYlog);
//        series_poly_avg->attachAxis(axisYlog);

        chart->createDefaultAxes();
        chart->legend()->setAlignment(Qt::AlignRight);

        static_cast<QValueAxis *>(chart->axisX())->setLabelFormat("%i");
        static_cast<QValueAxis *>(chart->axisX())->setMinorTickCount(10);

        double min = get_metrics_min(i);
        double max = get_metrics_max(i);

        std::string max_str = std::to_string(max);
        if (max == cinolib::max_double) max_str = "+inf";
        else if (max == 0)  max_str = "0";
        else if (max == 1)  max_str = "1";

        std::string min_str = std::to_string(min);
        if (min == 0)       min_str = "0";
        else if (min == 1)  min_str = "1";

        std::string title = metrics_names.at(i) +
                            " ["+min_str+", "+max_str+"]";
        chart->setTitle(title.c_str());

        CustomizedChartView *chartView = new CustomizedChartView();
        chartView->setChart(chart);
        chartView->setBackgroundBrush(QColor (230, 230, 230));
        chartView->setRenderHint(QPainter::Antialiasing);

        ui->metricsWidget->add_chart(chartView);
    }

    ui->scatterPlotsGGWidget->create_scatterPlots(dataset, metrics);
}

void MainWindow::show_sorted_mesh_metrics(const uint to_be_sort_id)
{
    ui->tab_widgets->setCurrentIndex(1);
    metrics = ui->datasetWidget->get_parametric_meshes_metrics();

    ui->metricsWidget->reset();

    //const int to_be_sort_id = 0; /// da chiedere all'utente
    std::string title = "";

    std::multimap<double, unsigned int> sorted_quality;

    for (unsigned int m=0; m < metrics.size(); m++)
    {
        double val = max_double;

        switch (to_be_sort_id)
        {
        case 0: val = metrics.at(m).IC_min;
            break;
        case 1: val = metrics.at(m).CC_min;
            break;
        case 2: val = metrics.at(m).CR_min;
            break;
        case 3: val = metrics.at(m).AR_min;
            break;
        case 4: val = metrics.at(m).KE_min;
            break;
        case 5: val = metrics.at(m).KAR_min;
            break;
        case 6: val = metrics.at(m).APR_min;
            break;
        case 7: val = metrics.at(m).MA_min;
            break;
        case 8: val = metrics.at(m).SE_min;
            break;
        case 9: val = metrics.at(m).ER_min;
            break;
        case 10: val = metrics.at(m).MPD_min;
            break;
        case 11: val = metrics.at(m).NS_min;
            break;
        case 12: val = metrics.at(m).MXA_min;
            break;
        case 13: val = metrics.at(m).SR_min;
            break;
        case 14: val = metrics.at(m).VEM_min;
            break;
        case 15: val = metrics.at(m).VEMA_min;
            break;
        default:
            std::cerr << "Invalid metric index" << std::endl;
        }

        sorted_quality.insert(std::pair<double, unsigned int> (val, m));
    }

    for (uint i=0; i < n_metrics; i++)
    {
        QLineSeries *series_min = new QLineSeries();
        QLineSeries *series_max = new QLineSeries();
        QLineSeries *series_avg = new QLineSeries();

        QLineSeries *series_poly_min = new QLineSeries();
        QLineSeries *series_poly_max = new QLineSeries();
        QLineSeries *series_poly_avg = new QLineSeries();

        series_poly_min->setColor(extra_colors.at(0));
        series_poly_max->setColor(extra_colors.at(1));
        series_poly_avg->setColor(extra_colors.at(2));

        series_min->setName("Minimum");
        series_max->setName("Maximum");
        series_avg->setName("Average");

        series_poly_min->setName("Polygon Min");
        series_poly_max->setName("Polygon Max");
        series_poly_avg->setName("Polygon Avg");

        QChart *chart = new QChart();
        uint metric_id = 0;

        for (auto it = sorted_quality.begin(); it != sorted_quality.end(); it++)
        {
            uint m = it->second;

            double val_min = 0.0;
            double val_max = 0.0;
            double val_avg = 0.0;

            double val_poly_min = 0.0;
            double val_poly_max = 0.0;
            double val_poly_avg = 0.0;

            uint min_id = 0;
            uint max_id = 0;
            uint min_poly_id = 0;
            uint max_poly_id = 0;

            switch (i)
            {
            case 0: val_min = metrics.at(m).IC_min;
                val_max = metrics.at(m).IC_max;
                val_avg = metrics.at(m).IC_avg;
                val_poly_min = metrics.at(m).IC_poly_min;
                val_poly_max = metrics.at(m).IC_poly_max;
                val_poly_avg = metrics.at(m).IC_poly_avg;
                min_id = metrics.at(m).IC_min_id;
                max_id = metrics.at(m).IC_max_id;
                min_poly_id = metrics.at(m).IC_poly_min_id;
                max_poly_id = metrics.at(m).IC_poly_max_id;
                break;
            case 1: val_min = metrics.at(m).CC_min;
                val_max = metrics.at(m).CC_max;
                val_avg = metrics.at(m).CC_avg;
                val_poly_min = metrics.at(m).CC_poly_min;
                val_poly_max = metrics.at(m).CC_poly_max;
                val_poly_avg = metrics.at(m).CC_poly_avg;
                min_id = metrics.at(m).CC_min_id;
                max_id = metrics.at(m).CC_max_id;
                min_poly_id = metrics.at(m).CC_poly_min_id;
                max_poly_id = metrics.at(m).CC_poly_max_id;
                break;
            case 2: val_min = metrics.at(m).CR_min;
                val_max = metrics.at(m).CR_max;
                val_avg = metrics.at(m).CR_avg;
                val_poly_min = metrics.at(m).CR_poly_min;
                val_poly_max = metrics.at(m).CR_poly_max;
                val_poly_avg = metrics.at(m).CR_poly_avg;
                min_id = metrics.at(m).CR_min_id;
                max_id = metrics.at(m).CR_max_id;
                min_poly_id = metrics.at(m).CR_poly_min_id;
                max_poly_id = metrics.at(m).CR_poly_max_id;
                break;
            case 3: val_min = metrics.at(m).AR_min;
                val_max = metrics.at(m).AR_max;
                val_avg = metrics.at(m).AR_avg;
                val_poly_min = metrics.at(m).AR_poly_min;
                val_poly_max = metrics.at(m).AR_poly_max;
                val_poly_avg = metrics.at(m).AR_poly_avg;
                min_id = metrics.at(m).AR_min_id;
                max_id = metrics.at(m).AR_max_id;
                min_poly_id = metrics.at(m).AR_poly_min_id;
                max_poly_id = metrics.at(m).AR_poly_max_id;
                break;
            case 4: val_min = metrics.at(m).KE_min;
                val_max = metrics.at(m).KE_max;
                val_avg = metrics.at(m).KE_avg;
                val_poly_min = metrics.at(m).KE_poly_min;
                val_poly_max = metrics.at(m).KE_poly_max;
                val_poly_avg = metrics.at(m).KE_poly_avg;
                min_id = metrics.at(m).KE_min_id;
                max_id = metrics.at(m).KE_max_id;
                min_poly_id = metrics.at(m).KE_poly_min_id;
                max_poly_id = metrics.at(m).KE_poly_max_id;
                break;
            case 5: val_min = metrics.at(m).KAR_min;
                val_max = metrics.at(m).KAR_max;
                val_avg = metrics.at(m).KAR_avg;
                val_poly_min = metrics.at(m).KAR_poly_min;
                val_poly_max = metrics.at(m).KAR_poly_max;
                val_poly_avg = metrics.at(m).KAR_poly_avg;
                min_id = metrics.at(m).KAR_min_id;
                max_id = metrics.at(m).KAR_max_id;
                min_poly_id = metrics.at(m).KAR_poly_min_id;
                max_poly_id = metrics.at(m).KAR_poly_max_id;
                break;
            case 6: val_min = metrics.at(m).APR_min;
                val_max = metrics.at(m).APR_max;
                val_avg = metrics.at(m).APR_avg;
                val_poly_min = metrics.at(m).APR_poly_min;
                val_poly_max = metrics.at(m).APR_poly_max;
                val_poly_avg = metrics.at(m).APR_poly_avg;
                min_id = metrics.at(m).APR_min_id;
                max_id = metrics.at(m).APR_max_id;
                min_poly_id = metrics.at(m).APR_poly_min_id;
                max_poly_id = metrics.at(m).APR_poly_max_id;
                break;
            case 7: val_min = metrics.at(m).MA_min;
                val_max = metrics.at(m).MA_max;
                val_avg = metrics.at(m).MA_avg;
                val_poly_min = metrics.at(m).MA_poly_min;
                val_poly_max = metrics.at(m).MA_poly_max;
                val_poly_avg = metrics.at(m).MA_poly_avg;
                min_id = metrics.at(m).MA_min_id;
                max_id = metrics.at(m).MA_max_id;
                min_poly_id = metrics.at(m).MA_poly_min_id;
                max_poly_id = metrics.at(m).MA_poly_max_id;
                break;
            case 8: val_min = metrics.at(m).SE_min;
                val_max = metrics.at(m).SE_max;
                val_avg = metrics.at(m).SE_avg;
                val_poly_min = metrics.at(m).SE_poly_min;
                val_poly_max = metrics.at(m).SE_poly_max;
                val_poly_avg = metrics.at(m).SE_poly_avg;
                min_id = metrics.at(m).SE_min_id;
                max_id = metrics.at(m).SE_max_id;
                min_poly_id = metrics.at(m).SE_poly_min_id;
                max_poly_id = metrics.at(m).SE_poly_max_id;
                break;
            case 9: val_min = metrics.at(m).ER_min;
                val_max = metrics.at(m).ER_max;
                val_avg = metrics.at(m).ER_avg;
                val_poly_min = metrics.at(m).ER_poly_min;
                val_poly_max = metrics.at(m).ER_poly_max;
                val_poly_avg = metrics.at(m).ER_poly_avg;
                min_id = metrics.at(m).ER_min_id;
                max_id = metrics.at(m).ER_max_id;
                min_poly_id = metrics.at(m).ER_poly_min_id;
                max_poly_id = metrics.at(m).ER_poly_max_id;
                break;
            case 10: val_min = metrics.at(m).MPD_min;
                val_max = metrics.at(m).MPD_max;
                val_avg = metrics.at(m).MPD_avg;
                val_poly_min = metrics.at(m).MPD_poly_min;
                val_poly_max = metrics.at(m).MPD_poly_max;
                val_poly_avg = metrics.at(m).MPD_poly_avg;
                min_id = metrics.at(m).MPD_min_id;
                max_id = metrics.at(m).MPD_max_id;
                min_poly_id = metrics.at(m).MPD_poly_min_id;
                max_poly_id = metrics.at(m).MPD_poly_max_id;
                break;
            case 11: val_min = metrics.at(m).NS_min;
                val_max = metrics.at(m).NS_max;
                val_avg = metrics.at(m).NS_avg;
                val_poly_min = metrics.at(m).NS_poly_min;
                val_poly_max = metrics.at(m).NS_poly_max;
                val_poly_avg = metrics.at(m).NS_poly_avg;
                min_id = metrics.at(m).NS_min_id;
                max_id = metrics.at(m).NS_max_id;
                min_poly_id = metrics.at(m).NS_poly_min_id;
                max_poly_id = metrics.at(m).NS_poly_max_id;
                break;
            case 12: val_min = metrics.at(m).MXA_min;
                val_max = metrics.at(m).MXA_max;
                val_avg = metrics.at(m).MXA_avg;
                val_poly_min = metrics.at(m).MXA_poly_min;
                val_poly_max = metrics.at(m).MXA_poly_max;
                val_poly_avg = metrics.at(m).MXA_poly_avg;
                min_id = metrics.at(m).MXA_min_id;
                max_id = metrics.at(m).MXA_max_id;
                min_poly_id = metrics.at(m).MXA_poly_min_id;
                max_poly_id = metrics.at(m).MXA_poly_max_id;
                break;
            case 13: val_min = metrics.at(m).SR_min;
                val_max = metrics.at(m).SR_max;
                val_avg = metrics.at(m).SR_avg;
                val_poly_min = metrics.at(m).SR_poly_min;
                val_poly_max = metrics.at(m).SR_poly_max;
                val_poly_avg = metrics.at(m).SR_poly_avg;
                min_id = metrics.at(m).SR_min_id;
                max_id = metrics.at(m).SR_max_id;
                min_poly_id = metrics.at(m).SR_poly_min_id;
                max_poly_id = metrics.at(m).SR_poly_max_id;
                break;
            case 14: val_min = metrics.at(m).VEM_min;
                val_max = metrics.at(m).VEM_max;
                val_avg = metrics.at(m).VEM_avg;
                val_poly_min = metrics.at(m).VEM_poly_min;
                val_poly_max = metrics.at(m).VEM_poly_max;
                val_poly_avg = metrics.at(m).VEM_poly_avg;
                min_id = metrics.at(m).VEM_min_id;
                max_id = metrics.at(m).VEM_max_id;
                min_poly_id = metrics.at(m).VEM_poly_min_id;
                max_poly_id = metrics.at(m).VEM_poly_max_id;
                break;
            case 15: val_min = sqrt(metrics.at(m).VEMA_sum + metrics.at(m).VEMA_poly_sum);
                val_max = sqrt(metrics.at(m).VEMA_sum + metrics.at(m).VEMA_poly_sum);
                val_avg = sqrt(metrics.at(m).VEMA_sum + metrics.at(m).VEMA_poly_sum);
                val_poly_min = sqrt(metrics.at(m).VEMA_sum + metrics.at(m).VEMA_poly_sum);
                val_poly_max = sqrt(metrics.at(m).VEMA_sum + metrics.at(m).VEMA_poly_sum);
                val_poly_avg = sqrt(metrics.at(m).VEMA_sum + metrics.at(m).VEMA_poly_sum);
                min_id = metrics.at(m).VEMA_min_id;
                max_id = metrics.at(m).VEMA_max_id;
                min_poly_id = metrics.at(m).VEMA_poly_min_id;
                max_poly_id = metrics.at(m).VEMA_poly_max_id;
                break;
            default:
                std::cerr << "Invalid metric index" << std::endl;
            }

            if (min_id < UINT_MAX)
                series_min->append(metric_id, val_min);

            if (max_id < UINT_MAX)
                series_max->append(metric_id, val_max);

            if (min_id < UINT_MAX && max_id < UINT_MAX)
                series_avg->append(metric_id, val_avg);

            if (min_poly_id < UINT_MAX)
                series_poly_min->append(metric_id, val_poly_min);

            if (max_poly_id < UINT_MAX)
                series_poly_max->append(metric_id, val_poly_max);

            if (min_poly_id < UINT_MAX && max_poly_id < UINT_MAX)
                series_poly_avg->append(metric_id, val_poly_avg);

            metric_id++;
        }

        chart->legend()->hide();

        chart->addSeries(series_min);
        chart->addSeries(series_max);
        chart->addSeries(series_avg);

        chart->addSeries(series_poly_min);
        chart->addSeries(series_poly_max);
        chart->addSeries(series_poly_avg);

        chart->createDefaultAxes();

        chart->setTitle(metrics_names.at(i).c_str());

        CustomizedChartView *chartView = new CustomizedChartView();
        chartView->setChart(chart);

        if (i == to_be_sort_id)
            chartView->setBackgroundBrush(Qt::red);
        else
            chartView->setBackgroundBrush(QColor (230, 230, 230));


        chartView->setRenderHint(QPainter::Antialiasing);

        ui->metricsWidget->add_chart(chartView);
    }

}

void MainWindow::show_solver_results(const uint solution_id, const std::string folder, const std::string filename)
{
    ui->solverResultsWidget->set_dataset(&dataset);
    ui->solverResultsWidget->clean_charts();

    ui->solverResultsWidget->set_solution_id(solution_id);
    ui->scatterPlotsGPWidget->set_solution_id(solution_id);

    const std::string filepath = folder + QString(QDir::separator()).toStdString() + filename;

    std::ifstream in;
    in.open(filepath.c_str());

    if (!in.is_open())
    {
        std::cerr << "Error opening " << filename << std::endl;
        return;
    }

    std::vector<std::vector<double>> errs (5);
    double e0,e1,e2,e3,e4;

    std::vector<std::string> labels
        {"errS", "errInf", "errL2", "hEmax", "condVect"};

    while ( in >> e0 >> e1 >> e2 >> e3 >> e4)
    {
        errs.at(0).push_back(e0);
        errs.at(1).push_back(e1);
        errs.at(2).push_back(e2);
        errs.at(3).push_back(e3);
        errs.at(4).push_back(e4);
    }

    in.close();

    errsToScatterPlots.clear();
    errsToScatterPlots.push_back(errs.at(0));
    errsToScatterPlots.push_back(errs.at(1));
    errsToScatterPlots.push_back(errs.at(2));
    errsToScatterPlots.push_back(errs.at(4));

    if (!metrics.empty())
        ui->scatterPlotsGPWidget->create_scatterPlots(dataset, metrics, errsToScatterPlots);
    else
        ui->scatterPlotsGPWidget->set_empty();

    const double fact = 1e12;
    std::ostringstream streamObj;
    //Add double to stream
    streamObj << fact;
    // Get string from output string stream
    const std::string str_fact = streamObj.str();

    for (uint i=0; i < errs.size(); i++)
    {
//        double min_val = *std::min_element(errs.at(i).begin(), errs.at(i).end());
//        bool range_adapt = (min_val < 1e-12);

        QChart *chart = new QChart();
        QLineSeries *series = new QLineSeries();

////          TOMMY
//        std::vector<std::pair<double,double>> pairs;
//        auto m = dataset.get_parametric_meshes_metrics();
//        if(m.size()!=0)
//        {
//            for (uint v=0; v < errs.at(i).size(); v++)
//            {
//                double e = std::min(m.at(v).SE_min, m.at(v).SE_poly_min);
//                pairs.push_back(std::pair<double,double> (e, errs.at(i).at(v)));
//            }
//            sort(pairs.begin(), pairs.end());

//            for(uint i=0; i<pairs.size(); i++)
//                series->append(pairs.at(i).first, pairs.at(i).second);
//        }
//        else
        {
            for (uint v=0; v < errs.at(i).size(); v++)
            {
    //            if (!range_adapt)
                    series->append(v, errs.at(i).at(v));
    //            else
    //                series->append(v, errs.at(i).at(v)*fact);
            }
        }

        for (uint v=0; v < errs.at(i).size(); v++)
            std::cout << series->at(static_cast<int>(v)).y() << std::endl;
        std::cout << std::endl;

        series->setPointsVisible(true);
        series->setName(labels.at(i).c_str());

        QString y_axis_title;
//        if (range_adapt)
//            y_axis_title = /*QString(labels.at(i).c_str()) + */ "*" + QString(str_fact.c_str());

        QValueAxis *axisX = new QValueAxis();
        axisX->setLabelFormat("%f");
        chart->addAxis(axisX, Qt::AlignBottom);

        QValueAxis *axisY = new QValueAxis();
        axisY->setLabelFormat("%e");
        axisY->setTitleText(y_axis_title);
        chart->addAxis(axisY, Qt::AlignLeft);

        chart->addSeries(series);

        series->attachAxis(axisX);
        series->attachAxis(axisY);

        if (i == errs.size()-1)
        {
            QLogValueAxis *axisYlog = new QLogValueAxis();
    //        axisY->setLabelFormat("%g");
            axisYlog->setBase(8.0);
            axisYlog->setMinorTickCount(-1);

            chart->removeAxis(chart->axes().at(1));
            chart->addAxis(axisYlog, Qt::AlignLeft);

            series->attachAxis(axisYlog);
        }

        chart->axes()[1]->setMax(*std::max_element(errs.at(i).begin(), errs.at(i).end()));
//        chart->createDefaultAxes();

        CustomizedChartView *chartView = new CustomizedChartView();
        chartView->setChart(chart);
        chartView->setRenderHint(QPainter::Antialiasing);

        ui->solverResultsWidget->add_chart(chartView, labels.at(i).c_str());
    }

    const std::string postfix_sol = "-VEM-sol.txt";

    for (uint i=0; i < dataset.get_parametric_meshes().size(); i++)
    {       
        std::string basename = dataset.get_parametric_mesh_filename(i);
        basename = basename.substr(basename.find_last_of(QDir::separator().toLatin1())+1);
        basename = basename.substr(0, basename.find_last_of("."));

        std::string filepath = folder + QString(QDir::separator()).toStdString() +
                               basename + postfix_sol;

        std::cout << "Parsing file " << filepath << std::endl;

        std::ifstream in;
        in.open(filepath.c_str());

        if (!in.is_open())
        {
            std::cerr << "Error opening " << filepath << std::endl;
            continue;
        }

        std::vector<double> vals;
        double val;

        for (uint vid=0; vid < dataset.get_parametric_mesh(i)->num_verts(); vid++)
        {
            in >> val;
            vals.push_back(val);
        }

        cinolib::ScalarField sf (vals);

        double min = DBL_MAX, max = -DBL_MAX;
        for (uint ii=0; ii < sf.size(); ii++)
        {
            if (sf[ii] < min) min = sf[ii];
            if (sf[ii] > max) max = sf[ii];
        }

        if (min != max)
            sf.normalize_in_01();

        sf.copy_to_mesh(*dataset.get_parametric_mesh(i));

        for (uint vid=0; vid < dataset.get_parametric_mesh(i)->num_verts(); vid++ )
        {
            dataset.get_parametric_mesh(i)->vert_data(vid).color = cinolib::Color::red_white_blue_ramp_01(sf[vid]);
            // std::cout << dataset.get_parametric_mesh(i)->vert(vid) <<  " -- " << sf[vid] << std::endl;
        }

        in.close();
    }

    const std::string postfix_gt = "-GROUND-TRUTH-sol.txt";

    for (uint i=0; i < dataset.get_parametric_meshes().size(); i++)
    {
        std::string basename = dataset.get_parametric_mesh_filename(i);
        basename = basename.substr(basename.find_last_of(QDir::separator().toLatin1())+1);
        basename = basename.substr(0, basename.find_last_of("."));

        std::string filepath = folder + QString(QDir::separator()).toStdString() +
                               basename + postfix_sol;

        std::cout << "Parsing file " << filepath << std::endl;

        std::ifstream in;
        in.open(filepath.c_str());

        if (!in.is_open())
        {
            std::cerr << "Error opening " << filepath << std::endl;
            continue;
        }

        std::vector<double> vals;
        double val;

        for (uint vid=0; vid < dataset.get_parametric_mesh(i)->num_verts(); vid++)
        {
            in >> val;
            vals.push_back(val);
        }

        cinolib::ScalarField sf (vals);

        double min = DBL_MAX, max = -DBL_MAX;
        for (uint ii=0; ii < sf.size(); ii++)
        {
            if (sf[ii] < min) min = sf[ii];
            if (sf[ii] > max) max = sf[ii];
        }

        if (min != max)
            sf.normalize_in_01();

        sf.copy_to_mesh(*ui->solverResultsWidget->get_gt_mesh(i));


        for (uint vid=0; vid < ui->solverResultsWidget->get_gt_mesh(i)->num_verts(); vid++ )
        {
            ui->solverResultsWidget->get_gt_mesh(i)->vert_data(vid).color = cinolib::Color::red_white_blue_ramp_01(sf[vid]);
            // std::cout << ui->solverResultsWidget->get_gt_mesh(i)->vert(vid) <<  " -- " << sf[vid] << std::endl;
        }

        in.close();
    }

    ui->solverResultsWidget->show_mesh_solution_and_groundtruth();

    ui->tab_widgets->setTabEnabled(3, true);
    ui->tab_widgets->setCurrentIndex(3);
}

void MainWindow::on_reset_btn_clicked()
{
    delete ui->datasetWidget;
    delete ui->metricsWidget;
    delete ui->graphicMeshMetricWidget;
    delete ui->solverWidget;
    delete ui->solverResultsWidget;

    metrics.clear();
    dataset.clean();
    folder = "";

    ui->datasetWidget = new DatasetWidget(this);
    ui->datasetWidget->set_dataset(&dataset);
    ui->dataset_tab->layout()->addWidget(ui->datasetWidget);

    ui->metricsWidget = new MeshMetricsWidget(this);
    ui->mesh_metrics_tab->layout()->addWidget(ui->metricsWidget);

    ui->graphicMeshMetricWidget = new MeshMetricsGraphicWidget(this);
    ui->graphicMeshMetricWidget->set_dataset(&dataset);

    ui->solverWidget = new SolverWidget (this);
    ui->solverWidget->set_dataset(&dataset);
    ui->solver_tab->layout()->addWidget(ui->solverWidget);

    ui->solverResultsWidget = new SolverResultsWidget(this);
    ui->solverResultsWidget->set_dataset(&dataset);
    ui->solver_results_tab->layout()->addWidget(ui->solverResultsWidget);

    connect(ui->datasetWidget, SIGNAL (computed_mesh_metrics()), this, SLOT(show_mesh_metrics()));
    connect(ui->metricsWidget, SIGNAL (sort_geometric_qualities(const uint)), this, SLOT(show_sorted_mesh_metrics(const uint)));
    connect(ui->metricsWidget, SIGNAL (show_unsorted_metrics()), this, SLOT(show_mesh_metrics()));
    connect(ui->solverWidget, SIGNAL (solver_completed (const uint, const std::string, const std::string)), this, SLOT (show_solver_results (const uint, const std::string, const std::string)));
    connect(ui->datasetWidget, SIGNAL (saved_in(const std::string)), this, SLOT (update_solver_input_folder (const std::string)));

    connect(ui->scatterPlotsGPWidget, SIGNAL(compute_GP_scatterplots()), this, SLOT(compute_GP_scatterplots()));

    ui->tab_widgets->setCurrentIndex(0);

    for (int i=1; i < ui->tab_widgets->count()-1; i++)
        ui->tab_widgets->setTabEnabled(i, false);

}

void MainWindow::on_tab_widgets_currentChanged(int index)
{
    if (index == 0)
    {
        if (dataset.get_parametric_meshes().size() == 0)
            return;

        for (cinolib::DrawablePolygonmesh<> * p : ui->datasetWidget->get_dataset()->get_parametric_meshes())
            p->show_vert_color();
    }
    else
    if (index == 1)
    {
        ui->graphicMeshMetricWidget->set_slider_max(static_cast<const uint>( dataset.get_parametric_meshes().size()-1));

        for (cinolib::DrawablePolygonmesh<> * p : ui->datasetWidget->get_dataset()->get_parametric_meshes())
            p->show_poly_color();
    }
    else
    if (index == 2) // solver widget
    {
        for (uint m=0; m < dataset.get_parametric_meshes().size(); m++)
        {
            dataset.get_parametric_mesh(m)->poly_set_color(cinolib::Color(1,1,1));
            dataset.get_parametric_mesh(m)->show_poly_color();
        }

        ui->solverWidget->update();
    }
    else
    if (index == 3) // solver results widget
    {
        ui->solverResultsWidget->show_mesh_solution_and_groundtruth();
    }
}

void MainWindow::update_solver_input_folder(const std::string folder)
{
    ui->solverWidget->update();

    ui->tab_widgets->setTabEnabled(2, true);
//    ui->tab_widgets->setCurrentIndex(2);

    ui->solverWidget->set_input_folder(folder);
    this->folder = folder;
}

void MainWindow::on_save_all_btn_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this,
        tr("Save Project"), "",
        tr("CHANGE Project (*.changeproj);;All Files (*)"));

    if (fileName.isNull() || fileName.isEmpty())
        return;
}

void MainWindow::compute_GP_scatterplots ()
{
    if (metrics.empty())
        ui->datasetWidget->compute_geometric_metrics();

    ui->scatterPlotsGPWidget->create_scatterPlots(dataset, metrics, errsToScatterPlots);

    ui->tabWidget->setCurrentIndex(3);
}
