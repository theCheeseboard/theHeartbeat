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

#include <QDir>
#include <QLibraryInfo>
#include <QMutex>
#include <QTranslator>
#include <tapplication.h>

QMutex checkerMutex;
int checkers = 0;

int main(int argc, char* argv[]) {
    tApplication a(argc, argv);
    a.setApplicationShareDir("theheartbeat");
    a.installTranslators();

    a.setOrganizationName("theSuite");
    a.setOrganizationDomain("vicr123.com");
    a.setApplicationVersion("2.0");
    a.setGenericName(QApplication::translate("main", "System Monitor"));
    a.setApplicationLicense(tApplication::Gpl3OrLater);
    a.setCopyrightHolder("Victor Tran");
    a.setCopyrightYear("2023");
    a.setApplicationUrl(tApplication::Sources, QUrl("https://github.com/vicr123/theheartbeat"));
    a.setApplicationUrl(tApplication::FileBug, QUrl("https://github.com/vicr123/theheartbeat/issues"));
    a.setApplicationName(T_APPMETA_READABLE_NAME);
    a.setDesktopFileName(T_APPMETA_DESKTOP_ID);

    MainWindow w;
    w.show();

    int retval = a.exec();

    while (checkers > 0) {
        a.processEvents();
    }

    return retval;
}
