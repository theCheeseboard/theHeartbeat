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
#include <QDir>

QMutex checkerMutex;
int checkers = 0;

int main(int argc, char *argv[])
{
    tApplication a(argc, argv);

    a.installTranslators();

    a.setOrganizationName("theSuite");
    a.setOrganizationDomain("vicr123.com");
    a.setDesktopFileName("com.vicr123.theheartbeat");
    a.setApplicationIcon(QIcon::fromTheme("theheartbeat", QIcon::fromTheme("utilities-system-monitor", QIcon(":/icons/icon.svg"))));
    a.setApplicationVersion("1.0");
    a.setGenericName(QApplication::translate("main", "System Monitor"));
    a.setAboutDialogSplashGraphic(a.aboutDialogSplashGraphicFromSvg(":/icons/aboutsplash.svg"));
    a.setApplicationLicense(tApplication::Gpl3OrLater);
    a.setCopyrightHolder("Victor Tran");
    a.setCopyrightYear("2019");
    #ifdef T_BLUEPRINT_BUILD
        a.setApplicationName("theHeartbeat Blueprint");
    #else
        a.setApplicationName("theHeartbeat");
    #endif
    if (QDir("/usr/share/theheartbeat").exists()) {
        a.setShareDir("/usr/share/theheartbeat");
    } else if (QDir(QDir::cleanPath(QApplication::applicationDirPath() + "/../share/theheartbeat/")).exists()) {
        a.setShareDir(QDir::cleanPath(QApplication::applicationDirPath() + "/../share/theheartbeat/"));
    }

    MainWindow w;
    w.show();

    int retval = a.exec();

    while (checkers > 0) {
        a.processEvents();
    }

    return retval;
}
