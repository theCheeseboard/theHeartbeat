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
#include "processmodel.h"

#include "process.h"
#include "processmanager.h"
#include <QMutexLocker>
#include <QSize>
#include <libcontemporary_global.h>

ProcessModel::ProcessModel(ProcessManager* pm, ModelType t, QObject* parent) :
    QAbstractTableModel(parent) {
    this->pm = pm;

    sortTimer = new QTimer();
    sortTimer->setInterval(100);
    sortTimer->setSingleShot(true);
    connect(sortTimer, &QTimer::timeout, this, [this] {
        performSort();
    });

    setModelType(t);
    connect(pm, SIGNAL(newPid(int)), this, SLOT(newPid(int)));
}

QVariant ProcessModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role == Qt::DisplayRole) {
        switch (section) {
            case Name:
                return tr("Name");
            case CPU:
                return tr("CPU %");
            case Memory:
                return tr("Memory");
            case Pid:
                return tr("PID");
        }
    } else if (role == Qt::SizeHintRole) {
        return QSize(500, 29);
    }
    return QAbstractTableModel::headerData(section, orientation, role);
}

int ProcessModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) {
        return 0;
    }

    return shownProcesses.count();
}

int ProcessModel::columnCount(const QModelIndex& parent) const {
    if (parent.isValid()) {
        return 0;
    }

    return 4;
}

QVariant ProcessModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid()) {
        return QVariant();
    }

    QLocale locale;
    Process* p = shownProcesses.value(index.row());

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
            case Name:
                return getProcessDisplayName(p);
            case CPU:
                {
                    double cpuUsage = p->property("cpuUsage").toDouble();
                    if (cpuUsage == 0) {
                        return "";
                    } else {
                        return locale.toString(cpuUsage * 100, 'f', 1) + "%";
                    }
                }
            case Memory:
                {
                    qulonglong val = 0;
                    if (type == Applications) {
                        val = p->property("totalX11PrivateMem").toULongLong();
                    } else if (type == Processes) {
                        val = p->property("privateMem").toULongLong();
                    }
                    if (val < 1024) {
                        return tr("%1 KiB").arg(locale.toString((double) val, 'f', 1));
                    } else if (val < 1048576) {
                        return tr("%1 MiB").arg(locale.toString((double) val / 1024, 'f', 1));
                    } else { /* (val < 1073741824) */
                        return tr("%1 GiB").arg(locale.toString((double) val / 1048576, 'f', 1));
                    }
                }
            case Pid:
                return QString::number(p->property("pid").toInt()); // Use QString::number here because it doesn't depend on the locale ;)
        }
    } else if (role == Qt::DecorationRole) {
        if (index.column() == Name) {
            return p->property("x11-icon");
        }
    } else if (role == Qt::UserRole) {
        return QVariant::fromValue(p);
    } else if (role == Qt::TextAlignmentRole) {
        switch (index.column()) {
            case Name:
                return Qt::AlignLeft;
            case CPU:
                return Qt::AlignHCenter;
            case Memory:
                return Qt::AlignRight;
            case Pid:
                return Qt::AlignLeft;
        }
    } else if (role == Qt::UserRole + 1) {
        QString status = p->property("status").toString();
        if (status == "running" || status == "sleeping" || status == "idle") {
            return "";
        } else if (status == "disk sleep") {
            return tr("Disk Sleep");
        } else if (status == "debugging") {
            return tr("Debugging");
        } else if (status == "stopped") {
            return tr("Stopped");
        } else if (status == "zombie") {
            return tr("Zombie");
        } else if (status == "dead") {
            return tr("Dead");
        } else {
            return tr("Unknown");
        }
    }

    return QVariant();
}

void ProcessModel::setModelType(ModelType type) {
    this->type = type;
    loadProcesses();
}

