/****************************************************************************
** Meta object code from reading C++ file 'searchdialog.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/Views/Search/searchdialog.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'searchdialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_SearchDialog_t {
    QByteArrayData data[18];
    char stringdata0[249];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_SearchDialog_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_SearchDialog_t qt_meta_stringdata_SearchDialog = {
    {
QT_MOC_LITERAL(0, 0, 12), // "SearchDialog"
QT_MOC_LITERAL(1, 13, 14), // "itemHoverEnter"
QT_MOC_LITERAL(2, 28, 0), // ""
QT_MOC_LITERAL(3, 29, 2), // "ID"
QT_MOC_LITERAL(4, 32, 14), // "itemHoverLeave"
QT_MOC_LITERAL(5, 47, 16), // "centerOnViewItem"
QT_MOC_LITERAL(6, 64, 13), // "popupViewItem"
QT_MOC_LITERAL(7, 78, 16), // "keyButtonChecked"
QT_MOC_LITERAL(8, 95, 3), // "key"
QT_MOC_LITERAL(9, 99, 19), // "searchButtonClicked"
QT_MOC_LITERAL(10, 119, 20), // "refreshButtonClicked"
QT_MOC_LITERAL(11, 140, 12), // "themeChanged"
QT_MOC_LITERAL(12, 153, 7), // "checked"
QT_MOC_LITERAL(13, 161, 18), // "searchItemSelected"
QT_MOC_LITERAL(14, 180, 18), // "viewItemDestructed"
QT_MOC_LITERAL(15, 199, 20), // "centerOnSelectedItem"
QT_MOC_LITERAL(16, 220, 17), // "popupSelectedItem"
QT_MOC_LITERAL(17, 238, 10) // "resetPanel"

    },
    "SearchDialog\0itemHoverEnter\0\0ID\0"
    "itemHoverLeave\0centerOnViewItem\0"
    "popupViewItem\0keyButtonChecked\0key\0"
    "searchButtonClicked\0refreshButtonClicked\0"
    "themeChanged\0checked\0searchItemSelected\0"
    "viewItemDestructed\0centerOnSelectedItem\0"
    "popupSelectedItem\0resetPanel"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SearchDialog[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       7,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   84,    2, 0x06 /* Public */,
       4,    1,   87,    2, 0x06 /* Public */,
       5,    1,   90,    2, 0x06 /* Public */,
       6,    1,   93,    2, 0x06 /* Public */,
       7,    1,   96,    2, 0x06 /* Public */,
       9,    0,   99,    2, 0x06 /* Public */,
      10,    0,  100,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      11,    0,  101,    2, 0x0a /* Public */,
       7,    1,  102,    2, 0x0a /* Public */,
      13,    1,  105,    2, 0x0a /* Public */,
      14,    1,  108,    2, 0x0a /* Public */,
      15,    0,  111,    2, 0x0a /* Public */,
      16,    0,  112,    2, 0x0a /* Public */,
      17,    0,  113,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::QString,    8,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   12,
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void SearchDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        SearchDialog *_t = static_cast<SearchDialog *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->itemHoverEnter((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->itemHoverLeave((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->centerOnViewItem((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->popupViewItem((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->keyButtonChecked((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 5: _t->searchButtonClicked(); break;
        case 6: _t->refreshButtonClicked(); break;
        case 7: _t->themeChanged(); break;
        case 8: _t->keyButtonChecked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 9: _t->searchItemSelected((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: _t->viewItemDestructed((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 11: _t->centerOnSelectedItem(); break;
        case 12: _t->popupSelectedItem(); break;
        case 13: _t->resetPanel(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (SearchDialog::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SearchDialog::itemHoverEnter)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (SearchDialog::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SearchDialog::itemHoverLeave)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (SearchDialog::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SearchDialog::centerOnViewItem)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (SearchDialog::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SearchDialog::popupViewItem)) {
                *result = 3;
                return;
            }
        }
        {
            typedef void (SearchDialog::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SearchDialog::keyButtonChecked)) {
                *result = 4;
                return;
            }
        }
        {
            typedef void (SearchDialog::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SearchDialog::searchButtonClicked)) {
                *result = 5;
                return;
            }
        }
        {
            typedef void (SearchDialog::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SearchDialog::refreshButtonClicked)) {
                *result = 6;
                return;
            }
        }
    }
}

const QMetaObject SearchDialog::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_SearchDialog.data,
      qt_meta_data_SearchDialog,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *SearchDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SearchDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_SearchDialog.stringdata0))
        return static_cast<void*>(const_cast< SearchDialog*>(this));
    return QWidget::qt_metacast(_clname);
}

int SearchDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 14)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 14)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 14;
    }
    return _id;
}

// SIGNAL 0
void SearchDialog::itemHoverEnter(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void SearchDialog::itemHoverLeave(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void SearchDialog::centerOnViewItem(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void SearchDialog::popupViewItem(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void SearchDialog::keyButtonChecked(QString _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void SearchDialog::searchButtonClicked()
{
    QMetaObject::activate(this, &staticMetaObject, 5, Q_NULLPTR);
}

// SIGNAL 6
void SearchDialog::refreshButtonClicked()
{
    QMetaObject::activate(this, &staticMetaObject, 6, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
