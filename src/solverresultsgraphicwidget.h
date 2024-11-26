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

    void set_t_slider_value (const uint val);
    void set_t_slider_min (const uint val);
    void set_t_slider_max (const uint val);

    cinolib::Polygonmesh<> * get_gt_mesh (const uint i);

    void add_solution_scalar_field (const cinolib::ScalarField &f, const double min, const double max)
        {solutions.push_back(f); solution_mins.push_back(min); solution_maxs.push_back(max);}

    void add_gt_scalar_field (const cinolib::ScalarField &f, const double min, const double max)
        {groundtruths.push_back(f); groundtruths_mins.push_back(min); groundtruths_maxs.push_back(max);}

    void add_errh1_scalar_field (const cinolib::ScalarField &f, const double min, const double max)
        {errH1s.push_back(f); errH1s_mins.push_back(min); errH1s_maxs.push_back(max);}

    void add_errl2_scalar_field (const cinolib::ScalarField &f, const double min, const double max)
        {errL2s.push_back(f); errL2s_mins.push_back(min); errL2s_maxs.push_back(max);}

    void add_Cond_scalar_field (const cinolib::ScalarField &f, const double min, const double max)
        {Conds.push_back(f); Conds_mins.push_back(min); Conds_maxs.push_back(max);}

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

    std::vector<double> solution_mins;
    std::vector<double> solution_maxs;

    std::vector<double> groundtruths_mins;
    std::vector<double> groundtruths_maxs;

    std::vector<double> errH1s_mins;
    std::vector<double> errH1s_maxs;

    std::vector<double> errL2s_mins;
    std::vector<double> errL2s_maxs;

    std::vector<double> Conds_mins;
    std::vector<double> Conds_maxs;
};

#endif // SOLVERRESULTSGRAPHICWIDGET_H
