/****************************************************************************
** Meta object code from reading C++ file 'settingscontroller.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/Controllers/SettingsController/settingscontroller.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'settingscontroller.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_SettingsController_t {
    QByteArrayData data[11];
    char stringdata0[129];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_SettingsController_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_SettingsController_t qt_meta_stringdata_SettingsController = {
    {
QT_MOC_LITERAL(0, 0, 18), // "SettingsController"
QT_MOC_LITERAL(1, 19, 15), // "settingsApplied"
QT_MOC_LITERAL(2, 35, 0), // ""
QT_MOC_LITERAL(3, 36, 14), // "settingChanged"
QT_MOC_LITERAL(4, 51, 11), // "SETTING_KEY"
QT_MOC_LITERAL(5, 63, 2), // "ID"
QT_MOC_LITERAL(6, 66, 5), // "value"
QT_MOC_LITERAL(7, 72, 10), // "setSetting"
QT_MOC_LITERAL(8, 83, 18), // "showSettingsWidget"
QT_MOC_LITERAL(9, 102, 13), // "resetSettings"
QT_MOC_LITERAL(10, 116, 12) // "saveSettings"

    },
    "SettingsController\0settingsApplied\0\0"
    "settingChanged\0SETTING_KEY\0ID\0value\0"
    "setSetting\0showSettingsWidget\0"
    "resetSettings\0saveSettings"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SettingsController[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   44,    2, 0x06 /* Public */,
       3,    2,   45,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       7,    2,   50,    2, 0x0a /* Public */,
       8,    0,   55,    2, 0x0a /* Public */,
       9,    0,   56,    2, 0x0a /* Public */,
      10,    0,   57,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 4, QMetaType::QVariant,    5,    6,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 4, QMetaType::QVariant,    5,    6,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void SettingsController::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        SettingsController *_t = static_cast<SettingsController *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->settingsApplied(); break;
        case 1: _t->settingChanged((*reinterpret_cast< SETTING_KEY(*)>(_a[1])),(*reinterpret_cast< QVariant(*)>(_a[2]))); break;
        case 2: _t->setSetting((*reinterpret_cast< SETTING_KEY(*)>(_a[1])),(*reinterpret_cast< QVariant(*)>(_a[2]))); break;
        case 3: _t->showSettingsWidget(); break;
        case 4: _t->resetSettings(); break;
        case 5: _t->saveSettings(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (SettingsController::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SettingsController::settingsApplied)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (SettingsController::*_t)(SETTING_KEY , QVariant );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SettingsController::settingChanged)) {
                *result = 1;
                return;
            }
        }
    }
}

const QMetaObject SettingsController::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_SettingsController.data,
      qt_meta_data_SettingsController,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *SettingsController::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SettingsController::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_SettingsController.stringdata0))
        return static_cast<void*>(const_cast< SettingsController*>(this));
    return QObject::qt_metacast(_clname);
}

int SettingsController::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void SettingsController::settingsApplied()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}

// SIGNAL 1
void SettingsController::settingChanged(SETTING_KEY _t1, QVariant _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
