/****************************************************************************
** Meta object code from reading C++ file 'viewitem.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/Controllers/ViewController/viewitem.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'viewitem.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_ViewItem_t {
    QByteArrayData data[16];
    char stringdata0[162];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ViewItem_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ViewItem_t qt_meta_stringdata_ViewItem = {
    {
QT_MOC_LITERAL(0, 0, 8), // "ViewItem"
QT_MOC_LITERAL(1, 9, 12), // "labelChanged"
QT_MOC_LITERAL(2, 22, 0), // ""
QT_MOC_LITERAL(3, 23, 5), // "label"
QT_MOC_LITERAL(4, 29, 11), // "iconChanged"
QT_MOC_LITERAL(5, 41, 9), // "dataAdded"
QT_MOC_LITERAL(6, 51, 7), // "keyName"
QT_MOC_LITERAL(7, 59, 4), // "data"
QT_MOC_LITERAL(8, 64, 11), // "dataChanged"
QT_MOC_LITERAL(9, 76, 11), // "dataRemoved"
QT_MOC_LITERAL(10, 88, 13), // "propertyAdded"
QT_MOC_LITERAL(11, 102, 12), // "propertyName"
QT_MOC_LITERAL(12, 115, 15), // "propertyChanged"
QT_MOC_LITERAL(13, 131, 15), // "propertyRemoved"
QT_MOC_LITERAL(14, 147, 11), // "destructing"
QT_MOC_LITERAL(15, 159, 2) // "ID"

    },
    "ViewItem\0labelChanged\0\0label\0iconChanged\0"
    "dataAdded\0keyName\0data\0dataChanged\0"
    "dataRemoved\0propertyAdded\0propertyName\0"
    "propertyChanged\0propertyRemoved\0"
    "destructing\0ID"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ViewItem[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       9,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   59,    2, 0x06 /* Public */,
       4,    0,   62,    2, 0x06 /* Public */,
       5,    2,   63,    2, 0x06 /* Public */,
       8,    2,   68,    2, 0x06 /* Public */,
       9,    1,   73,    2, 0x06 /* Public */,
      10,    2,   76,    2, 0x06 /* Public */,
      12,    2,   81,    2, 0x06 /* Public */,
      13,    1,   86,    2, 0x06 /* Public */,
      14,    1,   89,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::QVariant,    6,    7,
    QMetaType::Void, QMetaType::QString, QMetaType::QVariant,    6,    7,
    QMetaType::Void, QMetaType::QString,    6,
    QMetaType::Void, QMetaType::QString, QMetaType::QVariant,   11,    7,
    QMetaType::Void, QMetaType::QString, QMetaType::QVariant,   11,    7,
    QMetaType::Void, QMetaType::QString,   11,
    QMetaType::Void, QMetaType::Int,   15,

       0        // eod
};

void ViewItem::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ViewItem *_t = static_cast<ViewItem *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->labelChanged((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: _t->iconChanged(); break;
        case 2: _t->dataAdded((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QVariant(*)>(_a[2]))); break;
        case 3: _t->dataChanged((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QVariant(*)>(_a[2]))); break;
        case 4: _t->dataRemoved((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 5: _t->propertyAdded((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QVariant(*)>(_a[2]))); break;
        case 6: _t->propertyChanged((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QVariant(*)>(_a[2]))); break;
        case 7: _t->propertyRemoved((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 8: _t->destructing((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (ViewItem::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewItem::labelChanged)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (ViewItem::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewItem::iconChanged)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (ViewItem::*_t)(QString , QVariant );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewItem::dataAdded)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (ViewItem::*_t)(QString , QVariant );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewItem::dataChanged)) {
                *result = 3;
                return;
            }
        }
        {
            typedef void (ViewItem::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewItem::dataRemoved)) {
                *result = 4;
                return;
            }
        }
        {
            typedef void (ViewItem::*_t)(QString , QVariant );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewItem::propertyAdded)) {
                *result = 5;
                return;
            }
        }
        {
            typedef void (ViewItem::*_t)(QString , QVariant );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewItem::propertyChanged)) {
                *result = 6;
                return;
            }
        }
        {
            typedef void (ViewItem::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewItem::propertyRemoved)) {
                *result = 7;
                return;
            }
        }
        {
            typedef void (ViewItem::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewItem::destructing)) {
                *result = 8;
                return;
            }
        }
    }
}

const QMetaObject ViewItem::staticMetaObject = {
    { &QObjectRegistrar::staticMetaObject, qt_meta_stringdata_ViewItem.data,
      qt_meta_data_ViewItem,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *ViewItem::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ViewItem::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_ViewItem.stringdata0))
        return static_cast<void*>(const_cast< ViewItem*>(this));
    return QObjectRegistrar::qt_metacast(_clname);
}

int ViewItem::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObjectRegistrar::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void ViewItem::labelChanged(QString _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ViewItem::iconChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, Q_NULLPTR);
}

// SIGNAL 2
void ViewItem::dataAdded(QString _t1, QVariant _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void ViewItem::dataChanged(QString _t1, QVariant _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void ViewItem::dataRemoved(QString _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void ViewItem::propertyAdded(QString _t1, QVariant _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void ViewItem::propertyChanged(QString _t1, QVariant _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void ViewItem::propertyRemoved(QString _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void ViewItem::destructing(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}
QT_END_MOC_NAMESPACE
