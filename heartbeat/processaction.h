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
#ifndef PROCESSACTION_H
#define PROCESSACTION_H

#include <QWidget>


namespace Ui {
    class ProcessAction;
}

class Process;
class ProcessAction : public QWidget
{
        Q_OBJECT

    public:
        explicit ProcessAction(QWidget *parent = nullptr);
        ~ProcessAction();

    public slots:
        void setTitle(QString title);
        void setText(QString text);
        void addProcess(Process* p);

    signals:
        void dismiss();
        void accept();

    private slots:
        void on_backButton_clicked();

        void on_acceptButton_clicked();

    private:
        Ui::ProcessAction *ui;
};

#endif // PROCESSACTION_H
