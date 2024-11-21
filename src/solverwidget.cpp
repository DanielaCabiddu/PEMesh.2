/********************************************************************************
*  This file is part of PEMesh                                                  *
*  Copyright(C) 2022: Daniela Cabiddu                                           *

*                                                                               *
*  Author(s):                                                                   *
*                                                                               *
*     Daniela Cabiddu (daniela.cabiddu@cnr.it)                                  *
*                                                                               *
*     Italian National Research Council (CNR)                                   *
*     Institute for Applied Mathematics and Information Technologies (IMATI)    *
*     Via de Marini, 6                                                          *
*     16149 Genoa,                                                              *
*     Italy                                                                     *
*                                                                               *
*                                                                               *
*                                                                               *
*  This program is free software: you can redistribute it and/or modify it      *
*  under the terms of the GNU General Public License as published by the        *
*  Free Software Foundation, either version 3 of the License, or (at your       *
*  option) any later version.                                                   *
*                                                                               *
*  This program is distributed in the hope that it will be useful, but          *
*  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY   *
*  or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for  *
*  more details.                                                                *
*                                                                               *
*  You should have received a copy of the GNU General Public License along      *
*  with this program. If not, see <https://www.gnu.org/licenses/>.              *
*                                                                               *
*********************************************************************************/

#include "solverwidget.h"
#include "ui_solverwidget.h"

#include "solversettingsdialog.h"

#include <QDir>

SolverWidget::SolverWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SolverWidget)
{
    ui->setupUi(this);

    process = new QProcess();
    connect(process, SIGNAL(readyRead()), this, SLOT(update_log()));

    // ui->log_text->hide();
}

SolverWidget::~SolverWidget()
{
    delete ui;
}

void SolverWidget::set_dataset(Dataset *d)
{
    dataset = d;
}

void SolverWidget::set_input_folder(const std::string folder)
{
    input_folder = folder;
}

void SolverWidget::update()
{
    if (dataset == nullptr)
        return;

    if (dataset->get_parametric_meshes().size() > 0 )
    {
        ui->t_slider->setMaximum(static_cast<int>(dataset->get_parametric_meshes().size())-1);
        show_parametric_mesh(ui->t_slider->value());
    }
}

void SolverWidget::clean_canvas()
{
    // for (DrawablePolygonmesh<> * p : drawable_polys)
    //     ui->canvas->pop(p);

    drawable_polys.clear();
}

void SolverWidget::show_parametric_mesh(int index)
{
    clean_canvas();

    cinolib::Polygonmesh<> *p = dataset->get_parametric_mesh(static_cast<uint>(index));

    // dataset->get_parametric_mesh(static_cast<uint>(index))->updateGL();
    // ui->canvas->push_obj(p, update_scene);
    // ui->canvas->updateGL();

    ui->canvas->clear();
    ui->canvas->add_mesh(*p, false);

    drawable_polys.push_back(p);

    update_scene = false;

    curr_mesh_id = static_cast<uint>(index);
}

void SolverWidget::on_t_slider_valueChanged(int value)
{
    show_parametric_mesh(value);
}

/* create the directory if it does not exist, otherwise delete its content */
void open_directory(const std::string &path, bool erase = true) {
    if (!std::filesystem::exists(path)) {
        // if the folder does not exist, create it
        try {
            std::filesystem::create_directories(path);
        } catch (const std::exception &e) {
            std::cerr << "Error creating folder: " << e.what() << std::endl;
            assert(false);
        }
    } else if (erase) {
        // if the folder already exists, delete its content
        for (const auto &entry : std::filesystem::directory_iterator(path)) {
            if (entry.is_regular_file()) {
                std::filesystem::remove(entry.path());
            } else if (entry.is_directory()) {
                std::filesystem::remove_all(entry.path());
            }
        }
    }
}

