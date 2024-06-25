#ifndef OPTIMIZEDIALOG_H
#define OPTIMIZEDIALOG_H

#include <QDialog>
#include "meshes/mesh_metrics.h"

namespace Ui {
class OptimizeDialog;
}

class OptimizeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OptimizeDialog(QWidget *parent = nullptr);
    ~OptimizeDialog();

    void get_indicator ( double (*indicator)(const std::vector<cinolib::vec3d>&) ) const;
    void get_weights ( bool &node_weights, bool arc_weights ) const;
    void get_parameter ( double &parameter ) const;

private:
    Ui::OptimizeDialog *ui;
};

#endif // OPTIMIZEDIALOG_H
