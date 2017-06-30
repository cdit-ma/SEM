/****************************************************************************
** Meta object code from reading C++ file 'jenkinsrequest.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/Controllers/JenkinsManager/jenkinsrequest.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'jenkinsrequest.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_JenkinsRequest_t {
    QByteArrayData data[32];
    char stringdata0[442];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_JenkinsRequest_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_JenkinsRequest_t qt_meta_stringdata_JenkinsRequest = {
    {
QT_MOC_LITERAL(0, 0, 14), // "JenkinsRequest"
QT_MOC_LITERAL(1, 15, 16), // "GotJobParameters"
QT_MOC_LITERAL(2, 32, 0), // ""
QT_MOC_LITERAL(3, 33, 8), // "job_name"
QT_MOC_LITERAL(4, 42, 21), // "Jenkins_JobParameters"
QT_MOC_LITERAL(5, 64, 10), // "parameters"
QT_MOC_LITERAL(6, 75, 23), // "GotLiveJobConsoleOutput"
QT_MOC_LITERAL(7, 99, 12), // "build_number"
QT_MOC_LITERAL(8, 112, 13), // "configuration"
QT_MOC_LITERAL(9, 126, 20), // "console_output_chunk"
QT_MOC_LITERAL(10, 147, 19), // "GotJobConsoleOutput"
QT_MOC_LITERAL(11, 167, 14), // "console_output"
QT_MOC_LITERAL(12, 182, 17), // "GotJobStateChange"
QT_MOC_LITERAL(13, 200, 9), // "JOB_STATE"
QT_MOC_LITERAL(14, 210, 8), // "jobState"
QT_MOC_LITERAL(15, 219, 20), // "GotValidatedSettings"
QT_MOC_LITERAL(16, 240, 5), // "valid"
QT_MOC_LITERAL(17, 246, 13), // "error_message"
QT_MOC_LITERAL(18, 260, 21), // "GotGroovyScriptOutput"
QT_MOC_LITERAL(19, 282, 7), // "success"
QT_MOC_LITERAL(20, 290, 6), // "output"
QT_MOC_LITERAL(21, 297, 8), // "Finished"
QT_MOC_LITERAL(22, 306, 10), // "Terminate_"
QT_MOC_LITERAL(23, 317, 8), // "BuildJob"
QT_MOC_LITERAL(24, 326, 13), // "jobParameters"
QT_MOC_LITERAL(25, 340, 7), // "StopJob"
QT_MOC_LITERAL(26, 348, 19), // "GetJobConsoleOutput"
QT_MOC_LITERAL(27, 368, 16), // "GetJobParameters"
QT_MOC_LITERAL(28, 385, 15), // "RunGroovyScript"
QT_MOC_LITERAL(29, 401, 13), // "groovy_script"
QT_MOC_LITERAL(30, 415, 16), // "ValidateSettings"
QT_MOC_LITERAL(31, 432, 9) // "Teardown_"

    },
    "JenkinsRequest\0GotJobParameters\0\0"
    "job_name\0Jenkins_JobParameters\0"
    "parameters\0GotLiveJobConsoleOutput\0"
    "build_number\0configuration\0"
    "console_output_chunk\0GotJobConsoleOutput\0"
    "console_output\0GotJobStateChange\0"
    "JOB_STATE\0jobState\0GotValidatedSettings\0"
    "valid\0error_message\0GotGroovyScriptOutput\0"
    "success\0output\0Finished\0Terminate_\0"
    "BuildJob\0jobParameters\0StopJob\0"
    "GetJobConsoleOutput\0GetJobParameters\0"
    "RunGroovyScript\0groovy_script\0"
    "ValidateSettings\0Teardown_"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_JenkinsRequest[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      17,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       8,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   99,    2, 0x06 /* Public */,
       6,    4,  104,    2, 0x06 /* Public */,
      10,    4,  113,    2, 0x06 /* Public */,
      12,    4,  122,    2, 0x06 /* Public */,
      15,    2,  131,    2, 0x06 /* Public */,
      18,    2,  136,    2, 0x06 /* Public */,
      21,    0,  141,    2, 0x06 /* Public */,
      22,    0,  142,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      23,    2,  143,    2, 0x0a /* Public */,
      25,    3,  148,    2, 0x0a /* Public */,
      25,    2,  155,    2, 0x2a /* Public | MethodCloned */,
      26,    3,  160,    2, 0x0a /* Public */,
      26,    2,  167,    2, 0x2a /* Public | MethodCloned */,
      27,    1,  172,    2, 0x0a /* Public */,
      28,    1,  175,    2, 0x0a /* Public */,
      30,    0,  178,    2, 0x08 /* Private */,
      31,    0,  179,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString, 0x80000000 | 4,    3,    5,
    QMetaType::Void, QMetaType::QString, QMetaType::Int, QMetaType::QString, QMetaType::QString,    3,    7,    8,    9,
    QMetaType::Void, QMetaType::QString, QMetaType::Int, QMetaType::QString, QMetaType::QString,    3,    7,    8,   11,
    QMetaType::Void, QMetaType::QString, QMetaType::Int, QMetaType::QString, 0x80000000 | 13,    3,    7,    8,   14,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,   16,   17,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,   19,   20,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::QString, 0x80000000 | 4,    3,   24,
    QMetaType::Void, QMetaType::QString, QMetaType::Int, QMetaType::QString,    3,    7,    8,
    QMetaType::Void, QMetaType::QString, QMetaType::Int,    3,    7,
    QMetaType::Void, QMetaType::QString, QMetaType::Int, QMetaType::QString,    3,    7,    8,
    QMetaType::Void, QMetaType::QString, QMetaType::Int,    3,    7,
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString,   29,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void JenkinsRequest::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        JenkinsRequest *_t = static_cast<JenkinsRequest *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->GotJobParameters((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< Jenkins_JobParameters(*)>(_a[2]))); break;
        case 1: _t->GotLiveJobConsoleOutput((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3])),(*reinterpret_cast< QString(*)>(_a[4]))); break;
        case 2: _t->GotJobConsoleOutput((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3])),(*reinterpret_cast< QString(*)>(_a[4]))); break;
        case 3: _t->GotJobStateChange((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3])),(*reinterpret_cast< JOB_STATE(*)>(_a[4]))); break;
        case 4: _t->GotValidatedSettings((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 5: _t->GotGroovyScriptOutput((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 6: _t->Finished(); break;
        case 7: _t->Terminate_(); break;
        case 8: _t->BuildJob((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< Jenkins_JobParameters(*)>(_a[2]))); break;
        case 9: _t->StopJob((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3]))); break;
        case 10: _t->StopJob((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 11: _t->GetJobConsoleOutput((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3]))); break;
        case 12: _t->GetJobConsoleOutput((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 13: _t->GetJobParameters((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 14: _t->RunGroovyScript((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 15: _t->ValidateSettings(); break;
        case 16: _t->Teardown_(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (JenkinsRequest::*_t)(QString , Jenkins_JobParameters );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&JenkinsRequest::GotJobParameters)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (JenkinsRequest::*_t)(QString , int , QString , QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&JenkinsRequest::GotLiveJobConsoleOutput)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (JenkinsRequest::*_t)(QString , int , QString , QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&JenkinsRequest::GotJobConsoleOutput)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (JenkinsRequest::*_t)(QString , int , QString , JOB_STATE );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&JenkinsRequest::GotJobStateChange)) {
                *result = 3;
                return;
            }
        }
        {
            typedef void (JenkinsRequest::*_t)(bool , QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&JenkinsRequest::GotValidatedSettings)) {
                *result = 4;
                return;
            }
        }
        {
            typedef void (JenkinsRequest::*_t)(bool , QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&JenkinsRequest::GotGroovyScriptOutput)) {
                *result = 5;
                return;
            }
        }
        {
            typedef void (JenkinsRequest::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&JenkinsRequest::Finished)) {
                *result = 6;
                return;
            }
        }
        {
            typedef void (JenkinsRequest::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&JenkinsRequest::Terminate_)) {
                *result = 7;
                return;
            }
        }
    }
}

