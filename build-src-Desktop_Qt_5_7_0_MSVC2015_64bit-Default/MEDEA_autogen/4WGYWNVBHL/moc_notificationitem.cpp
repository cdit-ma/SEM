/****************************************************************************
** Meta object code from reading C++ file 'notificationitem.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/Views/Notification/notificationitem.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'notificationitem.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_NotificationItem_t {
    QByteArrayData data[19];
    char stringdata0[315];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_NotificationItem_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_NotificationItem_t qt_meta_stringdata_NotificationItem = {
    {
QT_MOC_LITERAL(0, 0, 16), // "NotificationItem"
QT_MOC_LITERAL(1, 17, 11), // "itemClicked"
QT_MOC_LITERAL(2, 29, 0), // ""
QT_MOC_LITERAL(3, 30, 17), // "NotificationItem*"
QT_MOC_LITERAL(4, 48, 4), // "item"
QT_MOC_LITERAL(5, 53, 12), // "currentState"
QT_MOC_LITERAL(6, 66, 11), // "controlDown"
QT_MOC_LITERAL(7, 78, 12), // "themeChanged"
QT_MOC_LITERAL(8, 91, 8), // "showItem"
QT_MOC_LITERAL(9, 100, 13), // "filterCleared"
QT_MOC_LITERAL(10, 114, 19), // "NOTIFICATION_FILTER"
QT_MOC_LITERAL(11, 134, 6), // "filter"
QT_MOC_LITERAL(12, 141, 21), // "severityFilterToggled"
QT_MOC_LITERAL(13, 163, 33), // "QHash<NOTIFICATION_SEVERITY,b..."
QT_MOC_LITERAL(14, 197, 13), // "checkedStates"
QT_MOC_LITERAL(15, 211, 17), // "typeFilterToggled"
QT_MOC_LITERAL(16, 229, 29), // "QHash<NOTIFICATION_TYPE,bool>"
QT_MOC_LITERAL(17, 259, 21), // "categoryFilterToggled"
QT_MOC_LITERAL(18, 281, 33) // "QHash<NOTIFICATION_CATEGORY,b..."

    },
    "NotificationItem\0itemClicked\0\0"
    "NotificationItem*\0item\0currentState\0"
    "controlDown\0themeChanged\0showItem\0"
    "filterCleared\0NOTIFICATION_FILTER\0"
    "filter\0severityFilterToggled\0"
    "QHash<NOTIFICATION_SEVERITY,bool>\0"
    "checkedStates\0typeFilterToggled\0"
    "QHash<NOTIFICATION_TYPE,bool>\0"
    "categoryFilterToggled\0"
    "QHash<NOTIFICATION_CATEGORY,bool>"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_NotificationItem[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    3,   49,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       7,    0,   56,    2, 0x0a /* Public */,
       8,    0,   57,    2, 0x0a /* Public */,
       9,    1,   58,    2, 0x0a /* Public */,
      12,    1,   61,    2, 0x0a /* Public */,
      15,    1,   64,    2, 0x0a /* Public */,
      17,    1,   67,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3, QMetaType::Bool, QMetaType::Bool,    4,    5,    6,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 10,   11,
    QMetaType::Void, 0x80000000 | 13,   14,
    QMetaType::Void, 0x80000000 | 16,   14,
    QMetaType::Void, 0x80000000 | 18,   14,

       0        // eod
};

void NotificationItem::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        NotificationItem *_t = static_cast<NotificationItem *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->itemClicked((*reinterpret_cast< NotificationItem*(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 1: _t->themeChanged(); break;
        case 2: _t->showItem(); break;
        case 3: _t->filterCleared((*reinterpret_cast< NOTIFICATION_FILTER(*)>(_a[1]))); break;
        case 4: _t->severityFilterToggled((*reinterpret_cast< QHash<NOTIFICATION_SEVERITY,bool>(*)>(_a[1]))); break;
        case 5: _t->typeFilterToggled((*reinterpret_cast< QHash<NOTIFICATION_TYPE,bool>(*)>(_a[1]))); break;
        case 6: _t->categoryFilterToggled((*reinterpret_cast< QHash<NOTIFICATION_CATEGORY,bool>(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< NotificationItem* >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (NotificationItem::*_t)(NotificationItem * , bool , bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NotificationItem::itemClicked)) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject NotificationItem::staticMetaObject = {
    { &QFrame::staticMetaObject, qt_meta_stringdata_NotificationItem.data,
      qt_meta_data_NotificationItem,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *NotificationItem::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *NotificationItem::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_NotificationItem.stringdata0))
        return static_cast<void*>(const_cast< NotificationItem*>(this));
    return QFrame::qt_metacast(_clname);
}

int NotificationItem::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QFrame::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void NotificationItem::itemClicked(NotificationItem * _t1, bool _t2, bool _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
