/****************************************************************************
** Meta object code from reading C++ file 'dockwidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/Views/Dock/dockwidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'dockwidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_DockWidget_t {
    QByteArrayData data[13];
    char stringdata0[185];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_DockWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_DockWidget_t qt_meta_stringdata_DockWidget = {
    {
QT_MOC_LITERAL(0, 0, 10), // "DockWidget"
QT_MOC_LITERAL(1, 11, 13), // "actionClicked"
QT_MOC_LITERAL(2, 25, 0), // ""
QT_MOC_LITERAL(3, 26, 21), // "DockWidgetActionItem*"
QT_MOC_LITERAL(4, 48, 6), // "action"
QT_MOC_LITERAL(5, 55, 17), // "backButtonClicked"
QT_MOC_LITERAL(6, 73, 12), // "themeChanged"
QT_MOC_LITERAL(7, 86, 17), // "dockActionClicked"
QT_MOC_LITERAL(8, 104, 13), // "highlightItem"
QT_MOC_LITERAL(9, 118, 2), // "ID"
QT_MOC_LITERAL(10, 121, 19), // "viewItemConstructed"
QT_MOC_LITERAL(11, 141, 24), // "viewItemGroupConstructed"
QT_MOC_LITERAL(12, 166, 18) // "viewItemDestructed"

    },
    "DockWidget\0actionClicked\0\0"
    "DockWidgetActionItem*\0action\0"
    "backButtonClicked\0themeChanged\0"
    "dockActionClicked\0highlightItem\0ID\0"
    "viewItemConstructed\0viewItemGroupConstructed\0"
    "viewItemDestructed"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_DockWidget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   59,    2, 0x06 /* Public */,
       5,    0,   62,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       6,    0,   63,    2, 0x0a /* Public */,
       7,    0,   64,    2, 0x0a /* Public */,
       8,    1,   65,    2, 0x0a /* Public */,
       8,    0,   68,    2, 0x2a /* Public | MethodCloned */,
      10,    1,   69,    2, 0x0a /* Public */,
      11,    1,   72,    2, 0x0a /* Public */,
      12,    1,   75,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    9,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    9,
    QMetaType::Void, QMetaType::Int,    9,
    QMetaType::Void, QMetaType::Int,    9,

       0        // eod
};

void DockWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        DockWidget *_t = static_cast<DockWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->actionClicked((*reinterpret_cast< DockWidgetActionItem*(*)>(_a[1]))); break;
        case 1: _t->backButtonClicked(); break;
        case 2: _t->themeChanged(); break;
        case 3: _t->dockActionClicked(); break;
        case 4: _t->highlightItem((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->highlightItem(); break;
        case 6: _t->viewItemConstructed((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->viewItemGroupConstructed((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: _t->viewItemDestructed((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< DockWidgetActionItem* >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (DockWidget::*_t)(DockWidgetActionItem * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&DockWidget::actionClicked)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (DockWidget::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&DockWidget::backButtonClicked)) {
                *result = 1;
                return;
            }
        }
    }
}

const QMetaObject DockWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_DockWidget.data,
      qt_meta_data_DockWidget,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *DockWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DockWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_DockWidget.stringdata0))
        return static_cast<void*>(const_cast< DockWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int DockWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void DockWidget::actionClicked(DockWidgetActionItem * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void DockWidget::backButtonClicked()
{
    QMetaObject::activate(this, &staticMetaObject, 1, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
