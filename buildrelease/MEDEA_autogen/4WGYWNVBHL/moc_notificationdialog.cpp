/****************************************************************************
** Meta object code from reading C++ file 'notificationdialog.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/Views/Notification/notificationdialog.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'notificationdialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_NotificationDialog_t {
    QByteArrayData data[54];
    char stringdata0[787];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_NotificationDialog_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_NotificationDialog_t qt_meta_stringdata_NotificationDialog = {
    {
QT_MOC_LITERAL(0, 0, 18), // "NotificationDialog"
QT_MOC_LITERAL(1, 19, 8), // "centerOn"
QT_MOC_LITERAL(2, 28, 0), // ""
QT_MOC_LITERAL(3, 29, 8), // "entityID"
QT_MOC_LITERAL(4, 38, 5), // "popup"
QT_MOC_LITERAL(5, 44, 18), // "deleteNotification"
QT_MOC_LITERAL(6, 63, 2), // "ID"
QT_MOC_LITERAL(7, 66, 18), // "lastNotificationID"
QT_MOC_LITERAL(8, 85, 19), // "updateSeverityCount"
QT_MOC_LITERAL(9, 105, 21), // "NOTIFICATION_SEVERITY"
QT_MOC_LITERAL(10, 127, 8), // "severity"
QT_MOC_LITERAL(11, 136, 5), // "count"
QT_MOC_LITERAL(12, 142, 12), // "mouseEntered"
QT_MOC_LITERAL(13, 155, 14), // "filtersCleared"
QT_MOC_LITERAL(14, 170, 13), // "filterCleared"
QT_MOC_LITERAL(15, 184, 19), // "NOTIFICATION_FILTER"
QT_MOC_LITERAL(16, 204, 6), // "filter"
QT_MOC_LITERAL(17, 211, 22), // "severityFiltersChanged"
QT_MOC_LITERAL(18, 234, 33), // "QHash<NOTIFICATION_SEVERITY,b..."
QT_MOC_LITERAL(19, 268, 6), // "states"
QT_MOC_LITERAL(20, 275, 18), // "typeFiltersChanged"
QT_MOC_LITERAL(21, 294, 30), // "QHash<NOTIFICATION_TYPE2,bool>"
QT_MOC_LITERAL(22, 325, 22), // "categoryFiltersChanged"
QT_MOC_LITERAL(23, 348, 33), // "QHash<NOTIFICATION_CATEGORY,b..."
QT_MOC_LITERAL(24, 382, 15), // "initialisePanel"
QT_MOC_LITERAL(25, 398, 10), // "resetPanel"
QT_MOC_LITERAL(26, 409, 21), // "getLastNotificationID"
QT_MOC_LITERAL(27, 431, 8), // "testSlot"
QT_MOC_LITERAL(28, 440, 11), // "checkedList"
QT_MOC_LITERAL(29, 452, 12), // "themeChanged"
QT_MOC_LITERAL(30, 465, 19), // "filterMenuTriggered"
QT_MOC_LITERAL(31, 485, 8), // "QAction*"
QT_MOC_LITERAL(32, 494, 6), // "action"
QT_MOC_LITERAL(33, 501, 13), // "filterToggled"
QT_MOC_LITERAL(34, 515, 7), // "checked"
QT_MOC_LITERAL(35, 523, 15), // "updateSelection"
QT_MOC_LITERAL(36, 539, 17), // "NotificationItem*"
QT_MOC_LITERAL(37, 557, 4), // "item"
QT_MOC_LITERAL(38, 562, 8), // "selected"
QT_MOC_LITERAL(39, 571, 11), // "controlDown"
QT_MOC_LITERAL(40, 583, 13), // "viewSelection"
QT_MOC_LITERAL(41, 597, 13), // "clearSelected"
QT_MOC_LITERAL(42, 611, 12), // "clearVisible"
QT_MOC_LITERAL(43, 624, 18), // "clearNotifications"
QT_MOC_LITERAL(44, 643, 9), // "filterVal"
QT_MOC_LITERAL(45, 653, 17), // "notificationAdded"
QT_MOC_LITERAL(46, 671, 19), // "NotificationObject*"
QT_MOC_LITERAL(47, 691, 3), // "obj"
QT_MOC_LITERAL(48, 695, 19), // "notificationDeleted"
QT_MOC_LITERAL(49, 715, 17), // "backgroundProcess"
QT_MOC_LITERAL(50, 733, 10), // "inProgress"
QT_MOC_LITERAL(51, 744, 18), // "BACKGROUND_PROCESS"
QT_MOC_LITERAL(52, 763, 7), // "process"
QT_MOC_LITERAL(53, 771, 15) // "intervalTimeout"

    },
    "NotificationDialog\0centerOn\0\0entityID\0"
    "popup\0deleteNotification\0ID\0"
    "lastNotificationID\0updateSeverityCount\0"
    "NOTIFICATION_SEVERITY\0severity\0count\0"
    "mouseEntered\0filtersCleared\0filterCleared\0"
    "NOTIFICATION_FILTER\0filter\0"
    "severityFiltersChanged\0"
    "QHash<NOTIFICATION_SEVERITY,bool>\0"
    "states\0typeFiltersChanged\0"
    "QHash<NOTIFICATION_TYPE2,bool>\0"
    "categoryFiltersChanged\0"
    "QHash<NOTIFICATION_CATEGORY,bool>\0"
    "initialisePanel\0resetPanel\0"
    "getLastNotificationID\0testSlot\0"
    "checkedList\0themeChanged\0filterMenuTriggered\0"
    "QAction*\0action\0filterToggled\0checked\0"
    "updateSelection\0NotificationItem*\0"
    "item\0selected\0controlDown\0viewSelection\0"
    "clearSelected\0clearVisible\0"
    "clearNotifications\0filterVal\0"
    "notificationAdded\0NotificationObject*\0"
    "obj\0notificationDeleted\0backgroundProcess\0"
    "inProgress\0BACKGROUND_PROCESS\0process\0"
    "intervalTimeout"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_NotificationDialog[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      27,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      11,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,  149,    2, 0x06 /* Public */,
       4,    1,  152,    2, 0x06 /* Public */,
       5,    1,  155,    2, 0x06 /* Public */,
       7,    1,  158,    2, 0x06 /* Public */,
       8,    2,  161,    2, 0x06 /* Public */,
      12,    0,  166,    2, 0x06 /* Public */,
      13,    0,  167,    2, 0x06 /* Public */,
      14,    1,  168,    2, 0x06 /* Public */,
      17,    1,  171,    2, 0x06 /* Public */,
      20,    1,  174,    2, 0x06 /* Public */,
      22,    1,  177,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      24,    0,  180,    2, 0x0a /* Public */,
      25,    0,  181,    2, 0x0a /* Public */,
      26,    0,  182,    2, 0x0a /* Public */,
      27,    1,  183,    2, 0x0a /* Public */,
      29,    0,  186,    2, 0x08 /* Private */,
      30,    1,  187,    2, 0x08 /* Private */,
      33,    1,  190,    2, 0x08 /* Private */,
      35,    3,  193,    2, 0x08 /* Private */,
      40,    0,  200,    2, 0x08 /* Private */,
      41,    0,  201,    2, 0x08 /* Private */,
      42,    0,  202,    2, 0x08 /* Private */,
      43,    2,  203,    2, 0x08 /* Private */,
      45,    1,  208,    2, 0x08 /* Private */,
      48,    1,  211,    2, 0x08 /* Private */,
      49,    2,  214,    2, 0x08 /* Private */,
      53,    0,  219,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Int,    6,
    QMetaType::Void, QMetaType::Int,    6,
    QMetaType::Void, 0x80000000 | 9, QMetaType::Int,   10,   11,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 15,   16,
    QMetaType::Void, 0x80000000 | 18,   19,
    QMetaType::Void, 0x80000000 | 21,   19,
    QMetaType::Void, 0x80000000 | 23,   19,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QStringList,   28,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 31,   32,
    QMetaType::Void, QMetaType::Bool,   34,
    QMetaType::Void, 0x80000000 | 36, QMetaType::Bool, QMetaType::Bool,   37,   38,   39,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 15, QMetaType::Int,   16,   44,
    QMetaType::Void, 0x80000000 | 46,   47,
    QMetaType::Void, QMetaType::Int,    6,
    QMetaType::Void, QMetaType::Bool, 0x80000000 | 51,   50,   52,
    QMetaType::Void,

       0        // eod
};

void NotificationDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        NotificationDialog *_t = static_cast<NotificationDialog *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->centerOn((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->popup((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->deleteNotification((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->lastNotificationID((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->updateSeverityCount((*reinterpret_cast< NOTIFICATION_SEVERITY(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 5: _t->mouseEntered(); break;
        case 6: _t->filtersCleared(); break;
        case 7: _t->filterCleared((*reinterpret_cast< NOTIFICATION_FILTER(*)>(_a[1]))); break;
        case 8: _t->severityFiltersChanged((*reinterpret_cast< QHash<NOTIFICATION_SEVERITY,bool>(*)>(_a[1]))); break;
        case 9: _t->typeFiltersChanged((*reinterpret_cast< QHash<NOTIFICATION_TYPE2,bool>(*)>(_a[1]))); break;
        case 10: _t->categoryFiltersChanged((*reinterpret_cast< QHash<NOTIFICATION_CATEGORY,bool>(*)>(_a[1]))); break;
        case 11: _t->initialisePanel(); break;
        case 12: _t->resetPanel(); break;
        case 13: _t->getLastNotificationID(); break;
        case 14: _t->testSlot((*reinterpret_cast< QStringList(*)>(_a[1]))); break;
        case 15: _t->themeChanged(); break;
        case 16: _t->filterMenuTriggered((*reinterpret_cast< QAction*(*)>(_a[1]))); break;
        case 17: _t->filterToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 18: _t->updateSelection((*reinterpret_cast< NotificationItem*(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 19: _t->viewSelection(); break;
        case 20: _t->clearSelected(); break;
        case 21: _t->clearVisible(); break;
        case 22: _t->clearNotifications((*reinterpret_cast< NOTIFICATION_FILTER(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 23: _t->notificationAdded((*reinterpret_cast< NotificationObject*(*)>(_a[1]))); break;
        case 24: _t->notificationDeleted((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 25: _t->backgroundProcess((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< BACKGROUND_PROCESS(*)>(_a[2]))); break;
        case 26: _t->intervalTimeout(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 16:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QAction* >(); break;
            }
            break;
        case 23:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< NotificationObject* >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (NotificationDialog::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NotificationDialog::centerOn)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (NotificationDialog::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NotificationDialog::popup)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (NotificationDialog::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NotificationDialog::deleteNotification)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (NotificationDialog::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NotificationDialog::lastNotificationID)) {
                *result = 3;
                return;
            }
        }
        {
            typedef void (NotificationDialog::*_t)(NOTIFICATION_SEVERITY , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NotificationDialog::updateSeverityCount)) {
                *result = 4;
                return;
            }
        }
        {
            typedef void (NotificationDialog::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NotificationDialog::mouseEntered)) {
                *result = 5;
                return;
            }
        }
        {
            typedef void (NotificationDialog::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NotificationDialog::filtersCleared)) {
                *result = 6;
                return;
            }
        }
        {
            typedef void (NotificationDialog::*_t)(NOTIFICATION_FILTER );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NotificationDialog::filterCleared)) {
                *result = 7;
                return;
            }
        }
        {
            typedef void (NotificationDialog::*_t)(QHash<NOTIFICATION_SEVERITY,bool> );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NotificationDialog::severityFiltersChanged)) {
                *result = 8;
                return;
            }
        }
        {
            typedef void (NotificationDialog::*_t)(QHash<NOTIFICATION_TYPE2,bool> );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NotificationDialog::typeFiltersChanged)) {
                *result = 9;
                return;
            }
        }
        {
            typedef void (NotificationDialog::*_t)(QHash<NOTIFICATION_CATEGORY,bool> );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NotificationDialog::categoryFiltersChanged)) {
                *result = 10;
                return;
            }
        }
    }
}

const QMetaObject NotificationDialog::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_NotificationDialog.data,
      qt_meta_data_NotificationDialog,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *NotificationDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *NotificationDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_NotificationDialog.stringdata0))
        return static_cast<void*>(const_cast< NotificationDialog*>(this));
    return QWidget::qt_metacast(_clname);
}

int NotificationDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 27)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 27;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 27)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 27;
    }
    return _id;
}

// SIGNAL 0
void NotificationDialog::centerOn(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void NotificationDialog::popup(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void NotificationDialog::deleteNotification(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void NotificationDialog::lastNotificationID(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void NotificationDialog::updateSeverityCount(NOTIFICATION_SEVERITY _t1, int _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void NotificationDialog::mouseEntered()
{
    QMetaObject::activate(this, &staticMetaObject, 5, Q_NULLPTR);
}

// SIGNAL 6
void NotificationDialog::filtersCleared()
{
    QMetaObject::activate(this, &staticMetaObject, 6, Q_NULLPTR);
}

// SIGNAL 7
void NotificationDialog::filterCleared(NOTIFICATION_FILTER _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void NotificationDialog::severityFiltersChanged(QHash<NOTIFICATION_SEVERITY,bool> _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void NotificationDialog::typeFiltersChanged(QHash<NOTIFICATION_TYPE2,bool> _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}

// SIGNAL 10
void NotificationDialog::categoryFiltersChanged(QHash<NOTIFICATION_CATEGORY,bool> _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 10, _a);
}
QT_END_MOC_NAMESPACE
