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
#include "percentagepane.h"
#include "ui_percentagepane.h"

PercentagePane::PercentagePane(QWidget *parent) :
    SidePane(parent),
    ui(new Ui::PercentagePane)
{
    ui->setupUi(this);

    ui->expandButton->setVisible(false);
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

void PercentagePane::on_expandButton_clicked()
{
    emit toggleExpand();
}

void PercentagePane::setExpanded(bool expanded) {
    if (expanded) {
        ui->expandButton->setArrowType(Qt::UpArrow);
    } else {
        ui->expandButton->setArrowType(Qt::DownArrow);
    }
}

void PercentagePane::setExpandable(bool expandable) {
    ui->expandButton->setVisible(expandable);
}
