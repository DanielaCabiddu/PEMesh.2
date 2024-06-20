#include "optimizedialog.h"
#include "ui_optimizedialog.h"

OptimizeDialog::OptimizeDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::OptimizeDialog)
{
    ui->setupUi(this);
}

OptimizeDialog::~OptimizeDialog()
{
    delete ui;
}
