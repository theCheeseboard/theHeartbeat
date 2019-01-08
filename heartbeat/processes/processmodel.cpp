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
#include "processmodel.h"

#include <QSize>
#include "processmanager.h"
#include "process.h"

ProcessModel::ProcessModel(ProcessManager* pm, ModelType t, QObject *parent)
    : QAbstractTableModel(parent)
{
    this->pm = pm;

    setModelType(t);
    connect(pm, &ProcessManager::newPid, [=](int pid) {
        Process* p = pm->processByPid(pid);
        connect(p, &Process::propertiesChanged, this, &ProcessModel::processPropertiesChanged);
        checkProcessForSetup(p);
    });
}

QVariant ProcessModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole) {
        switch (section) {
            case Name:
                return tr("Name");
            case CPU:
                return tr("CPU %");
            case Memory:
                return tr("Memory");
            case Pid:
                return tr("PID");
        }
    } else if (role == Qt::SizeHintRole) {
        return QSize(500, 29);
    }
    return QAbstractTableModel::headerData(section, orientation, role);
}

int ProcessModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }

    return shownProcesses.count();
}

int ProcessModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }

    return 4;
}

QVariant ProcessModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    Process* p = shownProcesses.value(index.row());
    if (role == Qt::DisplayRole) {
        switch (index.column()) {
            case Name:
                if (type == Applications) {
                    return p->property("x11-windowtitle");
                } else if (type == Processes) {
                    return p->property("process");
                }
                break;
            case CPU: {
                double cpuUsage = p->property("cpuUsage").toDouble();
                if (cpuUsage == 0) {
                    return "";
                } else {
                    return QString::number(cpuUsage * 100, 'f', 1) + "%";
                }
            }
            case Memory:
                return tr("%1 KB").arg(p->property("privateMem").toULongLong());
            case Pid:
                return QString::number(p->property("pid").toInt());
        }
    } else if (role == Qt::DecorationRole) {
        if (index.column() == Name) {
            return p->property("x11-icon");
        }
    } else if (role == Qt::UserRole) {
        return QVariant::fromValue(p);
    }

    return QVariant();
}

void ProcessModel::setModelType(ModelType type) {
    this->type = type;
    loadProcesses();
}

void ProcessModel::loadProcesses() {
    for (Process* p : shownProcesses) {
        disconnect(p, 0, this, 0);
    }
    shownProcesses.clear();

    for (int pid : pm->availablePids()) {
        Process* p = pm->processByPid(pid);
        connect(p, &Process::propertiesChanged, this, &ProcessModel::processPropertiesChanged);
        checkProcessForSetup(p);
    }
}

void ProcessModel::processPropertiesChanged(Process* p) {
    if (shownProcesses.contains(p)) {
        emit dataChanged(index(shownProcesses.indexOf(p), 0), index(shownProcesses.indexOf(p), columnCount()));
    } else {
        checkProcessForSetup(p);
    }
}

bool ProcessModel::checkProcessEligibility(Process *p) {
    switch (type) {
        case Applications:
            return p->property("x11-window").isValid();
        case Processes:
            return true;
    }
}

void ProcessModel::setupProcess(Process *p) {
    this->beginInsertRows(QModelIndex(), rowCount() - 1, rowCount() - 1);
    shownProcesses.append(p);
    connect(p, SIGNAL(processGone(Process*)), this, SLOT(processGone(Process*)));
    this->endInsertRows();
}

void ProcessModel::processGone(Process* p) {
    this->beginRemoveRows(QModelIndex(), shownProcesses.indexOf(p), shownProcesses.indexOf(p));
    shownProcesses.removeOne(p);
    this->endRemoveRows();
}

void ProcessModel::checkProcessForSetup(Process *p) {
    if (!shownProcesses.contains(p) && p != nullptr && checkProcessEligibility(p)) {
        setupProcess(p);
    }
}
