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

#include "stashview.h"
#include "terrainformation.h"
#include "ui_terrainformation.h"

#include "cosyscene/terrain.hh"

#include "selectpresetdialog.h"

namespace picogen { namespace qt4_gui {

TerrainFormation::TerrainFormation(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TerrainFormation),
    formation_(new cosyscene::TerrainFormation())
{
        ui->setupUi(this);
        ui->importCLB->setVisible(false);
}



TerrainFormation::~TerrainFormation() {
    delete ui;
}



void TerrainFormation::setFormation (
        redshift::shared_ptr<cosyscene::TerrainFormation> t,
        bool blockSignals
) {

        const bool prevBlocked = this->blockSignals(blockSignals);
        formation_ = t;
        updateViews();
        this->blockSignals(prevBlocked);
}



void TerrainFormation::setFormationByValue(cosyscene::TerrainFormation const &t,
                                           bool blockSignals
) {
        const bool prevBlocked = this->blockSignals(blockSignals);
        *formation_ = t;
        updateViews();
        this->blockSignals(prevBlocked);
}



void TerrainFormation::showQuatschEditor() {
        ui->stackedWidget->setCurrentWidget(ui->quatschCodeEditorPage);
}



void TerrainFormation::showQuatschPresetEditor() {
        ui->stackedWidget->setCurrentWidget(ui->quatschPresetEditorPage);
}



void TerrainFormation::showTerrainKindSelection() {
        ui->stackedWidget->setCurrentWidget(ui->selectKind);
}



void TerrainFormation::on_quatschCodeEditorCLB_clicked() {
        formation_->toQuatschSource(cosyscene::QuatschSource(
                "/* Press F1 for help :) */\n"
                "/* ... or move the caret over some entity and try out\n"
                "   the contextual help via F1 */\n"
                "(* 100 ([LibnoiseRidgedMulti \n"
                "         frequency{0.001}\n"
                "         octave-count{16}\n"
                "        ] x y)\n"
        ));
        updateViews();
        emit formationChanged();
}



void TerrainFormation::on_parametricPresetCLB_clicked() {
        picogen::shared_ptr<repository::Database> db
                        (new repository::Database());
        db->establishStandardConnection();

        SelectPresetDialog p;
        p.setDatabase(db);
        p.exec();
        if (p.lastSelected()) {
                formation_->toQuatschPreset(cosyscene::QuatschPreset(
                                p.lastSelected()->preset().toStdString()));
        }


        /*formation_->toQuatschPreset(cosyscene::QuatschPreset(
        "(($Width :real = {[1..99999]} default=1000))\n"
        "(($Depth :real = {[1..99999]} default=1000))\n"
        "(($Height:real = {[1..99999]} default=1000))\n"
        "(($Repeat:real = {[0.001..99999]} default=1))\n"
        "(($Filename:filename))\n"
        "(($Filter:enumeration={nearest, bilinear, cubic} default=2))\n"
        "(($RepeatMode:enumeration={zero,wrap,clamp,mirror} default=1))\n"
        "(($PixelToHeightMode:enumeration={average,luminance} default=0))\n"
        "\n"
        "([Heightmap filter{(($Filter))}\n"
        "            width {(($Width))}\n"
        "            depth {(($Depth))}\n"
        "            height{(($Height))}\n"
        "            filename{(($Filename))}\n"
        "            wrap  {(($RepeatMode))}\n"
        "            pixel-to-height{(($PixelToHeightMode))}\n"
        " ] (* (($Repeat)) x)  (* (($Repeat)) y))"
        ));*/
        updateViews();
        emit formationChanged();
}

void TerrainFormation::on_heightmapCLB_clicked() {
        formation_->toQuatschPreset(cosyscene::QuatschPreset(
        "(($Width :real = {[1..99999]} default=1000))\n"
        "(($Depth :real = {[1..99999]} default=1000))\n"
        "(($Height:real = {[1..99999]} default=1000))\n"
        "(($Repeat:real = {[0.001..99999]} default=1))\n"
        "(($Filename:filename))\n"
        "(($Filter:enumeration={nearest, bilinear, cubic} default=2))\n"
        "(($RepeatMode:enumeration={zero,wrap,clamp,mirror} default=1))\n"
        "(($PixelToHeightMode:enumeration={average,luminance} default=0))\n"
        "\n"
        "([Heightmap filter{(($Filter))}\n"
        "            width {(($Width))}\n"
        "            depth {(($Depth))}\n"
        "            height{(($Height))}\n"
        "            filename{(($Filename))}\n"
        "            wrap  {(($RepeatMode))}\n"
        "            pixel-to-height{(($PixelToHeightMode))}\n"
        " ] (* (($Repeat)) x)  (* (($Repeat)) y))"
        ));
        updateViews();
        emit formationChanged();
}

void TerrainFormation::on_quatschPresetEditor_formationChanged() {
        formation_->toQuatschPreset(ui->quatschPresetEditor->toCosy());
        emit formationChanged();
}



void TerrainFormation::on_quatschCodeEditor_codeChanged() {
        formation_->toQuatschSource(cosyscene::QuatschSource(
                ui->quatschCodeEditor->code().toStdString()
        ));
        emit formationChanged();
}



void TerrainFormation::updateViews() {
        switch (formation_->kind()) {
        case cosyscene::TerrainFormation::QuatschSource: {
                const bool blocked = ui->quatschCodeEditor->blockSignals(true);
                ui->quatschCodeEditor->setCode(
                                QString::fromStdString(
                                formation_->quatschSource().code()));
                showQuatschEditor();
                ui->quatschCodeEditor->blockSignals(blocked);
                break;
        }
        case cosyscene::TerrainFormation::QuatschPreset: {
                const bool blocked = ui->quatschPresetEditor->blockSignals(true);
                cosyscene::QuatschPreset qp = formation_->quatschPreset();
                ui->quatschPresetEditor->fromCosy(qp);
                showQuatschPresetEditor();
                ui->quatschPresetEditor->blockSignals(blocked);
                break;
        }
        case cosyscene::TerrainFormation::None:
                showTerrainKindSelection();
                break;
        }
}

void TerrainFormation::on_stashButton_clicked() {
        if (StashView::StashDialog (this, formation_)) {
                emit formationChanged();
        }
}
void TerrainFormation::on_stashRestoreButton_clicked() {
        if (StashView::RestoreDialog (this, formation_)) {
                setFormationByValue (*formation_, true);
                emit formationChanged();
        }
}
void TerrainFormation::on_stashResetButton_clicked() {
        if (StashView::ResetDialog(this, formation_)) {
                setFormationByValue(*formation_, true);
                emit formationChanged();
        }
}

} }

#include "terrainformation.moc"
