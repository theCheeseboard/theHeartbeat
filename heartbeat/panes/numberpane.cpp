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
#include "numberpane.h"
#include "ui_numberpane.h"

NumberPane::NumberPane(QWidget *parent) :
    SidePane(parent),
    ui(new Ui::NumberPane)
{
    ui->setupUi(this);
}

NumberPane::~NumberPane()
{
    delete ui;
}

void NumberPane::setTitle(QString title) {
    ui->titleLabel->setText(title.toUpper());
}

void NumberPane::setValue(qulonglong value) {
    ui->valueLabel->setText(calculateText(value));

    if (value > max && max != 0) {
        //Calculate how much we need to change each value by
        double factor = (double) value / (double) max;
        for (int i = 0; i < percentageHistory.count(); i++) {
            percentageHistory.replace(i, percentageHistory.at(i) * factor);
        }
    }

    if (value > max) max = value;

    if (max != 0) {
        percentageHistory.prepend((double) value / (double) max);
    }

    if (percentageHistory.count() > 300) {
        percentageHistory.removeLast();
    }
    this->update();
}

void NumberPane::on_expandButton_clicked()
{
    emit toggleExpand();
}

void NumberPane::setExpanded(bool expanded) {
    if (expanded) {
        ui->expandButton->setArrowType(Qt::UpArrow);
    } else {
        ui->expandButton->setArrowType(Qt::DownArrow);
    }
}

void NumberPane::setExpandable(bool expandable) {
    ui->expandButton->setVisible(expandable);
}

void NumberPane::setUnit(Unit u) {
    this->u = u;
}
