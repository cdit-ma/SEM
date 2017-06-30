/****************************************************************************
** Meta object code from reading C++ file 'notificationtoolbar.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/Views/Notification/notificationtoolbar.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'notificationtoolbar.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_NotificationToolbar_t {
    QByteArrayData data[14];
    char stringdata0[206];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_NotificationToolbar_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_NotificationToolbar_t qt_meta_stringdata_NotificationToolbar = {
    {
QT_MOC_LITERAL(0, 0, 19), // "NotificationToolbar"
QT_MOC_LITERAL(1, 20, 12), // "toggleDialog"
QT_MOC_LITERAL(2, 33, 0), // ""
QT_MOC_LITERAL(3, 34, 12), // "themeChanged"
QT_MOC_LITERAL(4, 47, 20), // "notificationReceived"
QT_MOC_LITERAL(5, 68, 17), // "notificationsSeen"
QT_MOC_LITERAL(6, 86, 23), // "lastNotificationDeleted"
QT_MOC_LITERAL(7, 110, 17), // "displayLoadingGif"
QT_MOC_LITERAL(8, 128, 4), // "show"
QT_MOC_LITERAL(9, 133, 15), // "updateIconFrame"
QT_MOC_LITERAL(10, 149, 19), // "updateSeverityCount"
QT_MOC_LITERAL(11, 169, 21), // "NOTIFICATION_SEVERITY"
QT_MOC_LITERAL(12, 191, 8), // "severity"
QT_MOC_LITERAL(13, 200, 5) // "count"

    },
    "NotificationToolbar\0toggleDialog\0\0"
    "themeChanged\0notificationReceived\0"
    "notificationsSeen\0lastNotificationDeleted\0"
    "displayLoadingGif\0show\0updateIconFrame\0"
    "updateSeverityCount\0NOTIFICATION_SEVERITY\0"
    "severity\0count"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_NotificationToolbar[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   59,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    0,   60,    2, 0x0a /* Public */,
       4,    0,   61,    2, 0x0a /* Public */,
       5,    0,   62,    2, 0x0a /* Public */,
       6,    0,   63,    2, 0x0a /* Public */,
       7,    1,   64,    2, 0x0a /* Public */,
       7,    0,   67,    2, 0x2a /* Public | MethodCloned */,
       9,    1,   68,    2, 0x0a /* Public */,
      10,    2,   71,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    8,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, 0x80000000 | 11, QMetaType::Int,   12,   13,

       0        // eod
};

void NotificationToolbar::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        NotificationToolbar *_t = static_cast<NotificationToolbar *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->toggleDialog(); break;
        case 1: _t->themeChanged(); break;
        case 2: _t->notificationReceived(); break;
        case 3: _t->notificationsSeen(); break;
        case 4: _t->lastNotificationDeleted(); break;
        case 5: _t->displayLoadingGif((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 6: _t->displayLoadingGif(); break;
        case 7: _t->updateIconFrame((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: _t->updateSeverityCount((*reinterpret_cast< NOTIFICATION_SEVERITY(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 8:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< NOTIFICATION_SEVERITY >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (NotificationToolbar::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NotificationToolbar::toggleDialog)) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject NotificationToolbar::staticMetaObject = {
    { &QToolBar::staticMetaObject, qt_meta_stringdata_NotificationToolbar.data,
      qt_meta_data_NotificationToolbar,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *NotificationToolbar::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *NotificationToolbar::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_NotificationToolbar.stringdata0))
        return static_cast<void*>(const_cast< NotificationToolbar*>(this));
    return QToolBar::qt_metacast(_clname);
}

int NotificationToolbar::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QToolBar::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void NotificationToolbar::toggleDialog()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
