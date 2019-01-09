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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include <QTreeView>
#include "processes/processmanager.h"
#include "system/systemmanager.h"
#include "panes/minipercentagepane.h"
#include "panes/mininumberpane.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
        Q_OBJECT

    public:
        explicit MainWindow(QWidget *parent = nullptr);
        ~MainWindow();

    private slots:
        void on_OverviewTerminateButton_clicked();

        void on_processTable_customContextMenuRequested(const QPoint &pos);

        void on_paneSelection_currentRowChanged(int currentRow);

        void on_cpuUsageWidget_toggleExpand();

        void sendSignal(QTreeView* tree, QString signalName, int signal);

        void on_netRxWidget_toggleExpand();

        void on_netTxWidget_toggleExpand();

        void on_pages_currentChanged(int arg1);

        void prepareContextMenu(QTreeView* tree, QPoint pos);

        void on_processesTable_customContextMenuRequested(const QPoint &pos);

    private:
        Ui::MainWindow *ui;

        ProcessManager* pm;
        SystemManager* sm;

        QList<MiniPercentagePane*> cpuPanes;
        QMap<QString, MiniNumberPane*> networkRxPanes;
        QMap<QString, MiniNumberPane*> networkTxPanes;

        void resizeEvent(QResizeEvent* event);
};

#endif // MAINWINDOW_H
