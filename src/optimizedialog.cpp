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
        cbID2metricsID.push_back(i);
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

int OptimizeDialog::get_weights () const
{
    return ui->weights_cb->currentIndex();
}

double OptimizeDialog::get_parameter () const
{
    return ui->parameter_cb->value();
}
