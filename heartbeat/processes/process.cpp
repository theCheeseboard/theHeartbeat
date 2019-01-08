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

#include "process.h"

#include <QFile>
#include <QDir>
#include <QVariant>
#include <QX11Info>
#include <QImage>
#include <QPixmap>
#include <QMutex>
#include <QMutexLocker>

#include <X11/Xlib.h>
#include <X11/Xatom.h>

struct ProcessPrivate {
    int pid;
    QMutex updateLocker;
};

Process::Process(int pid, QObject *parent) : QObject(parent)
{
    d = new ProcessPrivate();

    d->pid = pid;
    this->setProperty("pid", d->pid);
}

Process::~Process() {
    delete d;
}

void Process::performUpdate() {
    if (!d->updateLocker.tryLock()) {
        return;
    }

    d->updateLocker.unlock();
    QMutexLocker locker(&d->updateLocker);
    if (!QDir(QString("/proc/%1").arg(QString::number(d->pid))).exists()) {
        //Tell everyone we're gone
        emit processGone(this);
        this->deleteLater();
        return;
    }

    qulonglong sharedMem = 0, totalMem = 0;
    QString status = readFile("status");
    for (QString line : status.split("\n")) {
        QStringList splits = line.split(":");
        QString name = splits.first().trimmed();
        QString value = splits.last().trimmed();
        if (name == "Name") {
            this->setProperty("process", value);
        } else if (name == "VmRSS") {
            //sharedMem = value.split(" ").first().toULongLong();
            totalMem = value.split(" ").first().toULongLong();
        } else if (name == "RssFile") {
            sharedMem += value.split(" ").first().toULongLong();
        } else if (name == "RssShmem") {
            sharedMem += value.split(" ").first().toULongLong();
        } else if (name == "State") {
            switch (value.at(0).toLatin1()) {
                case 'R':
                    this->setProperty("status", "running");
                    break;
                case 'S':
                    this->setProperty("status", "sleeping");
                    break;
                case 'D':
                    this->setProperty("status", "disk sleep");
                    break;
                case 'T':
                    if (value.contains("tracing")) {
                        this->setProperty("status", "debugging");
                    } else {
                        this->setProperty("status", "stopped");
                    }
                    break;
                case 'Z':
                    this->setProperty("status", "zombie");
                    break;
                case 'X':
                    this->setProperty("status", "dead");
                    break;
                default:
                    this->setProperty("status", "unknown");
            }
        } else if (name == "PPid") {
            this->setProperty("parent", value.toInt());
        } else if (name == "TracerPid") {
            this->setProperty("tracer", value.toInt());
        } else if (name == "Uid") {
            QStringList parts = value.split("\t", QString::SkipEmptyParts);
            this->setProperty("uid", parts.at(0).toInt());
            this->setProperty("effectiveUid", parts.at(1).toInt());
            this->setProperty("savedUid", parts.at(2).toInt());
            this->setProperty("fsUid", parts.at(3).toInt());
        }
    }

    this->setProperty("sharedMem", sharedMem);
    this->setProperty("privateMem", totalMem - sharedMem);

    Atom WindowListType;
    int format;
    unsigned long items, bytes;
    unsigned char *data;
    XGetWindowProperty(QX11Info::display(), DefaultRootWindow(QX11Info::display()), XInternAtom(QX11Info::display(), "_NET_CLIENT_LIST", true), 0L, (~0L),
                                    False, AnyPropertyType, &WindowListType, &format, &items, &bytes, &data);

    bool foundWindow = false;
    quint64 *windows = (quint64*) data;
    for (unsigned int i = 0; i < items; i++) {
        Window win = windows[i];

        int ok;
        unsigned long items, bytes;
        unsigned char *returnVal;
        int format;
        Atom ReturnType;

        ok = XGetWindowProperty(QX11Info::display(), win, XInternAtom(QX11Info::display(), "_NET_WM_PID", False), 0, 1024, False,
                                XA_CARDINAL, &ReturnType, &format, &items, &bytes, &returnVal);
        if (ok == 0 && returnVal != 0x0) {
            unsigned long pid = *(unsigned long*) returnVal;

            if (pid == d->pid) {
                this->setProperty("x11-window", (qulonglong) win);
                foundWindow = true;
            }

            XFree(returnVal);
        }
    }
    XFree(data);

    if (foundWindow) {
        Window window = this->property("x11-window").toULongLong();

        int ok;
        unsigned long items, bytes;
        unsigned char *returnVal;
        int format;
        Atom ReturnType;

        { //Obtain the window title
            ok = XGetWindowProperty(QX11Info::display(), window, XInternAtom(QX11Info::display(), "_NET_WM_NAME", False), 0, 1024, False,
                               XInternAtom(QX11Info::display(), "UTF8_STRING", False), &ReturnType, &format, &items, &bytes, &returnVal);

            if (returnVal == nullptr) {
                ok = XGetWindowProperty(QX11Info::display(), window, XInternAtom(QX11Info::display(), "WM_NAME", False), 0, 1024, False,
                                   XInternAtom(QX11Info::display(), "UTF8_STRING", False), &ReturnType, &format, &items, &bytes, &returnVal);
            }

            if (ok == 0 && returnVal != nullptr) {
                this->setProperty("x11-windowtitle", QString::fromUtf8((char*) returnVal));
                XFree(returnVal);
            }
        }

        { //Obtain the window icon
            bool noIcon = false;
            int width, height;

            ok = XGetWindowProperty(QX11Info::display(), window, XInternAtom(QX11Info::display(), "_NET_WM_ICON", False), 0, 1, False,
                               XA_CARDINAL, &ReturnType, &format, &items, &bytes, &returnVal);
            if (returnVal == nullptr) {
                noIcon = true;
            } else {
                width = *(int*) returnVal;
                XFree(returnVal);
            }

            ok = XGetWindowProperty(QX11Info::display(), window, XInternAtom(QX11Info::display(), "_NET_WM_ICON", False), 1, 1, False,
                               XA_CARDINAL, &ReturnType, &format, &items, &bytes, &returnVal);

            if (returnVal == nullptr) {
                noIcon = true;
            } else {
                height = *(int*) returnVal;
                XFree(returnVal);
            }

            if (!noIcon) {
                ok = XGetWindowProperty(QX11Info::display(), window, XInternAtom(QX11Info::display(), "_NET_WM_ICON", False), 2, width * height * 4, False,
                                   XA_CARDINAL, &ReturnType, &format, &items, &bytes, &returnVal);

                if (returnVal != nullptr) {
                    QImage image(16, 16, QImage::Format_ARGB32);

                    float widthSpacing = (float) width / (float) 16;
                    float heightSpacing = (float) height / (float) 16;

                    for (int y = 0; y < 16; y++) {
                        for (int x = 0; x < 16 * 8; x = x + 8) {
                            unsigned long a, r, g, b;

                            b = (returnVal[(int) (y * heightSpacing * width * 8 + x * widthSpacing + 0)]);
                            g = (returnVal[(int) (y * heightSpacing * width * 8 + x * widthSpacing + 1)]);
                            r = (returnVal[(int) (y * heightSpacing * width * 8 + x * widthSpacing + 2)]);
                            a = (returnVal[(int) (y * heightSpacing * width * 8 + x * widthSpacing + 3)]);

                            QColor col = QColor(r, g, b, a);

                            image.setPixelColor(x / 8, y, col);
                        }
                    }

                    QPixmap iconPixmap(QPixmap::fromImage(image).scaled(16, 16, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
                    this->setProperty("x11-icon", iconPixmap);

                    XFree(returnVal);
                }
            }
        }
    } else {
        this->setProperty("x11-window", QVariant());
    }

    emit propertiesChanged(this);
}

QString Process::readFile(QString file) {
    QFile f(QString("/proc/%1/%2").arg(QString::number(d->pid), file));
    f.open(QFile::ReadOnly);
    return f.readAll();
}

QString Process::readLink(QString link) {
    QFile f(QString("/proc/%1/%2").arg(QString::number(d->pid), link));
    return f.symLinkTarget();
}
