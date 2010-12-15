//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Copyright (C) 2010  Sebastian Mach (*1983)
// * mail: phresnel/at/gmail/dot/com
// * http://phresnel.org
// * http://picogen.org
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


#include <QtGui/QApplication>
#include <QCleanlooksStyle>
#include <QMessageBox>
#include <QFile>

#include "mainwindow.hh"

#include <fstream>

#include "cosyscene/scene.hh"
#include "cosyscene/terrain.hh"
#include "cosyscene/save_load.hh"

#include "redshift/include/static_init.hh"
#include "redshift/include/redshift_file/scene.hh"
#include "renderwindow.hh"

#undef main


namespace redshift_file {
void save_scene (const redshift_file::Scene &scene_, std::ostream &fs_);
void save_scene (const redshift_file::Scene &scene, std::string const &name);
void load_scene (redshift_file::Scene &scene, std::istream &fs);
void load_scene (Scene &scene, std::string const &name);
}





int cosy_main (int argc, char *argv[]) {
        redshift::static_init();

        QCleanlooksStyle *style = new QCleanlooksStyle ();
        QApplication::setStyle(style);

        QApplication a(argc, argv);
        MainWindow w;
        w.show();
        return a.exec();
}

int production_render_main (int argc, char *argv[]) {
        QCleanlooksStyle *style = new QCleanlooksStyle ();
        QApplication::setStyle(style);

        redshift::static_init();
        QApplication a(argc, argv);

        const QString pathToSource =
                argc>2
                ? QString(argv[2])
                : "";
        if ("" == pathToSource) {
                QMessageBox::critical(0, "Missing argument",
                        "No source-file has been specified.");
                return -1;
        } else if (!QFile::exists(pathToSource)) {
                QMessageBox::critical(0, "File not found",
                        "The source-file \"" + pathToSource + "\" has not been found.");
                return -1;
        }

        const QString renderSettingsS =
                argc>3
                ? QString(argv[3])
                        : "";

        const QString cameraSettingsS =
                argc>4
                ? QString(argv[4])
                        : "";

        bool okayC, okayR;
        const int renderSetting = renderSettingsS.toInt(&okayC);
        const int cameraSetting = cameraSettingsS.toInt(&okayR);

        if ("" == renderSettingsS || "" == cameraSettingsS
            || cameraSetting<0 || renderSetting<0
            || !okayC || !okayR
        ) {
                QMessageBox::critical(0, "Missing argument(s)",
                  "Need an index [0..n] for the render setting and camera setting "
                  "to be used."
                );
                return -1;
        }

        try {
                redshift_file::Scene scene;
                redshift_file::load_scene(scene, pathToSource.toStdString());

                while (!QFile::remove(pathToSource)) {
                }

                redshift::shared_ptr<redshift_file::Scene> pscene =
                                redshift::shared_ptr<redshift_file::Scene>(
                                                new redshift_file::Scene(scene)
                                );
                RenderWindow w (pscene, renderSetting, cameraSetting, 0, 15.);
                w.show();
                const int ret = a.exec();
                return ret;
        } catch (std::exception const &e){
                QMessageBox::critical(0,
                      "Error upon loading",
                      "The selected file \"" + pathToSource + "\" could not be loaded, "
                      "are you sure this is a valid picogen file?\n\n"
                      + QString("(exception: ")+e.what()+")"
                      );
                return -1;
        }
}

int main(int argc, char *argv[]) {
        if (argc>1 && !strcmp ("--cosygui-production-render", argv[1])) {
                return production_render_main(argc, argv);
        }
        return cosy_main(argc, argv);
}
