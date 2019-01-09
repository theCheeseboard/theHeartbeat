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
#ifndef PROCESSMODEL_H
#define PROCESSMODEL_H

#include <QAbstractItemModel>
#include <QTimer>
#include <QMutex>
#include <QPainter>
#include <QStyleOptionViewItem>
#include <QStyledItemDelegate>

class ProcessManager;
class Process;

class ProcessModel : public QAbstractTableModel
{
        Q_OBJECT

    public:
        enum Columns {
            Name = 0,
            CPU,
            Memory,
            Pid
        };

        enum ModelType {
            Applications,
            Processes
        };
        explicit ProcessModel(ProcessManager* pm, ModelType t = Applications, QObject *parent = nullptr);


        // Header:
        QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

        int rowCount(const QModelIndex &parent = QModelIndex()) const override;
        int columnCount(const QModelIndex &parent = QModelIndex()) const override;

        QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

        void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;

        void setModelType(ModelType type);

    public slots:
        void setPerformSorting(bool performSorting);

    private slots:
        void processGone(Process* p);
        void processPropertiesChanged(Process* p);
        void newPid(int pid);

    private:
        ProcessManager* pm;
        ModelType type;

        QVector<Process*> shownProcesses;

        int sortColumn = -1;
        Qt::SortOrder sortOrder;
        void performSort();
        QTimer* sortTimer;
        bool performSorting = false;

        void loadProcesses();
        void checkProcessForSetup(Process* p);
        bool checkProcessEligibility(Process* p);
        void setupProcess(Process* p);
        QString getProcessDisplayName(const Process* p) const;
};


class ProcessTitleDelegate : public QStyledItemDelegate {
    Q_OBJECT

    public:
        explicit ProcessTitleDelegate(QObject* parent = nullptr);

        void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

#endif // PROCESSMODEL_H
