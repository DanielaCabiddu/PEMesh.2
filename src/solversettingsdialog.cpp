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

#include "solversettingsdialog.h"
#include "ui_solversettingsdialog.h"

#include <QFileDialog>
#include <QPushButton>

SolverSettingsDialog::SolverSettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SolverSettingsDialog)
{
    ui->setupUi(this);

    ui->matlab_folder_label->setVisible(false);
    ui->matlab_directory_btn->setVisible(false);
    ui->matlab_folder->setVisible(false);

    #ifdef WIN32
        matlab_exe_name = "matlab.exe";
    #else
        matlab_exe_name = "matlab";
    #endif

    #ifdef __APPLE__
        ui->matlab_folder->setText("/Applications/MATLAB_R2023b.app/bin/");
    #else
        ui->matlab_folder->setText(QDir::homePath().append("/matlab/bin/"));
    #endif

    std::string solver_path = QCoreApplication::applicationDirPath().toStdString() +
                                  QDir::separator().toLatin1() + "VEM_2D_SOLVER";
    ui->solver_script->setText(QString(solver_path.c_str()));

    ui->matlab_folder_error->hide();
    ui->solver_folder_error->hide();

    ui->matlab_folder_error->setStyleSheet("QLabel { color : red; }");
    ui->solver_folder_error->setStyleSheet("QLabel { color : red; }");

    bool matlab_ok = check_matlab_folder_ok();
    bool solver_ok = check_solver_folder_ok();

    if (!matlab_ok || !solver_ok)
    {
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    }
}

SolverSettingsDialog::~SolverSettingsDialog()
{
    delete ui;
}

const std::string SolverSettingsDialog::get_input_folder () const
{
    return ui->input_directory->text().toStdString();
}

const std::string SolverSettingsDialog::get_output_folder () const
{
    return  ui->output_directory->text().toStdString();
}

const std::string SolverSettingsDialog::get_output_filename () const
{
    return ui->output_filename->text().toStdString();
}

const std::string SolverSettingsDialog::get_matlab_folder () const
{
    return ui->matlab_folder->text().toStdString();
}

const std::string SolverSettingsDialog::get_matlab_exe_name() const
{
    return matlab_exe_name;
}

const std::string SolverSettingsDialog::get_solver_script_full_path() const
{
    return ui->solver_script->text().toStdString();
}

void SolverSettingsDialog::on_output_directory_btn_clicked()
{
    QString start_folder = "";

    if (ui->output_directory->text().length() > 0)
        start_folder = ui->output_directory->text();

    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                start_folder,
                                                QFileDialog::ShowDirsOnly
                                                | QFileDialog::DontResolveSymlinks);

    if (!dir.isNull())
    {
        ui->output_directory->setText(dir);
    }
}

void SolverSettingsDialog::on_input_directory_btn_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                "",
                                                QFileDialog::ShowDirsOnly
                                                | QFileDialog::DontResolveSymlinks);

    if (!dir.isNull())
    {
        ui->input_directory->setText(dir);
    }
}


void SolverSettingsDialog::set_input_folder (const std::string folder)
{
    ui->input_directory->setText(folder.c_str());
}

void SolverSettingsDialog::set_output_folder (const std::string folder)
{
    ui->output_directory->setText(folder.c_str());
}

void SolverSettingsDialog::disable_input_folder_selection()
{
    ui->input_directory_btn->setEnabled(false);
}

void SolverSettingsDialog::on_solver_directory_btn_clicked()
{
    QString file = QFileDialog::getOpenFileName(this, tr("Select Solver"),
                                                "");

    if (!file.isNull())
    {
        ui->solver_script->setText(file);

        // solver_folder = dir.toStdString();
        // check_solver_folder_ok();

        if (file.endsWith(".m"))
        {
            ui->matlab_folder_label->setVisible(true);
            ui->matlab_directory_btn->setVisible(true);
            ui->matlab_folder->setVisible(true);
        }
    }
}

void SolverSettingsDialog::on_matlab_directory_btn_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                "",
                                                QFileDialog::ShowDirsOnly
                                                | QFileDialog::DontResolveSymlinks);

    if (!dir.isNull())
    {
        ui->matlab_folder->setText(dir);

        matlab_folder = dir.toStdString();
        check_matlab_folder_ok();
    }
}

bool SolverSettingsDialog::check_matlab_folder_ok()
{
    if (!ui->matlab_folder->isVisible())
        return true;

    QString dir = ui->matlab_folder->text();

    QDir folder (dir);
    QString matlab_exe = QString(matlab_exe_name.c_str());

    if (!folder.exists(dir))
    {
        QString message = "ERROR: ";
        message.append(dir.append(" does not exist."));
        ui->matlab_folder_error->setText(tr(message.toStdString().c_str()));
        ui->matlab_folder_error->show();
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
        return false;
    }

    if (!QFile::exists(dir + QString(QDir::separator()) + matlab_exe))
    {
        QString message = dir.append(" does not contain Matlab executable ");
        message.append(matlab_exe_name.c_str());

        ui->matlab_folder_error->setText(tr(message.toStdString().c_str()));
        ui->matlab_folder_error->show();
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
        return false;
    }

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
    ui->matlab_folder_error->hide();
    return true;
}

bool SolverSettingsDialog::check_solver_folder_ok()
{
    QString script_path = ui->solver_script->text();

    if (!script_path.endsWith(".m"))
        return true;

    if (!QFile::exists(script_path))
    {
        QString message = script_path.append(" does not exists ");

        ui->solver_folder_error->setText(tr(message.toStdString().c_str()));
        ui->solver_folder_error->show();
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
        return false;
    }

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
    ui->solver_folder_error->hide();
    return true;

}
