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
#include "processaction.h"
#include "ui_processaction.h"

#include "processes/process.h"

ProcessAction::ProcessAction(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::ProcessAction) {
    ui->setupUi(this);

    ui->titleLabel->setBackButtonShown(true);
}

ProcessAction::~ProcessAction() {
    delete ui;
}

void ProcessAction::setTitle(QString title) {
    ui->titleLabel->setText(title);
}

void ProcessAction::setText(QString text) {
    ui->messageLabel->setText(text);
}

void ProcessAction::setOkText(QString text) {
    ui->acceptButton->setText(text);
}

void ProcessAction::setOkIcon(QIcon icon) {
    ui->acceptButton->setIcon(icon);
}

void ProcessAction::addProcess(Process* p) {
    ui->processesWidget->addItem(p->property("process").toString());
}

void ProcessAction::on_acceptButton_clicked() {
    emit accept();
}

void ProcessAction::on_titleLabel_backButtonClicked() {
    emit dismiss();
}
