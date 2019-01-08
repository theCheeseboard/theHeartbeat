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
#include "minipercentagepane.h"
#include "ui_minipercentagepane.h"

MiniPercentagePane::MiniPercentagePane(QWidget *parent) :
    SidePane(parent),
    ui(new Ui::MiniPercentagePane)
{
    ui->setupUi(this);
}

MiniPercentagePane::~MiniPercentagePane()
{
    delete ui;
}

void MiniPercentagePane::setTitle(QString title) {
    ui->titleLabel->setText(title.toUpper());
}

void MiniPercentagePane::setPercentage(double percentage) {
    ui->valueLabel->setText(QString::number(percentage * 100, 'f', 1) + "%");
    percentageHistory.prepend(percentage);
    if (percentageHistory.count() > 300) {
        percentageHistory.removeLast();
    }
    this->update();
}
