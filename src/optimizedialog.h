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

    int get_indicator () const;
    int get_weights () const;
    double get_parameter () const;

private:
    Ui::OptimizeDialog *ui;

    std::vector<uint> cbID2metricsID;

};

#endif // OPTIMIZEDIALOG_H
