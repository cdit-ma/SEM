/****************************************************************************
** Meta object code from reading C++ file 'basedockwidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/Widgets/DockWidgets/basedockwidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'basedockwidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_BaseDockWidget_t {
    QByteArrayData data[20];
    char stringdata0[225];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_BaseDockWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_BaseDockWidget_t qt_meta_stringdata_BaseDockWidget = {
    {
QT_MOC_LITERAL(0, 0, 14), // "BaseDockWidget"
QT_MOC_LITERAL(1, 15, 11), // "iconChanged"
QT_MOC_LITERAL(2, 27, 0), // ""
QT_MOC_LITERAL(3, 28, 12), // "titleChanged"
QT_MOC_LITERAL(4, 41, 13), // "dockSetActive"
QT_MOC_LITERAL(5, 55, 12), // "req_Maximize"
QT_MOC_LITERAL(6, 68, 2), // "ID"
QT_MOC_LITERAL(7, 71, 8), // "maximize"
QT_MOC_LITERAL(8, 80, 11), // "req_Visible"
QT_MOC_LITERAL(9, 92, 7), // "visible"
QT_MOC_LITERAL(10, 100, 10), // "req_PopOut"
QT_MOC_LITERAL(11, 111, 9), // "req_Close"
QT_MOC_LITERAL(12, 121, 14), // "title_Maximize"
QT_MOC_LITERAL(13, 136, 13), // "title_Visible"
QT_MOC_LITERAL(14, 150, 12), // "title_PopOut"
QT_MOC_LITERAL(15, 163, 11), // "title_Close"
QT_MOC_LITERAL(16, 175, 18), // "_visibilityChanged"
QT_MOC_LITERAL(17, 194, 8), // "destruct"
QT_MOC_LITERAL(18, 203, 15), // "showContextMenu"
QT_MOC_LITERAL(19, 219, 5) // "point"

    },
    "BaseDockWidget\0iconChanged\0\0titleChanged\0"
    "dockSetActive\0req_Maximize\0ID\0maximize\0"
    "req_Visible\0visible\0req_PopOut\0req_Close\0"
    "title_Maximize\0title_Visible\0title_PopOut\0"
    "title_Close\0_visibilityChanged\0destruct\0"
    "showContextMenu\0point"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_BaseDockWidget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       7,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   84,    2, 0x06 /* Public */,
       3,    0,   85,    2, 0x06 /* Public */,
       4,    1,   86,    2, 0x06 /* Public */,
       5,    2,   89,    2, 0x06 /* Public */,
       8,    2,   94,    2, 0x06 /* Public */,
      10,    1,   99,    2, 0x06 /* Public */,
      11,    1,  102,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      12,    1,  105,    2, 0x08 /* Private */,
      13,    1,  108,    2, 0x08 /* Private */,
      14,    1,  111,    2, 0x08 /* Private */,
      15,    1,  114,    2, 0x08 /* Private */,
      16,    1,  117,    2, 0x08 /* Private */,
      17,    0,  120,    2, 0x08 /* Private */,
      18,    1,  121,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Int, QMetaType::Bool,    6,    7,
    QMetaType::Void, QMetaType::Int, QMetaType::Bool,    6,    9,
    QMetaType::Void, QMetaType::Int,    6,
    QMetaType::Void, QMetaType::Int,    6,

 // slots: parameters
    QMetaType::Void, QMetaType::Bool,    7,
    QMetaType::Void, QMetaType::Bool,    9,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    9,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QPoint,   19,

       0        // eod
};

void BaseDockWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        BaseDockWidget *_t = static_cast<BaseDockWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->iconChanged(); break;
        case 1: _t->titleChanged(); break;
        case 2: _t->dockSetActive((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: _t->req_Maximize((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 4: _t->req_Visible((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 5: _t->req_PopOut((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: _t->req_Close((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->title_Maximize((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 8: _t->title_Visible((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 9: _t->title_PopOut((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 10: _t->title_Close((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 11: _t->_visibilityChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 12: _t->destruct(); break;
        case 13: _t->showContextMenu((*reinterpret_cast< const QPoint(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (BaseDockWidget::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&BaseDockWidget::iconChanged)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (BaseDockWidget::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&BaseDockWidget::titleChanged)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (BaseDockWidget::*_t)(bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&BaseDockWidget::dockSetActive)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (BaseDockWidget::*_t)(int , bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&BaseDockWidget::req_Maximize)) {
                *result = 3;
                return;
            }
        }
        {
            typedef void (BaseDockWidget::*_t)(int , bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&BaseDockWidget::req_Visible)) {
                *result = 4;
                return;
            }
        }
        {
            typedef void (BaseDockWidget::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&BaseDockWidget::req_PopOut)) {
                *result = 5;
                return;
            }
        }
        {
            typedef void (BaseDockWidget::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&BaseDockWidget::req_Close)) {
                *result = 6;
                return;
            }
        }
    }
}

const QMetaObject BaseDockWidget::staticMetaObject = {
    { &QDockWidget::staticMetaObject, qt_meta_stringdata_BaseDockWidget.data,
      qt_meta_data_BaseDockWidget,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *BaseDockWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *BaseDockWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_BaseDockWidget.stringdata0))
        return static_cast<void*>(const_cast< BaseDockWidget*>(this));
    return QDockWidget::qt_metacast(_clname);
}

int BaseDockWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDockWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 14)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 14)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 14;
    }
    return _id;
}

// SIGNAL 0
void BaseDockWidget::iconChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}

// SIGNAL 1
void BaseDockWidget::titleChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, Q_NULLPTR);
}

// SIGNAL 2
void BaseDockWidget::dockSetActive(bool _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void BaseDockWidget::req_Maximize(int _t1, bool _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void BaseDockWidget::req_Visible(int _t1, bool _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void BaseDockWidget::req_PopOut(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void BaseDockWidget::req_Close(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}
QT_END_MOC_NAMESPACE