void ProcessModel::loadProcesses() {
    for (Process* p : shownProcesses) {
        disconnect(p, 0, this, 0);
    }
    shownProcesses.clear();

    for (int pid : pm->availablePids()) {
        Process* p = pm->processByPid(pid);
        connect(p, &Process::propertiesChanged, this, &ProcessModel::processPropertiesChanged);
        checkProcessForSetup(p);
    }
}

void ProcessModel::processPropertiesChanged(Process* p) {
    if (shownProcesses.contains(p)) {
        performSort();
        // emit dataChanged(index(shownProcesses.indexOf(p), 0), index(shownProcesses.indexOf(p), columnCount()));
    } else {
        checkProcessForSetup(p);
    }
}

bool ProcessModel::checkProcessEligibility(Process* p) {
    switch (type) {
        case Applications:
            return p->property("x11-window").isValid();
        case Processes:
            return true;
    }
    return false;
}

void ProcessModel::setupProcess(Process* p) {
    this->beginInsertRows(QModelIndex(), rowCount() - 1, rowCount() - 1);
    shownProcesses.append(p);
    connect(p, SIGNAL(processGone(Process*)), this, SLOT(processGone(Process*)));
    this->endInsertRows();

    if (sortTimer->isActive()) {
        sortTimer->stop();
    }
    sortTimer->start();
}

void ProcessModel::processGone(Process* p) {
    this->beginRemoveRows(QModelIndex(), shownProcesses.indexOf(p), shownProcesses.indexOf(p));
    shownProcesses.removeOne(p);
    this->endRemoveRows();
}

void ProcessModel::checkProcessForSetup(Process* p) {
    if (!shownProcesses.contains(p) && p != nullptr && checkProcessEligibility(p)) {
        setupProcess(p);
    }
}

void ProcessModel::sort(int column, Qt::SortOrder order) {
    sortColumn = column;
    sortOrder = order;
    performSort();
}

void ProcessModel::performSort() {
    if (sortColumn == -1 || !performSorting) return;

    std::stable_sort(shownProcesses.begin(), shownProcesses.end(), [this](const Process* a, const Process* b) -> bool {
        if (a == nullptr || b == nullptr) return false;
        // Check if a < b

        qlonglong val1, val2;

        switch (sortColumn) {
            case Name:
                val1 = getProcessDisplayName(a).toLower().localeAwareCompare(getProcessDisplayName(b).toLower());
                val2 = 0;
                break;
            case CPU:
                val1 = a->property("cpuUsage").toDouble();
                val2 = b->property("cpuUsage").toDouble();
                break;
            case Memory:
                if (type == Applications) {
                    val1 = a->property("totalX11PrivateMem").toULongLong();
                    val2 = b->property("totalX11PrivateMem").toULongLong();
                } else {
                    val1 = a->property("privateMem").toULongLong();
                    val2 = b->property("privateMem").toULongLong();
                }
                break;
            case Pid:
                val1 = a->property("pid").toInt();
                val2 = b->property("pid").toInt();
                break;
        }

        bool retVal;
        if (val1 == val2) {
            // Compare on PIDs: they should never be the same
            retVal = a->property("pid").toInt() < b->property("pid").toInt();
        } else if (val1 < val2) {
            retVal = true;
        } else {
            retVal = false;
        }

        if (sortOrder == Qt::DescendingOrder) {
            return !retVal;
        } else {
            return retVal;
        }
    });

    emit dataChanged(index(0, 0), index(rowCount(), columnCount()));
}

QString ProcessModel::getProcessDisplayName(const Process* p) const {
    if (type == Applications) {
        if (p->property("exe").toString().endsWith("/theshell")) {
            return "theShell";
        } else if (p->property("exe").toString().endsWith("/theshellb")) {
            return "theShell Blueprint";
        } else if (p->property("exe").toString().endsWith("/thedesk")) {
            return "theDesk";
        } else {
            return p->property("x11-windowtitle").toString();
        }
    } else if (type == Processes) {
        return p->property("process").toString();
    }

    return "";
}

