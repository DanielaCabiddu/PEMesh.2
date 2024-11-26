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
#include <QRegExp>
#include <QFontDatabase>

SolverWidget::SolverWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SolverWidget)
{
    ui->setupUi(this);

    process = new QProcess();
    // connect(process, SIGNAL(readyRead()), this, SLOT(update_log()));

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

                // std::string running_log = "\n\n\033[1;30mRunning : \033[0m" + cmd + "\n";
                std::string running_log = "Solving equation on mesh: " + filename.toStdString() + "...\n";
                update_log(running_log.c_str());

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

                if (output.length()>0)
                    update_log(output);

                if (error.length()>0) {
                    update_log("\033[1;30mDone!\033[0m\n");
                    update_log(error);
                } else {
                    std::cerr << "Error: solver crashed!" << std::endl;
                    exit(0);
                }

                update_log("\n");
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

        update_log("Solving completed.\n");
        std::cout << "Solving completed!" << std::endl;
    }

    delete dialog;
}

/// code from https://stackoverflow.com/questions/26500429/qtextedit-and-colored-bash-like-output-emulation
void SolverWidget::parseEscapeSequence(int attribute, QListIterator< QString > & i, QTextCharFormat & textCharFormat, QTextCharFormat const & defaultTextCharFormat)
{
    switch (attribute) {
    case 0 : { // Normal/Default (reset all attributes)
        textCharFormat = defaultTextCharFormat;
        break;
    }
    case 1 : { // Bold/Bright (bold or increased intensity)
        textCharFormat.setFontWeight(QFont::Bold);
        break;
    }
    case 2 : { // Dim/Faint (decreased intensity)
        textCharFormat.setFontWeight(QFont::Light);
        break;
    }
    case 3 : { // Italicized (italic on)
        textCharFormat.setFontItalic(true);
        break;
    }
    case 4 : { // Underscore (single underlined)
        textCharFormat.setUnderlineStyle(QTextCharFormat::SingleUnderline);
        textCharFormat.setFontUnderline(true);
        break;
    }
    case 5 : { // Blink (slow, appears as Bold)
        textCharFormat.setFontWeight(QFont::Bold);
        break;
    }
    case 6 : { // Blink (rapid, appears as very Bold)
        textCharFormat.setFontWeight(QFont::Black);
        break;
    }
    case 7 : { // Reverse/Inverse (swap foreground and background)
        QBrush foregroundBrush = textCharFormat.foreground();
        textCharFormat.setForeground(textCharFormat.background());
        textCharFormat.setBackground(foregroundBrush);
        break;
    }
    case 8 : { // Concealed/Hidden/Invisible (usefull for passwords)
        textCharFormat.setForeground(textCharFormat.background());
        break;
    }
    case 9 : { // Crossed-out characters
        textCharFormat.setFontStrikeOut(true);
        break;
    }
    case 10 : { // Primary (default) font
        textCharFormat.setFont(defaultTextCharFormat.font());
        break;
    }
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
    case 19 : {
        QFontDatabase fontDatabase;
        QString fontFamily = textCharFormat.fontFamily();
        QStringList fontStyles = fontDatabase.styles(fontFamily);
        int fontStyleIndex = attribute - 11;
        if (fontStyleIndex < fontStyles.length()) {
            textCharFormat.setFont(fontDatabase.font(fontFamily, fontStyles.at(fontStyleIndex), textCharFormat.font().pointSize()));
        }
        break;
    }
    case 20 : { // Fraktur (unsupported)
        break;
    }
    case 21 : { // Set Bold off
        textCharFormat.setFontWeight(QFont::Normal);
        break;
    }
    case 22 : { // Set Dim off
        textCharFormat.setFontWeight(QFont::Normal);
        break;
    }
    case 23 : { // Unset italic and unset fraktur
        textCharFormat.setFontItalic(false);
        break;
    }
    case 24 : { // Unset underlining
        textCharFormat.setUnderlineStyle(QTextCharFormat::NoUnderline);
        textCharFormat.setFontUnderline(false);
        break;
    }
    case 25 : { // Unset Blink/Bold
        textCharFormat.setFontWeight(QFont::Normal);
        break;
    }
    case 26 : { // Reserved
        break;
    }
    case 27 : { // Positive (non-inverted)
        QBrush backgroundBrush = textCharFormat.background();
        textCharFormat.setBackground(textCharFormat.foreground());
        textCharFormat.setForeground(backgroundBrush);
        break;
    }
    case 28 : {
        textCharFormat.setForeground(defaultTextCharFormat.foreground());
        textCharFormat.setBackground(defaultTextCharFormat.background());
        break;
    }
    case 29 : {
        textCharFormat.setUnderlineStyle(QTextCharFormat::NoUnderline);
        textCharFormat.setFontUnderline(false);
        break;
    }
    case 30:
    case 31:
    case 32:
    case 33:
    case 34:
    case 35:
    case 36:
    case 37 : {
        int colorIndex = attribute - 30;
        QColor color;
        if (QFont::Normal < textCharFormat.fontWeight()) {
            switch (colorIndex) {
            case 0 : {
                color = Qt::darkGray;
                break;
            }
            case 1 : {
                color = Qt::red;
                break;
            }
            case 2 : {
                color = Qt::green;
                break;
            }
            case 3 : {
                color = Qt::yellow;
                break;
            }
            case 4 : {
                color = Qt::blue;
                break;
            }
            case 5 : {
                color = Qt::magenta;
                break;
            }
            case 6 : {
                color = Qt::cyan;
                break;
            }
            case 7 : {
                color = Qt::white;
                break;
            }
            default : {
                Q_ASSERT(false);
            }
            }
        } else {
            switch (colorIndex) {
            case 0 : {
                color = Qt::black;
                break;
            }
            case 1 : {
                color = Qt::darkRed;
                break;
            }
            case 2 : {
                color = Qt::darkGreen;
                break;
            }
            case 3 : {
                color = Qt::darkYellow;
                break;
            }
            case 4 : {
                color = Qt::darkBlue;
                break;
            }
            case 5 : {
                color = Qt::darkMagenta;
                break;
            }
            case 6 : {
                color = Qt::darkCyan;
                break;
            }
            case 7 : {
                color = Qt::lightGray;
                break;
            }
            default : {
                Q_ASSERT(false);
            }
            }
        }
        textCharFormat.setForeground(color);
        break;
    }
    case 38 : {
        if (i.hasNext()) {
            bool ok = false;
            int selector = i.next().toInt(&ok);
            Q_ASSERT(ok);
            QColor color;
            switch (selector) {
            case 2 : {
                if (!i.hasNext()) {
                    break;
                }
                int red = i.next().toInt(&ok);
                Q_ASSERT(ok);
                if (!i.hasNext()) {
                    break;
                }
                int green = i.next().toInt(&ok);
                Q_ASSERT(ok);
                if (!i.hasNext()) {
                    break;
                }
                int blue = i.next().toInt(&ok);
                Q_ASSERT(ok);
                color.setRgb(red, green, blue);
                break;
            }
            case 5 : {
                if (!i.hasNext()) {
                    break;
                }
                int index = i.next().toInt(&ok);
                Q_ASSERT(ok);
                switch (index) {
                case 0x00:
                case 0x01:
                case 0x02:
                case 0x03:
                case 0x04:
                case 0x05:
                case 0x06:
                case 0x07 : { // 0x00-0x07:  standard colors (as in ESC [ 30..37 m)
                    return parseEscapeSequence(index - 0x00 + 30, i, textCharFormat, defaultTextCharFormat);
                }
                case 0x08:
//                    ... 0x0F :
                    { // 0x08-0x0F:  high intensity colors (as in ESC [ 90..97 m)
                    return parseEscapeSequence(index - 0x08 + 90, i, textCharFormat, defaultTextCharFormat);
                }
                case 0x10:
                    // ... 0xE7 :
                    { // 0x10-0xE7:  6*6*6=216 colors: 16 + 36*r + 6*g + b (0≤r,g,b≤5)
                    index -= 0x10;
                    int red = index % 6;
                    index /= 6;
                    int green = index % 6;
                    index /= 6;
                    int blue = index % 6;
                    index /= 6;
                    Q_ASSERT(index == 0);
                    color.setRgb(red, green, blue);
                    break;
                }
                case 0xE8:
//                    ... 0xFF :
                    { // 0xE8-0xFF:  grayscale from black to white in 24 steps
                    qreal intensity = qreal(index - 0xE8) / (0xFF - 0xE8);
                    color.setRgbF(intensity, intensity, intensity);
                    break;
                }
                }
                textCharFormat.setForeground(color);
                break;
            }
            default : {
                break;
            }
            }
        }
        break;
    }
    case 39 : {
        textCharFormat.setForeground(defaultTextCharFormat.foreground());
        break;
    }
    case 40:
    case 41:
    case 42:
    case 43:
    case 44:
    case 45:
    case 46:
    case 47 : {
        int colorIndex = attribute - 40;
        QColor color;
        switch (colorIndex) {
        case 0 : {
            color = Qt::darkGray;
            break;
        }
        case 1 : {
            color = Qt::red;
            break;
        }
        case 2 : {
            color = Qt::green;
            break;
        }
        case 3 : {
            color = Qt::yellow;
            break;
        }
        case 4 : {
            color = Qt::blue;
            break;
        }
        case 5 : {
            color = Qt::magenta;
            break;
        }
        case 6 : {
            color = Qt::cyan;
            break;
        }
        case 7 : {
            color = Qt::white;
            break;
        }
        default : {
            Q_ASSERT(false);
        }
        }
        textCharFormat.setBackground(color);
        break;
    }
    case 48 : {
        if (i.hasNext()) {
            bool ok = false;
            int selector = i.next().toInt(&ok);
            Q_ASSERT(ok);
            QColor color;
            switch (selector) {
            case 2 : {
                if (!i.hasNext()) {
                    break;
                }
                int red = i.next().toInt(&ok);
                Q_ASSERT(ok);
                if (!i.hasNext()) {
                    break;
                }
                int green = i.next().toInt(&ok);
                Q_ASSERT(ok);
                if (!i.hasNext()) {
                    break;
                }
                int blue = i.next().toInt(&ok);
                Q_ASSERT(ok);
                color.setRgb(red, green, blue);
                break;
            }
            case 5 : {
                if (!i.hasNext()) {
                    break;
                }
                int index = i.next().toInt(&ok);
                Q_ASSERT(ok);
                switch (index) {
                case 0x00:
                case 0x01:
                case 0x02:
                case 0x03:
                case 0x04:
                case 0x05:
                case 0x06:
                case 0x07 : { // 0x00-0x07:  standard colors (as in ESC [ 40..47 m)
                    return parseEscapeSequence(index - 0x00 + 40, i, textCharFormat, defaultTextCharFormat);
                }
                case 0x08:
//                    ... 0x0F :
                    { // 0x08-0x0F:  high intensity colors (as in ESC [ 100..107 m)
                    return parseEscapeSequence(index - 0x08 + 100, i, textCharFormat, defaultTextCharFormat);
                }
                case 0x10:
//                    ... 0xE7 :
                    { // 0x10-0xE7:  6*6*6=216 colors: 16 + 36*r + 6*g + b (0≤r,g,b≤5)
                    index -= 0x10;
                    int red = index % 6;
                    index /= 6;
                    int green = index % 6;
                    index /= 6;
                    int blue = index % 6;
                    index /= 6;
                    Q_ASSERT(index == 0);
                    color.setRgb(red, green, blue);
                    break;
                }
                case 0xE8:
//                    ... 0xFF :
                    { // 0xE8-0xFF:  grayscale from black to white in 24 steps
                    qreal intensity = qreal(index - 0xE8) / (0xFF - 0xE8);
                    color.setRgbF(intensity, intensity, intensity);
                    break;
                }
                }
                textCharFormat.setBackground(color);
                break;
            }
            default : {
                break;
            }
            }
        }
        break;
    }
    case 49 : {
        textCharFormat.setBackground(defaultTextCharFormat.background());
        break;
    }
    case 90:
    case 91:
    case 92:
    case 93:
    case 94:
    case 95:
    case 96:
    case 97 : {
        int colorIndex = attribute - 90;
        QColor color;
        switch (colorIndex) {
        case 0 : {
            color = Qt::darkGray;
            break;
        }
        case 1 : {
            color = Qt::red;
            break;
        }
        case 2 : {
            color = Qt::green;
            break;
        }
        case 3 : {
            color = Qt::yellow;
            break;
        }
        case 4 : {
            color = Qt::blue;
            break;
        }
        case 5 : {
            color = Qt::magenta;
            break;
        }
        case 6 : {
            color = Qt::cyan;
            break;
        }
        case 7 : {
            color = Qt::white;
            break;
        }
        default : {
            Q_ASSERT(false);
        }
        }
        color.setRedF(color.redF() * 0.8);
        color.setGreenF(color.greenF() * 0.8);
        color.setBlueF(color.blueF() * 0.8);
        textCharFormat.setForeground(color);
        break;
    }
    case 100:
    case 101:
    case 102:
    case 103:
    case 104:
    case 105:
    case 106:
    case 107 : {
        int colorIndex = attribute - 100;
        QColor color;
        switch (colorIndex) {
        case 0 : {
            color = Qt::darkGray;
            break;
        }
        case 1 : {
            color = Qt::red;
            break;
        }
        case 2 : {
            color = Qt::green;
            break;
        }
        case 3 : {
            color = Qt::yellow;
            break;
        }
        case 4 : {
            color = Qt::blue;
            break;
        }
        case 5 : {
            color = Qt::magenta;
            break;
        }
        case 6 : {
            color = Qt::cyan;
            break;
        }
        case 7 : {
            color = Qt::white;
            break;
        }
        default : {
            Q_ASSERT(false);
        }
        }
        color.setRedF(color.redF() * 0.8);
        color.setGreenF(color.greenF() * 0.8);
        color.setBlueF(color.blueF() * 0.8);
        textCharFormat.setBackground(color);
        break;
    }
    default : {
        break;
    }
    }
}

