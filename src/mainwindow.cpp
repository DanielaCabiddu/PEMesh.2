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

#include "licence_dialog.h"

#include <cinolib/scalar_field.h>

#include <QDir>
#include <QFileDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QMessageBox>
#include <QDirIterator>

#include <QtCharts/QChartView>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QLogValueAxis>
#include <QtCharts/QValueAxis>

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
    connect(ui->datasetWidget, SIGNAL (computed_mesh_metrics()), this, SLOT(show_full_mesh_metrics()));
    connect(ui->datasetWidget, SIGNAL (mirrored()), this, SLOT(disable_all_tabs()));
    connect(ui->metricsWidget, SIGNAL (sort_geometric_qualities(const uint)), this, SLOT(show_sorted_mesh_metrics(const uint)));
    connect(ui->metricsWidget, SIGNAL (show_unsorted_metrics()), this, SLOT(show_mesh_metrics()));
    connect(ui->metricsWidget, SIGNAL (show_unsorted_metrics()), this, SLOT(show_full_mesh_metrics()));
    connect(ui->solverWidget,  SIGNAL (solver_completed (const uint, const std::string, const std::string)), this, SLOT (show_solver_results (const uint, const std::string, const std::string)));
    connect(ui->datasetWidget, SIGNAL (saved_in(const std::string)), this, SLOT (update_solver_input_folder (const std::string)));

    connect(ui->scatterPlotsGPWidget, SIGNAL(compute_GP_scatterplots()), this, SLOT(compute_GP_scatterplots()));

    setWindowIcon(QIcon(":/logo/img/logo.png"));
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
    ui->graphicMeshMetricWidget->set_slider_value(0);

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

        QLineSeries *series_mesh = new QLineSeries();

        series_poly_min->setColor(extra_colors.at(0));
        series_poly_max->setColor(extra_colors.at(1));
        series_poly_avg->setColor(extra_colors.at(2));
        series_mesh->setColor(extra_colors.at(3));

        series_min->setName("Tri Min");
        series_max->setName("Tri Max");
        series_avg->setName("Tri Avg");

        series_poly_min->setName("Poly Min");
        series_poly_max->setName("Poly Max");
        series_poly_avg->setName("Poly Avg");

        series_mesh->setName("Mesh");

        QChart *chart = new QChart();

        for (uint m=0; m < metrics.size(); m++)
        {
            double val_min = 0.0;
            double val_max = 0.0;
            double val_avg = 0.0;

            double val_poly_min = 0.0;
            double val_poly_max = 0.0;
            double val_poly_avg = 0.0;

            double val_mesh = 0.0;

            uint min_id = 0;
            uint max_id = 0;
            uint min_poly_id = 0;
            uint max_poly_id = 0;

            switch (i)
            {
            case 0: val_min  = metrics.at(m).INR_min;
                val_max      = metrics.at(m).INR_max;
                val_avg      = metrics.at(m).INR_avg;
                val_poly_min = metrics.at(m).INR_poly_min;
                val_poly_max = metrics.at(m).INR_poly_max;
                val_poly_avg = metrics.at(m).INR_poly_avg;
                val_mesh     = metrics.at(m).INR_mesh;
                min_id       = metrics.at(m).INR_min_id;
                max_id       = metrics.at(m).INR_max_id;
                min_poly_id  = metrics.at(m).INR_poly_min_id;
                max_poly_id  = metrics.at(m).INR_poly_max_id;
                break;
            case 1: val_min  = metrics.at(m).OUR_min;
                val_max      = metrics.at(m).OUR_max;
                val_avg      = metrics.at(m).OUR_avg;
                val_poly_min = metrics.at(m).OUR_poly_min;
                val_poly_max = metrics.at(m).OUR_poly_max;
                val_poly_avg = metrics.at(m).OUR_poly_avg;
                val_mesh     = metrics.at(m).OUR_mesh;
                min_id       = metrics.at(m).OUR_min_id;
                max_id       = metrics.at(m).OUR_max_id;
                min_poly_id  = metrics.at(m).OUR_poly_min_id;
                max_poly_id  = metrics.at(m).OUR_poly_max_id;
                break;
            case 2: val_min  = metrics.at(m).CIR_min;
                val_max      = metrics.at(m).CIR_max;
                val_avg      = metrics.at(m).CIR_avg;
                val_poly_min = metrics.at(m).CIR_poly_min;
                val_poly_max = metrics.at(m).CIR_poly_max;
                val_poly_avg = metrics.at(m).CIR_poly_avg;
                val_mesh     = metrics.at(m).CIR_mesh;
                min_id       = metrics.at(m).CIR_min_id;
                max_id       = metrics.at(m).CIR_max_id;
                min_poly_id  = metrics.at(m).CIR_poly_min_id;
                max_poly_id  = metrics.at(m).CIR_poly_max_id;
                break;
            case 3: val_min  = metrics.at(m).KRR_min;
                val_max      = metrics.at(m).KRR_max;
                val_avg      = metrics.at(m).KRR_avg;
                val_poly_min = metrics.at(m).KRR_poly_min;
                val_poly_max = metrics.at(m).KRR_poly_max;
                val_poly_avg = metrics.at(m).KRR_poly_avg;
                val_mesh     = metrics.at(m).KRR_mesh;
                min_id       = metrics.at(m).KRR_min_id;
                max_id       = metrics.at(m).KRR_max_id;
                min_poly_id  = metrics.at(m).KRR_poly_min_id;
                max_poly_id  = metrics.at(m).KRR_poly_max_id;
                break;
            case 4: val_min  = metrics.at(m).KAR_min;
                val_max      = metrics.at(m).KAR_max;
                val_avg      = metrics.at(m).KAR_avg;
                val_poly_min = metrics.at(m).KAR_poly_min;
                val_poly_max = metrics.at(m).KAR_poly_max;
                val_poly_avg = metrics.at(m).KAR_poly_avg;
                val_mesh     = metrics.at(m).KAR_mesh;
                min_id       = metrics.at(m).KAR_min_id;
                max_id       = metrics.at(m).KAR_max_id;
                min_poly_id  = metrics.at(m).KAR_poly_min_id;
                max_poly_id  = metrics.at(m).KAR_poly_max_id;
                break;
            case 5: val_min  = metrics.at(m).APR_min;
                val_max      = metrics.at(m).APR_max;
                val_avg      = metrics.at(m).APR_avg;
                val_poly_min = metrics.at(m).APR_poly_min;
                val_poly_max = metrics.at(m).APR_poly_max;
                val_poly_avg = metrics.at(m).APR_poly_avg;
                val_mesh     = metrics.at(m).APR_mesh;
                min_id       = metrics.at(m).APR_min_id;
                max_id       = metrics.at(m).APR_max_id;
                min_poly_id  = metrics.at(m).APR_poly_min_id;
                max_poly_id  = metrics.at(m).APR_poly_max_id;
                break;
            case 6: val_min  = metrics.at(m).MIA_min;
                val_max      = metrics.at(m).MIA_max;
                val_avg      = metrics.at(m).MIA_avg;
                val_poly_min = metrics.at(m).MIA_poly_min;
                val_poly_max = metrics.at(m).MIA_poly_max;
                val_poly_avg = metrics.at(m).MIA_poly_avg;
                val_mesh     = metrics.at(m).MIA_mesh;
                min_id       = metrics.at(m).MIA_min_id;
                max_id       = metrics.at(m).MIA_max_id;
                min_poly_id  = metrics.at(m).MIA_poly_min_id;
                max_poly_id  = metrics.at(m).MIA_poly_max_id;
                break;
            case 7: val_min  = metrics.at(m).MAA_min;
                val_max      = metrics.at(m).MAA_max;
                val_avg      = metrics.at(m).MAA_avg;
                val_poly_min = metrics.at(m).MAA_poly_min;
                val_poly_max = metrics.at(m).MAA_poly_max;
                val_poly_avg = metrics.at(m).MAA_poly_avg;
                val_mesh     = metrics.at(m).MAA_mesh;
                min_id       = metrics.at(m).MAA_min_id;
                max_id       = metrics.at(m).MAA_max_id;
                min_poly_id  = metrics.at(m).MAA_poly_min_id;
                max_poly_id  = metrics.at(m).MAA_poly_max_id;
                break;
            case 8: val_min  = metrics.at(m).ANR_min;
                val_max      = metrics.at(m).ANR_max;
                val_avg      = metrics.at(m).ANR_avg;
                val_poly_min = metrics.at(m).ANR_poly_min;
                val_poly_max = metrics.at(m).ANR_poly_max;
                val_poly_avg = metrics.at(m).ANR_poly_avg;
                val_mesh     = metrics.at(m).ANR_mesh;
                min_id       = metrics.at(m).ANR_min_id;
                max_id       = metrics.at(m).ANR_max_id;
                min_poly_id  = metrics.at(m).ANR_poly_min_id;
                max_poly_id  = metrics.at(m).ANR_poly_max_id;
                break;
            case 9: val_min  = metrics.at(m).VEM_min;
                val_max      = metrics.at(m).VEM_max;
                val_avg      = metrics.at(m).VEM_avg;
                val_poly_min = metrics.at(m).VEM_poly_min;
                val_poly_max = metrics.at(m).VEM_poly_max;
                val_poly_avg = metrics.at(m).VEM_poly_avg;
                val_mesh     = metrics.at(m).VEM_mesh;
                min_id       = metrics.at(m).VEM_min_id;
                max_id       = metrics.at(m).VEM_max_id;
                min_poly_id  = metrics.at(m).VEM_poly_min_id;
                max_poly_id  = metrics.at(m).VEM_poly_max_id;
                break;
            case 10: val_min = metrics.at(m).JAC_min;
                val_max      = metrics.at(m).JAC_max;
                val_avg      = metrics.at(m).JAC_avg;
                val_poly_min = metrics.at(m).JAC_poly_min;
                val_poly_max = metrics.at(m).JAC_poly_max;
                val_poly_avg = metrics.at(m).JAC_poly_avg;
                val_mesh     = metrics.at(m).JAC_mesh;
                min_id       = metrics.at(m).JAC_min_id;
                max_id       = metrics.at(m).JAC_max_id;
                min_poly_id  = metrics.at(m).JAC_poly_min_id;
                max_poly_id  = metrics.at(m).JAC_poly_max_id;
                break;
            case 11: val_min = metrics.at(m).FRO_min;
                val_max      = metrics.at(m).FRO_max;
                val_avg      = metrics.at(m).FRO_avg;
                val_poly_min = metrics.at(m).FRO_poly_min;
                val_poly_max = metrics.at(m).FRO_poly_max;
                val_poly_avg = metrics.at(m).FRO_poly_avg;
                val_mesh     = metrics.at(m).FRO_mesh;
                min_id       = metrics.at(m).FRO_min_id;
                max_id       = metrics.at(m).FRO_max_id;
                min_poly_id  = metrics.at(m).FRO_poly_min_id;
                max_poly_id  = metrics.at(m).FRO_poly_max_id;
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

            series_mesh->append(m, val_mesh);
        }

        chart->legend()->hide();

        series_min->setPointsVisible();
        series_max->setPointsVisible();
        series_avg->setPointsVisible();

        series_poly_min->setPointsVisible();
        series_poly_max->setPointsVisible();
        series_poly_avg->setPointsVisible();

        series_mesh->setPointsVisible();

        chart->addSeries(series_min);
        chart->addSeries(series_max);
        chart->addSeries(series_avg);

        chart->addSeries(series_poly_min);
        chart->addSeries(series_poly_max);
        chart->addSeries(series_poly_avg);

        chart->addSeries(series_mesh);

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
        // static_cast<QValueAxis *>(chart->axisX())->setMinorTickCount(10);

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
        chartView->setBackgroundBrush(QColor (230, 230, 230));

        // chart->setAnimationOptions(QChart::SeriesAnimations);
        chartView->setRenderHint(QPainter::Antialiasing);
        chartView->setChart(chart);
        QValueAxis *axisX = static_cast<QValueAxis *>(chart->axes(Qt::Horizontal).at(0)); // <--
        axisX->setTickCount(metrics.size());  // <--
        axisX->setLabelFormat("%d"); // <--

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
        double val = cinolib::max_double;

        switch (to_be_sort_id)
        {
        case 0:  val = metrics.at(m).INR_mesh;
            break;
        case 1:  val = metrics.at(m).OUR_mesh;
            break;
        case 2:  val = metrics.at(m).CIR_mesh;
            break;
        case 3:  val = metrics.at(m).KRR_mesh;
            break;
        case 4:  val = metrics.at(m).KAR_mesh;
            break;
        case 5:  val = metrics.at(m).APR_mesh;
            break;
        case 6:  val = metrics.at(m).MIA_mesh;
            break;
        case 7:  val = metrics.at(m).MAA_mesh;
            break;
        case 8:  val = metrics.at(m).ANR_mesh;
            break;
        case 9:  val = metrics.at(m).VEM_mesh;
            break;
        case 10: val = metrics.at(m).JAC_mesh;
            break;
        case 11: val = metrics.at(m).FRO_mesh;
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

        QLineSeries *series_mesh = new QLineSeries();

        series_poly_min->setColor(extra_colors.at(0));
        series_poly_max->setColor(extra_colors.at(1));
        series_poly_avg->setColor(extra_colors.at(2));
        series_mesh->setColor(extra_colors.at(3));

        series_min->setName("Triangle Minimum");
        series_max->setName("Triangle Maximum");
        series_avg->setName("Triangle Average");

        series_poly_min->setName("Polygon Min");
        series_poly_max->setName("Polygon Max");
        series_poly_avg->setName("Polygon Avg");

        series_mesh->setName("Polygon Avg");

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

            double val_mesh = 0.0;

            uint min_id = 0;
            uint max_id = 0;
            uint min_poly_id = 0;
            uint max_poly_id = 0;

            switch (i)
            {
            case 0: val_min  = metrics.at(m).INR_min;
                val_max      = metrics.at(m).INR_max;
                val_avg      = metrics.at(m).INR_avg;
                val_poly_min = metrics.at(m).INR_poly_min;
                val_poly_max = metrics.at(m).INR_poly_max;
                val_poly_avg = metrics.at(m).INR_poly_avg;
                val_mesh     = metrics.at(m).INR_mesh;
                min_id       = metrics.at(m).INR_min_id;
                max_id       = metrics.at(m).INR_max_id;
                min_poly_id  = metrics.at(m).INR_poly_min_id;
                max_poly_id  = metrics.at(m).INR_poly_max_id;
                break;
            case 1: val_min  = metrics.at(m).OUR_min;
                val_max      = metrics.at(m).OUR_max;
                val_avg      = metrics.at(m).OUR_avg;
                val_poly_min = metrics.at(m).OUR_poly_min;
                val_poly_max = metrics.at(m).OUR_poly_max;
                val_poly_avg = metrics.at(m).OUR_poly_avg;
                val_mesh     = metrics.at(m).OUR_mesh;
                min_id       = metrics.at(m).OUR_min_id;
                max_id       = metrics.at(m).OUR_max_id;
                min_poly_id  = metrics.at(m).OUR_poly_min_id;
                max_poly_id  = metrics.at(m).OUR_poly_max_id;
                break;
            case 2: val_min  = metrics.at(m).CIR_min;
                val_max      = metrics.at(m).CIR_max;
                val_avg      = metrics.at(m).CIR_avg;
                val_poly_min = metrics.at(m).CIR_poly_min;
                val_poly_max = metrics.at(m).CIR_poly_max;
                val_poly_avg = metrics.at(m).CIR_poly_avg;
                val_mesh     = metrics.at(m).CIR_mesh;
                min_id       = metrics.at(m).CIR_min_id;
                max_id       = metrics.at(m).CIR_max_id;
                min_poly_id  = metrics.at(m).CIR_poly_min_id;
                max_poly_id  = metrics.at(m).CIR_poly_max_id;
                break;
            case 3: val_min  = metrics.at(m).KRR_min;
                val_max      = metrics.at(m).KRR_max;
                val_avg      = metrics.at(m).KRR_avg;
                val_poly_min = metrics.at(m).KRR_poly_min;
                val_poly_max = metrics.at(m).KRR_poly_max;
                val_poly_avg = metrics.at(m).KRR_poly_avg;
                val_mesh     = metrics.at(m).KRR_mesh;
                min_id       = metrics.at(m).KRR_min_id;
                max_id       = metrics.at(m).KRR_max_id;
                min_poly_id  = metrics.at(m).KRR_poly_min_id;
                max_poly_id  = metrics.at(m).KRR_poly_max_id;
                break;
            case 4: val_min  = metrics.at(m).KAR_min;
                val_max      = metrics.at(m).KAR_max;
                val_avg      = metrics.at(m).KAR_avg;
                val_poly_min = metrics.at(m).KAR_poly_min;
                val_poly_max = metrics.at(m).KAR_poly_max;
                val_poly_avg = metrics.at(m).KAR_poly_avg;
                val_mesh     = metrics.at(m).KAR_mesh;
                min_id       = metrics.at(m).KAR_min_id;
                max_id       = metrics.at(m).KAR_max_id;
                min_poly_id  = metrics.at(m).KAR_poly_min_id;
                max_poly_id  = metrics.at(m).KAR_poly_max_id;
                break;
            case 5: val_min  = metrics.at(m).APR_min;
                val_max      = metrics.at(m).APR_max;
                val_avg      = metrics.at(m).APR_avg;
                val_poly_min = metrics.at(m).APR_poly_min;
                val_poly_max = metrics.at(m).APR_poly_max;
                val_poly_avg = metrics.at(m).APR_poly_avg;
                val_mesh     = metrics.at(m).APR_mesh;
                min_id       = metrics.at(m).APR_min_id;
                max_id       = metrics.at(m).APR_max_id;
                min_poly_id  = metrics.at(m).APR_poly_min_id;
                max_poly_id  = metrics.at(m).APR_poly_max_id;
                break;
            case 6: val_min  = metrics.at(m).MIA_min;
                val_max      = metrics.at(m).MIA_max;
                val_avg      = metrics.at(m).MIA_avg;
                val_poly_min = metrics.at(m).MIA_poly_min;
                val_poly_max = metrics.at(m).MIA_poly_max;
                val_poly_avg = metrics.at(m).MIA_poly_avg;
                val_mesh     = metrics.at(m).MIA_mesh;
                min_id       = metrics.at(m).MIA_min_id;
                max_id       = metrics.at(m).MIA_max_id;
                min_poly_id  = metrics.at(m).MIA_poly_min_id;
                max_poly_id  = metrics.at(m).MIA_poly_max_id;
                break;
            case 7: val_min  = metrics.at(m).MAA_min;
                val_max      = metrics.at(m).MAA_max;
                val_avg      = metrics.at(m).MAA_avg;
                val_poly_min = metrics.at(m).MAA_poly_min;
                val_poly_max = metrics.at(m).MAA_poly_max;
                val_poly_avg = metrics.at(m).MAA_poly_avg;
                val_mesh     = metrics.at(m).MAA_mesh;
                min_id       = metrics.at(m).MAA_min_id;
                max_id       = metrics.at(m).MAA_max_id;
                min_poly_id  = metrics.at(m).MAA_poly_min_id;
                max_poly_id  = metrics.at(m).MAA_poly_max_id;
                break;
            case 8: val_min  = metrics.at(m).ANR_min;
                val_max      = metrics.at(m).ANR_max;
                val_avg      = metrics.at(m).ANR_avg;
                val_poly_min = metrics.at(m).ANR_poly_min;
                val_poly_max = metrics.at(m).ANR_poly_max;
                val_poly_avg = metrics.at(m).ANR_poly_avg;
                val_mesh     = metrics.at(m).ANR_mesh;
                min_id       = metrics.at(m).ANR_min_id;
                max_id       = metrics.at(m).ANR_max_id;
                min_poly_id  = metrics.at(m).ANR_poly_min_id;
                max_poly_id  = metrics.at(m).ANR_poly_max_id;
                break;
            case 9: val_min  = metrics.at(m).VEM_min;
                val_max      = metrics.at(m).VEM_max;
                val_avg      = metrics.at(m).VEM_avg;
                val_poly_min = metrics.at(m).VEM_poly_min;
                val_poly_max = metrics.at(m).VEM_poly_max;
                val_poly_avg = metrics.at(m).VEM_poly_avg;
                val_mesh     = metrics.at(m).VEM_mesh;
                min_id       = metrics.at(m).VEM_min_id;
                max_id       = metrics.at(m).VEM_max_id;
                min_poly_id  = metrics.at(m).VEM_poly_min_id;
                max_poly_id  = metrics.at(m).VEM_poly_max_id;
                break;
            case 10: val_min = metrics.at(m).JAC_min;
                val_max      = metrics.at(m).JAC_max;
                val_avg      = metrics.at(m).JAC_avg;
                val_poly_min = metrics.at(m).JAC_poly_min;
                val_poly_max = metrics.at(m).JAC_poly_max;
                val_poly_avg = metrics.at(m).JAC_poly_avg;
                val_mesh     = metrics.at(m).JAC_mesh;
                min_id       = metrics.at(m).JAC_min_id;
                max_id       = metrics.at(m).JAC_max_id;
                min_poly_id  = metrics.at(m).JAC_poly_min_id;
                max_poly_id  = metrics.at(m).JAC_poly_max_id;
                break;
            case 11: val_min = metrics.at(m).FRO_min;
                val_max      = metrics.at(m).FRO_max;
                val_avg      = metrics.at(m).FRO_avg;
                val_poly_min = metrics.at(m).FRO_poly_min;
                val_poly_max = metrics.at(m).FRO_poly_max;
                val_poly_avg = metrics.at(m).FRO_poly_avg;
                val_mesh     = metrics.at(m).FRO_mesh;
                min_id       = metrics.at(m).FRO_min_id;
                max_id       = metrics.at(m).FRO_max_id;
                min_poly_id  = metrics.at(m).FRO_poly_min_id;
                max_poly_id  = metrics.at(m).FRO_poly_max_id;
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

            series_mesh->append(m, val_mesh);

            metric_id++;
        }

        chart->legend()->hide();

        chart->addSeries(series_min);
        chart->addSeries(series_max);
        chart->addSeries(series_avg);

        chart->addSeries(series_poly_min);
        chart->addSeries(series_poly_max);
        chart->addSeries(series_poly_avg);

        chart->addSeries(series_mesh);

        chart->createDefaultAxes();

        chart->setTitle(metrics_names.at(i).c_str());

        CustomizedChartView *chartView = new CustomizedChartView();
        chartView->setBackgroundBrush(QColor (230, 230, 230));

        // chart->setAnimationOptions(QChart::SeriesAnimations);
        chartView->setRenderHint(QPainter::Antialiasing);
        chartView->setChart(chart);
        QValueAxis *axisX = static_cast<QValueAxis *>(chart->axisX(chart->series().at(0))); // <--
        axisX->setTickCount(metrics.size());  // <--
        axisX->setLabelFormat("%d"); // <--

        ui->metricsWidget->add_chart(chartView);
    }

}

