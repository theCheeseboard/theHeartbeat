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
#include "processmanager.h"

#include "process.h"
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QHash>
#include <QMutexLocker>
#include <QProcess>
#include <QRegularExpression>
#include <QTimer>
#include <tpromise.h>

struct ProcessManagerPrivate {
        QHash<int, Process*> processes;
        QMutex processesLocker;
        SystemManager* sm;
};

ProcessManager::ProcessManager(SystemManager* sm, QObject* parent) :
    QObject(parent) {
    d = new ProcessManagerPrivate();
    d->sm = sm;

    checkProcesses();

    QTimer* timer = new QTimer();
    timer->setInterval(1000);
    connect(timer, &QTimer::timeout, this, &ProcessManager::checkProcesses);
    timer->start();
}

ProcessManager::~ProcessManager() {
    delete d;
}

void ProcessManager::checkProcesses() {
    (new tPromise<void>([this](QString& error) {
        QDir procDir("/proc");
        QStringList procDirs = procDir.entryList(QDir::Dirs);

        QRegularExpression numberMatch("\\d+");
        for (QString process : procDirs) {
            bool isAnInteger;
            int pid = process.toInt(&isAnInteger);
            if (isAnInteger && !d->processes.contains(pid)) {
                Process* p = new Process(pid, this, d->sm);
                connect(p, SIGNAL(processGone(Process*)), this, SLOT(processGone(Process*)));

                d->processesLocker.lock();
                d->processes.insert(pid, p);
                d->processesLocker.unlock();

                emit newPid(pid);
            }
        }
    }))->then([this] {
        for (int pid : d->processes.keys()) {
            d->processes.value(pid)->performUpdate();
        }
    });
}

void ProcessManager::processGone(Process* p) {
    QMutexLocker locker(&d->processesLocker);
    d->processes.remove(p->property("pid").toInt());
    p->deleteLater();
}

Process* ProcessManager::processByPid(int pid) {
    QMutexLocker locker(&d->processesLocker);
    if (d->processes.contains(pid)) {
        return d->processes.value(pid);
    } else {
        return nullptr;
    }
}

QList<int> ProcessManager::availablePids() {
    QMutexLocker locker(&d->processesLocker);
    return d->processes.keys();
}
