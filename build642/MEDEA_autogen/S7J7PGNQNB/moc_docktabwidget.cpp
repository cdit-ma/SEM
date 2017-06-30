/****************************************************************************
** Meta object code from reading C++ file 'docktabwidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/Views/Dock/docktabwidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'docktabwidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_DockTabWidget_t {
    QByteArrayData data[11];
    char stringdata0[150];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_DockTabWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_DockTabWidget_t qt_meta_stringdata_DockTabWidget = {
    {
QT_MOC_LITERAL(0, 0, 13), // "DockTabWidget"
QT_MOC_LITERAL(1, 14, 12), // "themeChanged"
QT_MOC_LITERAL(2, 27, 0), // ""
QT_MOC_LITERAL(3, 28, 16), // "selectionChanged"
QT_MOC_LITERAL(4, 45, 10), // "tabClicked"
QT_MOC_LITERAL(5, 56, 7), // "checked"
QT_MOC_LITERAL(6, 64, 17), // "dockActionClicked"
QT_MOC_LITERAL(7, 82, 21), // "DockWidgetActionItem*"
QT_MOC_LITERAL(8, 104, 6), // "action"
QT_MOC_LITERAL(9, 111, 21), // "dockBackButtonClicked"
QT_MOC_LITERAL(10, 133, 16) // "onActionFinished"

    },
    "DockTabWidget\0themeChanged\0\0"
    "selectionChanged\0tabClicked\0checked\0"
    "dockActionClicked\0DockWidgetActionItem*\0"
    "action\0dockBackButtonClicked\0"
    "onActionFinished"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_DockTabWidget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   44,    2, 0x0a /* Public */,
       3,    0,   45,    2, 0x0a /* Public */,
       4,    1,   46,    2, 0x0a /* Public */,
       6,    1,   49,    2, 0x0a /* Public */,
       9,    0,   52,    2, 0x0a /* Public */,
      10,    0,   53,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    5,
    QMetaType::Void, 0x80000000 | 7,    8,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void DockTabWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        DockTabWidget *_t = static_cast<DockTabWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->themeChanged(); break;
        case 1: _t->selectionChanged(); break;
        case 2: _t->tabClicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: _t->dockActionClicked((*reinterpret_cast< DockWidgetActionItem*(*)>(_a[1]))); break;
        case 4: _t->dockBackButtonClicked(); break;
        case 5: _t->onActionFinished(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 3:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< DockWidgetActionItem* >(); break;
            }
            break;
        }
    }
}

const QMetaObject DockTabWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_DockTabWidget.data,
      qt_meta_data_DockTabWidget,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *DockTabWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DockTabWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_DockTabWidget.stringdata0))
        return static_cast<void*>(const_cast< DockTabWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int DockTabWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
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
QT_END_MOC_NAMESPACE
