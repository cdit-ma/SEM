/****************************************************************************
** Meta object code from reading C++ file 'windowmanager.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/Controllers/WindowManager/windowmanager.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'windowmanager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_WindowManager_t {
    QByteArrayData data[23];
    char stringdata0[334];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_WindowManager_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_WindowManager_t qt_meta_stringdata_WindowManager = {
    {
QT_MOC_LITERAL(0, 0, 13), // "WindowManager"
QT_MOC_LITERAL(1, 14, 17), // "windowConstructed"
QT_MOC_LITERAL(2, 32, 0), // ""
QT_MOC_LITERAL(3, 33, 11), // "BaseWindow*"
QT_MOC_LITERAL(4, 45, 6), // "window"
QT_MOC_LITERAL(5, 52, 16), // "windowDestructed"
QT_MOC_LITERAL(6, 69, 2), // "ID"
QT_MOC_LITERAL(7, 72, 25), // "viewDockWidgetConstructed"
QT_MOC_LITERAL(8, 98, 15), // "BaseDockWidget*"
QT_MOC_LITERAL(9, 114, 6), // "widget"
QT_MOC_LITERAL(10, 121, 24), // "viewDockWidgetDestructed"
QT_MOC_LITERAL(11, 146, 27), // "activeViewDockWidgetChanged"
QT_MOC_LITERAL(12, 174, 15), // "ViewDockWidget*"
QT_MOC_LITERAL(13, 190, 10), // "prevWidget"
QT_MOC_LITERAL(14, 201, 12), // "focusChanged"
QT_MOC_LITERAL(15, 214, 8), // "QWidget*"
QT_MOC_LITERAL(16, 223, 3), // "old"
QT_MOC_LITERAL(17, 227, 3), // "now"
QT_MOC_LITERAL(18, 231, 16), // "dockWidget_Close"
QT_MOC_LITERAL(19, 248, 17), // "dockWidget_PopOut"
QT_MOC_LITERAL(20, 266, 24), // "reparentDockWidgetAction"
QT_MOC_LITERAL(21, 291, 8), // "windowID"
QT_MOC_LITERAL(22, 300, 33) // "activeDockWidgetVisibilityCha..."

    },
    "WindowManager\0windowConstructed\0\0"
    "BaseWindow*\0window\0windowDestructed\0"
    "ID\0viewDockWidgetConstructed\0"
    "BaseDockWidget*\0widget\0viewDockWidgetDestructed\0"
    "activeViewDockWidgetChanged\0ViewDockWidget*\0"
    "prevWidget\0focusChanged\0QWidget*\0old\0"
    "now\0dockWidget_Close\0dockWidget_PopOut\0"
    "reparentDockWidgetAction\0windowID\0"
    "activeDockWidgetVisibilityChanged"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_WindowManager[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       6,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   69,    2, 0x06 /* Public */,
       5,    1,   72,    2, 0x06 /* Public */,
       7,    1,   75,    2, 0x06 /* Public */,
      10,    1,   78,    2, 0x06 /* Public */,
      11,    2,   81,    2, 0x06 /* Public */,
      11,    1,   86,    2, 0x26 /* Public | MethodCloned */,

 // slots: name, argc, parameters, tag, flags
      14,    2,   89,    2, 0x08 /* Private */,
      18,    1,   94,    2, 0x08 /* Private */,
      19,    1,   97,    2, 0x08 /* Private */,
      20,    1,  100,    2, 0x08 /* Private */,
      22,    0,  103,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, QMetaType::Int,    6,
    QMetaType::Void, 0x80000000 | 8,    9,
    QMetaType::Void, QMetaType::Int,    6,
    QMetaType::Void, 0x80000000 | 12, 0x80000000 | 12,    9,   13,
    QMetaType::Void, 0x80000000 | 12,    9,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 15, 0x80000000 | 15,   16,   17,
    QMetaType::Void, QMetaType::Int,    6,
    QMetaType::Void, QMetaType::Int,    6,
    QMetaType::Void, QMetaType::Int,   21,
    QMetaType::Void,

       0        // eod
};

void WindowManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        WindowManager *_t = static_cast<WindowManager *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->windowConstructed((*reinterpret_cast< BaseWindow*(*)>(_a[1]))); break;
        case 1: _t->windowDestructed((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->viewDockWidgetConstructed((*reinterpret_cast< BaseDockWidget*(*)>(_a[1]))); break;
        case 3: _t->viewDockWidgetDestructed((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->activeViewDockWidgetChanged((*reinterpret_cast< ViewDockWidget*(*)>(_a[1])),(*reinterpret_cast< ViewDockWidget*(*)>(_a[2]))); break;
        case 5: _t->activeViewDockWidgetChanged((*reinterpret_cast< ViewDockWidget*(*)>(_a[1]))); break;
        case 6: _t->focusChanged((*reinterpret_cast< QWidget*(*)>(_a[1])),(*reinterpret_cast< QWidget*(*)>(_a[2]))); break;
        case 7: _t->dockWidget_Close((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: _t->dockWidget_PopOut((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: _t->reparentDockWidgetAction((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: _t->activeDockWidgetVisibilityChanged(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 6:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 1:
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QWidget* >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (WindowManager::*_t)(BaseWindow * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&WindowManager::windowConstructed)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (WindowManager::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&WindowManager::windowDestructed)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (WindowManager::*_t)(BaseDockWidget * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&WindowManager::viewDockWidgetConstructed)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (WindowManager::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&WindowManager::viewDockWidgetDestructed)) {
                *result = 3;
                return;
            }
        }
        {
            typedef void (WindowManager::*_t)(ViewDockWidget * , ViewDockWidget * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&WindowManager::activeViewDockWidgetChanged)) {
                *result = 4;
                return;
            }
        }
    }
}

const QMetaObject WindowManager::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_WindowManager.data,
      qt_meta_data_WindowManager,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *WindowManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *WindowManager::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_WindowManager.stringdata0))
        return static_cast<void*>(const_cast< WindowManager*>(this));
    return QObject::qt_metacast(_clname);
}

int WindowManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    }
    return _id;
}

// SIGNAL 0
void WindowManager::windowConstructed(BaseWindow * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void WindowManager::windowDestructed(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void WindowManager::viewDockWidgetConstructed(BaseDockWidget * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void WindowManager::viewDockWidgetDestructed(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void WindowManager::activeViewDockWidgetChanged(ViewDockWidget * _t1, ViewDockWidget * _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}
QT_END_MOC_NAMESPACE
