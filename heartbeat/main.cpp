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
#include "mainwindow.h"

#include <tapplication.h>
#include <QMutex>
#include <QTranslator>
#include <QLibraryInfo>

QMutex checkerMutex;
int checkers = 0;

int main(int argc, char *argv[])
{
    tApplication a(argc, argv);

    a.setOrganizationName("theSuite");
    a.setOrganizationDomain("");
    a.setApplicationName("theHeartbeat");

    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name(), QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    a.installTranslator(&qtTranslator);

    QTranslator localTranslator;
    localTranslator.load(QLocale::system().name(), "/usr/share/theheartbeat/translations");
    a.installTranslator(&localTranslator);

    MainWindow w;
    w.show();

    int retval = a.exec();

    while (checkers > 0) {
        a.processEvents();
    }

    return retval;
}
