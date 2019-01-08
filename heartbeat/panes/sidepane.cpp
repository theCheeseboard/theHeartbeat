/****************************************
 *
 *   theHeartbeat - System Monitor
 *   Copyright (C) 2019 Victor Tran
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * *************************************/
#include "sidepane.h"

#include <QPaintEvent>
#include <QPainter>

SidePane::SidePane(QWidget *parent) : QWidget(parent)
{

}

void SidePane::paintEvent(QPaintEvent *event) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    QPainterPath path;
    path.moveTo(this->width() + 1, 0);

    for (int i = 0; i < percentageHistory.count(); i++) {
        path.lineTo(this->width() - i * 4, this->height() - ((double) this->height() * percentageHistory.at(i)));
    }
    path.lineTo(this->width() - percentageHistory.count() * 4, this->height());
    path.lineTo(this->width() + 1, this->height());

    QColor col = this->palette().color(QPalette::Highlight);
    col.setAlpha(100);
    p.fillPath(path, col);

    QLinearGradient g;
    g.setStart(100, 0);
    g.setFinalStop(140, 0);
    g.setColorAt(0, this->palette().color(QPalette::Window));

    QColor c = this->palette().color(QPalette::Window);
    c.setAlpha(0);
    g.setColorAt(1, c);
    p.fillRect(100, 0, 140, this->height(), g);
    p.fillRect(0, 0, 100, this->height(), this->palette().color(QPalette::Window));
}
