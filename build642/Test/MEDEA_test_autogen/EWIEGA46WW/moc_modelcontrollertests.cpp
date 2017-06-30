/****************************************************************************
** Meta object code from reading C++ file 'modelcontrollertests.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/Test/modelcontrollertests.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'modelcontrollertests.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_ModelControllerTests_t {
    QByteArrayData data[9];
    char stringdata0[124];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ModelControllerTests_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ModelControllerTests_t qt_meta_stringdata_ModelControllerTests = {
    {
QT_MOC_LITERAL(0, 0, 20), // "ModelControllerTests"
QT_MOC_LITERAL(1, 21, 16), // "setup_controller"
QT_MOC_LITERAL(2, 38, 0), // ""
QT_MOC_LITERAL(3, 39, 19), // "teardown_controller"
QT_MOC_LITERAL(4, 59, 12), // "open_project"
QT_MOC_LITERAL(5, 72, 9), // "file_name"
QT_MOC_LITERAL(6, 82, 9), // "file_data"
QT_MOC_LITERAL(7, 92, 15), // "test_controller"
QT_MOC_LITERAL(8, 108, 15) // "load_helloworld"

    },
    "ModelControllerTests\0setup_controller\0"
    "\0teardown_controller\0open_project\0"
    "file_name\0file_data\0test_controller\0"
    "load_helloworld"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ModelControllerTests[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   39,    2, 0x06 /* Public */,
       3,    0,   40,    2, 0x06 /* Public */,
       4,    2,   41,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       7,    0,   46,    2, 0x08 /* Private */,
       8,    0,   47,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    5,    6,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void ModelControllerTests::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ModelControllerTests *_t = static_cast<ModelControllerTests *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->setup_controller(); break;
        case 1: _t->teardown_controller(); break;
        case 2: _t->open_project((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 3: _t->test_controller(); break;
        case 4: _t->load_helloworld(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (ModelControllerTests::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ModelControllerTests::setup_controller)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (ModelControllerTests::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ModelControllerTests::teardown_controller)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (ModelControllerTests::*_t)(QString , QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ModelControllerTests::open_project)) {
                *result = 2;
                return;
            }
        }
    }
}

const QMetaObject ModelControllerTests::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_ModelControllerTests.data,
      qt_meta_data_ModelControllerTests,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *ModelControllerTests::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ModelControllerTests::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_ModelControllerTests.stringdata0))
        return static_cast<void*>(const_cast< ModelControllerTests*>(this));
    return QObject::qt_metacast(_clname);
}

int ModelControllerTests::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void ModelControllerTests::setup_controller()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}

// SIGNAL 1
void ModelControllerTests::teardown_controller()
{
    QMetaObject::activate(this, &staticMetaObject, 1, Q_NULLPTR);
}

// SIGNAL 2
void ModelControllerTests::open_project(QString _t1, QString _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_END_MOC_NAMESPACE
