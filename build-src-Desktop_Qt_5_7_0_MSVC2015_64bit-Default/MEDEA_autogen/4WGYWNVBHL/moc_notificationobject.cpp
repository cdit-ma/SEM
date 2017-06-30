/****************************************************************************
** Meta object code from reading C++ file 'notificationobject.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/Views/Notification/notificationobject.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'notificationobject.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_NotificationObject_t {
    QByteArrayData data[10];
    char stringdata0[112];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_NotificationObject_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_NotificationObject_t qt_meta_stringdata_NotificationObject = {
    {
QT_MOC_LITERAL(0, 0, 18), // "NotificationObject"
QT_MOC_LITERAL(1, 19, 12), // "titleChanged"
QT_MOC_LITERAL(2, 32, 0), // ""
QT_MOC_LITERAL(3, 33, 5), // "title"
QT_MOC_LITERAL(4, 39, 18), // "descriptionChanged"
QT_MOC_LITERAL(5, 58, 11), // "description"
QT_MOC_LITERAL(6, 70, 15), // "iconPathChanged"
QT_MOC_LITERAL(7, 86, 4), // "path"
QT_MOC_LITERAL(8, 91, 15), // "iconNameChanged"
QT_MOC_LITERAL(9, 107, 4) // "name"

    },
    "NotificationObject\0titleChanged\0\0title\0"
    "descriptionChanged\0description\0"
    "iconPathChanged\0path\0iconNameChanged\0"
    "name"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_NotificationObject[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   34,    2, 0x06 /* Public */,
       4,    1,   37,    2, 0x06 /* Public */,
       6,    1,   40,    2, 0x06 /* Public */,
       8,    1,   43,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void, QMetaType::QString,    7,
    QMetaType::Void, QMetaType::QString,    9,

       0        // eod
};

void NotificationObject::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        NotificationObject *_t = static_cast<NotificationObject *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->titleChanged((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: _t->descriptionChanged((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: _t->iconPathChanged((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 3: _t->iconNameChanged((*reinterpret_cast< QString(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (NotificationObject::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NotificationObject::titleChanged)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (NotificationObject::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NotificationObject::descriptionChanged)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (NotificationObject::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NotificationObject::iconPathChanged)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (NotificationObject::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NotificationObject::iconNameChanged)) {
                *result = 3;
                return;
            }
        }
    }
}

const QMetaObject NotificationObject::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_NotificationObject.data,
      qt_meta_data_NotificationObject,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *NotificationObject::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *NotificationObject::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_NotificationObject.stringdata0))
        return static_cast<void*>(const_cast< NotificationObject*>(this));
    return QObject::qt_metacast(_clname);
}

int NotificationObject::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void NotificationObject::titleChanged(QString _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void NotificationObject::descriptionChanged(QString _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void NotificationObject::iconPathChanged(QString _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void NotificationObject::iconNameChanged(QString _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_END_MOC_NAMESPACE
