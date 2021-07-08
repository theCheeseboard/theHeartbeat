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
#include <QLocale>
#include <QPainterPath>

SidePane::SidePane(QWidget* parent) : QWidget(parent) {

}

void SidePane::paintEvent(QPaintEvent* event) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    QPainterPath path;
    if (this->layoutDirection() == Qt::LeftToRight) {
        path.moveTo(this->width() + 1, 0);

        for (int i = 0; i < percentageHistory.count(); i++) {
            path.lineTo(this->width() - i * 4, this->height() - ((double) this->height() * percentageHistory.at(i)));
        }
        path.lineTo(this->width() - percentageHistory.count() * 4, this->height());
        path.lineTo(this->width() + 1, this->height());
    } else {
        path.moveTo(-1, 0);

        for (int i = 0; i < percentageHistory.count(); i++) {
            path.lineTo(i * 4, this->height() - ((double) this->height() * percentageHistory.at(i)));
        }
        path.lineTo(percentageHistory.count() * 4, this->height());
        path.lineTo(-1, this->height());
    }

    QColor col = this->palette().color(QPalette::Highlight);
    col.setAlpha(100);
    p.fillPath(path, col);

    QLinearGradient g;
    g.setStart(100, 0);
    g.setFinalStop(140, 0);
    g.setColorAt(this->layoutDirection() == Qt::RightToLeft ? 1 : 0, this->palette().color(QPalette::Window));

    QColor c = this->palette().color(QPalette::Window);
    c.setAlpha(0);
    g.setColorAt(this->layoutDirection() == Qt::RightToLeft ? 0 : 1, c);
    p.fillRect(100, 0, 40, this->height(), g);

    if (this->layoutDirection() == Qt::RightToLeft) {
        p.fillRect(140, 0, 100, this->height(), this->palette().color(QPalette::Window));
    } else {
        p.fillRect(0, 0, 100, this->height(), this->palette().color(QPalette::Window));
    }
}

QString SidePane::calculateText(qulonglong val) {
    QLocale locale;
    switch (u) {
        case Kilobyte:
            if (val < 1024) {
                return tr("%1 KiB").arg(locale.toString((double) val, 'f', 1));
            } else if (val < 1048576) {
                return tr("%1 MiB").arg(locale.toString((double) val / 1024, 'f', 1));
            } else { /* (val < 1073741824) */
                return tr("%1 GiB").arg(locale.toString((double) val / 1048576, 'f', 1));
            }
        case KilobytePerSecond:
            if (val < 1024) {
                return tr("%1 KiB/s").arg(locale.toString((double) val, 'f', 1));
            } else if (val < 1048576) {
                return tr("%1 MiB/s").arg(locale.toString((double) val / 1024, 'f', 1));
            } else { /* (val < 1073741824) */
                return tr("%1 GiB/s").arg(locale.toString((double) val / 1048576, 'f', 1));
            }
        case MillidegreeCelsius:
            return tr("%1 °C").arg(locale.toString((double) val / 1000, 'f', 1));
    }
    return "";
}
