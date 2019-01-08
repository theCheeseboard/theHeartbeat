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
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "processes/processmodel.h"
#include "processes/process.h"
#include <tstackedwidget.h>
#include <tpopover.h>
#include <QMenu>
#include "processaction.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    sm = new SystemManager();
    pm = new ProcessManager(sm);

    ui->processTable->setModel(new ProcessModel(pm));
    ui->processTable->header()->setStretchLastSection(false);
    ui->processTable->header()->setDefaultSectionSize(100 * theLibsGlobal::getDPIScaling());
    ui->processTable->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->processTable->header()->setSectionResizeMode(1, QHeaderView::Interactive);
    ui->processTable->header()->setSectionResizeMode(2, QHeaderView::Interactive);
    ui->processTable->header()->setSectionResizeMode(3, QHeaderView::Interactive);

    ui->processesTable->setModel(new ProcessModel(pm, ProcessModel::Processes));
    ui->processesTable->header()->setStretchLastSection(false);
    ui->processesTable->header()->setDefaultSectionSize(100 * theLibsGlobal::getDPIScaling());
    ui->processesTable->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->processesTable->header()->setSectionResizeMode(1, QHeaderView::Interactive);
    ui->processesTable->header()->setSectionResizeMode(2, QHeaderView::Interactive);
    ui->processesTable->header()->setSectionResizeMode(3, QHeaderView::Interactive);

    ui->cpuUsageWidget->setTitle(tr("CPU Usage"));
    ui->memoryUsageWidget->setTitle(tr("Memory Usage"));
    ui->swapUsageWidget->setTitle(tr("Swap Usage"));

    if (!sm->property("swapTotal").isValid() || sm->property("swapTotal").toULongLong() == 0) {
        ui->swapUsageWidget->setVisible(false);
    }

    ui->cpuIndividualUsageWidget->setFixedHeight(0);
    ui->cpuUsageWidget->setExpandable(true);

    for (int i = 0; i < sm->property("cpuCount").toInt(); i++) {
        MiniPercentagePane* p = new MiniPercentagePane();
        p->setTitle(tr("CPU %1").arg(i));
        ui->cpuIndividualUsageLayout->addWidget(p);
        cpuPanes.append(p);
    }

    connect(sm, &SystemManager::newDataAvailable, [=] {
        QVariant cpuUsage = sm->property("cpu");
        if (cpuUsage.isValid()) {
            ui->cpuUsageWidget->setPercentage(cpuUsage.toDouble());
        }
        for (int i = 0; i < sm->property("cpuCount").toInt(); i++) {
            MiniPercentagePane* p = cpuPanes.at(i);

            QVariant cpuUsage = sm->property(QString("cpu").append(QString::number(i)).toUtf8());
            if (cpuUsage.isValid()) {
                p->setPercentage(cpuUsage.toDouble());
            }
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
    ui->pages->setCurrentAnimation(tStackedWidget::Lift);

    this->resizeEvent(nullptr);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_OverviewTerminateButton_clicked()
{
    sendSignal(ui->processTable, "SIGTERM", SIGTERM);
}

void MainWindow::sendSignal(QTreeView* tree, QString signalName, int signal) {
    if (tree->selectionModel()->selectedRows().count() > 0) {
        ProcessAction* act = new ProcessAction();

        switch (signal) {
            case SIGTERM:
                act->setTitle(tr("Terminate Process"));
                act->setText(tr("Are you sure you want to terminate these processes? You may lose any unsaved work."));
                break;
            case SIGKILL:
                act->setTitle(tr("Kill Process"));
                act->setText(tr("Are you sure you want to kill these processes? You'll lose any unsaved work."));
                break;
            default:
                act->setTitle(tr("Send %1").arg(signalName));
                act->setText(tr("Do you want to send %1 to these processes?").arg(signalName));
        }

        for (QModelIndex i : tree->selectionModel()->selectedRows()) {
            act->addProcess(i.data(Qt::UserRole).value<Process*>());
        }

        tPopover* p = new tPopover(act);
        p->setPopoverWidth(400 * theLibsGlobal::getDPIScaling());
        connect(act, &ProcessAction::dismiss, p, &tPopover::dismiss);
        connect(act, &ProcessAction::accept, [=] {
            for (QModelIndex i : tree->selectionModel()->selectedRows()) {
                i.data(Qt::UserRole).value<Process*>()->sendSignal(signal);
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

void MainWindow::on_processTable_customContextMenuRequested(const QPoint &pos)
{
    if (ui->processTable->selectionModel()->selectedRows().count() > 0) {
        QMenu* m = new QMenu();
        QModelIndexList selected = ui->processTable->selectionModel()->selectedRows();
        if (selected.count() == 1) {
            m->addSection(tr("For %1").arg(selected.first().data(Qt::DisplayRole).toString()));

        } else {
            m->addSection(tr("For %1").arg(tr("%n processes", nullptr, selected.count())));
        }

        QMenu* signalsMenu = new QMenu();
        signalsMenu->setTitle(tr("Send Signal"));
        signalsMenu->addAction("SIGSTOP", [=] {sendSignal(ui->processTable, "SIGSTOP", SIGSTOP);});
        signalsMenu->addAction("SIGCONT", [=] {sendSignal(ui->processTable, "SIGCONT", SIGCONT);});
        signalsMenu->addAction("SIGHUP" , [=] {sendSignal(ui->processTable, "SIGHUP" , SIGHUP );});
        signalsMenu->addAction("SIGINT" , [=] {sendSignal(ui->processTable, "SIGINT" , SIGINT );});
        signalsMenu->addAction("SIGUSR1", [=] {sendSignal(ui->processTable, "SIGUSR1", SIGUSR1);});
        signalsMenu->addAction("SIGUSR2", [=] {sendSignal(ui->processTable, "SIGUSR2", SIGUSR2);});
        signalsMenu->addSeparator();
        signalsMenu->addAction("SIGTERM", [=] {sendSignal(ui->processTable, "SIGTERM", SIGTERM);});
        signalsMenu->addAction("SIGKILL", [=] {sendSignal(ui->processTable, "SIGKILL", SIGKILL);});

        m->addMenu(signalsMenu);
        m->addAction(QIcon::fromTheme("window-close"), tr("Terminate"), [=] {sendSignal(ui->processTable, "SIGTERM", SIGTERM);});
        m->addAction(QIcon::fromTheme("application-exit"), tr("Kill"), [=] {sendSignal(ui->processTable, "SIGKILL", SIGKILL);});
        m->exec(ui->processTable->mapToGlobal(pos));
    }
}

void MainWindow::on_paneSelection_currentRowChanged(int currentRow)
{
    ui->pages->setCurrentIndex(currentRow);
}

void MainWindow::resizeEvent(QResizeEvent *event) {
    //Move items around accordingly
    if (this->width() < 1000 * theLibsGlobal::getDPIScaling()) {
        //Collapse sidebar
        ui->paneSelection->setMaximumSize(42 * theLibsGlobal::getDPIScaling(), QWIDGETSIZE_MAX);
        ui->appTitleLabel->setPixmap(QIcon::fromTheme("utilities-system-monitor").pixmap(QSize(24, 24) * theLibsGlobal::getDPIScaling()));
    } else {
        //Expand sidebar
        ui->paneSelection->setMaximumSize(300 * theLibsGlobal::getDPIScaling(), QWIDGETSIZE_MAX);
        ui->appTitleLabel->setText(tr("theHeartbeat"));
    }
}

void MainWindow::on_cpuUsageWidget_toggleExpand()
{
    tVariantAnimation* anim = new tVariantAnimation();
    anim->setStartValue(ui->cpuIndividualUsageWidget->height());
    if (ui->cpuIndividualUsageWidget->height() == 0) {
        anim->setEndValue(ui->cpuIndividualUsageWidget->sizeHint().height());
        ui->cpuUsageWidget->setExpanded(true);
    } else {
        anim->setEndValue(0);
        ui->cpuUsageWidget->setExpanded(false);
    }
    anim->setDuration(500);
    anim->setEasingCurve(QEasingCurve::OutCubic);
    connect(anim, &tVariantAnimation::valueChanged, [=](QVariant value) {
        ui->cpuIndividualUsageWidget->setFixedHeight(value.toInt());
    });
    connect(anim, &tVariantAnimation::finished, anim, &tVariantAnimation::deleteLater);
    anim->start();
}