void get_min_max(const std::vector<double> &list, double &min, double &max)
{
    if (list.empty()) return;
    std::vector<double> tmp = list;
    sort(tmp.begin(), tmp.end());
    min = tmp.front();
    max = tmp.back();
}

void rescale_mesh_metrics(std::vector<MeshMetrics> &metrics)
{
    std::vector<double> INR_vals;
    std::vector<double> OUR_vals;
    std::vector<double> CIR_vals;
    std::vector<double> KRR_vals;
    std::vector<double> KAR_vals;
    std::vector<double> APR_vals;
    std::vector<double> MIA_vals;
    std::vector<double> MAA_vals;
    std::vector<double> ANR_vals;
    std::vector<double> VEM_vals;
    std::vector<double> JAC_vals;
    std::vector<double> FRO_vals;
    for (const MeshMetrics &metric : metrics)
    {
        INR_vals.push_back(metric.INR_mesh);
        OUR_vals.push_back(metric.OUR_mesh);
        CIR_vals.push_back(metric.CIR_mesh);
        KRR_vals.push_back(metric.KRR_mesh);
        KAR_vals.push_back(metric.KAR_mesh);
        APR_vals.push_back(metric.APR_mesh);
        MIA_vals.push_back(metric.MIA_mesh);
        MAA_vals.push_back(metric.MAA_mesh);
        ANR_vals.push_back(metric.ANR_mesh);
        VEM_vals.push_back(metric.VEM_mesh);
        JAC_vals.push_back(metric.JAC_mesh);
        FRO_vals.push_back(metric.FRO_mesh);
    }
    double INR_min, INR_max;
    double OUR_min, OUR_max;
    double CIR_min, CIR_max;
    double KRR_min, KRR_max;
    double KAR_min, KAR_max;
    double APR_min, APR_max;
    double MIA_min, MIA_max;
    double MAA_min, MAA_max;
    double ANR_min, ANR_max;
    double VEM_min, VEM_max;
    double JAC_min, JAC_max;
    double FRO_min, FRO_max;
    get_min_max(INR_vals, INR_min, INR_max);
    get_min_max(OUR_vals, OUR_min, OUR_max);
    get_min_max(CIR_vals, CIR_min, CIR_max);
    get_min_max(KRR_vals, KRR_min, KRR_max);
    get_min_max(KAR_vals, KAR_min, KAR_max);
    get_min_max(APR_vals, APR_min, APR_max);
    get_min_max(MIA_vals, MIA_min, MIA_max);
    get_min_max(MAA_vals, MAA_min, MAA_max);
    get_min_max(ANR_vals, ANR_min, ANR_max);
    get_min_max(VEM_vals, VEM_min, VEM_max);
    get_min_max(JAC_vals, JAC_min, JAC_max);
    get_min_max(FRO_vals, FRO_min, FRO_max);

    for (MeshMetrics &metric : metrics)
    {
        metric.INR_mesh = (metric.INR_mesh - INR_min) / (INR_max - INR_min);
        metric.OUR_mesh = (metric.OUR_mesh - OUR_min) / (OUR_max - OUR_min);
        metric.CIR_mesh = (metric.CIR_mesh - CIR_min) / (CIR_max - CIR_min);
        metric.KRR_mesh = (metric.KRR_mesh - KRR_min) / (KRR_max - KRR_min);
        metric.KAR_mesh = (metric.KAR_mesh - KAR_min) / (KAR_max - KAR_min);
        metric.APR_mesh = (metric.APR_mesh - APR_min) / (APR_max - APR_min);
        metric.MIA_mesh = (metric.MIA_mesh - MIA_min) / (MIA_max - MIA_min);
        metric.MAA_mesh = (metric.MAA_mesh - MAA_min) / (MAA_max - MAA_min);
        metric.ANR_mesh = (metric.ANR_mesh - ANR_min) / (ANR_max - ANR_min);
        metric.VEM_mesh = (metric.VEM_mesh - VEM_min) / (VEM_max - VEM_min);
        metric.JAC_mesh = (metric.JAC_mesh - JAC_min) / (JAC_max - JAC_min);
        metric.FRO_mesh = (metric.FRO_mesh - FRO_min) / (FRO_max - FRO_min);
    }
}

