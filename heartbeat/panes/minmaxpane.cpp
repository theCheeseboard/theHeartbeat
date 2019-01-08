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
#include "minmaxpane.h"
#include "ui_minmaxpane.h"

#include <QPainter>

MinMaxPane::MinMaxPane(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MinMaxPane)
{
    ui->setupUi(this);
}

MinMaxPane::~MinMaxPane()
{
    delete ui;
}

void MinMaxPane::setTitle(QString title) {
    ui->titleLabel->setText(title.toUpper());
}

void MinMaxPane::setMax(qulonglong max) {
    this->max = max;

    ui->maxLabel->setText("/ " + calculateText(max));
}

void MinMaxPane::setValue(qulonglong val) {
    this->val = val;

    percentageHistory.prepend((double) val / (double) max);
    if (percentageHistory.count() > 300) {
        percentageHistory.removeLast();
    }

    ui->valueLabel->setText(calculateText(val));
    this->update();
}

QString MinMaxPane::calculateText(qulonglong val) {
    switch (u) {
        case Kilobyte:
            if (val < 1024) {
                return tr("%1 KB").arg(QString::number((double) val, 'f', 1));
            } else if (val < 1048576) {
                return tr("%1 MB").arg(QString::number((double) val / 1024, 'f', 1));
            } else if (val < 1073741824) {
                return tr("%1 GB").arg(QString::number((double) val / 1048576, 'f', 1));
            }
    }
    return "";
}

void MinMaxPane::setUnit(Unit u) {
    this->u = u;
}

void MinMaxPane::paintEvent(QPaintEvent *event) {
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
