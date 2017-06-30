/****************************************************************************
** Meta object code from reading C++ file 'viewmanagerwidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/Widgets/ViewManager/viewmanagerwidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'viewmanagerwidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_ViewManagerWidget_t {
    QByteArrayData data[12];
    char stringdata0[159];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ViewManagerWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ViewManagerWidget_t qt_meta_stringdata_ViewManagerWidget = {
    {
QT_MOC_LITERAL(0, 0, 17), // "ViewManagerWidget"
QT_MOC_LITERAL(1, 18, 12), // "themeChanged"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 17), // "windowConstructed"
QT_MOC_LITERAL(4, 50, 11), // "BaseWindow*"
QT_MOC_LITERAL(5, 62, 6), // "window"
QT_MOC_LITERAL(6, 69, 16), // "windowDestructed"
QT_MOC_LITERAL(7, 86, 2), // "ID"
QT_MOC_LITERAL(8, 89, 21), // "dockWidgetConstructed"
QT_MOC_LITERAL(9, 111, 15), // "BaseDockWidget*"
QT_MOC_LITERAL(10, 127, 10), // "dockWidget"
QT_MOC_LITERAL(11, 138, 20) // "dockWidgetDestructed"

    },
    "ViewManagerWidget\0themeChanged\0\0"
    "windowConstructed\0BaseWindow*\0window\0"
    "windowDestructed\0ID\0dockWidgetConstructed\0"
    "BaseDockWidget*\0dockWidget\0"
    "dockWidgetDestructed"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ViewManagerWidget[] = {

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
       1,    0,   39,    2, 0x08 /* Private */,
       3,    1,   40,    2, 0x08 /* Private */,
       6,    1,   43,    2, 0x08 /* Private */,
       8,    1,   46,    2, 0x08 /* Private */,
      11,    1,   49,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 4,    5,
    QMetaType::Void, QMetaType::Int,    7,
    QMetaType::Void, 0x80000000 | 9,   10,
    QMetaType::Void, QMetaType::Int,    7,

       0        // eod
};

void ViewManagerWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ViewManagerWidget *_t = static_cast<ViewManagerWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->themeChanged(); break;
        case 1: _t->windowConstructed((*reinterpret_cast< BaseWindow*(*)>(_a[1]))); break;
        case 2: _t->windowDestructed((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->dockWidgetConstructed((*reinterpret_cast< BaseDockWidget*(*)>(_a[1]))); break;
        case 4: _t->dockWidgetDestructed((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 1:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< BaseWindow* >(); break;
            }
            break;
        case 3:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< BaseDockWidget* >(); break;
            }
            break;
        }
    }
}

const QMetaObject ViewManagerWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_ViewManagerWidget.data,
      qt_meta_data_ViewManagerWidget,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *ViewManagerWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ViewManagerWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_ViewManagerWidget.stringdata0))
        return static_cast<void*>(const_cast< ViewManagerWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int ViewManagerWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
