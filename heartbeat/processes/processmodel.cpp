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
#include <QMutexLocker>
#include "processmanager.h"
#include "process.h"

ProcessModel::ProcessModel(ProcessManager* pm, ModelType t, QObject *parent)
    : QAbstractTableModel(parent)
{
    this->pm = pm;

    sortTimer = new QTimer();
    sortTimer->setInterval(100);
    sortTimer->setSingleShot(true);
    connect(sortTimer, &QTimer::timeout, [=] {
        performSort();
    });

    setModelType(t);
    connect(pm, SIGNAL(newPid(int)), this, SLOT(newPid(int)));
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
                return getProcessDisplayName(p);
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
        performSort();
        //emit dataChanged(index(shownProcesses.indexOf(p), 0), index(shownProcesses.indexOf(p), columnCount()));
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

    if (sortTimer->isActive()) {
        sortTimer->stop();
    }
    sortTimer->start();
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

void ProcessModel::sort(int column, Qt::SortOrder order) {
    sortColumn = column;
    sortOrder = order;
    performSort();
}

void ProcessModel::performSort() {
    if (sortColumn == -1) return;

    std::stable_sort(shownProcesses.begin(), shownProcesses.end(), [=](const Process* a, const Process* b) -> bool {
        if (a == nullptr || b == nullptr) return false;
        //Check if a < b
        bool retVal = false;
        switch (sortColumn) {
            case Name:
                retVal = getProcessDisplayName(a).toLower().localeAwareCompare(getProcessDisplayName(b).toLower()) < 0;
                break;
            case CPU:
                retVal = a->property("cpuUsage").toDouble() < b->property("cpuUsage").toDouble();
                break;
            case Memory:
                retVal = a->property("privateMem").toULongLong() < b->property("privateMem").toULongLong();
                break;
            case Pid:
                retVal = a->property("pid").toInt() < b->property("pid").toInt();
                break;
        }

        if (sortOrder == Qt::DescendingOrder) {
            return !retVal;
        } else {
            return retVal;
        }
    });

    emit dataChanged(index(0, 0), index(rowCount(), columnCount()));
}

QString ProcessModel::getProcessDisplayName(const Process *p) const {
    if (type == Applications) {
        if (p->property("exe").toString().endsWith("/theshell")) {
            return "theShell";
        } else if (p->property("exe").toString().endsWith("/theshellb")) {
            return "theShell Blueprint";
        } else {
            return p->property("x11-windowtitle").toString();
        }
    } else if (type == Processes) {
        return p->property("process").toString();
    }

    return "";
}

void ProcessModel::newPid(int pid) {
    Process* p = pm->processByPid(pid);
    connect(p, &Process::propertiesChanged, this, &ProcessModel::processPropertiesChanged);
    checkProcessForSetup(p);
}
