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

#include <QStringList>
#include <QMessageBox>

#include <QtTreePropertyBrowser>
#include <QtProperty>
#include <QtGroupPropertyManager>
#include <QtVariantPropertyManager>
#include <QtEnumPropertyManager>

#include "redshift/include/jobfile.hh"

#include "propertybrowser-helpers.hh"

#include "coloreditmanager.h"

#include "rendersettingspropertybrowser.hh"

RenderSettingsPropertyBrowser::RenderSettingsPropertyBrowser(
        QWidget *ownerWidget_,
        QtTreePropertyBrowser *root,
        QtGroupPropertyManager *groupManager,
        QtVariantPropertyManager *variantManager,
        QtVariantPropertyManager *codeEditManager,
        ColorEditManager *colorEditManager
)
: QObject(ownerWidget_)
, ownerWidget(ownerWidget_)
, root(root)
, groupManager(groupManager)
, variantManager(variantManager)
, codeEditManager(codeEditManager)
, renderSettingsProperty(0)
, colorEditManager(colorEditManager)
{
        initializeScene();
}



void RenderSettingsPropertyBrowser::initializeScene() {
        QtProperty *topItem = groupManager->addProperty("render-settings");
        renderSettingsProperty = topItem;
        ui->settings->addProperty(topItem);

        ui->settings->setBackgroundColor(
                     ui->settings->topLevelItem(renderSettingsProperty),
                     QColor(130,90,90));
}



QList<QtProperty*> RenderSettingsPropertyBrowser::subProperties() {
        return renderSettingsProperty->subProperties();
}



