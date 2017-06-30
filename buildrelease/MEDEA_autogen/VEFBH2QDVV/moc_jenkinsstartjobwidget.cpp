/****************************************************************************
** Meta object code from reading C++ file 'jenkinsstartjobwidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/Widgets/Jenkins/jenkinsstartjobwidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'jenkinsstartjobwidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_JenkinsStartJobWidget_t {
    QByteArrayData data[10];
    char stringdata0[126];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_JenkinsStartJobWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_JenkinsStartJobWidget_t qt_meta_stringdata_JenkinsStartJobWidget = {
    {
QT_MOC_LITERAL(0, 0, 21), // "JenkinsStartJobWidget"
QT_MOC_LITERAL(1, 22, 16), // "getJobParameters"
QT_MOC_LITERAL(2, 39, 0), // ""
QT_MOC_LITERAL(3, 40, 7), // "jobName"
QT_MOC_LITERAL(4, 48, 8), // "buildJob"
QT_MOC_LITERAL(5, 57, 21), // "Jenkins_JobParameters"
QT_MOC_LITERAL(6, 79, 10), // "parameters"
QT_MOC_LITERAL(7, 90, 12), // "themeChanged"
QT_MOC_LITERAL(8, 103, 16), // "gotJobParameters"
QT_MOC_LITERAL(9, 120, 5) // "build"

    },
    "JenkinsStartJobWidget\0getJobParameters\0"
    "\0jobName\0buildJob\0Jenkins_JobParameters\0"
    "parameters\0themeChanged\0gotJobParameters\0"
    "build"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_JenkinsStartJobWidget[] = {

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
       4,    2,   42,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       7,    0,   47,    2, 0x0a /* Public */,
       8,    2,   48,    2, 0x0a /* Public */,
       9,    0,   53,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString, 0x80000000 | 5,    3,    6,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, 0x80000000 | 5,    3,    6,
    QMetaType::Void,

       0        // eod
};

void JenkinsStartJobWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        JenkinsStartJobWidget *_t = static_cast<JenkinsStartJobWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->getJobParameters((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: _t->buildJob((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< Jenkins_JobParameters(*)>(_a[2]))); break;
        case 2: _t->themeChanged(); break;
        case 3: _t->gotJobParameters((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< Jenkins_JobParameters(*)>(_a[2]))); break;
        case 4: _t->build(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (JenkinsStartJobWidget::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&JenkinsStartJobWidget::getJobParameters)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (JenkinsStartJobWidget::*_t)(QString , Jenkins_JobParameters );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&JenkinsStartJobWidget::buildJob)) {
                *result = 1;
                return;
            }
        }
    }
}

const QMetaObject JenkinsStartJobWidget::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_JenkinsStartJobWidget.data,
      qt_meta_data_JenkinsStartJobWidget,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *JenkinsStartJobWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *JenkinsStartJobWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_JenkinsStartJobWidget.stringdata0))
        return static_cast<void*>(const_cast< JenkinsStartJobWidget*>(this));
    return QDialog::qt_metacast(_clname);
}

int JenkinsStartJobWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
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
void JenkinsStartJobWidget::getJobParameters(QString _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void JenkinsStartJobWidget::buildJob(QString _t1, Jenkins_JobParameters _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