void MainWindow::show_full_mesh_metrics()
{
    ui->tab_widgets->setCurrentIndex(1);
    ui->tabWidget->setCurrentIndex(0);
    ui->tab_widgets->setTabEnabled(1, true);
    ui->meshFullMetricsWidget->set_dataset(&dataset);
    ui->meshFullMetricsWidget->set_metrics(&metrics);
    ui->graphicMeshMetricWidget->clean_canvas();

    metrics_local = ui->datasetWidget->get_parametric_meshes_metrics();
    rescale_mesh_metrics(metrics_local);

    ui->graphicMeshMetricWidget->set_metrics(&metrics_local);
    ui->graphicMeshMetricWidget->set_slider_max(dataset.get_parametric_meshes().size()-1);

    ui->graphicMeshMetricWidget->show_mesh(0);
    ui->meshFullMetricsWidget->reset();

    QChart *chart = new QChart();
    double min = cinolib::max_double, max = cinolib::min_double;

    for (uint i=0; i < n_metrics; i++)
    {
        QLineSeries *series_mesh = new QLineSeries();
        series_mesh->setName(metrics_names.at(i).c_str());
        series_mesh->setColor(extra_colors.at(i % extra_colors.size()));
        series_mesh->setMarkerSize(5.);
        series_mesh->setPointsVisible();

        for (uint m=0; m < metrics_local.size(); m++)
        {
            double val_mesh = 0.0;
            switch (i)
            {
            case 0:  { val_mesh = metrics_local.at(m).INR_mesh; break; }
            case 1:  { val_mesh = metrics_local.at(m).OUR_mesh; break; }
            case 2:  { val_mesh = metrics_local.at(m).CIR_mesh; break; }
            case 3:  { val_mesh = metrics_local.at(m).KRR_mesh; break; }
            case 4:  { val_mesh = metrics_local.at(m).KAR_mesh; break; }
            case 5:  { val_mesh = metrics_local.at(m).APR_mesh; break; }
            case 6:  { val_mesh = metrics_local.at(m).MIA_mesh; break; }
            case 7:  { val_mesh = metrics_local.at(m).MAA_mesh; break; }
            case 8:  { val_mesh = metrics_local.at(m).ANR_mesh; break; }
            case 9:  { val_mesh = metrics_local.at(m).VEM_mesh; break; }
            case 10: { val_mesh = metrics_local.at(m).JAC_mesh; break; }
            case 11: { val_mesh = metrics_local.at(m).FRO_mesh; break; }
            default: std::cerr << "Invalid metric index" << std::endl;
            }
            series_mesh->append(m, val_mesh);
        }
        chart->addSeries(series_mesh);

        min = std::min(min, get_metrics_min(i));
        max = std::max(max, get_metrics_max(i));
    }

    chart->legend()->show();
    chart->createDefaultAxes();
    chart->legend()->setAlignment(Qt::AlignRight);

    // chart->axes()[1]->setMax(0.93*1.001);
    // chart->axes()[1]->setMin(0.20*0.999);

    static_cast<QValueAxis *>(chart->axisX())->setLabelFormat("%i");
    // static_cast<QValueAxis *>(chart->axisX())->setMinorTickCount(10);

    std::string max_str = std::to_string(max);
    if (max == cinolib::max_double) max_str = "+inf";
    else if (max == 0) max_str = "0";
    else if (max == 1) max_str = "1";

    std::string min_str = std::to_string(min);
    if      (min == 0) min_str = "0";
    else if (min == 1) min_str = "1";

    std::string title = "Metrics Rescaled in [0,1]";
    // std::string title = "All Metrics ["+min_str+", "+max_str+"]";
    chart->setTitle(title.c_str());


    CustomizedChartView *chartView = new CustomizedChartView();
    chartView->setBackgroundBrush(QColor (230, 230, 230));

    // chart->setAnimationOptions(QChart::SeriesAnimations);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setChart(chart);
    QValueAxis *axisX = static_cast<QValueAxis *>(chart->axes(Qt::Horizontal).at(0)); // <--
    axisX->setTickCount(metrics_local.size());  // <--
    axisX->setLabelFormat("%d"); // <--

    ui->metricsWidget->add_chart(chartView);

    ui->meshFullMetricsWidget->add_chart(chartView);

    ui->scatterPlotsGGWidget->create_scatterPlots(dataset, metrics_local);
}

