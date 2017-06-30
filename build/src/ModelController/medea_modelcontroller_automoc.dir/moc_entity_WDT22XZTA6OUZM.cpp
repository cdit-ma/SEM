/****************************************************************************
** Meta object code from reading C++ file 'entity.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/ModelController/Entities/entity.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'entity.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Entity_t {
    QByteArrayData data[11];
    char stringdata0[105];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Entity_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Entity_t qt_meta_stringdata_Entity = {
    {
QT_MOC_LITERAL(0, 0, 6), // "Entity"
QT_MOC_LITERAL(1, 7, 11), // "dataChanged"
QT_MOC_LITERAL(2, 19, 0), // ""
QT_MOC_LITERAL(3, 20, 2), // "ID"
QT_MOC_LITERAL(4, 23, 7), // "keyName"
QT_MOC_LITERAL(5, 31, 4), // "data"
QT_MOC_LITERAL(6, 36, 11), // "dataRemoved"
QT_MOC_LITERAL(7, 48, 15), // "propertyChanged"
QT_MOC_LITERAL(8, 64, 12), // "propertyName"
QT_MOC_LITERAL(9, 77, 15), // "propertyRemoved"
QT_MOC_LITERAL(10, 93, 11) // "terminating"

    },
    "Entity\0dataChanged\0\0ID\0keyName\0data\0"
    "dataRemoved\0propertyChanged\0propertyName\0"
    "propertyRemoved\0terminating"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Entity[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    3,   39,    2, 0x06 /* Public */,
       6,    2,   46,    2, 0x06 /* Public */,
       7,    3,   51,    2, 0x06 /* Public */,
       9,    2,   58,    2, 0x06 /* Public */,
      10,    0,   63,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::QString, QMetaType::QVariant,    3,    4,    5,
    QMetaType::Void, QMetaType::Int, QMetaType::QString,    3,    4,
    QMetaType::Void, QMetaType::Int, QMetaType::QString, QMetaType::QVariant,    3,    8,    5,
    QMetaType::Void, QMetaType::Int, QMetaType::QString,    3,    8,
    QMetaType::Void,

       0        // eod
};

void Entity::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Entity *_t = static_cast<Entity *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->dataChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QVariant(*)>(_a[3]))); break;
        case 1: _t->dataRemoved((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 2: _t->propertyChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QVariant(*)>(_a[3]))); break;
        case 3: _t->propertyRemoved((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 4: _t->terminating(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (Entity::*_t)(int , QString , QVariant );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Entity::dataChanged)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (Entity::*_t)(int , QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Entity::dataRemoved)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (Entity::*_t)(int , QString , QVariant );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Entity::propertyChanged)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (Entity::*_t)(int , QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Entity::propertyRemoved)) {
                *result = 3;
                return;
            }
        }
        {
            typedef void (Entity::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Entity::terminating)) {
                *result = 4;
                return;
            }
        }
    }
}

const QMetaObject Entity::staticMetaObject = {
    { &GraphML::staticMetaObject, qt_meta_stringdata_Entity.data,
      qt_meta_data_Entity,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Entity::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Entity::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Entity.stringdata0))
        return static_cast<void*>(const_cast< Entity*>(this));
    return GraphML::qt_metacast(_clname);
}

int Entity::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = GraphML::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void Entity::dataChanged(int _t1, QString _t2, QVariant _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Entity::dataRemoved(int _t1, QString _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void Entity::propertyChanged(int _t1, QString _t2, QVariant _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void Entity::propertyRemoved(int _t1, QString _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void Entity::terminating()
{
    QMetaObject::activate(this, &staticMetaObject, 4, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
