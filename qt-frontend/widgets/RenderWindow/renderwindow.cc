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

#include <stdexcept>
#include <QImage>
#include <QThread>
#include <QResizeEvent>
#include <QFont>
#include <QProcess>
#include <QFileDialog>
#include <QUrl>
#include <QDesktopServices>
#include <QMessageBox>

#include "renderwindow.hh"
#include "ui_renderwindow.h"
#include "../../redshift/include/redshift.hh"
#include "../../redshift/include/jobfile.hh"
#include "../../redshift/include/interaction/progressreporter.hh"
#include "../../redshift/include/rendertargets/colorrendertarget.hh"
#include "../../redshift/include/rendertargets/rendertargetlock.hh"
#include "qimagerendertarget.hh"








//=============================================================================
// RenderWindowImpl
//=============================================================================
RenderWindowImpl::RenderWindowImpl (
                redshift::shared_ptr<redshift::scenefile::Scene> scenefile,
                int renderSettings, int camera
) : renderSettings(renderSettings), camera(camera)
  , scenefile(scenefile), error_(false), errorMessage_("") {
}



RenderWindowImpl::~RenderWindowImpl () {
}



void RenderWindowImpl::report (
        redshift::shared_ptr<redshift::RenderTargetLock const> /*rlock*/,
        int completed, int total
) {
        if (++reportW>20) {
                copy(renderBuffer, target);
                emit updateImage(*target, (double)completed / total);
                reportW = 0;
        }
}



void RenderWindowImpl::reportDone () {
        copy(renderBuffer, target);
        emit updateImage(*target, 2.f);
}



void RenderWindowImpl::run() {
        using namespace redshift;
        using namespace redshift::interaction;
        // --

        using namespace redshift;

        try {
                const scenefile::FilmSettings &fs = scenefile->filmSettings();

                const int
                      width = scenefile->renderSettings(renderSettings).width,
                      height = scenefile->renderSettings(renderSettings).height;

                renderBuffer = shared_ptr<ColorRenderTarget>(
                                new ColorRenderTarget (width, height));
                target = shared_ptr<QImageRenderTarget>(new QImageRenderTarget (
                                        width, height,
                                        fs.colorscale, fs.convertToSrgb));

                redshift::shared_ptr<redshift::Scene> scene =
                                sceneDescriptionToScene(*scenefile,
                                                        renderBuffer,
                                                        renderSettings, camera);

                shared_ptr<interaction::ProgressReporter> rep =
                        shared_ptr<redshift::interaction::ProgressReporter>(shared_from_this());
                UserCommandProcessor::Ptr commandProcessor (new PassiveCommandProcessor());

                scene->render(
                     rep, commandProcessor,
                     scenefile->renderSettings(renderSettings).samplesPerPixel,
                     scenefile->renderSettings(renderSettings).min_y,
                     scenefile->renderSettings(renderSettings).max_y,
                     scenefile->renderSettings(renderSettings).userSeed
                );

        } catch (std::exception const &ex) {
                error_ = true;
                errorMessage_ = QString()
                        + "Critical exception occured:\n"
                        + ex.what();
                emit updateImage (QImage(), 2);
        } catch (...) {
                error_ = true;
                errorMessage_ = QString()
                        + "An unknown, critical exception occured";
                emit updateImage (QImage(), 2);
        }
}








