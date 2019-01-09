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
#ifndef PROCESS_H
#define PROCESS_H

#include <QObject>
#include <signal.h>

struct ProcessPrivate;
class SystemManager;
class ProcessManager;

class Process : public QObject
{
        Q_OBJECT
    public:
        explicit Process(int pid, ProcessManager* pm, SystemManager* sm, QObject *parent = nullptr);
        ~Process();

        bool setProperty(const char *name, const QVariant &value);
        QVariant property(const char *name) const;

    signals:
        void processGone(Process* p);
        void propertiesChanged(Process* p);

    public slots:
        void performUpdate();
        void sendSignal(int signal);

    private:
        ProcessPrivate* d;

        void addCascadingProcess(Process* p);

        QString readFile(QString file);
        QString readLink(QString link);
};

#endif // PROCESS_H
