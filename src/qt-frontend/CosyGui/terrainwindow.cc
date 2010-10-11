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


#include "terrainwindow.hh"
#include "ui_terrainwindow.h"

#include "cosyscene/scene.hh"
#include "cosyscene/terrain.hh"

#include "stashview.hh"

#include <QMessageBox>

TerrainWindow::TerrainWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TerrainWindow),
    terrain(new cosyscene::Terrain)
{
        ui->setupUi(this);
}



TerrainWindow::~TerrainWindow() {
        delete ui;
}



void TerrainWindow::setTerrain (redshift::shared_ptr<cosyscene::Terrain> t,
                                bool blockSignals
) {
        bool prevBlocked;
        if (blockSignals)
                prevBlocked = this->blockSignals(true);
        terrain = t;
        updateViews();
        if (blockSignals)
                this->blockSignals(prevBlocked);
}



void TerrainWindow::setTerrainByValue (cosyscene::Terrain const &t,
                                       bool blockSignals
) {
        bool prevBlocked;
        if (blockSignals)
                prevBlocked = this->blockSignals(true);
        *terrain = t;
        ui->formationStackedWidget->setFormation(t.formation(), blockSignals);
        updateViews();
        if (blockSignals)
                this->blockSignals(prevBlocked);

}



void TerrainWindow::updateViews () {
        const bool wasBlocked = blockSignals(true);
        blockSignals(wasBlocked);
}



void TerrainWindow::sceneInvalidated(
        redshift::shared_ptr<cosyscene::Scene> scene
) {
        setTerrain (scene->terrain());
}



void TerrainWindow::on_stashButton_clicked() {
        terrain->stash();
}



void TerrainWindow::on_stashRestoreButton_clicked() {
        StashView *sw = new StashView (this);
        sw->addItems(terrain->getStash());
        if (QDialog::Accepted == sw->exec()) {
                redshift::shared_ptr<cosyscene::Terrain> newTerrain (
                  new cosyscene::Terrain(sw->selectedData<cosyscene::Terrain>())
                );
                newTerrain->setStash(sw->itemsToStash<cosyscene::Terrain>());
                setTerrainByValue(*newTerrain, true);
        }
}



void TerrainWindow::on_stashResetButton_clicked() {
        if (!terrain->getStash().contains_data(*terrain)) {
                switch (confirmReset (this)) {
                case ConfirmReset_Abort: return;
                case ConfirmReset_StashBeforeReset: terrain->stash(); break;
                case ConfirmReset_Reset: break;
                }
        }
        cosyscene::Terrain t;
        t.setStash(terrain->getStash());
        setTerrainByValue(t, true);
}

void TerrainWindow::on_showFormationTab_clicked() {
        ui->showFormationTab->setChecked(true);
        ui->showTexturingTab->setChecked(false);
        ui->showWorldFittingTab->setChecked(false);
        ui->terrainTaskStackedWidget->setCurrentIndex(0);
}

void TerrainWindow::on_showTexturingTab_clicked() {
        ui->showFormationTab->setChecked(false);
        ui->showTexturingTab->setChecked(true);
        ui->showWorldFittingTab->setChecked(false);
        ui->terrainTaskStackedWidget->setCurrentIndex(1);
}

void TerrainWindow::on_showWorldFittingTab_clicked() {
        ui->showFormationTab->setChecked(false);
        ui->showTexturingTab->setChecked(false);
        ui->showWorldFittingTab->setChecked(true);
        ui->terrainTaskStackedWidget->setCurrentIndex(2);
}
