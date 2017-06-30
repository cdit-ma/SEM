/****************************************************************************
** Meta object code from reading C++ file 'actioncontroller.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/Controllers/ActionController/actioncontroller.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'actioncontroller.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_ActionController_t {
    QByteArrayData data[27];
    char stringdata0[352];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ActionController_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ActionController_t qt_meta_stringdata_ActionController = {
    {
QT_MOC_LITERAL(0, 0, 16), // "ActionController"
QT_MOC_LITERAL(1, 17, 21), // "recentProjectsUpdated"
QT_MOC_LITERAL(2, 39, 0), // ""
QT_MOC_LITERAL(3, 40, 18), // "showShortcutDialog"
QT_MOC_LITERAL(4, 59, 19), // "clearRecentProjects"
QT_MOC_LITERAL(5, 79, 20), // "updateRecentProjects"
QT_MOC_LITERAL(6, 100, 8), // "filePath"
QT_MOC_LITERAL(7, 109, 14), // "settingChanged"
QT_MOC_LITERAL(8, 124, 11), // "SETTING_KEY"
QT_MOC_LITERAL(9, 136, 3), // "key"
QT_MOC_LITERAL(10, 140, 5), // "value"
QT_MOC_LITERAL(11, 146, 16), // "jenkinsValidated"
QT_MOC_LITERAL(12, 163, 7), // "success"
QT_MOC_LITERAL(13, 171, 7), // "gotJava"
QT_MOC_LITERAL(14, 179, 4), // "java"
QT_MOC_LITERAL(15, 184, 16), // "selectionChanged"
QT_MOC_LITERAL(16, 201, 13), // "selectionSize"
QT_MOC_LITERAL(17, 215, 14), // "actionFinished"
QT_MOC_LITERAL(18, 230, 15), // "controllerReady"
QT_MOC_LITERAL(19, 246, 10), // "modelReady"
QT_MOC_LITERAL(20, 257, 5), // "ready"
QT_MOC_LITERAL(21, 263, 12), // "themeChanged"
QT_MOC_LITERAL(22, 276, 20), // "updateJenkinsActions"
QT_MOC_LITERAL(23, 297, 14), // "updateUndoRedo"
QT_MOC_LITERAL(24, 312, 16), // "getSettingAction"
QT_MOC_LITERAL(25, 329, 8), // "QAction*"
QT_MOC_LITERAL(26, 338, 13) // "updateActions"

    },
    "ActionController\0recentProjectsUpdated\0"
    "\0showShortcutDialog\0clearRecentProjects\0"
    "updateRecentProjects\0filePath\0"
    "settingChanged\0SETTING_KEY\0key\0value\0"
    "jenkinsValidated\0success\0gotJava\0java\0"
    "selectionChanged\0selectionSize\0"
    "actionFinished\0controllerReady\0"
    "modelReady\0ready\0themeChanged\0"
    "updateJenkinsActions\0updateUndoRedo\0"
    "getSettingAction\0QAction*\0updateActions"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ActionController[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      16,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   94,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    0,   95,    2, 0x08 /* Private */,
       4,    0,   96,    2, 0x08 /* Private */,
       5,    1,   97,    2, 0x08 /* Private */,
       7,    2,  100,    2, 0x08 /* Private */,
      11,    1,  105,    2, 0x08 /* Private */,
      13,    1,  108,    2, 0x08 /* Private */,
      15,    1,  111,    2, 0x08 /* Private */,
      17,    0,  114,    2, 0x08 /* Private */,
      18,    1,  115,    2, 0x08 /* Private */,
      19,    1,  118,    2, 0x08 /* Private */,
      21,    0,  121,    2, 0x08 /* Private */,
      22,    0,  122,    2, 0x08 /* Private */,
      23,    0,  123,    2, 0x08 /* Private */,
      24,    1,  124,    2, 0x08 /* Private */,
      26,    0,  127,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    6,
    QMetaType::Void, 0x80000000 | 8, QMetaType::QVariant,    9,   10,
    QMetaType::Void, QMetaType::Bool,   12,
    QMetaType::Void, QMetaType::Bool,   14,
    QMetaType::Void, QMetaType::Int,   16,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,   20,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    0x80000000 | 25, 0x80000000 | 8,    9,
    QMetaType::Void,

       0        // eod
};

void ActionController::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ActionController *_t = static_cast<ActionController *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->recentProjectsUpdated(); break;
        case 1: _t->showShortcutDialog(); break;
        case 2: _t->clearRecentProjects(); break;
        case 3: _t->updateRecentProjects((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 4: _t->settingChanged((*reinterpret_cast< SETTING_KEY(*)>(_a[1])),(*reinterpret_cast< QVariant(*)>(_a[2]))); break;
        case 5: _t->jenkinsValidated((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 6: _t->gotJava((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 7: _t->selectionChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: _t->actionFinished(); break;
        case 9: _t->controllerReady((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 10: _t->modelReady((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 11: _t->themeChanged(); break;
        case 12: _t->updateJenkinsActions(); break;
        case 13: _t->updateUndoRedo(); break;
        case 14: { QAction* _r = _t->getSettingAction((*reinterpret_cast< SETTING_KEY(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QAction**>(_a[0]) = _r; }  break;
        case 15: _t->updateActions(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (ActionController::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ActionController::recentProjectsUpdated)) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject ActionController::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_ActionController.data,
      qt_meta_data_ActionController,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *ActionController::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ActionController::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_ActionController.stringdata0))
        return static_cast<void*>(const_cast< ActionController*>(this));
    return QObject::qt_metacast(_clname);
}

int ActionController::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 16)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 16;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 16)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 16;
    }
    return _id;
}

// SIGNAL 0
void ActionController::recentProjectsUpdated()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
