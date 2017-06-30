/****************************************************************************
** Meta object code from reading C++ file 'nodeviewitem.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/Controllers/ViewController/nodeviewitem.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'nodeviewitem.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_NodeViewItem_t {
    QByteArrayData data[6];
    char stringdata0[55];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_NodeViewItem_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_NodeViewItem_t qt_meta_stringdata_NodeViewItem = {
    {
QT_MOC_LITERAL(0, 0, 12), // "NodeViewItem"
QT_MOC_LITERAL(1, 13, 9), // "edgeAdded"
QT_MOC_LITERAL(2, 23, 0), // ""
QT_MOC_LITERAL(3, 24, 9), // "EDGE_KIND"
QT_MOC_LITERAL(4, 34, 8), // "edgeKind"
QT_MOC_LITERAL(5, 43, 11) // "edgeRemoved"

    },
    "NodeViewItem\0edgeAdded\0\0EDGE_KIND\0"
    "edgeKind\0edgeRemoved"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_NodeViewItem[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   24,    2, 0x06 /* Public */,
       5,    1,   27,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    4,

       0        // eod
};

void NodeViewItem::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        NodeViewItem *_t = static_cast<NodeViewItem *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->edgeAdded((*reinterpret_cast< EDGE_KIND(*)>(_a[1]))); break;
        case 1: _t->edgeRemoved((*reinterpret_cast< EDGE_KIND(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< EDGE_KIND >(); break;
            }
            break;
        case 1:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< EDGE_KIND >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (NodeViewItem::*_t)(EDGE_KIND );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NodeViewItem::edgeAdded)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (NodeViewItem::*_t)(EDGE_KIND );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NodeViewItem::edgeRemoved)) {
                *result = 1;
                return;
            }
        }
    }
}

const QMetaObject NodeViewItem::staticMetaObject = {
    { &ViewItem::staticMetaObject, qt_meta_stringdata_NodeViewItem.data,
      qt_meta_data_NodeViewItem,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *NodeViewItem::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *NodeViewItem::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_NodeViewItem.stringdata0))
        return static_cast<void*>(const_cast< NodeViewItem*>(this));
    return ViewItem::qt_metacast(_clname);
}

int NodeViewItem::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ViewItem::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void NodeViewItem::edgeAdded(EDGE_KIND _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void NodeViewItem::edgeRemoved(EDGE_KIND _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
