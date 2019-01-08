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
#ifndef PROCESSMODEL_H
#define PROCESSMODEL_H

#include <QAbstractItemModel>

class ProcessManager;
class Process;

class ProcessModel : public QAbstractTableModel
{
        Q_OBJECT

    public:
        explicit ProcessModel(ProcessManager* pm, QObject *parent = nullptr);

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

        // Header:
        QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

        int rowCount(const QModelIndex &parent = QModelIndex()) const override;
        int columnCount(const QModelIndex &parent = QModelIndex()) const override;

        QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

        void setModelType(ModelType type);

    private slots:
        void processGone(Process* p);
        void processPropertiesChanged(Process* p);

    private:
        ProcessManager* pm;
        ModelType type;
        QList<Process*> shownProcesses;

        void loadProcesses();
        void checkProcessForSetup(Process* p);
        bool checkProcessEligibility(Process* p);
        void setupProcess(Process* p);
};

#endif // PROCESSMODEL_H
