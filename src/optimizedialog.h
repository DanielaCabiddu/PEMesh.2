#ifndef OPTIMIZEDIALOG_H
#define OPTIMIZEDIALOG_H

#include <QDialog>

namespace Ui {
class OptimizeDialog;
}

class OptimizeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OptimizeDialog(QWidget *parent = nullptr);
    ~OptimizeDialog();

private:
    Ui::OptimizeDialog *ui;
};

#endif // OPTIMIZEDIALOG_H