void MainWindow::show_solver_results(const uint solution_id, const std::string folder, const std::string filename)
{
    ui->solverResultsWidget->set_dataset(&dataset);
    ui->solverResultsWidget->clean_charts();

    if (solution_id < UINT_MAX)
    {
        ui->solverResultsWidget->set_solution_id(solution_id);
        ui->scatterPlotsGPWidget->set_solution_id(solution_id);
    }

    const std::string postfix_sol = "-VEM-sol.txt";
    const std::string postfix_gt = "-GROUND-TRUTH-sol.txt";
    const std::string filepath = folder + QString(QDir::separator()).toStdString() + filename; // global_stats.txt

    if (solution_id == UINT_MAX)
    {
        std::ofstream ofile;
        ofile.open(filepath);
        ofile << "errorH1, errInf, errorL2, h, condA" << std::endl;

        QDir oDir(folder.c_str());
        QStringList oDirList = oDir.entryList(QDir::Dirs|QDir::NoDotAndDotDot);

        for (QString subf : oDirList)
        {
            std::string folder_name = folder + QDir::separator().toLatin1() + subf.toStdString();
            std::cout << folder_name << std::endl;
            std::string solution_folder = folder_name     + QDir::separator().toLatin1() + "Solution";

            // print the content of "Errors.csv" to "global_stats.txt"

            std::string output_filename = solution_folder + QDir::separator().toLatin1() + "Errors.csv";

            std::ifstream efile;
            efile.open(output_filename);
            std::string line;
            std::getline(efile, line); // header
            std::getline(efile, line); // values

            std::stringstream lline (line);
            std::string esegment;
            std::vector<std::string> seglist;

            while (std::getline(lline, esegment, ';'))
            {
                seglist.push_back(esegment);
            }

            double global_h      = std::stod(seglist.at(3));
            double global_errL2  = std::stod(seglist.at(4));
            double global_errH1  = std::stod(seglist.at(5));
            double global_normL2 = std::stod(seglist.at(6));
            double global_normH1 = std::stod(seglist.at(7));
            double global_errInf = std::stod(seglist.at(8));
            double global_condA  = std::stod(seglist.at(10));

            efile.close();

            ofile << global_errH1 / global_normH1 << " " << global_errInf << " "
                  << global_errL2 / global_normL2 << " " << global_h << " " << global_condA << std::endl;

            // print the content of "Solution_Cell0Ds.csv" to sol_file and gt_file

            std::string solution_filename = solution_folder + QDir::separator().toLatin1() + "Solution_Cell0Ds.csv";
            std::ifstream sfile;
            sfile.open(solution_filename);
            std::getline(sfile, line);

            const std::string sol_filepath = folder + QString(QDir::separator()).toStdString() + folder_name.substr(folder_name.find_last_of(QDir::separator().toLatin1())+1) + postfix_sol;
            std::ofstream solfile;
            solfile.open(sol_filepath);
            std::cout << sol_filepath << std::endl;

            const std::string gt_filepath = folder + QString(QDir::separator()).toStdString() + folder_name.substr(folder_name.find_last_of(QDir::separator().toLatin1())+1) + postfix_gt;
            std::ofstream gtfile;
            gtfile.open(gt_filepath);
            std::cout << gt_filepath << std::endl;

            while (std::getline(sfile , line))
            {
                std::stringstream lline (line);
                std::string sssegment;
                std::vector<std::string> ssseglist;

                while (std::getline(lline, sssegment, ';'))
                {
                    ssseglist.push_back(sssegment);
                }

                solfile << ssseglist.at(4) << std::endl;
                gtfile << ssseglist.at(5) << std::endl;
            }

            sfile.close();
            solfile.close();
            gtfile.close();

            // copy the content of "Solution_Cell2Ds.csv" to errh1_scalar_field and errl2_scalar_field

            solution_filename = solution_folder + QDir::separator().toLatin1() + "Solution_Cell2Ds.csv";
            sfile.open(solution_filename);
            std::getline(sfile, line);

            std::vector<double> errH1_normH1s, errL2_normL2s;
            while (std::getline(sfile , line))
            {
                std::stringstream lline (line);
                std::string sssegment;
                std::vector<std::string> ssseglist;
                while (std::getline(lline, sssegment, ';'))
                {
                    ssseglist.push_back(sssegment);
                }

                double local_errL2  = std::stod(ssseglist.at(1));
                double local_errH1  = std::stod(ssseglist.at(2));
                // double local_normL2 = std::stod(ssseglist.at(3));
                // double local_normH1 = std::stod(ssseglist.at(4));

                errH1_normH1s.push_back(local_errH1/* / local_normH1*/);
                errL2_normL2s.push_back(local_errL2/* / local_normL2*/);
                // I do not normalize the errors because I am normalizing the whole field in [0,1] later
            }

            sfile.close();

            cinolib::ScalarField sf_errH1 (errH1_normH1s), sf_errL2 (errL2_normL2s);
            sf_errH1.normalize_in_01();
            sf_errL2.normalize_in_01();

            ui->solverResultsWidget->add_errh1_scalar_field(sf_errH1);
            ui->solverResultsWidget->add_errl2_scalar_field(sf_errL2);

            // copy the content of "Cell2Ds_VEMPerformance.csv" to Cond_scalar_field

            solution_filename = solution_folder + QDir::separator().toLatin1() + "Cell2Ds_VEMPerformance.csv";
            sfile.open(solution_filename);
            std::getline(sfile, line);

            std::vector<double> PIN_conds, PIZ_conds;

            while (std::getline(sfile , line))
            {
                std::stringstream lline (line);
                std::string sssegment;
                std::vector<std::string> ssseglist;
                while (std::getline(lline, sssegment, ';'))
                {
                    ssseglist.push_back(sssegment);
                }

                double local_PiNabla_Cond = std::stod(ssseglist.at(3));
                double local_Pi0k_Cond    = std::stod(ssseglist.at(4));

                PIN_conds.push_back(local_PiNabla_Cond);
                PIZ_conds.push_back(local_Pi0k_Cond);
            }

            sfile.close();

            cinolib::ScalarField sf_Cond (PIN_conds);
            sf_Cond.normalize_in_01();
            ui->solverResultsWidget->add_Cond_scalar_field(sf_Cond);
        }

        ofile.close();
    }

    std::cout << "processing " << filepath << std::endl;

    std::ifstream in;
    in.open(filepath.c_str());

    if (!in.is_open())
    {
        std::cerr << "Error opening " << filename << std::endl;
        return;
    }
    std::string name;
    std::getline(in, name); // skip header line

    std::vector<std::vector<double>> errs (5);
    double e0,e1,e2,e3,e4;

    std::vector<std::string> labels
        {"errH1", "errInf", "errL2", "hEmax", "condVect"};

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

        for (uint v=0; v < errs.at(i).size(); v++)
        {
//            if (!range_adapt)
                series->append(v, errs.at(i).at(v));
//            else
//                series->append(v, errs.at(i).at(v)*fact);
        }

        for (uint v=0; v < errs.at(i).size(); v++)
            std::cout << series->at(static_cast<int>(v)).x() << ", " << series->at(static_cast<int>(v)).y() << std::endl;
        std::cout << std::endl;

        series->setPointsVisible(true);
        series->setName(labels.at(i).c_str());

        QString y_axis_title;
//        if (range_adapt)
//            y_axis_title = /*QString(labels.at(i).c_str()) + */ "*" + QString(str_fact.c_str());

        QValueAxis *axisX = new QValueAxis();
        axisX->setLabelFormat("%d");
        chart->addAxis(axisX, Qt::AlignBottom);

        QValueAxis *axisY = new QValueAxis();
        axisY->setLabelFormat("%4.3f");
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

    std::cout << "processing solution and groundtruth" << std::endl;

    for (uint i=0; i < dataset.get_parametric_meshes().size(); i++)
    {       
        std::string basename = dataset.get_parametric_mesh_filename(i);
        basename = basename.substr(basename.find_last_of(QDir::separator().toLatin1())+1);
        basename = basename.substr(0, basename.find_last_of("."));

        std::string filepath_gt  = folder + QString(QDir::separator()).toStdString() +
                                   basename + postfix_gt;
        std::string filepath_sol = folder + QString(QDir::separator()).toStdString() +
                                   basename + postfix_sol;

        std::cout << "Parsing files " << filepath_gt << " and " << filepath_sol << std::endl;

        std::ifstream in_gt, in_sol;
        in_gt.open(filepath_gt.c_str());
        in_sol.open(filepath_sol.c_str());

        if (!in_gt.is_open() || !in_sol.is_open())
        {
            std::cerr << "Error opening " << filepath_gt << " or " << filepath_sol << std::endl;
            continue;
        }

        std::vector<double> vals_gt, vals_sol;
        double val_gt, val_sol;
        for (uint vid=0; vid < dataset.get_parametric_mesh(i)->num_verts(); vid++)
        {
            in_gt >> val_gt;
            vals_gt.push_back(val_gt);
            in_sol >> val_sol;
            vals_sol.push_back(val_sol);
        }

        cinolib::ScalarField sf_gt (vals_gt);
        cinolib::ScalarField sf_sol (vals_sol);

        double min = DBL_MAX, max = -DBL_MAX;
        for (uint ii=0; ii < sf_gt.size(); ii++)
        {
            if (sf_gt[ii] < min)  min = sf_gt[ii];
            if (sf_sol[ii] < min) min = sf_sol[ii];
            if (sf_gt[ii] > max)  max = sf_gt[ii];
            if (sf_sol[ii] > max) max = sf_sol[ii];
        }
        if (min != max)
        {
            double delta = max - min;
            for(int i=0; i<sf_gt.rows(); ++i)
            {
                sf_gt[i]  = (sf_gt[i] - min) / delta;
                sf_sol[i] = (sf_sol[i] - min) / delta;
            }
        }

        sf_gt.copy_to_mesh(*ui->solverResultsWidget->get_gt_mesh(i));
        sf_sol.copy_to_mesh(*dataset.get_parametric_mesh(i));

        ui->solverResultsWidget->add_gt_scalar_field(sf_gt);
        ui->solverResultsWidget->add_solution_scalar_field(sf_sol);

        for (uint vid=0; vid < ui->solverResultsWidget->get_gt_mesh(i)->num_verts(); vid++ )
        {
            ui->solverResultsWidget->get_gt_mesh(i)->vert_data(vid).color = cinolib::Color::red_white_blue_ramp_01(sf_gt[vid]);
            // std::cout << ui->solverResultsWidget->get_gt_mesh(i)->vert(vid) <<  " -- " << sf_gt[vid] << std::endl;
        }
        for (uint vid=0; vid < dataset.get_parametric_mesh(i)->num_verts(); vid++ )
        {
            dataset.get_parametric_mesh(i)->vert_data(vid).color = cinolib::Color::red_white_blue_ramp_01(sf_sol[vid]);
            // std::cout << dataset.get_parametric_mesh(i)->vert(vid) <<  " -- " << sf_sol[vid] << std::endl;
        }

        in_gt.close();
        in_sol.close();
    }

    ui->tab_widgets->setTabEnabled(3, true);
    ui->tab_widgets->setCurrentIndex(3);

    ui->solverResultsWidget->show_mesh_solution_and_groundtruth();

}

