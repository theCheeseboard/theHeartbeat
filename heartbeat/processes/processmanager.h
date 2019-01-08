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
#ifndef PROCESSMANAGER_H
#define PROCESSMANAGER_H

#include <QObject>

struct ProcessManagerPrivate;
class Process;
class SystemManager;

class ProcessManager : public QObject
{
        Q_OBJECT
    public:
        explicit ProcessManager(SystemManager* sm, QObject *parent = nullptr);
        ~ProcessManager();

        Process* processByPid(int pid);
        QList<int> availablePids();

    signals:
        void newPid(int pid);

    public slots:
        void checkProcesses();

    private slots:
        void processGone(Process* p);

    private:
        ProcessManagerPrivate* d;
};

#endif // PROCESSMANAGER_H
