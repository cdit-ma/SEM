/****************************************************************************
** Meta object code from reading C++ file 'jenkinsjobmonitorwidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/Widgets/Jenkins/jenkinsjobmonitorwidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'jenkinsjobmonitorwidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_JenkinsJobMonitorWidget_t {
    QByteArrayData data[20];
    char stringdata0[235];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_JenkinsJobMonitorWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_JenkinsJobMonitorWidget_t qt_meta_stringdata_JenkinsJobMonitorWidget = {
    {
QT_MOC_LITERAL(0, 0, 23), // "JenkinsJobMonitorWidget"
QT_MOC_LITERAL(1, 24, 19), // "getJobConsoleOutput"
QT_MOC_LITERAL(2, 44, 0), // ""
QT_MOC_LITERAL(3, 45, 7), // "jobName"
QT_MOC_LITERAL(4, 53, 11), // "buildNumber"
QT_MOC_LITERAL(5, 65, 19), // "activeConfiguration"
QT_MOC_LITERAL(6, 85, 11), // "getJobState"
QT_MOC_LITERAL(7, 97, 7), // "stopJob"
QT_MOC_LITERAL(8, 105, 7), // "gotoURL"
QT_MOC_LITERAL(9, 113, 3), // "url"
QT_MOC_LITERAL(10, 117, 17), // "gotJobStateChange"
QT_MOC_LITERAL(11, 135, 9), // "JOB_STATE"
QT_MOC_LITERAL(12, 145, 8), // "jobState"
QT_MOC_LITERAL(13, 154, 19), // "gotJobConsoleOutput"
QT_MOC_LITERAL(14, 174, 13), // "consoleOutput"
QT_MOC_LITERAL(15, 188, 12), // "themeChanged"
QT_MOC_LITERAL(16, 201, 12), // "frameChanged"
QT_MOC_LITERAL(17, 214, 5), // "frame"
QT_MOC_LITERAL(18, 220, 8), // "closeTab"
QT_MOC_LITERAL(19, 229, 5) // "tabID"

    },
    "JenkinsJobMonitorWidget\0getJobConsoleOutput\0"
    "\0jobName\0buildNumber\0activeConfiguration\0"
    "getJobState\0stopJob\0gotoURL\0url\0"
    "gotJobStateChange\0JOB_STATE\0jobState\0"
    "gotJobConsoleOutput\0consoleOutput\0"
    "themeChanged\0frameChanged\0frame\0"
    "closeTab\0tabID"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_JenkinsJobMonitorWidget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    3,   59,    2, 0x06 /* Public */,
       6,    3,   66,    2, 0x06 /* Public */,
       7,    3,   73,    2, 0x06 /* Public */,
       8,    1,   80,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      10,    4,   83,    2, 0x0a /* Public */,
      13,    4,   92,    2, 0x0a /* Public */,
      15,    0,  101,    2, 0x08 /* Private */,
      16,    1,  102,    2, 0x08 /* Private */,
      18,    1,  105,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::Int, QMetaType::QString,    3,    4,    5,
    QMetaType::Void, QMetaType::QString, QMetaType::Int, QMetaType::QString,    3,    4,    5,
    QMetaType::Void, QMetaType::QString, QMetaType::Int, QMetaType::QString,    3,    4,    5,
    QMetaType::Void, QMetaType::QString,    9,

 // slots: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::Int, QMetaType::QString, 0x80000000 | 11,    3,    4,    5,   12,
    QMetaType::Void, QMetaType::QString, QMetaType::Int, QMetaType::QString, QMetaType::QString,    3,    4,    5,   14,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   17,
    QMetaType::Void, QMetaType::Int,   19,

       0        // eod
};

void JenkinsJobMonitorWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        JenkinsJobMonitorWidget *_t = static_cast<JenkinsJobMonitorWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->getJobConsoleOutput((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3]))); break;
        case 1: _t->getJobState((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3]))); break;
        case 2: _t->stopJob((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3]))); break;
        case 3: _t->gotoURL((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 4: _t->gotJobStateChange((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3])),(*reinterpret_cast< JOB_STATE(*)>(_a[4]))); break;
        case 5: _t->gotJobConsoleOutput((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3])),(*reinterpret_cast< QString(*)>(_a[4]))); break;
        case 6: _t->themeChanged(); break;
        case 7: _t->frameChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: _t->closeTab((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (JenkinsJobMonitorWidget::*_t)(QString , int , QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&JenkinsJobMonitorWidget::getJobConsoleOutput)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (JenkinsJobMonitorWidget::*_t)(QString , int , QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&JenkinsJobMonitorWidget::getJobState)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (JenkinsJobMonitorWidget::*_t)(QString , int , QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&JenkinsJobMonitorWidget::stopJob)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (JenkinsJobMonitorWidget::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&JenkinsJobMonitorWidget::gotoURL)) {
                *result = 3;
                return;
            }
        }
    }
}

const QMetaObject JenkinsJobMonitorWidget::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_JenkinsJobMonitorWidget.data,
      qt_meta_data_JenkinsJobMonitorWidget,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *JenkinsJobMonitorWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *JenkinsJobMonitorWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_JenkinsJobMonitorWidget.stringdata0))
        return static_cast<void*>(const_cast< JenkinsJobMonitorWidget*>(this));
    return QDialog::qt_metacast(_clname);
}

int JenkinsJobMonitorWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void JenkinsJobMonitorWidget::getJobConsoleOutput(QString _t1, int _t2, QString _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void JenkinsJobMonitorWidget::getJobState(QString _t1, int _t2, QString _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void JenkinsJobMonitorWidget::stopJob(QString _t1, int _t2, QString _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void JenkinsJobMonitorWidget::gotoURL(QString _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_END_MOC_NAMESPACE
