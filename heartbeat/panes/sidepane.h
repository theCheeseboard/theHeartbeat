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
#ifndef SIDEPANE_H
#define SIDEPANE_H

#include <QObject>
#include <QWidget>

class SidePane : public QWidget
{
        Q_OBJECT
    public:
        enum Unit {
            Kilobyte,
            KilobytePerSecond,
            MillidegreeCelsius
        };

        explicit SidePane(QWidget *parent = nullptr);

    signals:

    public slots:

    protected:
        QList<double> percentageHistory;

        QString calculateText(qulonglong value);
        Unit u = Kilobyte;

    private:
        void paintEvent(QPaintEvent* event);
};

#endif // SIDEPANE_H
