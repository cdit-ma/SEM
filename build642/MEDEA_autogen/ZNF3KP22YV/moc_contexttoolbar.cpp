/****************************************************************************
** Meta object code from reading C++ file 'contexttoolbar.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/Views/ContextToolbar/contexttoolbar.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'contexttoolbar.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_ContextToolbar_t {
    QByteArrayData data[16];
    char stringdata0[173];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ContextToolbar_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ContextToolbar_t qt_meta_stringdata_ContextToolbar = {
    {
QT_MOC_LITERAL(0, 0, 14), // "ContextToolbar"
QT_MOC_LITERAL(1, 15, 12), // "themeChanged"
QT_MOC_LITERAL(2, 28, 0), // ""
QT_MOC_LITERAL(3, 29, 11), // "showToolbar"
QT_MOC_LITERAL(4, 41, 9), // "globalPos"
QT_MOC_LITERAL(5, 51, 7), // "itemPos"
QT_MOC_LITERAL(6, 59, 10), // "setVisible"
QT_MOC_LITERAL(7, 70, 7), // "visible"
QT_MOC_LITERAL(8, 78, 8), // "execMenu"
QT_MOC_LITERAL(9, 87, 19), // "populateDynamicMenu"
QT_MOC_LITERAL(10, 107, 17), // "menuActionTrigged"
QT_MOC_LITERAL(11, 125, 8), // "QAction*"
QT_MOC_LITERAL(12, 134, 6), // "action"
QT_MOC_LITERAL(13, 141, 12), // "addChildNode"
QT_MOC_LITERAL(14, 154, 7), // "addEdge"
QT_MOC_LITERAL(15, 162, 10) // "removeEdge"

    },
    "ContextToolbar\0themeChanged\0\0showToolbar\0"
    "globalPos\0itemPos\0setVisible\0visible\0"
    "execMenu\0populateDynamicMenu\0"
    "menuActionTrigged\0QAction*\0action\0"
    "addChildNode\0addEdge\0removeEdge"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ContextToolbar[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   64,    2, 0x0a /* Public */,
       3,    2,   65,    2, 0x0a /* Public */,
       3,    1,   70,    2, 0x2a /* Public | MethodCloned */,
       6,    1,   73,    2, 0x0a /* Public */,
       8,    0,   76,    2, 0x0a /* Public */,
       9,    0,   77,    2, 0x0a /* Public */,
      10,    1,   78,    2, 0x0a /* Public */,
      13,    1,   81,    2, 0x0a /* Public */,
      14,    1,   84,    2, 0x0a /* Public */,
      15,    1,   87,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QPoint, QMetaType::QPointF,    4,    5,
    QMetaType::Void, QMetaType::QPoint,    4,
    QMetaType::Void, QMetaType::Bool,    7,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 11,   12,
    QMetaType::Void, 0x80000000 | 11,   12,
    QMetaType::Void, 0x80000000 | 11,   12,
    QMetaType::Void, 0x80000000 | 11,   12,

       0        // eod
};

void ContextToolbar::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ContextToolbar *_t = static_cast<ContextToolbar *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->themeChanged(); break;
        case 1: _t->showToolbar((*reinterpret_cast< QPoint(*)>(_a[1])),(*reinterpret_cast< QPointF(*)>(_a[2]))); break;
        case 2: _t->showToolbar((*reinterpret_cast< QPoint(*)>(_a[1]))); break;
        case 3: _t->setVisible((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: _t->execMenu(); break;
        case 5: _t->populateDynamicMenu(); break;
        case 6: _t->menuActionTrigged((*reinterpret_cast< QAction*(*)>(_a[1]))); break;
        case 7: _t->addChildNode((*reinterpret_cast< QAction*(*)>(_a[1]))); break;
        case 8: _t->addEdge((*reinterpret_cast< QAction*(*)>(_a[1]))); break;
        case 9: _t->removeEdge((*reinterpret_cast< QAction*(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 6:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QAction* >(); break;
            }
            break;
        case 7:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QAction* >(); break;
            }
            break;
        case 8:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QAction* >(); break;
            }
            break;
        case 9:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QAction* >(); break;
            }
            break;
        }
    }
}

const QMetaObject ContextToolbar::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_ContextToolbar.data,
      qt_meta_data_ContextToolbar,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *ContextToolbar::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ContextToolbar::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_ContextToolbar.stringdata0))
        return static_cast<void*>(const_cast< ContextToolbar*>(this));
    return QWidget::qt_metacast(_clname);
}

int ContextToolbar::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
