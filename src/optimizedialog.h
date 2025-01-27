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

    int  get_indicator () const;
    void get_indicator ( double (*indicator)(const std::vector<cinolib::vec3d>&) ) const;
    void get_weights   ( bool &node_weights, bool arc_weights ) const;
    void get_parameter ( double &parameter ) const;
    void get_preserve  ( bool &preserve  ) const;
    void get_overwrite ( bool &overwrite ) const;
    void get_optimize_all ( bool &optimize_all ) const;

private:
    Ui::OptimizeDialog *ui;
};

#endif // OPTIMIZEDIALOG_H