void MainWindow::on_tab_widgets_currentChanged(int index)
{
    if (index == 0)
    {
        if (dataset.get_parametric_meshes().size() == 0)
            return;

        // for (cinolib::Polygonmesh<> * p : ui->datasetWidget->get_dataset()->get_parametric_meshes())
        //     p->show_vert_color();
    }
    else
    if (index == 1)
    {
        ui->graphicMeshMetricWidget->set_slider_max(static_cast<const uint>( dataset.get_parametric_meshes().size()-1));

        // for (cinolib::Polygonmesh<> * p : ui->datasetWidget->get_dataset()->get_parametric_meshes())
        //     p->show_poly_color();
    }
    else
    if (index == 2) // solver widget
    {
        // for (uint m=0; m < dataset.get_parametric_meshes().size(); m++)
        // {
        //     dataset.get_parametric_mesh(m)->poly_set_color(cinolib::Color(1,1,1));
        //     dataset.get_parametric_mesh(m)->show_poly_color();
        // }

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


void MainWindow::compute_GP_scatterplots ()
{
    if (metrics.empty())
        ui->datasetWidget->compute_geometric_metrics();

    ui->scatterPlotsGPWidget->create_scatterPlots(dataset, metrics, errsToScatterPlots);

    ui->tab_widgets->setCurrentIndex(3);
    ui->tabWidget->setCurrentIndex(1);
}

void MainWindow::on_actionReset_triggered()
{
    delete ui->datasetWidget;
    delete ui->metricsWidget;
    // delete ui->graphicMeshMetricWidget;
    delete ui->meshFullMetricsWidget;
    delete ui->scatterPlotsGGWidget;
    delete ui->solverWidget;
    delete ui->solverResultsWidget;

    metrics.clear();
    dataset.clean();
    folder = "";

    ui->datasetWidget = new DatasetWidget(this);
    ui->datasetWidget->set_dataset(&dataset);
    ui->dataset_tab->layout()->addWidget(ui->datasetWidget);

    ui->meshFullMetricsWidget = new MeshFullMetricsWidget(this);
    ui->tab_6->layout()->addWidget(ui->meshFullMetricsWidget);

    ui->metricsWidget = new MeshMetricsWidget(this);
    ui->tab->layout()->addWidget(ui->metricsWidget);

    ui->scatterPlotsGGWidget = new GeometryGeometryScatterPlotsWidget(this);
    ui->tab_2->layout()->addWidget(ui->scatterPlotsGGWidget);

    // ui->graphicMeshMetricWidget = new MeshMetricsGraphicWidget(this);
    ui->graphicMeshMetricWidget->set_dataset(&dataset);
    // ui->tab_3->layout()->addWidget(ui->graphicMeshMetricWidget);

    ui->solverWidget = new SolverWidget (this);
    ui->solverWidget->set_dataset(&dataset);
    ui->solver_tab->layout()->addWidget(ui->solverWidget);

    ui->solverResultsWidget = new SolverResultsWidget(this);
    ui->solverResultsWidget->set_dataset(&dataset);
    ui->solver_results_tab->layout()->addWidget(ui->solverResultsWidget);

    connect(ui->datasetWidget, SIGNAL (computed_mesh_metrics()), this, SLOT(show_mesh_metrics()));
    connect(ui->datasetWidget, SIGNAL (computed_mesh_metrics()), this, SLOT(show_full_mesh_metrics()));
    connect(ui->metricsWidget, SIGNAL (sort_geometric_qualities(const uint)), this, SLOT(show_sorted_mesh_metrics(const uint)));
    connect(ui->metricsWidget, SIGNAL (show_unsorted_metrics()), this, SLOT(show_mesh_metrics()));
    connect(ui->metricsWidget, SIGNAL (show_unsorted_metrics()), this, SLOT(show_full_mesh_metrics()));
    connect(ui->solverWidget, SIGNAL (solver_completed (const uint, const std::string, const std::string)), this, SLOT (show_solver_results (const uint, const std::string, const std::string)));
    connect(ui->datasetWidget, SIGNAL (saved_in(const std::string)), this, SLOT (update_solver_input_folder (const std::string)));

    connect(ui->scatterPlotsGPWidget, SIGNAL(compute_GP_scatterplots()), this, SLOT(compute_GP_scatterplots()));

    ui->tab_widgets->setCurrentIndex(0);

    for (int i=1; i < ui->tab_widgets->count()-1; i++)
        ui->tab_widgets->setTabEnabled(i, false);

}

void MainWindow::disable_all_tabs()
{
    for (int i=1; i < ui->tab_widgets->count()-1; i++)
        ui->tab_widgets->setTabEnabled(i, false);
}


void MainWindow::on_actionSaveAll_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Save Project"), "",
                                                    tr("CHANGE Project (*.changeproj);;All Files (*)"));

    if (fileName.isNull() || fileName.isEmpty())
        return;
}


void MainWindow::on_actionAbout_triggered()
{
    LicenceDialog dialog;
    dialog.exec();
}



