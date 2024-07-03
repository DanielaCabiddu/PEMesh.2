#ifndef LICENCE_DIALOG_H
#define LICENCE_DIALOG_H

#include <QDialog>

namespace Ui {
class LicenceDialog;
}

class LicenceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LicenceDialog(QWidget *parent = nullptr);
    ~LicenceDialog();

private slots:
    void on_buttonBox_rejected();

private:
    Ui::LicenceDialog *ui;
};

#endif // LICENCE_DIALOG_H
