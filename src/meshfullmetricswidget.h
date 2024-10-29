#ifndef MESHFULLMETRICSWIDGET_H
#define MESHFULLMETRICSWIDGET_H

#include "customizedchartview.h"
#include "dataset.h"

#include <QWidget>

#include <QtCharts/QChartView>

namespace Ui {
class MeshFullMetricsWidget;
}

class MeshFullMetricsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MeshFullMetricsWidget(QWidget *parent = nullptr);
    ~MeshFullMetricsWidget();

    void add_chart (CustomizedChartView *);

    void set_dataset (Dataset *d) { dataset = d; }
    void set_metrics (std::vector<MeshMetrics> *m) { metrics = m;}

    void reset ();

private slots:

    void on_inr_cb_stateChanged(int checked);
    void on_our_cb_stateChanged(int checked);
    void on_cir_cb_stateChanged(int checked);
    void on_krr_cb_stateChanged(int checked);
    void on_kar_cb_stateChanged(int checked);
    void on_apr_cb_stateChanged(int checked);
    void on_mia_cb_stateChanged(int checked);
    void on_maa_cb_stateChanged(int checked);
    void on_anr_cb_stateChanged(int checked);
    void on_vem_cb_stateChanged(int checked);
    void on_jac_cb_stateChanged(int checked);
    void on_fro_cb_stateChanged(int checked);

    void on_inr_color_cb_clicked();
    void on_our_color_cb_clicked();
    void on_cir_color_cb_clicked();
    void on_krr_color_cb_clicked();
    void on_kar_color_cb_clicked();
    void on_apr_color_cb_clicked();
    void on_mia_color_cb_clicked();
    void on_maa_color_cb_clicked();
    void on_anr_color_cb_clicked();
    void on_vem_color_cb_clicked();
    void on_jac_color_cb_clicked();
    void on_fro_color_cb_clicked();

    void on_save_txt_btn_clicked();
    void on_save_plots_btn_clicked();

    void update_cb_stateChanged();
    void on_all_btn_clicked();
    void on_none_btn_clicked();

private:
    Ui::MeshFullMetricsWidget *ui;

    std::vector<CustomizedChartView *> chart_views;

    Dataset *dataset = nullptr;
    std::vector<MeshMetrics> *metrics = nullptr;

    unsigned int last_row = 0;
    unsigned int last_col = 0;
    const unsigned int n_cols = 3;

    void change_series_color (const int series_id);
    void show_series(const unsigned int series_id, const bool check);
};

#endif // MESHFULLMETRICSWIDGET_H
