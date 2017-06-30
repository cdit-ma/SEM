/****************************************************************************
** Meta object code from reading C++ file 'nodeitem.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/Views/NodeView/SceneItems/Node/nodeitem.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'nodeitem.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_NodeItem_t {
    QByteArrayData data[24];
    char stringdata0[274];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_NodeItem_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_NodeItem_t qt_meta_stringdata_NodeItem = {
    {
QT_MOC_LITERAL(0, 0, 8), // "NodeItem"
QT_MOC_LITERAL(1, 9, 15), // "req_connectMode"
QT_MOC_LITERAL(2, 25, 0), // ""
QT_MOC_LITERAL(3, 26, 9), // "NodeItem*"
QT_MOC_LITERAL(4, 36, 4), // "item"
QT_MOC_LITERAL(5, 41, 21), // "req_popOutRelatedNode"
QT_MOC_LITERAL(6, 63, 13), // "NodeViewItem*"
QT_MOC_LITERAL(7, 77, 9), // "NODE_KIND"
QT_MOC_LITERAL(8, 87, 4), // "kind"
QT_MOC_LITERAL(9, 92, 15), // "req_StartResize"
QT_MOC_LITERAL(10, 108, 10), // "req_Resize"
QT_MOC_LITERAL(11, 119, 5), // "delta"
QT_MOC_LITERAL(12, 125, 11), // "RECT_VERTEX"
QT_MOC_LITERAL(13, 137, 4), // "vert"
QT_MOC_LITERAL(14, 142, 16), // "req_FinishResize"
QT_MOC_LITERAL(15, 159, 13), // "gotChildNodes"
QT_MOC_LITERAL(16, 173, 18), // "gotChildProxyEdges"
QT_MOC_LITERAL(17, 192, 11), // "dataChanged"
QT_MOC_LITERAL(18, 204, 7), // "keyName"
QT_MOC_LITERAL(19, 212, 4), // "data"
QT_MOC_LITERAL(20, 217, 15), // "propertyChanged"
QT_MOC_LITERAL(21, 233, 12), // "propertyName"
QT_MOC_LITERAL(22, 246, 11), // "dataRemoved"
QT_MOC_LITERAL(23, 258, 15) // "childPosChanged"

    },
    "NodeItem\0req_connectMode\0\0NodeItem*\0"
    "item\0req_popOutRelatedNode\0NodeViewItem*\0"
    "NODE_KIND\0kind\0req_StartResize\0"
    "req_Resize\0delta\0RECT_VERTEX\0vert\0"
    "req_FinishResize\0gotChildNodes\0"
    "gotChildProxyEdges\0dataChanged\0keyName\0"
    "data\0propertyChanged\0propertyName\0"
    "dataRemoved\0childPosChanged"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_NodeItem[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       7,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   69,    2, 0x06 /* Public */,
       5,    2,   72,    2, 0x06 /* Public */,
       9,    0,   77,    2, 0x06 /* Public */,
      10,    3,   78,    2, 0x06 /* Public */,
      14,    0,   85,    2, 0x06 /* Public */,
      15,    1,   86,    2, 0x06 /* Public */,
      16,    1,   89,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      17,    2,   92,    2, 0x0a /* Public */,
      20,    2,   97,    2, 0x0a /* Public */,
      22,    1,  102,    2, 0x0a /* Public */,
      23,    0,  105,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 6, 0x80000000 | 7,    4,    8,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 3, QMetaType::QSizeF, 0x80000000 | 12,    4,   11,   13,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    2,

 // slots: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::QVariant,   18,   19,
    QMetaType::Void, QMetaType::QString, QMetaType::QVariant,   21,   19,
    QMetaType::Void, QMetaType::QString,   18,
    QMetaType::Void,

       0        // eod
};

void NodeItem::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        NodeItem *_t = static_cast<NodeItem *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->req_connectMode((*reinterpret_cast< NodeItem*(*)>(_a[1]))); break;
        case 1: _t->req_popOutRelatedNode((*reinterpret_cast< NodeViewItem*(*)>(_a[1])),(*reinterpret_cast< NODE_KIND(*)>(_a[2]))); break;
        case 2: _t->req_StartResize(); break;
        case 3: _t->req_Resize((*reinterpret_cast< NodeItem*(*)>(_a[1])),(*reinterpret_cast< QSizeF(*)>(_a[2])),(*reinterpret_cast< RECT_VERTEX(*)>(_a[3]))); break;
        case 4: _t->req_FinishResize(); break;
        case 5: _t->gotChildNodes((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 6: _t->gotChildProxyEdges((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 7: _t->dataChanged((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QVariant(*)>(_a[2]))); break;
        case 8: _t->propertyChanged((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QVariant(*)>(_a[2]))); break;
        case 9: _t->dataRemoved((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 10: _t->childPosChanged(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< NodeItem* >(); break;
            }
            break;
        case 1:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< NODE_KIND >(); break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< NodeViewItem* >(); break;
            }
            break;
        case 3:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< NodeItem* >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (NodeItem::*_t)(NodeItem * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NodeItem::req_connectMode)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (NodeItem::*_t)(NodeViewItem * , NODE_KIND );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NodeItem::req_popOutRelatedNode)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (NodeItem::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NodeItem::req_StartResize)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (NodeItem::*_t)(NodeItem * , QSizeF , RECT_VERTEX );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NodeItem::req_Resize)) {
                *result = 3;
                return;
            }
        }
        {
            typedef void (NodeItem::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NodeItem::req_FinishResize)) {
                *result = 4;
                return;
            }
        }
        {
            typedef void (NodeItem::*_t)(bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NodeItem::gotChildNodes)) {
                *result = 5;
                return;
            }
        }
        {
            typedef void (NodeItem::*_t)(bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NodeItem::gotChildProxyEdges)) {
                *result = 6;
                return;
            }
        }
    }
}

const QMetaObject NodeItem::staticMetaObject = {
    { &EntityItem::staticMetaObject, qt_meta_stringdata_NodeItem.data,
      qt_meta_data_NodeItem,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *NodeItem::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *NodeItem::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_NodeItem.stringdata0))
        return static_cast<void*>(const_cast< NodeItem*>(this));
    return EntityItem::qt_metacast(_clname);
}

int NodeItem::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = EntityItem::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    }
    return _id;
}

// SIGNAL 0
void NodeItem::req_connectMode(NodeItem * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void NodeItem::req_popOutRelatedNode(NodeViewItem * _t1, NODE_KIND _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void NodeItem::req_StartResize()
{
    QMetaObject::activate(this, &staticMetaObject, 2, Q_NULLPTR);
}

// SIGNAL 3
void NodeItem::req_Resize(NodeItem * _t1, QSizeF _t2, RECT_VERTEX _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void NodeItem::req_FinishResize()
{
    QMetaObject::activate(this, &staticMetaObject, 4, Q_NULLPTR);
}

// SIGNAL 5
void NodeItem::gotChildNodes(bool _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void NodeItem::gotChildProxyEdges(bool _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}
QT_END_MOC_NAMESPACE
