/****************************************************************************
** Meta object code from reading C++ file 'notificationmanager.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/Controllers/NotificationManager/notificationmanager.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'notificationmanager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_NotificationManager_t {
    QByteArrayData data[31];
    char stringdata0[475];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_NotificationManager_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_NotificationManager_t qt_meta_stringdata_NotificationManager = {
    {
QT_MOC_LITERAL(0, 0, 19), // "NotificationManager"
QT_MOC_LITERAL(1, 20, 17), // "notificationAlert"
QT_MOC_LITERAL(2, 38, 0), // ""
QT_MOC_LITERAL(3, 39, 16), // "notificationSeen"
QT_MOC_LITERAL(4, 56, 17), // "notificationAdded"
QT_MOC_LITERAL(5, 74, 8), // "iconPath"
QT_MOC_LITERAL(6, 83, 8), // "iconName"
QT_MOC_LITERAL(7, 92, 11), // "description"
QT_MOC_LITERAL(8, 104, 21), // "notificationItemAdded"
QT_MOC_LITERAL(9, 126, 19), // "NotificationObject*"
QT_MOC_LITERAL(10, 146, 3), // "obj"
QT_MOC_LITERAL(11, 150, 19), // "notificationDeleted"
QT_MOC_LITERAL(12, 170, 2), // "ID"
QT_MOC_LITERAL(13, 173, 23), // "lastNotificationDeleted"
QT_MOC_LITERAL(14, 197, 22), // "req_lastNotificationID"
QT_MOC_LITERAL(15, 220, 18), // "clearNotifications"
QT_MOC_LITERAL(16, 239, 19), // "NOTIFICATION_FILTER"
QT_MOC_LITERAL(17, 259, 6), // "filter"
QT_MOC_LITERAL(18, 266, 9), // "filterVal"
QT_MOC_LITERAL(19, 276, 17), // "backgroundProcess"
QT_MOC_LITERAL(20, 294, 10), // "inProgress"
QT_MOC_LITERAL(21, 305, 18), // "BACKGROUND_PROCESS"
QT_MOC_LITERAL(22, 324, 7), // "process"
QT_MOC_LITERAL(23, 332, 21), // "showNotificationPanel"
QT_MOC_LITERAL(24, 354, 4), // "show"
QT_MOC_LITERAL(25, 359, 29), // "updateNotificationToolbarSize"
QT_MOC_LITERAL(26, 389, 18), // "deleteNotification"
QT_MOC_LITERAL(27, 408, 23), // "setLastNotificationItem"
QT_MOC_LITERAL(28, 432, 20), // "showLastNotification"
QT_MOC_LITERAL(29, 453, 14), // "modelValidated"
QT_MOC_LITERAL(30, 468, 6) // "report"

    },
    "NotificationManager\0notificationAlert\0"
    "\0notificationSeen\0notificationAdded\0"
    "iconPath\0iconName\0description\0"
    "notificationItemAdded\0NotificationObject*\0"
    "obj\0notificationDeleted\0ID\0"
    "lastNotificationDeleted\0req_lastNotificationID\0"
    "clearNotifications\0NOTIFICATION_FILTER\0"
    "filter\0filterVal\0backgroundProcess\0"
    "inProgress\0BACKGROUND_PROCESS\0process\0"
    "showNotificationPanel\0show\0"
    "updateNotificationToolbarSize\0"
    "deleteNotification\0setLastNotificationItem\0"
    "showLastNotification\0modelValidated\0"
    "report"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_NotificationManager[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      19,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      15,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,  109,    2, 0x06 /* Public */,
       3,    0,  110,    2, 0x06 /* Public */,
       4,    3,  111,    2, 0x06 /* Public */,
       8,    1,  118,    2, 0x06 /* Public */,
      11,    1,  121,    2, 0x06 /* Public */,
      13,    0,  124,    2, 0x06 /* Public */,
      14,    0,  125,    2, 0x06 /* Public */,
      15,    2,  126,    2, 0x06 /* Public */,
      15,    1,  131,    2, 0x26 /* Public | MethodCloned */,
      15,    0,  134,    2, 0x26 /* Public | MethodCloned */,
      19,    2,  135,    2, 0x06 /* Public */,
      19,    1,  140,    2, 0x26 /* Public | MethodCloned */,
      23,    1,  143,    2, 0x06 /* Public */,
      23,    0,  146,    2, 0x26 /* Public | MethodCloned */,
      25,    0,  147,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      26,    1,  148,    2, 0x0a /* Public */,
      27,    1,  151,    2, 0x0a /* Public */,
      28,    0,  154,    2, 0x0a /* Public */,
      29,    1,  155,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::QString,    5,    6,    7,
    QMetaType::Void, 0x80000000 | 9,   10,
    QMetaType::Void, QMetaType::Int,   12,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 16, QMetaType::Int,   17,   18,
    QMetaType::Void, 0x80000000 | 16,   17,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool, 0x80000000 | 21,   20,   22,
    QMetaType::Void, QMetaType::Bool,   20,
    QMetaType::Void, QMetaType::Bool,   24,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,   12,
    QMetaType::Void, QMetaType::Int,   12,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   30,

       0        // eod
};

void NotificationManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        NotificationManager *_t = static_cast<NotificationManager *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->notificationAlert(); break;
        case 1: _t->notificationSeen(); break;
        case 2: _t->notificationAdded((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3]))); break;
        case 3: _t->notificationItemAdded((*reinterpret_cast< NotificationObject*(*)>(_a[1]))); break;
        case 4: _t->notificationDeleted((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->lastNotificationDeleted(); break;
        case 6: _t->req_lastNotificationID(); break;
        case 7: _t->clearNotifications((*reinterpret_cast< NOTIFICATION_FILTER(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 8: _t->clearNotifications((*reinterpret_cast< NOTIFICATION_FILTER(*)>(_a[1]))); break;
        case 9: _t->clearNotifications(); break;
        case 10: _t->backgroundProcess((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< BACKGROUND_PROCESS(*)>(_a[2]))); break;
        case 11: _t->backgroundProcess((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 12: _t->showNotificationPanel((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 13: _t->showNotificationPanel(); break;
        case 14: _t->updateNotificationToolbarSize(); break;
        case 15: _t->deleteNotification((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 16: _t->setLastNotificationItem((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 17: _t->showLastNotification(); break;
        case 18: _t->modelValidated((*reinterpret_cast< QString(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (NotificationManager::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NotificationManager::notificationAlert)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (NotificationManager::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NotificationManager::notificationSeen)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (NotificationManager::*_t)(QString , QString , QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NotificationManager::notificationAdded)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (NotificationManager::*_t)(NotificationObject * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NotificationManager::notificationItemAdded)) {
                *result = 3;
                return;
            }
        }
        {
            typedef void (NotificationManager::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NotificationManager::notificationDeleted)) {
                *result = 4;
                return;
            }
        }
        {
            typedef void (NotificationManager::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NotificationManager::lastNotificationDeleted)) {
                *result = 5;
                return;
            }
        }
        {
            typedef void (NotificationManager::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NotificationManager::req_lastNotificationID)) {
                *result = 6;
                return;
            }
        }
        {
            typedef void (NotificationManager::*_t)(NOTIFICATION_FILTER , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NotificationManager::clearNotifications)) {
                *result = 7;
                return;
            }
        }
        {
            typedef void (NotificationManager::*_t)(bool , BACKGROUND_PROCESS );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NotificationManager::backgroundProcess)) {
                *result = 10;
                return;
            }
        }
        {
            typedef void (NotificationManager::*_t)(bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NotificationManager::showNotificationPanel)) {
                *result = 12;
                return;
            }
        }
        {
            typedef void (NotificationManager::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NotificationManager::updateNotificationToolbarSize)) {
                *result = 14;
                return;
            }
        }
    }
}

const QMetaObject NotificationManager::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_NotificationManager.data,
      qt_meta_data_NotificationManager,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *NotificationManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *NotificationManager::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_NotificationManager.stringdata0))
        return static_cast<void*>(const_cast< NotificationManager*>(this));
    return QObject::qt_metacast(_clname);
}

int NotificationManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 19)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 19;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 19)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 19;
    }
    return _id;
}

// SIGNAL 0
void NotificationManager::notificationAlert()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}

// SIGNAL 1
void NotificationManager::notificationSeen()
{
    QMetaObject::activate(this, &staticMetaObject, 1, Q_NULLPTR);
}

// SIGNAL 2
void NotificationManager::notificationAdded(QString _t1, QString _t2, QString _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void NotificationManager::notificationItemAdded(NotificationObject * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void NotificationManager::notificationDeleted(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void NotificationManager::lastNotificationDeleted()
{
    QMetaObject::activate(this, &staticMetaObject, 5, Q_NULLPTR);
}

// SIGNAL 6
void NotificationManager::req_lastNotificationID()
{
    QMetaObject::activate(this, &staticMetaObject, 6, Q_NULLPTR);
}

// SIGNAL 7
void NotificationManager::clearNotifications(NOTIFICATION_FILTER _t1, int _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 10
void NotificationManager::backgroundProcess(bool _t1, BACKGROUND_PROCESS _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 10, _a);
}

// SIGNAL 12
void NotificationManager::showNotificationPanel(bool _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 12, _a);
}

// SIGNAL 14
void NotificationManager::updateNotificationToolbarSize()
{
    QMetaObject::activate(this, &staticMetaObject, 14, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