const QMetaObject JenkinsRequest::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_JenkinsRequest.data,
      qt_meta_data_JenkinsRequest,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *JenkinsRequest::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *JenkinsRequest::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_JenkinsRequest.stringdata0))
        return static_cast<void*>(const_cast< JenkinsRequest*>(this));
    return QObject::qt_metacast(_clname);
}

int JenkinsRequest::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 17)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 17;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 17)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 17;
    }
    return _id;
}

// SIGNAL 0
void JenkinsRequest::GotJobParameters(QString _t1, Jenkins_JobParameters _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void JenkinsRequest::GotLiveJobConsoleOutput(QString _t1, int _t2, QString _t3, QString _t4)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void JenkinsRequest::GotJobConsoleOutput(QString _t1, int _t2, QString _t3, QString _t4)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void JenkinsRequest::GotJobStateChange(QString _t1, int _t2, QString _t3, JOB_STATE _t4)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void JenkinsRequest::GotValidatedSettings(bool _t1, QString _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void JenkinsRequest::GotGroovyScriptOutput(bool _t1, QString _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void JenkinsRequest::Finished()
{
    QMetaObject::activate(this, &staticMetaObject, 6, Q_NULLPTR);
}

// SIGNAL 7
void JenkinsRequest::Terminate_()
{
    QMetaObject::activate(this, &staticMetaObject, 7, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