//=============================================================================
// RenderWindow
//=============================================================================
// TODO: this function should not be part of render-window, as it is specific to
//       simplex-gui
void RenderWindow::RenderProcess (QString pathToSource,
                                  int renderSettings, int camera,
                                  QWidget* parent
) {
        QFileDialog dialog(parent);
        dialog.setWindowTitle("Set a target filename");
        dialog.setFileMode(QFileDialog::AnyFile);

        QStringList nameFilters;
        nameFilters << "Portable Network Graphics (*.png)"
                    << "Bitmap (*.bmp)"
                    << "JPEG (*.jpg *.jpeg)"
                    << "Portable Pixmap (*.ppm)"
                    << "Tagged Image File Format (*.tif *.tiff)"
                    << "X11 Bitmap (*.xbm)"
                    << "X11 Pixmap (*.xpm)"
                    ;
        dialog.setNameFilters(nameFilters);

        QList<QUrl> urls = dialog.sidebarUrls();
        urls << QUrl::fromLocalFile(QDesktopServices::storageLocation(QDesktopServices::DesktopLocation));
        urls << QUrl::fromLocalFile(QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation));
        urls << QUrl::fromLocalFile(QDesktopServices::storageLocation(QDesktopServices::HomeLocation));
        dialog.setSidebarUrls(urls);

        again:

        if (dialog.exec()) {
                QString pathToTarget = dialog.selectedFiles()[0];

                // Check extension.
                if (pathToTarget.endsWith(".bmp", Qt::CaseInsensitive)) {}
                else if (pathToTarget.endsWith(".jpg", Qt::CaseInsensitive)) {}
                else if (pathToTarget.endsWith(".jpeg", Qt::CaseInsensitive)) {}
                else if (pathToTarget.endsWith(".png", Qt::CaseInsensitive)) {}
                else if (pathToTarget.endsWith(".ppm", Qt::CaseInsensitive)) {}
                else if (pathToTarget.endsWith(".tif", Qt::CaseInsensitive)) {}
                else if (pathToTarget.endsWith(".tiff", Qt::CaseInsensitive)) {}
                else if (pathToTarget.endsWith(".xbm", Qt::CaseInsensitive)) {}
                else if (pathToTarget.endsWith(".xpm", Qt::CaseInsensitive)) {}
                else if (pathToTarget.lastIndexOf('.')>=0){
                        QMessageBox::information(parent,
                           "Unsupported filename extension",
                           "Please choose a filename with one of the following extensions:\n"
                           " * .bmp\n"
                           " * .jpg / .jpeg\n"
                           " * .png\n"
                           " * .ppm\n"
                           " * .tif / .tiff\n"
                           " * .xbm\n"
                           " * .xpm\n"
                        );
                        goto again;
                } else {
                        // No filename given, so try to guess from file-dialog.
                        if (dialog.selectedNameFilter().contains(".bmp"))
                                pathToTarget += ".bmp";
                        else if (dialog.selectedNameFilter().contains(".jpg"))
                                pathToTarget += ".jpg";
                        else if (dialog.selectedNameFilter().contains(".png"))
                                pathToTarget += ".png";
                        else if (dialog.selectedNameFilter().contains(".ppm"))
                                pathToTarget += ".ppm";
                        else if (dialog.selectedNameFilter().contains(".tif"))
                                pathToTarget += ".tif";
                        else if (dialog.selectedNameFilter().contains(".xbm"))
                                pathToTarget += ".xbm";
                        else if (dialog.selectedNameFilter().contains(".xpm"))
                                pathToTarget += ".xpm";
                }

                // Check if overwrites.
                if (QFile::exists(pathToTarget) &&
                    QMessageBox::question(parent, "Overwrite file?",
                                          QString()+
                                          "Do you really want to overwrite the file "
                                          + "\"" + pathToTarget + "\"?",
                                          QMessageBox::Yes | QMessageBox::No,
                                          QMessageBox::No
                                          ) == QMessageBox::No
                ) {
                        goto again;
                }

        /*QString name = QFileDialog::getSaveFileName(this, "Select a file to save to");
        if (name != "") {*/

                // Check if we can write.
                QFile file (pathToTarget);
                if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                        QMessageBox::warning(parent, "Failed to save",
                             "The file \"" + file.fileName() + "\" could not be"
                             +" opened for writing. Please select another file "
                             +" or one that does not exist yet.");
                        goto again;
                }


                QStringList args;
                args << "picogen-production-render";
                args << pathToSource;
                args << pathToTarget;
                QProcess::startDetached(
                        "/home/smach/Projects/picogen/git/qt-frontend/SimplexGui/simplex-gui",
                        args);
        }
}



RenderWindow::RenderWindow(
        redshift::shared_ptr<redshift::scenefile::Scene> scenefile,
        int renderSettings, int camera,
        QString pathToTarget,
        QWidget *parent
) :
    QDialog(parent),
    ui(new Ui::RenderWindow),
    scenefile(scenefile),
    pathToTarget(pathToTarget)
{
        ui->setupUi(this);

        // The following 3 lines are to ensure that we are properly sized.
        // It seems impossible to do proper resizing within updateImage
        // (I've tried to setMinimumSize() on pixmap, layout, myself, and also
        //  the others like repaint(), update(), and all permutations)
        QPixmap map(scenefile->renderSettings(renderSettings).width,
                    scenefile->renderSettings(renderSettings).height);
        map.fill(QColor(0,0,0));
        ui->pix->setPixmap(map);

        impl = redshift::shared_ptr<RenderWindowImpl>(
                  new RenderWindowImpl(scenefile, renderSettings, camera));

        connect(
                impl.get(), SIGNAL(updateImage (QImage, double)),
                this, SLOT(updateImage (QImage, double))
        );

        impl->start();
}



RenderWindow::~RenderWindow() {
        delete ui;
}



void RenderWindow::updateImage (QImage image, double percentage) {
        if (impl->error()) {
                setWindowTitle ("Error");

                QFont font;
                font.setStyleHint (QFont::TypeWriter, QFont::PreferAntialias);
                font.setFamily("Monospace");
                font.setFixedPitch(true);
                ui->pix->setFont(font);

                ui->pix->setText(impl->errorMessage());
        } else {
                if (percentage>=1) {
                        if ("" != pathToTarget) {
                                setWindowTitle("saving " + pathToTarget);
                                image.save(pathToTarget, 0, 100);
                        }

                        setWindowTitle("Done.");
                } else {
                        setWindowTitle(QString::number(percentage*100, 'f', 3) + "%");
                }

                ui->pix->setPixmap(QPixmap::fromImage(image));
        }
}



void RenderWindow::changeEvent(QEvent *e) {
        QDialog::changeEvent(e);
        switch (e->type()) {
        case QEvent::LanguageChange:
                ui->retranslateUi(this);
                break;
        default:
                break;
        }
}
