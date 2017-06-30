/****************************************************************************
** Meta object code from reading C++ file 'selectioncontroller.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/Controllers/SelectionController/selectioncontroller.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'selectioncontroller.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_SelectionController_t {
    QByteArrayData data[17];
    char stringdata0[270];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_SelectionController_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_SelectionController_t qt_meta_stringdata_SelectionController = {
    {
QT_MOC_LITERAL(0, 0, 19), // "SelectionController"
QT_MOC_LITERAL(1, 20, 26), // "itemActiveSelectionChanged"
QT_MOC_LITERAL(2, 47, 0), // ""
QT_MOC_LITERAL(3, 48, 9), // "ViewItem*"
QT_MOC_LITERAL(4, 58, 4), // "item"
QT_MOC_LITERAL(5, 63, 8), // "isActive"
QT_MOC_LITERAL(6, 72, 16), // "selectionChanged"
QT_MOC_LITERAL(7, 89, 8), // "selected"
QT_MOC_LITERAL(8, 98, 15), // "viewItemDeleted"
QT_MOC_LITERAL(9, 114, 14), // "clearSelection"
QT_MOC_LITERAL(10, 129, 9), // "selectAll"
QT_MOC_LITERAL(11, 139, 27), // "activeViewDockWidgetChanged"
QT_MOC_LITERAL(12, 167, 15), // "ViewDockWidget*"
QT_MOC_LITERAL(13, 183, 6), // "widget"
QT_MOC_LITERAL(14, 190, 28), // "cycleActiveSelectionBackward"
QT_MOC_LITERAL(15, 219, 27), // "cycleActiveSelectionForward"
QT_MOC_LITERAL(16, 247, 22) // "removeSelectionHandler"

    },
    "SelectionController\0itemActiveSelectionChanged\0"
    "\0ViewItem*\0item\0isActive\0selectionChanged\0"
    "selected\0viewItemDeleted\0clearSelection\0"
    "selectAll\0activeViewDockWidgetChanged\0"
    "ViewDockWidget*\0widget\0"
    "cycleActiveSelectionBackward\0"
    "cycleActiveSelectionForward\0"
    "removeSelectionHandler"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SelectionController[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   59,    2, 0x06 /* Public */,
       6,    1,   64,    2, 0x06 /* Public */,
       8,    1,   67,    2, 0x06 /* Public */,
       9,    0,   70,    2, 0x06 /* Public */,
      10,    0,   71,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      11,    1,   72,    2, 0x08 /* Private */,
      14,    0,   75,    2, 0x08 /* Private */,
      15,    0,   76,    2, 0x08 /* Private */,
      16,    0,   77,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3, QMetaType::Bool,    4,    5,
    QMetaType::Void, QMetaType::Int,    7,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 12,   13,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void SelectionController::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        SelectionController *_t = static_cast<SelectionController *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->itemActiveSelectionChanged((*reinterpret_cast< ViewItem*(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 1: _t->selectionChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->viewItemDeleted((*reinterpret_cast< ViewItem*(*)>(_a[1]))); break;
        case 3: _t->clearSelection(); break;
        case 4: _t->selectAll(); break;
        case 5: _t->activeViewDockWidgetChanged((*reinterpret_cast< ViewDockWidget*(*)>(_a[1]))); break;
        case 6: _t->cycleActiveSelectionBackward(); break;
        case 7: _t->cycleActiveSelectionForward(); break;
        case 8: _t->removeSelectionHandler(); break;
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
        case 2:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< ViewItem* >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (SelectionController::*_t)(ViewItem * , bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SelectionController::itemActiveSelectionChanged)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (SelectionController::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SelectionController::selectionChanged)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (SelectionController::*_t)(ViewItem * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SelectionController::viewItemDeleted)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (SelectionController::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SelectionController::clearSelection)) {
                *result = 3;
                return;
            }
        }
        {
            typedef void (SelectionController::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SelectionController::selectAll)) {
                *result = 4;
                return;
            }
        }
    }
}

const QMetaObject SelectionController::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_SelectionController.data,
      qt_meta_data_SelectionController,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *SelectionController::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SelectionController::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_SelectionController.stringdata0))
        return static_cast<void*>(const_cast< SelectionController*>(this));
    return QObject::qt_metacast(_clname);
}

int SelectionController::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
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
void SelectionController::itemActiveSelectionChanged(ViewItem * _t1, bool _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void SelectionController::selectionChanged(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void SelectionController::viewItemDeleted(ViewItem * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void SelectionController::clearSelection()
{
    QMetaObject::activate(this, &staticMetaObject, 3, Q_NULLPTR);
}

// SIGNAL 4
void SelectionController::selectAll()
{
    QMetaObject::activate(this, &staticMetaObject, 4, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
