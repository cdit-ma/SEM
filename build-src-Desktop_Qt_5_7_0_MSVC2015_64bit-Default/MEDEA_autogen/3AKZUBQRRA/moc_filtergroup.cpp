/****************************************************************************
** Meta object code from reading C++ file 'filtergroup.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/Utils/filtergroup.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'filtergroup.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_FilterGroup_t {
    QByteArrayData data[8];
    char stringdata0[107];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_FilterGroup_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_FilterGroup_t qt_meta_stringdata_FilterGroup = {
    {
QT_MOC_LITERAL(0, 0, 11), // "FilterGroup"
QT_MOC_LITERAL(1, 12, 14), // "filtersChanged"
QT_MOC_LITERAL(2, 27, 0), // ""
QT_MOC_LITERAL(3, 28, 4), // "keys"
QT_MOC_LITERAL(4, 33, 16), // "resetFilterGroup"
QT_MOC_LITERAL(5, 50, 12), // "themeChanged"
QT_MOC_LITERAL(6, 63, 15), // "filterTriggered"
QT_MOC_LITERAL(7, 79, 27) // "updateResetButtonVisibility"

    },
    "FilterGroup\0filtersChanged\0\0keys\0"
    "resetFilterGroup\0themeChanged\0"
    "filterTriggered\0updateResetButtonVisibility"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_FilterGroup[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   39,    2, 0x06 /* Public */,
       4,    0,   42,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    0,   43,    2, 0x0a /* Public */,
       6,    0,   44,    2, 0x0a /* Public */,
       7,    0,   45,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QStringList,    3,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void FilterGroup::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        FilterGroup *_t = static_cast<FilterGroup *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->filtersChanged((*reinterpret_cast< QStringList(*)>(_a[1]))); break;
        case 1: _t->resetFilterGroup(); break;
        case 2: _t->themeChanged(); break;
        case 3: _t->filterTriggered(); break;
        case 4: _t->updateResetButtonVisibility(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (FilterGroup::*_t)(QStringList );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&FilterGroup::filtersChanged)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (FilterGroup::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&FilterGroup::resetFilterGroup)) {
                *result = 1;
                return;
            }
        }
    }
}

const QMetaObject FilterGroup::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_FilterGroup.data,
      qt_meta_data_FilterGroup,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *FilterGroup::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *FilterGroup::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_FilterGroup.stringdata0))
        return static_cast<void*>(const_cast< FilterGroup*>(this));
    return QObject::qt_metacast(_clname);
}

int FilterGroup::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void FilterGroup::filtersChanged(QStringList _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void FilterGroup::resetFilterGroup()
{
    QMetaObject::activate(this, &staticMetaObject, 1, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
