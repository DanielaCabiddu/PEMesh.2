#include "optimizedialog.h"
#include "ui_optimizedialog.h"

#include "quality_metrics.h"

OptimizeDialog::OptimizeDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::OptimizeDialog)
{
    ui->setupUi(this);

    for (uint i=0; i < metrics_names.size(); i++)
    {
        ui->indicator_cb->addItem(metrics_names.at(i).c_str());
    }
}

OptimizeDialog::~OptimizeDialog()
{
    delete ui;
}

int OptimizeDialog::get_indicator () const
{
    return ui->indicator_cb->currentIndex();
}

void OptimizeDialog::get_indicator ( double (*indicator)(const std::vector<cinolib::vec3d>&) ) const
{
    switch (ui->indicator_cb->currentIndex()) {
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
}

void OptimizeDialog::get_weights ( bool &node_weights, bool arc_weights ) const
{
    switch (ui->weights_cb->currentIndex()) {
    case 0:
        node_weights = true;
        arc_weights  = true;
        break;
    case 1:
        node_weights = true;
        arc_weights  = false;
        break;
    case 2:
        node_weights = false;
        arc_weights  = true;
        break;
    case 3:
        node_weights = false;
        arc_weights  = false;
        break;
    default:
        break;
    }
}

void OptimizeDialog::get_parameter ( double &parameter ) const
{
    parameter = ui->parameter_cb->value();
}

void OptimizeDialog::get_overwrite ( bool &overwrite ) const
{
    overwrite = ui->overwrite_cb->checkState();
}
