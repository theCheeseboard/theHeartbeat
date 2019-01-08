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
#ifndef MINMAXPANE_H
#define MINMAXPANE_H

#include <QWidget>

namespace Ui {
    class MinMaxPane;
}

class MinMaxPane : public QWidget
{
        Q_OBJECT

    public:
        explicit MinMaxPane(QWidget *parent = nullptr);
        ~MinMaxPane();

        enum Unit {
            Kilobyte
        };

        void setTitle(QString title);
        void setUnit(Unit u);
        void setMax(qulonglong max);
        void setValue(qulonglong val);

    private:
        Ui::MinMaxPane *ui;

        qulonglong max = 0;
        qulonglong val = 0;

        QList<double> percentageHistory;

        Unit u = Kilobyte;

        QString calculateText(qulonglong value);
        void paintEvent(QPaintEvent* event);
};

#endif // MINMAXPANE_H