void SolverWidget::update_log()
{
    update_log(process->readAll());
}

// code from https://stackoverflow.com/questions/26500429/qtextedit-and-colored-bash-like-output-emulation
void SolverWidget::update_log(const QString text)
{
    // ui->log_text->append(appendText);

    // std::cout << appendText.toStdString() << std::endl;

    QTextDocument * document = ui->log_text->document();
    QRegExp const escapeSequenceExpression(R"(\x1B\[([\d;]+)m)");
    QTextCursor cursor(document);
    cursor.clearSelection();
    cursor.movePosition(QTextCursor::End);
    QTextCharFormat const defaultTextCharFormat = cursor.charFormat();
    cursor.beginEditBlock();
    int offset = escapeSequenceExpression.indexIn(text);
    cursor.insertText(text.mid(0, offset));
    QTextCharFormat textCharFormat = defaultTextCharFormat;
    while (!(offset < 0)) {
        int previousOffset = offset + escapeSequenceExpression.matchedLength();
        QStringList capturedTexts = escapeSequenceExpression.capturedTexts().back().split(';');
        QListIterator< QString > i(capturedTexts);
        while (i.hasNext()) {
            bool ok = false;
            int attribute = i.next().toInt(&ok);
            Q_ASSERT(ok);
            parseEscapeSequence(attribute, i, textCharFormat, defaultTextCharFormat);
        }
        offset = escapeSequenceExpression.indexIn(text, previousOffset);
        if (offset < 0) {
            cursor.insertText(text.mid(previousOffset), textCharFormat);
        } else {
            cursor.insertText(text.mid(previousOffset, offset - previousOffset), textCharFormat);
        }
    }
    cursor.setCharFormat(defaultTextCharFormat);
    cursor.endEditBlock();
    //cursor.movePosition(QTextCursor::Start);
    ui->log_text->setTextCursor(cursor);
}
