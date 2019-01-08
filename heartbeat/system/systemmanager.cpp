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
#include "systemmanager.h"

#include <QTimer>
#include <QFile>
#include <QVariant>
#include <QDebug>

struct SystemManagerPrivate {
    qulonglong cpuWork = 0;
    qulonglong cpuIdle = 0;

    QList<qulonglong> cpusWork;
    QList<qulonglong> cpusIdle;
};

SystemManager::SystemManager(QObject *parent) : QObject(parent)
{
    d = new SystemManagerPrivate();

    updateData();

    QTimer* timer = new QTimer();
    timer->setInterval(1000);
    connect(timer, &QTimer::timeout, this, &SystemManager::updateData);
    timer->start();
}

SystemManager::~SystemManager() {
    delete d;
}

void SystemManager::updateData() {
    QFile stat("/proc/stat");
    stat.open(QFile::ReadOnly);
    QString statFile = stat.readAll();

    for (QString line : statFile.split("\n")) {
        QStringList splits = line.split(" ", QString::SkipEmptyParts);
        if (splits.count() == 0) continue;
        QString name = splits.first().trimmed();
        if (name.startsWith("cpu")) {
            qulonglong sumOfCpu = 0;
            qulonglong sumOfWork = 0;
            for (int i = 1; i < 8; i++) {
                sumOfCpu += splits.at(i).toULongLong();
            }
            for (int i = 1; i < 4; i++) {
                sumOfWork += splits.at(i).toULongLong();
            }

            if (name == "cpu") {
                if (d->cpuWork != 0) {
                    qulonglong cpuOverPeriod = sumOfCpu - d->cpuIdle;
                    qulonglong workOverPeriod = sumOfWork - d->cpuWork;

                    this->setProperty("cpu", (double) workOverPeriod / (double) cpuOverPeriod);
                    this->setProperty("cpuJiffies", cpuOverPeriod);
                }

                d->cpuIdle = sumOfCpu;
                d->cpuWork = sumOfWork;
            } else {
                int cpuNo = name.mid(3).toInt();
                if (d->cpusWork.count() <= cpuNo) {
                    d->cpusIdle.append(sumOfCpu);
                    d->cpusWork.append(sumOfWork);

                    this->setProperty("cpuCount", d->cpusWork.count());
                } else {
                    qulonglong cpuOverPeriod = sumOfCpu - d->cpusIdle.at(cpuNo);
                    qulonglong workOverPeriod = sumOfWork - d->cpusWork.at(cpuNo);

                    this->setProperty(QString("cpu").append(QString::number(cpuNo)).toUtf8(), (double) workOverPeriod / (double) cpuOverPeriod);
                    this->setProperty(QString("cpuJiffies").append(QString::number(cpuNo)).toUtf8(), cpuOverPeriod);
                }
            }

        }
    }


    QFile mem("/proc/meminfo");
    mem.open(QFile::ReadOnly);
    QString memFile = mem.readAll();

    for (QString line : memFile.split("\n")) {
        QStringList splits = line.split(":", QString::SkipEmptyParts);
        if (splits.count() == 0) continue;
        QString name = splits.first().trimmed();
        QString value = splits.last().trimmed();
        if (name == "MemTotal") {
            this->setProperty("memTotal", value.split(" ").first().toULongLong());
        } else if (name == "MemAvailable") {
            this->setProperty("memAvailable", value.split(" ").first().toULongLong());
        } else if (name == "SwapTotal") {
            this->setProperty("swapTotal", value.split(" ").first().toULongLong());
        } else if (name == "SwapFree") {
            this->setProperty("swapFree", value.split(" ").first().toULongLong());
        }
    }

    emit newDataAvailable();
}
