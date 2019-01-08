/****************************************
 *
 *   INSERT-PROJECT-NAME-HERE - INSERT-GENERIC-NAME-HERE
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
#include "percentagepane.h"
#include "ui_percentagepane.h"

#include <QPaintEvent>
#include <QPainter>

PercentagePane::PercentagePane(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PercentagePane)
{
    ui->setupUi(this);
}

PercentagePane::~PercentagePane()
{
    delete ui;
}

void PercentagePane::setTitle(QString title) {
    ui->titleLabel->setText(title.toUpper());
}

void PercentagePane::setPercentage(double percentage) {
    ui->valueLabel->setText(QString::number(percentage * 100, 'f', 1) + "%");
    percentageHistory.prepend(percentage);
    if (percentageHistory.count() > 300) {
        percentageHistory.removeLast();
    }
    this->update();
}

void PercentagePane::paintEvent(QPaintEvent *event) {
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
