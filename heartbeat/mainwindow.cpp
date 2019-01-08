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
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "processes/processmodel.h"
#include "processes/process.h"
#include <tstackedwidget.h>
#include <tpopover.h>
#include "processaction.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    sm = new SystemManager();
    pm = new ProcessManager(sm);

    ui->processTable->setModel(new ProcessModel(pm));

    ui->cpuUsageWidget->setTitle(tr("CPU Usage"));
    ui->memoryUsageWidget->setTitle(tr("Memory Usage"));
    ui->swapUsageWidget->setTitle(tr("Swap Usage"));

    if (!sm->property("swapTotal").isValid() || sm->property("swapTotal").toULongLong() == 0) {
        ui->swapUsageWidget->setVisible(false);
    }

    connect(sm, &SystemManager::newDataAvailable, [=] {
        QVariant cpuUsage = sm->property("cpu");
        if (cpuUsage.isValid()) {
            ui->cpuUsageWidget->setPercentage(cpuUsage.toDouble());
        }

        QVariant memTotal = sm->property("memTotal");
        QVariant memAvail = sm->property("memAvailable");
        if (memTotal.isValid() && memAvail.isValid()) {
            ui->memoryUsageWidget->setMax(memTotal.toULongLong());
            ui->memoryUsageWidget->setValue(memTotal.toULongLong() - memAvail.toULongLong());
        }

        QVariant swapTotal = sm->property("swapTotal");
        QVariant swapAvail = sm->property("swapFree");
        if (swapTotal.isValid() && swapAvail.isValid()) {
            ui->swapUsageWidget->setMax(swapTotal.toULongLong());
            ui->swapUsageWidget->setValue(swapTotal.toULongLong() - swapAvail.toULongLong());
        }
    });

    ui->sideStatusPane->setFixedWidth(200 * theLibsGlobal::getDPIScaling());
    ui->sideStatusPaneContents->setFixedWidth(200 * theLibsGlobal::getDPIScaling());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_OverviewTerminateButton_clicked()
{
    if (ui->processTable->selectionModel()->selectedRows().count() > 0) {
        ProcessAction* act = new ProcessAction();
        act->setTitle(tr("Terminate Process"));
        act->setText(tr("Are you sure you want to terminate these processes? You'll lose any unsaved work."));

        for (QModelIndex i : ui->processTable->selectionModel()->selectedRows()) {
            act->addProcess(i.data(Qt::UserRole).value<Process*>());
        }

        tPopover* p = new tPopover(act);
        p->setPopoverWidth(400 * theLibsGlobal::getDPIScaling());
        connect(act, &ProcessAction::dismiss, p, &tPopover::dismiss);
        connect(act, &ProcessAction::accept, [=] {
            for (QModelIndex i : ui->processTable->selectionModel()->selectedRows()) {
                i.data(Qt::UserRole).value<Process*>()->sendSignal(SIGTERM);
            }
            p->dismiss();
        });
        connect(p, &tPopover::dismissed, [=] {
            p->deleteLater();
            act->deleteLater();
        });
        p->show(this);
    }
}