void SolverWidget::on_run_btn_clicked()
{
    SolverSettingsDialog *dialog = new SolverSettingsDialog();

    dialog->set_input_folder(input_folder);
    std::string output_folder = input_folder + "/out";
    open_directory(output_folder, true);
    dialog->set_output_folder(output_folder);
    dialog->disable_input_folder_selection();

    if (dialog->exec() == 1)
    {
        //ui->log_label->append("Running solver ... ");

        QApplication::setOverrideCursor(Qt::WaitCursor);

        QString path_sep = QString(QDir::separator());

        const std::string in_folder = dialog->get_input_folder();
        const std::string out_folder = dialog->get_output_folder();
        const std::string out_filename = dialog->get_output_filename();
        const std::string out_filepath = out_folder + path_sep.toStdString() + out_filename;

        std::cout << in_folder << std::endl;
        std::cout << out_folder << std::endl;
        std::cout << out_filename << std::endl;

        const std::string solver_script_path = dialog->get_solver_script_full_path();

        std::cout << solver_script_path << std::endl;

        const std::string scripts_folder = solver_script_path.substr(0, solver_script_path.find_last_of(QDir::separator().unicode()));

        std::cout << scripts_folder << std::endl;

        std::string solver_script_name = solver_script_path.substr(solver_script_path.find_last_of(QDir::separator().unicode())+1);

        std::cout << solver_script_name << std::endl;

        std::cout << solver_script_name.find('.') << std::endl;

        const unsigned int solution_id = static_cast<uint>(ui->solver_cb->currentIndex());
        const unsigned int solution_order = static_cast<uint>(ui->solver_order_cb->value());
        const bool compute_cond_number =ui->cond_number_cb->isChecked();

        if (solver_script_name.find('.') != std::string::npos &&
            solver_script_name.substr(solver_script_name.find_last_of(".")).compare(".m") == 0)
        {

            std::cout << "matlab ... " << std::endl;

            const std::string matlab_folder = dialog->get_matlab_folder();
            const std::string matlab_exe = matlab_folder + QString(QDir::separator()).toStdString() + dialog->get_matlab_exe_name();

            const std::string cd_cmd = std::string ("cd ") + std::string(scripts_folder);

            solver_script_name = solver_script_name.substr(0, solver_script_name.find_last_of("."));
            const std::string matlab_script = solver_script_name + std::string(" ('")
                                                + in_folder + "','" + out_filepath + "'," + std::to_string(solution_id) + "," + std::to_string(solution_order) +");exit;";

            std::cout << "RUNNING : " << matlab_script << std::endl;

            const std::string matlab_args = "-nodisplay -nosplash -nodesktop -r \"" + matlab_script + "\";";


            process->setWorkingDirectory(scripts_folder.c_str());

            process->moveToThread(QCoreApplication::instance()->thread());

            process->start(
                QString(matlab_exe.c_str()),
                QStringList() << QString( (matlab_args).c_str() ));

            emit (solver_completed (solution_id, out_folder, out_filename));
        }
        else
        {
            std::cout << "exe ... " << std::endl;

            process->setWorkingDirectory(scripts_folder.c_str());

            process->moveToThread(QCoreApplication::instance()->thread());

            QDir directory(in_folder.c_str());
            QStringList meshes = directory.entryList(QStringList() << "*.off" << "*.OFF",QDir::Files);

            std::string docker_exe = "docker";

            foreach(QString filename, meshes)
            {
                std::string out_mesh_folder = "/shared_out/" + filename.toStdString().substr(0,filename.lastIndexOf("."));
                std::string mesh_gen_args = "MeshGenerator:uint=1";
                std::string concavity_args = "ConcavityType:uint=1";
                std::string mesh_args = "MeshOFF_Aggregated_FilePath:string=/shared/" + filename.toStdString();
                std::string out_args = "ExportFolder:string=" + out_mesh_folder;
                std::string cond_args = "ComputeConditionNumber:bool=";

                if (compute_cond_number)
                    cond_args += std::to_string(0);
                else
                    cond_args += std::to_string(1);

                std::string solution_args = "ProgramType:uint=" + std::to_string(solution_id);
                std::string order_args = "VemOrder:uint=" + std::to_string(solution_order);

                std::string docker_params = "run --rm -it -v";
                std::string shared_in_folder = in_folder + ":/shared";
                std::string shared_out_folder = out_folder + ":/shared_out";
                std::string docker_image = "siggraph_2024_solver:1.0.0";

                // docker build --no-cache -f Dockerfile --target siggraph_2024_solver . -t siggraph_2024_solver:1.0.0

                // run --rm -it -v .\simulations:/shared siggraph_2024_solver:1.0.0
                // /bin/bash -c "./VEM_2D_SOLVER MeshGenerator:uint=1 MeshOFF_Aggregated_FilePath:string=/shared/meshes/mesh.off ExportFolder:string=/shared/Run ComputeConditionNumber:bool=1 ProgramType:uint=0"

                std::string cmd = "\"./VEM_2D_SOLVER " + mesh_gen_args + " " + concavity_args + " " + mesh_args + " " +
                                  out_args + " " + cond_args + " " + solution_args + " " + order_args + "\"";

                cmd =  "docker run --rm -v " + shared_in_folder + " -v " + shared_out_folder + " " + docker_image.c_str() + " " + "/bin/bash " +  "-c " + cmd;

                std::cout << cmd << std::endl;

                ui->log_text->append(cmd.c_str());


                // QStringList args
                // {
                //     "run",
                //     "--rm",
                //     "-it",
                //     "-v",
                //     shard_folder.c_str(),
                //     docker_image.c_str(),
                //     "/bin/bash",
                //     "-c",
                //     cmd.c_str()
                //     //mesh_gen_args.c_str(), concavity_args.c_str(), mesh_args.c_str(), out_args.c_str(), cond_args.c_str(), solution_args.c_str(), order_args.c_str()
                // };

                // for (const auto& s : args) std::cout << " -- " << s.toStdString() << std::endl;

                process->moveToThread(QCoreApplication::instance()->thread());

                process->startCommand(QString(cmd.c_str()));

                process->waitForFinished();

                QString output(process->readAllStandardOutput());
                QString error(process->readAllStandardError());

                std::cout << process->exitCode() << std::endl;
                std::cout << error.toStdString() << std::endl;
                std::cout << output.toStdString() << std::endl << std::flush;

                if (error.length()>0)
                    ui->log_text->append(error);
                else {
                    std::cerr << "Error: solver crashed!" << std::endl;
                    exit(0);
                }

                if (output.length()>0)
                    ui->log_text->append(output);

                QApplication::processEvents();
            }

            emit (solver_completed (UINT_MAX, out_folder, out_filename));
        }

        QApplication::restoreOverrideCursor();


        // std::string file_finished = out_filepath + "_DONE";

        // while (!QFile::exists(file_finished.c_str()))
        // {
        // QCoreApplication::processEvents();
        // }

        // remove(file_finished.c_str());

        // std::cout << process->exitStatus() << std::endl;
        // std::cout << process->readAllStandardError().toStdString() << std::endl;
        // std::cout << process->readAllStandardOutput().toStdString() << std::endl;

        std::cout << "DONE" << std::endl;
    }

    delete dialog;
}

void SolverWidget::update_log()
{
    QString appendText(process->readAll());
    ui->log_text->append(appendText);

    std::cout << appendText.toStdString() << std::endl;
}
