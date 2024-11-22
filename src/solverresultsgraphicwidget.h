#ifndef SOLVERRESULTSGRAPHICWIDGET_H
#define SOLVERRESULTSGRAPHICWIDGET_H

#include "dataset.h"

#include <cinolib/cino_inline.h>
#include <cinolib/meshes/drawable_polygonmesh.h>
#include <cinolib/scalar_field.h>

#include <QWidget>

namespace Ui {
class SolverResultsGraphicWidget;
}

class SolverResultsGraphicWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SolverResultsGraphicWidget(QWidget *parent = nullptr);
    ~SolverResultsGraphicWidget();

    void set_dataset (Dataset *d);
    void set_solution_id (const uint id) { solution_id = id; }

    void show_mesh (const uint index);

    void show_mesh_solution_and_groundtruth () const;

    void clean_canvas ();

    cinolib::Polygonmesh<> * get_gt_mesh (const uint i);

    void add_solution_scalar_field (const cinolib::ScalarField &f) {solutions.push_back(f);}
    void add_gt_scalar_field (const cinolib::ScalarField &f) {groundtruths.push_back(f);}
    void add_errh1_scalar_field (const cinolib::ScalarField &f) {errH1s.push_back(f);}
    void add_errl2_scalar_field (const cinolib::ScalarField &f) {errL2s.push_back(f);}
    void add_Cond_scalar_field (const cinolib::ScalarField &f) {Conds.push_back(f);}

public slots:

    void show_parametric_mesh (int);

private slots:

    void on_t_slider_valueChanged(int value);

    void on_canvas1_cb_currentIndexChanged(int index);

    void on_canvas2_cb_currentIndexChanged(int index);

private:
    Ui::SolverResultsGraphicWidget *ui;

    uint solution_id = UINT_MAX;

    bool update_scene = true;
    uint curr_mesh_id = cinolib::max_uint;

    std::vector<cinolib::Polygonmesh<> *> meshes_canvas1;
    std::vector<cinolib::Polygonmesh<> *> meshes_canvas2;

    std::vector<cinolib::ScalarField> solutions;
    std::vector<cinolib::ScalarField> groundtruths;
    std::vector<cinolib::ScalarField> errH1s;
    std::vector<cinolib::ScalarField> errL2s;
    std::vector<cinolib::ScalarField> Conds;
};

#endif // SOLVERRESULTSGRAPHICWIDGET_H
