/****************************************************************************
** Meta object code from reading C++ file 'dockwidgetactionitem.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/Views/Dock/dockwidgetactionitem.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'dockwidgetactionitem.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_DockWidgetActionItem_t {
    QByteArrayData data[6];
    char stringdata0[74];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_DockWidgetActionItem_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_DockWidgetActionItem_t qt_meta_stringdata_DockWidgetActionItem = {
    {
QT_MOC_LITERAL(0, 0, 20), // "DockWidgetActionItem"
QT_MOC_LITERAL(1, 21, 13), // "actionChanged"
QT_MOC_LITERAL(2, 35, 0), // ""
QT_MOC_LITERAL(3, 36, 12), // "themeChanged"
QT_MOC_LITERAL(4, 49, 19), // "updateDisplayedText"
QT_MOC_LITERAL(5, 69, 4) // "text"

    },
    "DockWidgetActionItem\0actionChanged\0\0"
    "themeChanged\0updateDisplayedText\0text"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_DockWidgetActionItem[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   29,    2, 0x0a /* Public */,
       3,    0,   30,    2, 0x0a /* Public */,
       4,    1,   31,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    5,

       0        // eod
};

void DockWidgetActionItem::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        DockWidgetActionItem *_t = static_cast<DockWidgetActionItem *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->actionChanged(); break;
        case 1: _t->themeChanged(); break;
        case 2: _t->updateDisplayedText((*reinterpret_cast< QString(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject DockWidgetActionItem::staticMetaObject = {
    { &DockWidgetItem::staticMetaObject, qt_meta_stringdata_DockWidgetActionItem.data,
      qt_meta_data_DockWidgetActionItem,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *DockWidgetActionItem::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DockWidgetActionItem::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_DockWidgetActionItem.stringdata0))
        return static_cast<void*>(const_cast< DockWidgetActionItem*>(this));
    return DockWidgetItem::qt_metacast(_clname);
}

int DockWidgetActionItem::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = DockWidgetItem::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
