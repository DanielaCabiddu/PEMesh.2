#include "licence_dialog.h"
#include "ui_licence_dialog.h"

#include <QApplication>

LicenceDialog::LicenceDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LicenceDialog)
{
    ui->setupUi(this);
}

LicenceDialog::~LicenceDialog()
{
    delete ui;
}

void LicenceDialog::on_buttonBox_rejected()
{
    QCoreApplication::quit();
}

