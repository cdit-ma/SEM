/****************************************************************************
** Meta object code from reading C++ file 'selectionhandler.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/Controllers/SelectionController/selectionhandler.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'selectionhandler.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_SelectionHandler_t {
    QByteArrayData data[14];
    char stringdata0[160];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_SelectionHandler_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_SelectionHandler_t qt_meta_stringdata_SelectionHandler = {
    {
QT_MOC_LITERAL(0, 0, 16), // "SelectionHandler"
QT_MOC_LITERAL(1, 17, 20), // "itemSelectionChanged"
QT_MOC_LITERAL(2, 38, 0), // ""
QT_MOC_LITERAL(3, 39, 9), // "ViewItem*"
QT_MOC_LITERAL(4, 49, 8), // "selected"
QT_MOC_LITERAL(5, 58, 26), // "itemActiveSelectionChanged"
QT_MOC_LITERAL(6, 85, 4), // "item"
QT_MOC_LITERAL(7, 90, 6), // "active"
QT_MOC_LITERAL(8, 97, 9), // "selectAll"
QT_MOC_LITERAL(9, 107, 16), // "selectionChanged"
QT_MOC_LITERAL(10, 124, 5), // "count"
QT_MOC_LITERAL(11, 130, 14), // "clearSelection"
QT_MOC_LITERAL(12, 145, 11), // "itemDeleted"
QT_MOC_LITERAL(13, 157, 2) // "ID"

    },
    "SelectionHandler\0itemSelectionChanged\0"
    "\0ViewItem*\0selected\0itemActiveSelectionChanged\0"
    "item\0active\0selectAll\0selectionChanged\0"
    "count\0clearSelection\0itemDeleted\0ID"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SelectionHandler[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   44,    2, 0x06 /* Public */,
       5,    2,   49,    2, 0x06 /* Public */,
       8,    0,   54,    2, 0x06 /* Public */,
       9,    1,   55,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      11,    0,   58,    2, 0x0a /* Public */,
      12,    2,   59,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3, QMetaType::Bool,    2,    4,
    QMetaType::Void, 0x80000000 | 3, QMetaType::Bool,    6,    7,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   10,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 3,   13,    6,

       0        // eod
};

void SelectionHandler::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        SelectionHandler *_t = static_cast<SelectionHandler *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->itemSelectionChanged((*reinterpret_cast< ViewItem*(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 1: _t->itemActiveSelectionChanged((*reinterpret_cast< ViewItem*(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 2: _t->selectAll(); break;
        case 3: _t->selectionChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->clearSelection(); break;
        case 5: _t->itemDeleted((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< ViewItem*(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< ViewItem* >(); break;
            }
            break;
        case 1:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< ViewItem* >(); break;
            }
            break;
        case 5:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< ViewItem* >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (SelectionHandler::*_t)(ViewItem * , bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SelectionHandler::itemSelectionChanged)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (SelectionHandler::*_t)(ViewItem * , bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SelectionHandler::itemActiveSelectionChanged)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (SelectionHandler::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SelectionHandler::selectAll)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (SelectionHandler::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SelectionHandler::selectionChanged)) {
                *result = 3;
                return;
            }
        }
    }
}

const QMetaObject SelectionHandler::staticMetaObject = {
    { &QObjectRegistrar::staticMetaObject, qt_meta_stringdata_SelectionHandler.data,
      qt_meta_data_SelectionHandler,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *SelectionHandler::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SelectionHandler::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_SelectionHandler.stringdata0))
        return static_cast<void*>(const_cast< SelectionHandler*>(this));
    return QObjectRegistrar::qt_metacast(_clname);
}

int SelectionHandler::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObjectRegistrar::qt_metacall(_c, _id, _a);
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
void SelectionHandler::itemSelectionChanged(ViewItem * _t1, bool _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void SelectionHandler::itemActiveSelectionChanged(ViewItem * _t1, bool _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void SelectionHandler::selectAll()
{
    QMetaObject::activate(this, &staticMetaObject, 2, Q_NULLPTR);
}

// SIGNAL 3
void SelectionHandler::selectionChanged(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_END_MOC_NAMESPACE
