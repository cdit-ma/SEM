/****************************************************************************
** Meta object code from reading C++ file 'jenkinsmanager.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/Controllers/JenkinsManager/jenkinsmanager.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'jenkinsmanager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_JenkinsManager_t {
    QByteArrayData data[19];
    char stringdata0[213];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_JenkinsManager_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_JenkinsManager_t qt_meta_stringdata_JenkinsManager = {
    {
QT_MOC_LITERAL(0, 0, 14), // "JenkinsManager"
QT_MOC_LITERAL(1, 15, 15), // "GotJenkinsNodes"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 4), // "data"
QT_MOC_LITERAL(4, 37, 12), // "JenkinsReady"
QT_MOC_LITERAL(5, 50, 5), // "ready"
QT_MOC_LITERAL(6, 56, 16), // "gotValidSettings"
QT_MOC_LITERAL(7, 73, 5), // "valid"
QT_MOC_LITERAL(8, 79, 7), // "message"
QT_MOC_LITERAL(9, 87, 17), // "ValidateSettings_"
QT_MOC_LITERAL(10, 105, 16), // "RunGroovyScript_"
QT_MOC_LITERAL(11, 122, 6), // "script"
QT_MOC_LITERAL(12, 129, 16), // "GotJenkinsNodes_"
QT_MOC_LITERAL(13, 146, 7), // "success"
QT_MOC_LITERAL(14, 154, 21), // "GotValidatedSettings_"
QT_MOC_LITERAL(15, 176, 14), // "SettingChanged"
QT_MOC_LITERAL(16, 191, 11), // "SETTING_KEY"
QT_MOC_LITERAL(17, 203, 3), // "key"
QT_MOC_LITERAL(18, 207, 5) // "value"

    },
    "JenkinsManager\0GotJenkinsNodes\0\0data\0"
    "JenkinsReady\0ready\0gotValidSettings\0"
    "valid\0message\0ValidateSettings_\0"
    "RunGroovyScript_\0script\0GotJenkinsNodes_\0"
    "success\0GotValidatedSettings_\0"
    "SettingChanged\0SETTING_KEY\0key\0value"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_JenkinsManager[] = {

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
       1,    1,   54,    2, 0x06 /* Public */,
       4,    1,   57,    2, 0x06 /* Public */,
       6,    2,   60,    2, 0x06 /* Public */,
       9,    0,   65,    2, 0x06 /* Public */,
      10,    1,   66,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      12,    2,   69,    2, 0x08 /* Private */,
      14,    2,   74,    2, 0x08 /* Private */,
      15,    2,   79,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::Bool,    5,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,    7,    8,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   11,

 // slots: parameters
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,   13,    3,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,    7,    8,
    QMetaType::Void, 0x80000000 | 16, QMetaType::QVariant,   17,   18,

       0        // eod
};

void JenkinsManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        JenkinsManager *_t = static_cast<JenkinsManager *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->GotJenkinsNodes((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: _t->JenkinsReady((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: _t->gotValidSettings((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 3: _t->ValidateSettings_(); break;
        case 4: _t->RunGroovyScript_((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 5: _t->GotJenkinsNodes_((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 6: _t->GotValidatedSettings_((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 7: _t->SettingChanged((*reinterpret_cast< SETTING_KEY(*)>(_a[1])),(*reinterpret_cast< QVariant(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (JenkinsManager::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&JenkinsManager::GotJenkinsNodes)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (JenkinsManager::*_t)(bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&JenkinsManager::JenkinsReady)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (JenkinsManager::*_t)(bool , QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&JenkinsManager::gotValidSettings)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (JenkinsManager::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&JenkinsManager::ValidateSettings_)) {
                *result = 3;
                return;
            }
        }
        {
            typedef void (JenkinsManager::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&JenkinsManager::RunGroovyScript_)) {
                *result = 4;
                return;
            }
        }
    }
}

const QMetaObject JenkinsManager::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_JenkinsManager.data,
      qt_meta_data_JenkinsManager,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *JenkinsManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *JenkinsManager::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_JenkinsManager.stringdata0))
        return static_cast<void*>(const_cast< JenkinsManager*>(this));
    return QObject::qt_metacast(_clname);
}

int JenkinsManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void JenkinsManager::GotJenkinsNodes(QString _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void JenkinsManager::JenkinsReady(bool _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void JenkinsManager::gotValidSettings(bool _t1, QString _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void JenkinsManager::ValidateSettings_()
{
    QMetaObject::activate(this, &staticMetaObject, 3, Q_NULLPTR);
}

// SIGNAL 4
void JenkinsManager::RunGroovyScript_(QString _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}
QT_END_MOC_NAMESPACE
