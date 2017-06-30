/****************************************************************************
** Meta object code from reading C++ file 'dataeditwidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/Widgets/dataeditwidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'dataeditwidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_DataEditWidget_t {
    QByteArrayData data[11];
    char stringdata0[105];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_DataEditWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_DataEditWidget_t qt_meta_stringdata_DataEditWidget = {
    {
QT_MOC_LITERAL(0, 0, 14), // "DataEditWidget"
QT_MOC_LITERAL(1, 15, 12), // "valueChanged"
QT_MOC_LITERAL(2, 28, 0), // ""
QT_MOC_LITERAL(3, 29, 7), // "datakey"
QT_MOC_LITERAL(4, 37, 4), // "data"
QT_MOC_LITERAL(5, 42, 12), // "themeChanged"
QT_MOC_LITERAL(6, 55, 11), // "dataChanged"
QT_MOC_LITERAL(7, 67, 5), // "value"
QT_MOC_LITERAL(8, 73, 12), // "editFinished"
QT_MOC_LITERAL(9, 86, 9), // "pickColor"
QT_MOC_LITERAL(10, 96, 8) // "pickPath"

    },
    "DataEditWidget\0valueChanged\0\0datakey\0"
    "data\0themeChanged\0dataChanged\0value\0"
    "editFinished\0pickColor\0pickPath"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_DataEditWidget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   44,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    0,   49,    2, 0x08 /* Private */,
       6,    1,   50,    2, 0x08 /* Private */,
       8,    0,   53,    2, 0x08 /* Private */,
       9,    0,   54,    2, 0x08 /* Private */,
      10,    0,   55,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::QVariant,    3,    4,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QVariant,    7,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void DataEditWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        DataEditWidget *_t = static_cast<DataEditWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->valueChanged((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QVariant(*)>(_a[2]))); break;
        case 1: _t->themeChanged(); break;
        case 2: _t->dataChanged((*reinterpret_cast< QVariant(*)>(_a[1]))); break;
        case 3: _t->editFinished(); break;
        case 4: _t->pickColor(); break;
        case 5: _t->pickPath(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (DataEditWidget::*_t)(QString , QVariant );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&DataEditWidget::valueChanged)) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject DataEditWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_DataEditWidget.data,
      qt_meta_data_DataEditWidget,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *DataEditWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DataEditWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_DataEditWidget.stringdata0))
        return static_cast<void*>(const_cast< DataEditWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int DataEditWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
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
void DataEditWidget::valueChanged(QString _t1, QVariant _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
