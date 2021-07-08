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
#include <QDesktopServices>
#include <QScroller>
#include <QUrl>
#include <taboutdialog.h>
#include <tcsdtools.h>
#include <thelpmenu.h>
#include "processaction.h"

struct MainWindowPrivate {
    ProcessManager* pm;
    SystemManager* sm;

    QList<MiniPercentagePane*> cpuPanes;
    QMap<QString, MiniNumberPane*> networkRxPanes;
    QMap<QString, MiniNumberPane*> networkTxPanes;

    tCsdTools csd;
};

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {
    ui->setupUi(this);

    d = new MainWindowPrivate;
    d->csd.installMoveAction(ui->topWidget);
    d->csd.installResizeAction(this);

    if (tCsdGlobal::windowControlsEdge() == tCsdGlobal::Left) {
        ui->leftCsdLayout->addWidget(d->csd.csdBoxForWidget(this));
    } else {
        ui->rightCsdLayout->addWidget(d->csd.csdBoxForWidget(this));
    }

    this->setWindowIcon(QIcon::fromTheme("theheartbeat", QIcon::fromTheme("utilities-system-monitor")));

    QMenu* menu = new QMenu();
    menu->addMenu(new tHelpMenu(this));
    menu->addSeparator();
    menu->addAction(QIcon::fromTheme("application-exit"), tr("Exit"), [ = ] {
        QApplication::exit();
    });

    ui->menuButton->setIconSize(SC_DPI_T(QSize(24, 24), QSize));
    ui->menuButton->setMenu(menu);

    d->sm = new SystemManager();
    d->pm = new ProcessManager(d->sm);

    ui->processTable->setModel(new ProcessModel(d->pm));
    ui->processTable->setItemDelegateForColumn(0, new ProcessTitleDelegate());
    ui->processTable->header()->setStretchLastSection(false);
    ui->processTable->header()->setDefaultSectionSize(SC_DPI(100));
    ui->processTable->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->processTable->header()->setSectionResizeMode(1, QHeaderView::Interactive);
    ui->processTable->header()->setSectionResizeMode(2, QHeaderView::Interactive);
    ui->processTable->header()->setSectionResizeMode(3, QHeaderView::Interactive);
    ui->processTable->model()->sort(0);

    ui->processesTable->setModel(new ProcessModel(d->pm, ProcessModel::Processes));
    ui->processesTable->setItemDelegateForColumn(0, new ProcessTitleDelegate());
    ui->processesTable->header()->setStretchLastSection(false);
    ui->processesTable->header()->setDefaultSectionSize(SC_DPI(100));
    ui->processesTable->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->processesTable->header()->setSectionResizeMode(1, QHeaderView::Interactive);
    ui->processesTable->header()->setSectionResizeMode(2, QHeaderView::Interactive);
    ui->processesTable->header()->setSectionResizeMode(3, QHeaderView::Interactive);
    ui->processesTable->model()->sort(0);

    ui->cpuUsageWidget->setTitle(tr("CPU Usage"));
    ui->memoryUsageWidget->setTitle(tr("Memory Usage"));
    ui->swapUsageWidget->setTitle(tr("Swap Usage"));
    ui->netRxWidget->setTitle(tr("Network Receive"));
    ui->netTxWidget->setTitle(tr("Network Send"));
    ui->netRxWidget->setUnit(NumberPane::KilobytePerSecond);
    ui->netTxWidget->setUnit(NumberPane::KilobytePerSecond);
    ui->cpuTempWidget->setTitle(tr("CPU Temperature"));
    ui->cpuTempWidget->setUnit(NumberPane::MillidegreeCelsius);
    ui->gpuTempWidget->setTitle(tr("GPU Temperature"));
    ui->gpuTempWidget->setUnit(NumberPane::MillidegreeCelsius);

    if (!d->sm->property("swapTotal").isValid() || d->sm->property("swapTotal").toULongLong() == 0) {
        ui->swapUsageWidget->setVisible(false);
    }

    ui->cpuIndividualUsageWidget->setFixedHeight(0);
    ui->cpuUsageWidget->setExpandable(true);
    ui->netRxIndividualWidget->setFixedHeight(0);
    ui->netRxWidget->setExpandable(true);
    ui->netTxIndividualWidget->setFixedHeight(0);
    ui->netTxWidget->setExpandable(true);

    for (int i = 0; i < d->sm->property("cpuCount").toInt(); i++) {
        MiniPercentagePane* p = new MiniPercentagePane(this);
        p->setTitle(tr("CPU %1").arg(i));
        ui->cpuIndividualUsageLayout->addWidget(p);
        d->cpuPanes.append(p);
    }

    for (QString netDevice : d->sm->property("netDevices").toStringList()) {
        MiniNumberPane* rxPane = new MiniNumberPane(this);
        rxPane->setTitle(netDevice);
        rxPane->setUnit(MiniNumberPane::KilobytePerSecond);
        ui->netRxIndividualLayout->addWidget(rxPane);
        d->networkRxPanes.insert(netDevice, rxPane);

        MiniNumberPane* txPane = new MiniNumberPane(this);
        txPane->setTitle(netDevice);
        txPane->setUnit(MiniNumberPane::KilobytePerSecond);
        ui->netTxIndividualLayout->addWidget(txPane);
        d->networkTxPanes.insert(netDevice, txPane);
    }

    connect(d->sm, &SystemManager::newDataAvailable, [ = ] {
        QVariant cpuUsage = d->sm->property("cpu");
        if (cpuUsage.isValid()) {
            ui->cpuUsageWidget->setPercentage(cpuUsage.toDouble());
        }
        for (int i = 0; i < d->sm->property("cpuCount").toInt(); i++) {
            MiniPercentagePane* p = d->cpuPanes.at(i);

            QVariant cpuUsage = d->sm->property(QString("cpu").append(QString::number(i)).toUtf8());
            if (cpuUsage.isValid()) {
                p->setPercentage(cpuUsage.toDouble());
            }
        }

        QVariant memTotal = d->sm->property("memTotal");
        QVariant memAvail = d->sm->property("memAvailable");
        if (memTotal.isValid() && memAvail.isValid()) {
            ui->memoryUsageWidget->setMax(memTotal.toULongLong());
            ui->memoryUsageWidget->setValue(memTotal.toULongLong() - memAvail.toULongLong());
        }

        QVariant swapTotal = d->sm->property("swapTotal");
        QVariant swapAvail = d->sm->property("swapFree");
        if (swapTotal.isValid() && swapAvail.isValid()) {
            ui->swapUsageWidget->setMax(swapTotal.toULongLong());
            ui->swapUsageWidget->setValue(swapTotal.toULongLong() - swapAvail.toULongLong());
        }

        QVariant netRx = d->sm->property("netRx");
        QVariant netTx = d->sm->property("netTx");
        if (netRx.isValid() && netTx.isValid()) {
            ui->netRxWidget->setValue(netRx.toULongLong() / 1024);
            ui->netTxWidget->setValue(netTx.toULongLong() / 1024);
        }

        for (QString netDevice : d->sm->property("netDevices").toStringList()) {
            d->networkRxPanes.value(netDevice)->setValue(d->sm->property(QString("netRx-").append(netDevice).toUtf8()).toULongLong() / 1024);
            d->networkTxPanes.value(netDevice)->setValue(d->sm->property(QString("netTx-").append(netDevice).toUtf8()).toULongLong() / 1024);
        }

        QVariant cpuTemp = d->sm->property("cpuTemp");
        if (cpuTemp.isValid()) {
            ui->cpuTempWidget->setValue(cpuTemp.toLongLong());
            ui->cpuTempWidget->setVisible(true);
        } else {
            ui->cpuTempWidget->setVisible(false);
        }

        QVariant gpuTemp = d->sm->property("gpuTemp");
        if (gpuTemp.isValid()) {
            ui->gpuTempWidget->setValue(gpuTemp.toLongLong());
            ui->gpuTempWidget->setVisible(true);
        } else {
            ui->gpuTempWidget->setVisible(false);
        }
    });

    ui->sideStatusPane->setFixedWidth(SC_DPI(200));
    ui->sideStatusPaneContents->setFixedWidth(SC_DPI(200));

    ui->pages->setCurrentAnimation(tStackedWidget::SlideHorizontal);
    ui->pages->setCurrentIndex(0);
    on_pages_currentChanged(0);

    QScroller::grabGesture(ui->processTable->viewport(), QScroller::LeftMouseButtonGesture);
    QScroller::grabGesture(ui->processesTable->viewport(), QScroller::LeftMouseButtonGesture);
    QScroller::grabGesture(ui->sideStatusPane->viewport(), QScroller::LeftMouseButtonGesture);

    this->resizeEvent(nullptr);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_OverviewTerminateButton_clicked() {
    sendSignal(ui->processTable, "SIGTERM", SIGTERM);
}

void MainWindow::sendSignal(QTreeView* tree, QString signalName, int signal) {
    if (tree->selectionModel()->selectedRows().count() > 0) {
        ProcessAction* act = new ProcessAction();

        switch (signal) {
            case SIGTERM:
                act->setTitle(tr("Terminate Process"));
                act->setText(tr("Are you sure you want to terminate these processes? You may lose any unsaved work."));
                act->setOkText(tr("Terminate"));
                act->setOkIcon(QIcon::fromTheme("window-close"));
                break;
            case SIGKILL:
                act->setTitle(tr("Force Stop"));
                act->setText(tr("Are you sure you want to force these processes to stop? You'll lose any unsaved work."));
                act->setOkText(tr("Force Stop"));
                act->setOkIcon(QIcon::fromTheme("application-exit"));
                break;
            default:
                act->setTitle(tr("Send %1").arg(signalName));
                act->setText(tr("Do you want to send %1 to these processes?").arg(signalName));
                act->setOkText(tr("Send %1").arg(signalName));
                act->setOkIcon(QIcon::fromTheme("dialog-ok"));
        }

        for (QModelIndex i : tree->selectionModel()->selectedRows()) {
            act->addProcess(i.data(Qt::UserRole).value<Process*>());
        }

        tPopover* p = new tPopover(act);
        p->setPopoverWidth(SC_DPI(400));
        connect(act, &ProcessAction::dismiss, p, &tPopover::dismiss);
        connect(act, &ProcessAction::accept, [ = ] {
            for (QModelIndex i : tree->selectionModel()->selectedRows()) {
                i.data(Qt::UserRole).value<Process*>()->sendSignal(signal);
            }
            p->dismiss();
        });
        connect(p, &tPopover::dismissed, this, [ = ] {
            p->deleteLater();
            act->deleteLater();
        });
        p->show(this);
    }
}

void MainWindow::on_processTable_customContextMenuRequested(const QPoint& pos) {
    prepareContextMenu(ui->processTable, pos);
}

void MainWindow::prepareContextMenu(QTreeView* view, QPoint pos) {
    if (view->selectionModel()->selectedRows().count() > 0) {
        QMenu* m = new QMenu();
        QModelIndexList selected = view->selectionModel()->selectedRows();
        if (selected.count() == 1) {
            m->addSection(tr("For %1").arg(selected.first().data(Qt::DisplayRole).toString()));

        } else {
            m->addSection(tr("For %1").arg(tr("%n processes", nullptr, selected.count())));
        }

        QMenu* signalsMenu = new QMenu();
        signalsMenu->setTitle(tr("Send Signal"));
        signalsMenu->addAction("SIGSTOP", this, [ = ] {sendSignal(view, "SIGSTOP", SIGSTOP);});
        signalsMenu->addAction("SIGCONT", this, [ = ] {sendSignal(view, "SIGCONT", SIGCONT);});
        signalsMenu->addAction("SIGHUP", this, [ = ] {sendSignal(view, "SIGHUP", SIGHUP );});
        signalsMenu->addAction("SIGINT", this, [ = ] {sendSignal(view, "SIGINT", SIGINT );});
        signalsMenu->addAction("SIGUSR1", this, [ = ] {sendSignal(view, "SIGUSR1", SIGUSR1);});
        signalsMenu->addAction("SIGUSR2", this, [ = ] {sendSignal(view, "SIGUSR2", SIGUSR2);});
        signalsMenu->addSeparator();
        signalsMenu->addAction("SIGTERM", this, [ = ] {sendSignal(view, "SIGTERM", SIGTERM);});
        signalsMenu->addAction("SIGKILL", this, [ = ] {sendSignal(view, "SIGKILL", SIGKILL);});

        m->addMenu(signalsMenu);
        m->addAction(QIcon::fromTheme("window-close"), tr("Terminate"), [ = ] {sendSignal(view, "SIGTERM", SIGTERM);});
        m->addAction(QIcon::fromTheme("application-exit"), tr("Force Stop"), [ = ] {sendSignal(view, "SIGKILL", SIGKILL);});
        connect(m, &QMenu::aboutToHide, m, &QMenu::deleteLater);
        m->popup(view->mapToGlobal(pos));
    }
}

void MainWindow::on_paneSelection_currentRowChanged(int currentRow) {
    ui->pages->setCurrentIndex(currentRow);
}

void MainWindow::on_cpuUsageWidget_toggleExpand() {
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
    connect(anim, &tVariantAnimation::valueChanged, this, [ = ](QVariant value) {
        ui->cpuIndividualUsageWidget->setFixedHeight(value.toInt());
    });
    connect(anim, &tVariantAnimation::finished, anim, &tVariantAnimation::deleteLater);
    anim->start();
}

void MainWindow::on_netRxWidget_toggleExpand() {
    tVariantAnimation* anim = new tVariantAnimation();
    anim->setStartValue(ui->netRxIndividualWidget->height());
    if (ui->netRxIndividualWidget->height() == 0) {
        anim->setEndValue(ui->netRxIndividualWidget->sizeHint().height());
        ui->netRxWidget->setExpanded(true);
    } else {
        anim->setEndValue(0);
        ui->netRxWidget->setExpanded(false);
    }
    anim->setDuration(500);
    anim->setEasingCurve(QEasingCurve::OutCubic);
    connect(anim, &tVariantAnimation::valueChanged, this, [ = ](QVariant value) {
        ui->netRxIndividualWidget->setFixedHeight(value.toInt());
    });
    connect(anim, &tVariantAnimation::finished, anim, &tVariantAnimation::deleteLater);
    anim->start();
}

void MainWindow::on_netTxWidget_toggleExpand() {
    tVariantAnimation* anim = new tVariantAnimation();
    anim->setStartValue(ui->netTxIndividualWidget->height());
    if (ui->netTxIndividualWidget->height() == 0) {
        anim->setEndValue(ui->netTxIndividualWidget->sizeHint().height());
        ui->netTxWidget->setExpanded(true);
    } else {
        anim->setEndValue(0);
        ui->netTxWidget->setExpanded(false);
    }
    anim->setDuration(500);
    anim->setEasingCurve(QEasingCurve::OutCubic);
    connect(anim, &tVariantAnimation::valueChanged, this, [ = ](QVariant value) {
        ui->netTxIndividualWidget->setFixedHeight(value.toInt());
    });
    connect(anim, &tVariantAnimation::finished, anim, &tVariantAnimation::deleteLater);
    anim->start();
}

void MainWindow::on_pages_currentChanged(int arg1) {
    //Don't bother sorting process tables that aren't visible
    ((ProcessModel*) ui->processTable->model())->setPerformSorting(false);
    ((ProcessModel*) ui->processesTable->model())->setPerformSorting(false);

    switch (arg1) {
        case 0:
            ((ProcessModel*) ui->processTable->model())->setPerformSorting(true);
            break;
        case 1:
            ((ProcessModel*) ui->processesTable->model())->setPerformSorting(true);
    }
}

void MainWindow::on_processesTable_customContextMenuRequested(const QPoint& pos) {
    prepareContextMenu(ui->processesTable, pos);
}

void MainWindow::on_overviewButton_toggled(bool checked) {
    if (checked) {
        ui->pages->setCurrentWidget(ui->overviewPane);
    }
}

void MainWindow::on_processesButton_toggled(bool checked) {
    if (checked) {
        ui->pages->setCurrentWidget(ui->processesPane);
    }
}
