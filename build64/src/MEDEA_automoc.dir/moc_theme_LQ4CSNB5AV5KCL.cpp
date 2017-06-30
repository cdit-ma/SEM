/****************************************************************************
** Meta object code from reading C++ file 'theme.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/theme.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'theme.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Theme_t {
    QByteArrayData data[13];
    char stringdata0[142];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Theme_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Theme_t qt_meta_stringdata_Theme = {
    {
QT_MOC_LITERAL(0, 0, 5), // "Theme"
QT_MOC_LITERAL(1, 6, 13), // "theme_Changed"
QT_MOC_LITERAL(2, 20, 0), // ""
QT_MOC_LITERAL(3, 21, 13), // "refresh_Icons"
QT_MOC_LITERAL(4, 35, 13), // "changeSetting"
QT_MOC_LITERAL(5, 49, 11), // "SETTING_KEY"
QT_MOC_LITERAL(6, 61, 7), // "setting"
QT_MOC_LITERAL(7, 69, 5), // "value"
QT_MOC_LITERAL(8, 75, 15), // "preloadFinished"
QT_MOC_LITERAL(9, 91, 8), // "_preload"
QT_MOC_LITERAL(10, 100, 13), // "preloadImages"
QT_MOC_LITERAL(11, 114, 14), // "settingChanged"
QT_MOC_LITERAL(12, 129, 12) // "clearIconMap"

    },
    "Theme\0theme_Changed\0\0refresh_Icons\0"
    "changeSetting\0SETTING_KEY\0setting\0"
    "value\0preloadFinished\0_preload\0"
    "preloadImages\0settingChanged\0clearIconMap"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Theme[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   54,    2, 0x06 /* Public */,
       3,    0,   55,    2, 0x06 /* Public */,
       4,    2,   56,    2, 0x06 /* Public */,
       8,    0,   61,    2, 0x06 /* Public */,
       9,    0,   62,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      10,    0,   63,    2, 0x0a /* Public */,
      11,    2,   64,    2, 0x0a /* Public */,
      12,    0,   69,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 5, QMetaType::QVariant,    6,    7,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 5, QMetaType::QVariant,    6,    7,
    QMetaType::Void,

       0        // eod
};

void Theme::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Theme *_t = static_cast<Theme *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->theme_Changed(); break;
        case 1: _t->refresh_Icons(); break;
        case 2: _t->changeSetting((*reinterpret_cast< SETTING_KEY(*)>(_a[1])),(*reinterpret_cast< QVariant(*)>(_a[2]))); break;
        case 3: _t->preloadFinished(); break;
        case 4: _t->_preload(); break;
        case 5: _t->preloadImages(); break;
        case 6: _t->settingChanged((*reinterpret_cast< SETTING_KEY(*)>(_a[1])),(*reinterpret_cast< QVariant(*)>(_a[2]))); break;
        case 7: _t->clearIconMap(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (Theme::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Theme::theme_Changed)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (Theme::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Theme::refresh_Icons)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (Theme::*_t)(SETTING_KEY , QVariant );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Theme::changeSetting)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (Theme::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Theme::preloadFinished)) {
                *result = 3;
                return;
            }
        }
        {
            typedef void (Theme::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Theme::_preload)) {
                *result = 4;
                return;
            }
        }
    }
}

const QMetaObject Theme::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Theme.data,
      qt_meta_data_Theme,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Theme::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Theme::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Theme.stringdata0))
        return static_cast<void*>(const_cast< Theme*>(this));
    return QObject::qt_metacast(_clname);
}

int Theme::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void Theme::theme_Changed()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}

// SIGNAL 1
void Theme::refresh_Icons()
{
    QMetaObject::activate(this, &staticMetaObject, 1, Q_NULLPTR);
}

// SIGNAL 2
void Theme::changeSetting(SETTING_KEY _t1, QVariant _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void Theme::preloadFinished()
{
    QMetaObject::activate(this, &staticMetaObject, 3, Q_NULLPTR);
}

// SIGNAL 4
void Theme::_preload()
{
    QMetaObject::activate(this, &staticMetaObject, 4, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
