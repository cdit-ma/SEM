/****************************************************************************
** Meta object code from reading C++ file 'edgeitem.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/Views/NodeView/SceneItems/Edge/edgeitem.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'edgeitem.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_EdgeItem_t {
    QByteArrayData data[9];
    char stringdata0[116];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_EdgeItem_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_EdgeItem_t qt_meta_stringdata_EdgeItem = {
    {
QT_MOC_LITERAL(0, 0, 8), // "EdgeItem"
QT_MOC_LITERAL(1, 9, 11), // "dataChanged"
QT_MOC_LITERAL(2, 21, 0), // ""
QT_MOC_LITERAL(3, 22, 7), // "keyName"
QT_MOC_LITERAL(4, 30, 4), // "data"
QT_MOC_LITERAL(5, 35, 11), // "dataRemoved"
QT_MOC_LITERAL(6, 47, 10), // "updateEdge"
QT_MOC_LITERAL(7, 58, 28), // "srcAncestorVisibilityChanged"
QT_MOC_LITERAL(8, 87, 28) // "dstAncestorVisibilityChanged"

    },
    "EdgeItem\0dataChanged\0\0keyName\0data\0"
    "dataRemoved\0updateEdge\0"
    "srcAncestorVisibilityChanged\0"
    "dstAncestorVisibilityChanged"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_EdgeItem[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    2,   39,    2, 0x08 /* Private */,
       5,    1,   44,    2, 0x08 /* Private */,
       6,    0,   47,    2, 0x08 /* Private */,
       7,    0,   48,    2, 0x08 /* Private */,
       8,    0,   49,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::QVariant,    3,    4,
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void EdgeItem::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        EdgeItem *_t = static_cast<EdgeItem *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->dataChanged((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QVariant(*)>(_a[2]))); break;
        case 1: _t->dataRemoved((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: _t->updateEdge(); break;
        case 3: _t->srcAncestorVisibilityChanged(); break;
        case 4: _t->dstAncestorVisibilityChanged(); break;
        default: ;
        }
    }
}

const QMetaObject EdgeItem::staticMetaObject = {
    { &EntityItem::staticMetaObject, qt_meta_stringdata_EdgeItem.data,
      qt_meta_data_EdgeItem,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *EdgeItem::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *EdgeItem::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_EdgeItem.stringdata0))
        return static_cast<void*>(const_cast< EdgeItem*>(this));
    return EntityItem::qt_metacast(_clname);
}

int EdgeItem::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = EntityItem::qt_metacall(_c, _id, _a);
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
QT_END_MOC_NAMESPACE