void RenderSettingsPropertyBrowser::addRenderSettings (
        redshift::scenefile::RenderSettings const &rs
) {

        QtProperty *topItem = groupManager->addProperty(QString::fromStdString(rs.title));
        renderSettingsProperty->addSubProperty(topItem);

        QtProperty *title = rsTitleManager->addProperty("title");
        rsTitleManager->setRegExp(title, QRegExp("([a-z0-9]|-|_)+", Qt::CaseInsensitive, QRegExp::RegExp));
        rsTitleManager->setValue(title, QString::fromStdString(rs.title));
        topItem->addSubProperty(title);


        QtVariantProperty *it = variantManager->addProperty(QVariant::Int, "width");
        it->setAttribute(QLatin1String("minimum"), 1);
        it->setAttribute(QLatin1String("maximum"), 0xFFFFFF);
        it->setAttribute(QLatin1String("singleStep"), 1);
        it->setValue(rs.width);
        topItem->addSubProperty(it);

        it = variantManager->addProperty(QVariant::Int, "height");
        it->setAttribute(QLatin1String("minimum"), 1);
        it->setAttribute(QLatin1String("maximum"), 0xFFFFFF);
        it->setAttribute(QLatin1String("singleStep"), 1);
        it->setValue(rs.height);
        topItem->addSubProperty(it);

        it = variantManager->addProperty(QVariant::Int, "samples-per-pixel");
        it->setAttribute(QLatin1String("minimum"), 1);
        it->setAttribute(QLatin1String("maximum"), 0xFFFFFF);
        it->setAttribute(QLatin1String("singleStep"), 1);
        it->setValue(rs.samplesPerPixel);
        topItem->addSubProperty(it);

        it = variantManager->addProperty(QVariant::Int, "min-y");
        it->setAttribute(QLatin1String("minimum"), 0);
        it->setAttribute(QLatin1String("singleStep"), 1);
        it->setValue(rs.min_y);
        topItem->addSubProperty(it);

        it = variantManager->addProperty(QVariant::Int, "max-y");
        it->setAttribute(QLatin1String("minimum"), 0);
        it->setAttribute(QLatin1String("singleStep"), 1);
        it->setValue(rs.max_y);
        topItem->addSubProperty(it);

        it = variantManager->addProperty(QVariant::Int, "seed");
        it->setAttribute(QLatin1String("minimum"), 0);
        it->setAttribute(QLatin1String("singleStep"), 1);
        it->setValue(rs.userSeed);
        it->setToolTip("Use this to configure another random seed for rendering.");
        topItem->addSubProperty(it);


        {
                QtProperty *surfaceIntegrator = groupManager
                                                ->addProperty("surface-integrator");
                topItem->addSubProperty(surfaceIntegrator);

                QtProperty *integratorType = surfaceIntegratorTypeEnumManager->addProperty("type");
                QStringList enumNames;
                enumNames << "none" << "whitted" << "whitted_ambient" << "path";
                surfaceIntegratorTypeEnumManager->setEnumNames(integratorType, enumNames);

                surfaceIntegrator->addSubProperty(integratorType);


                switch (rs.surfaceIntegrator.type) {
                case redshift::scenefile::SurfaceIntegrator::none:
                        surfaceIntegratorTypeEnumManager->setValue(integratorType, 0);
                        surfaceIntegratorTypeEnumManager_valueChanged(integratorType, 0);
                        break;
                case redshift::scenefile::SurfaceIntegrator::whitted:
                        surfaceIntegratorTypeEnumManager->setValue(integratorType, 1);
                        surfaceIntegratorTypeEnumManager_valueChanged(integratorType, 1);
                        break;
                case redshift::scenefile::SurfaceIntegrator::whitted_ambient:
                        surfaceIntegratorTypeEnumManager->setValue(integratorType, 2);
                        surfaceIntegratorTypeEnumManager_valueChanged(integratorType, 2);
                        break;
                case redshift::scenefile::SurfaceIntegrator::path:
                        surfaceIntegratorTypeEnumManager->setValue(integratorType, 3);
                        surfaceIntegratorTypeEnumManager_valueChanged(integratorType, 3);
                        break;
                default:
                        QMessageBox::warning(this,
                          "Unsupported Surface Integrator",
                          "It has been tried to load a surface integrator "
                          "\""
                          + QString::fromStdString(redshift::scenefile::SurfaceIntegrator::Typenames[rs.surfaceIntegrator.type])
                          + "\", but this type is currently not supported here."
                        );
                        surfaceIntegratorTypeEnumManager->setValue(integratorType, 0);
                        surfaceIntegratorTypeEnumManager_valueChanged(integratorType, 0);
                }
        }
        {
                QtProperty *volumeIntegrator = groupManager->addProperty("volume-integrator");
                topItem->addSubProperty(volumeIntegrator);

                QtProperty *integratorType = enumManager->addProperty("type");
                QStringList enumNames;
                enumNames << "none" << "emission" << "single";
                enumManager->setEnumNames(integratorType, enumNames);
                volumeIntegrator->addSubProperty(integratorType);
                it = variantManager->addProperty(QVariant::Double, "step-size");
                it->setAttribute(QLatin1String("minimum"), 1.);
                //it->setAttribute(QLatin1String("maximum"), 32768.);
                it->setAttribute(QLatin1String("singleStep"), 1.);
                it->setAttribute(QLatin1String("decimals"), 1);
                it->setValue(rs.volumeIntegrator.stepSize);
                volumeIntegrator->addSubProperty(it);

                it = variantManager->addProperty(QVariant::Double, "cutoff-distance");
                it->setAttribute(QLatin1String("minimum"), 1.);
                //it->setAttribute(QLatin1String("maximum"), 32768.);
                it->setAttribute(QLatin1String("singleStep"), 1.);
                it->setAttribute(QLatin1String("decimals"), 1);
                it->setValue(rs.volumeIntegrator.cutoffDistance);
                volumeIntegrator->addSubProperty(it);


                switch (rs.volumeIntegrator.type) {
                case redshift::scenefile::VolumeIntegrator::none:
                        enumManager->setValue(integratorType, 0);
                        break;
                case redshift::scenefile::VolumeIntegrator::emission:
                        enumManager->setValue(integratorType, 1);
                        break;
                case redshift::scenefile::VolumeIntegrator::single:
                        enumManager->setValue(integratorType, 2);
                        break;
                default:
                        QMessageBox::warning(this,
                          "Unsupported Volume Integrator",
                          "It has been tried to load a volume integrator "
                          "\""
                          + QString::fromStdString(redshift::scenefile::VolumeIntegrator::Typenames[rs.volumeIntegrator.type])
                          + "\", but this type is currently not supported here."
                        );
                        enumManager->setValue(integratorType, 0);
                }
        }
        collapse (ui->settings, topItem);
        resyncRenderSettingConfig();

        emit updateUi();
}



RenderSettingsPropertyBrowser::remove(QtProperty *property) {
        renderSettingsProperty->removeSubProperty(property);
        emit sceneChanged();
}