void ProcessModel::newPid(int pid) {
    Process* p = pm->processByPid(pid);
    connect(p, &Process::propertiesChanged, this, &ProcessModel::processPropertiesChanged);
    checkProcessForSetup(p);
}

void ProcessModel::setPerformSorting(bool performSorting) {
    this->performSorting = performSorting;
    if (performSorting) {
        performSort();
    }
}

ProcessTitleDelegate::ProcessTitleDelegate(QObject* parent) :
    QStyledItemDelegate(parent) {
}

void ProcessTitleDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    QPen transientColor = option.palette.color(QPalette::Disabled, QPalette::WindowText);

    painter->setPen(Qt::transparent);
    QPen textPen;
    if (option.state & QStyle::State_Selected) {
        painter->setBrush(option.palette.brush(QPalette::Highlight));
        textPen = option.palette.color(QPalette::HighlightedText);
        transientColor = textPen;
    } else if (option.state & QStyle::State_MouseOver) {
        QColor col = option.palette.color(QPalette::Highlight);
        col.setAlpha(127);
        painter->setBrush(col);
        textPen = option.palette.color(QPalette::HighlightedText);
    } else {
        if (index.row() % 2 == 0) {
            painter->setBrush(option.palette.brush(QPalette::Base));
        } else {
            painter->setBrush(option.palette.brush(QPalette::AlternateBase));
        }
        textPen = option.palette.color(QPalette::WindowText);
    }
    painter->drawRect(option.rect);

    QRect iconRect, textRect = option.rect;

    iconRect.setSize(SC_DPI_T(QSize(16, 16), QSize));
    QPixmap icon = index.data(Qt::DecorationRole).value<QPixmap>();

    if (option.direction == Qt::LeftToRight) {
        iconRect.moveLeft(option.rect.left() + SC_DPI(2));
        textRect.setLeft(iconRect.right() + SC_DPI(6));
    } else {
        iconRect.moveRight(option.rect.right() - SC_DPI(2));
        textRect.setRight(iconRect.left() - SC_DPI(6));
    }
    iconRect.moveTop(option.rect.top() + (option.rect.height() / 2) - (iconRect.height() / 2));
    painter->drawPixmap(iconRect, icon);

    // Draw the process name
    QRect nameRect = textRect;
    painter->setPen(option.palette.color(QPalette::WindowText));
    nameRect.setWidth(option.fontMetrics.horizontalAdvance(index.data().toString()) + 1);
    if (option.direction == Qt::LeftToRight) {
        textRect.setLeft(nameRect.right() + SC_DPI(6));
    } else {
        nameRect.moveRight(textRect.right());
        textRect.setRight(nameRect.left() - SC_DPI(6));
    }

    if ((option.direction == Qt::LeftToRight && nameRect.right() > option.rect.right()) ||
        (option.direction == Qt::RightToLeft && nameRect.left() < option.rect.left())) {
        // We need to squish the text
        painter->save();

        int availableSpace = option.direction == Qt::LeftToRight ? (option.rect.right() - nameRect.left()) : (nameRect.right() - option.rect.left());
        int requestedSpace = nameRect.width();

        qreal scaleFactor = static_cast<qreal>(availableSpace) / requestedSpace;
        painter->scale(scaleFactor, 1);

        if (option.direction == Qt::LeftToRight) {
            nameRect.moveLeft(nameRect.left() / scaleFactor);
        } else {
            nameRect.moveRight(nameRect.right() / scaleFactor);
        }

        painter->drawText(nameRect, Qt::AlignLeft | Qt::AlignVCenter, index.data().toString());
        painter->restore();
    } else {
        painter->drawText(nameRect, Qt::AlignLeft | Qt::AlignVCenter, index.data().toString());
    }

    // Draw the process status
    if (index.data(Qt::UserRole + 1).toString() != "") {
        painter->setPen(transientColor);
        painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, "· " + index.data(Qt::UserRole + 1).toString());
    }
}
