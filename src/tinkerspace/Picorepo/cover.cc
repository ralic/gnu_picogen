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

#include "cover.hh"

#include <QPixmap>
#include <QGraphicsPixmapItem>
#include <QPainter>


Cover::Cover (QString const &filename, QGraphicsItem *parent)
: QGraphicsItem(parent)
, pixmap(new QPixmap(filename))
{
        QTransform adjustToImageCenter, adjustSizeTo01;
        adjustToImageCenter.translate(-pixmap->width()*.5, -pixmap->height()*.5);
        adjustSizeTo01.scale(1.f/pixmap->width(),1.f/pixmap->width());
        setTransform(adjustToImageCenter * adjustSizeTo01);
}



Cover::~Cover() {
        delete pixmap;
}



QRectF Cover::boundingRect () const {
        return QRectF(
                0, 0,
                pixmap->width(), pixmap->height()
        );
}



void Cover::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                  QWidget *widget
) {
        painter->drawPixmap(0, 0, *pixmap);
}

