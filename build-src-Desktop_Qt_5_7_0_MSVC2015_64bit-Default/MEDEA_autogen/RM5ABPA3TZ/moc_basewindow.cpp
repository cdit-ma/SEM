/****************************************************************************
** Meta object code from reading C++ file 'basewindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/Widgets/Windows/basewindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'basewindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_BaseWindow_t {
    QByteArrayData data[14];
    char stringdata0[164];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_BaseWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_BaseWindow_t qt_meta_stringdata_BaseWindow = {
    {
QT_MOC_LITERAL(0, 0, 10), // "BaseWindow"
QT_MOC_LITERAL(1, 11, 15), // "dockWidgetAdded"
QT_MOC_LITERAL(2, 27, 0), // ""
QT_MOC_LITERAL(3, 28, 15), // "BaseDockWidget*"
QT_MOC_LITERAL(4, 44, 6), // "widget"
QT_MOC_LITERAL(5, 51, 12), // "themeChanged"
QT_MOC_LITERAL(6, 64, 15), // "showContextMenu"
QT_MOC_LITERAL(7, 80, 5), // "point"
QT_MOC_LITERAL(8, 86, 22), // "setDockWidgetMaximized"
QT_MOC_LITERAL(9, 109, 2), // "ID"
QT_MOC_LITERAL(10, 112, 9), // "maximized"
QT_MOC_LITERAL(11, 122, 24), // "_setDockWidgetVisibility"
QT_MOC_LITERAL(12, 147, 7), // "visible"
QT_MOC_LITERAL(13, 155, 8) // "tryClose"

    },
    "BaseWindow\0dockWidgetAdded\0\0BaseDockWidget*\0"
    "widget\0themeChanged\0showContextMenu\0"
    "point\0setDockWidgetMaximized\0ID\0"
    "maximized\0_setDockWidgetVisibility\0"
    "visible\0tryClose"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_BaseWindow[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   44,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    0,   47,    2, 0x08 /* Private */,
       6,    1,   48,    2, 0x08 /* Private */,
       8,    2,   51,    2, 0x08 /* Private */,
      11,    2,   56,    2, 0x08 /* Private */,
      13,    0,   61,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QPoint,    7,
    QMetaType::Void, QMetaType::Int, QMetaType::Bool,    9,   10,
    QMetaType::Void, QMetaType::Int, QMetaType::Bool,    9,   12,
    QMetaType::Void,

       0        // eod
};

void BaseWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        BaseWindow *_t = static_cast<BaseWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->dockWidgetAdded((*reinterpret_cast< BaseDockWidget*(*)>(_a[1]))); break;
        case 1: _t->themeChanged(); break;
        case 2: _t->showContextMenu((*reinterpret_cast< const QPoint(*)>(_a[1]))); break;
        case 3: _t->setDockWidgetMaximized((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 4: _t->_setDockWidgetVisibility((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 5: _t->tryClose(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< BaseDockWidget* >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (BaseWindow::*_t)(BaseDockWidget * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&BaseWindow::dockWidgetAdded)) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject BaseWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_BaseWindow.data,
      qt_meta_data_BaseWindow,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *BaseWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *BaseWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_BaseWindow.stringdata0))
        return static_cast<void*>(const_cast< BaseWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int BaseWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void BaseWindow::dockWidgetAdded(BaseDockWidget * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
