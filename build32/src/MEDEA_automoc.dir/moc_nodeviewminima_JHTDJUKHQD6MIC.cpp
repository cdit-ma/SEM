/****************************************************************************
** Meta object code from reading C++ file 'nodeviewminimap.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/Views/NodeView/nodeviewminimap.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'nodeviewminimap.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_NodeViewMinimap_t {
    QByteArrayData data[10];
    char stringdata0[113];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_NodeViewMinimap_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_NodeViewMinimap_t qt_meta_stringdata_NodeViewMinimap = {
    {
QT_MOC_LITERAL(0, 0, 15), // "NodeViewMinimap"
QT_MOC_LITERAL(1, 16, 11), // "minimap_Pan"
QT_MOC_LITERAL(2, 28, 0), // ""
QT_MOC_LITERAL(3, 29, 5), // "delta"
QT_MOC_LITERAL(4, 35, 12), // "minimap_Zoom"
QT_MOC_LITERAL(5, 48, 16), // "sceneRectChanged"
QT_MOC_LITERAL(6, 65, 9), // "sceneRect"
QT_MOC_LITERAL(7, 75, 19), // "viewportRectChanged"
QT_MOC_LITERAL(8, 95, 12), // "viewportRect"
QT_MOC_LITERAL(9, 108, 4) // "zoom"

    },
    "NodeViewMinimap\0minimap_Pan\0\0delta\0"
    "minimap_Zoom\0sceneRectChanged\0sceneRect\0"
    "viewportRectChanged\0viewportRect\0zoom"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_NodeViewMinimap[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   34,    2, 0x06 /* Public */,
       4,    1,   37,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    1,   40,    2, 0x0a /* Public */,
       7,    2,   43,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QPointF,    3,
    QMetaType::Void, QMetaType::Int,    3,

 // slots: parameters
    QMetaType::Void, QMetaType::QRectF,    6,
    QMetaType::Void, QMetaType::QRectF, QMetaType::QReal,    8,    9,

       0        // eod
};

void NodeViewMinimap::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        NodeViewMinimap *_t = static_cast<NodeViewMinimap *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->minimap_Pan((*reinterpret_cast< QPointF(*)>(_a[1]))); break;
        case 1: _t->minimap_Zoom((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->sceneRectChanged((*reinterpret_cast< QRectF(*)>(_a[1]))); break;
        case 3: _t->viewportRectChanged((*reinterpret_cast< QRectF(*)>(_a[1])),(*reinterpret_cast< qreal(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (NodeViewMinimap::*_t)(QPointF );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NodeViewMinimap::minimap_Pan)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (NodeViewMinimap::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NodeViewMinimap::minimap_Zoom)) {
                *result = 1;
                return;
            }
        }
    }
}

const QMetaObject NodeViewMinimap::staticMetaObject = {
    { &QGraphicsView::staticMetaObject, qt_meta_stringdata_NodeViewMinimap.data,
      qt_meta_data_NodeViewMinimap,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *NodeViewMinimap::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *NodeViewMinimap::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_NodeViewMinimap.stringdata0))
        return static_cast<void*>(const_cast< NodeViewMinimap*>(this));
    return QGraphicsView::qt_metacast(_clname);
}

int NodeViewMinimap::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGraphicsView::qt_metacall(_c, _id, _a);
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
void NodeViewMinimap::minimap_Pan(QPointF _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void NodeViewMinimap::minimap_Zoom(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
