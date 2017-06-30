/****************************************************************************
** Meta object code from reading C++ file 'modelcontroller.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/ModelController/modelcontroller.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QList>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'modelcontroller.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_ModelController_t {
    QByteArrayData data[75];
    char stringdata0[821];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ModelController_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ModelController_t qt_meta_stringdata_ModelController = {
    {
QT_MOC_LITERAL(0, 0, 15), // "ModelController"
QT_MOC_LITERAL(1, 16, 9), // "highlight"
QT_MOC_LITERAL(2, 26, 0), // ""
QT_MOC_LITERAL(3, 27, 10), // "QList<int>"
QT_MOC_LITERAL(4, 38, 3), // "ids"
QT_MOC_LITERAL(5, 42, 16), // "ActionProcessing"
QT_MOC_LITERAL(6, 59, 7), // "running"
QT_MOC_LITERAL(7, 67, 7), // "success"
QT_MOC_LITERAL(8, 75, 12), // "error_string"
QT_MOC_LITERAL(9, 88, 15), // "NodeConstructed"
QT_MOC_LITERAL(10, 104, 9), // "parent_id"
QT_MOC_LITERAL(11, 114, 2), // "id"
QT_MOC_LITERAL(12, 117, 9), // "NODE_KIND"
QT_MOC_LITERAL(13, 127, 4), // "kind"
QT_MOC_LITERAL(14, 132, 15), // "EdgeConstructed"
QT_MOC_LITERAL(15, 148, 9), // "EDGE_KIND"
QT_MOC_LITERAL(16, 158, 6), // "src_id"
QT_MOC_LITERAL(17, 165, 6), // "dst_id"
QT_MOC_LITERAL(18, 172, 16), // "EntityDestructed"
QT_MOC_LITERAL(19, 189, 2), // "ID"
QT_MOC_LITERAL(20, 192, 12), // "GRAPHML_KIND"
QT_MOC_LITERAL(21, 205, 16), // "InitiateTeardown"
QT_MOC_LITERAL(22, 222, 15), // "ProjectModified"
QT_MOC_LITERAL(23, 238, 8), // "modified"
QT_MOC_LITERAL(24, 247, 11), // "DataChanged"
QT_MOC_LITERAL(25, 259, 7), // "keyName"
QT_MOC_LITERAL(26, 267, 4), // "data"
QT_MOC_LITERAL(27, 272, 11), // "DataRemoved"
QT_MOC_LITERAL(28, 284, 18), // "ProjectFileChanged"
QT_MOC_LITERAL(29, 303, 18), // "ProjectNameChanged"
QT_MOC_LITERAL(30, 322, 15), // "UndoRedoUpdated"
QT_MOC_LITERAL(31, 338, 10), // "ModelReady"
QT_MOC_LITERAL(32, 349, 5), // "ready"
QT_MOC_LITERAL(33, 355, 15), // "ProgressChanged"
QT_MOC_LITERAL(34, 371, 12), // "ShowProgress"
QT_MOC_LITERAL(35, 384, 14), // "ActionFinished"
QT_MOC_LITERAL(36, 399, 16), // "SetClipboardData"
QT_MOC_LITERAL(37, 416, 12), // "Notification"
QT_MOC_LITERAL(38, 429, 14), // "MODEL_SEVERITY"
QT_MOC_LITERAL(39, 444, 8), // "severity"
QT_MOC_LITERAL(40, 453, 11), // "description"
QT_MOC_LITERAL(41, 465, 9), // "entity_id"
QT_MOC_LITERAL(42, 475, 15), // "SetupController"
QT_MOC_LITERAL(43, 491, 9), // "file_path"
QT_MOC_LITERAL(44, 501, 3), // "cut"
QT_MOC_LITERAL(45, 505, 4), // "copy"
QT_MOC_LITERAL(46, 510, 5), // "paste"
QT_MOC_LITERAL(47, 516, 3), // "xml"
QT_MOC_LITERAL(48, 520, 9), // "replicate"
QT_MOC_LITERAL(49, 530, 6), // "remove"
QT_MOC_LITERAL(50, 537, 4), // "undo"
QT_MOC_LITERAL(51, 542, 4), // "redo"
QT_MOC_LITERAL(52, 547, 14), // "importProjects"
QT_MOC_LITERAL(53, 562, 11), // "xmlDataList"
QT_MOC_LITERAL(54, 574, 7), // "setData"
QT_MOC_LITERAL(55, 582, 8), // "key_name"
QT_MOC_LITERAL(56, 591, 5), // "value"
QT_MOC_LITERAL(57, 597, 10), // "removeData"
QT_MOC_LITERAL(58, 608, 13), // "constructNode"
QT_MOC_LITERAL(59, 622, 8), // "parentID"
QT_MOC_LITERAL(60, 631, 11), // "centerPoint"
QT_MOC_LITERAL(61, 643, 13), // "constructEdge"
QT_MOC_LITERAL(62, 657, 6), // "srcIDs"
QT_MOC_LITERAL(63, 664, 5), // "dstID"
QT_MOC_LITERAL(64, 670, 9), // "edgeClass"
QT_MOC_LITERAL(65, 680, 22), // "constructWorkerProcess"
QT_MOC_LITERAL(66, 703, 22), // "constructConnectedNode"
QT_MOC_LITERAL(67, 726, 8), // "nodeKind"
QT_MOC_LITERAL(68, 735, 8), // "edgeKind"
QT_MOC_LITERAL(69, 744, 3), // "pos"
QT_MOC_LITERAL(70, 748, 13), // "destructEdges"
QT_MOC_LITERAL(71, 762, 16), // "destructAllEdges"
QT_MOC_LITERAL(72, 779, 13), // "triggerAction"
QT_MOC_LITERAL(73, 793, 10), // "actionName"
QT_MOC_LITERAL(74, 804, 16) // "ModelNameChanged"

    },
    "ModelController\0highlight\0\0QList<int>\0"
    "ids\0ActionProcessing\0running\0success\0"
    "error_string\0NodeConstructed\0parent_id\0"
    "id\0NODE_KIND\0kind\0EdgeConstructed\0"
    "EDGE_KIND\0src_id\0dst_id\0EntityDestructed\0"
    "ID\0GRAPHML_KIND\0InitiateTeardown\0"
    "ProjectModified\0modified\0DataChanged\0"
    "keyName\0data\0DataRemoved\0ProjectFileChanged\0"
    "ProjectNameChanged\0UndoRedoUpdated\0"
    "ModelReady\0ready\0ProgressChanged\0"
    "ShowProgress\0ActionFinished\0"
    "SetClipboardData\0Notification\0"
    "MODEL_SEVERITY\0severity\0description\0"
    "entity_id\0SetupController\0file_path\0"
    "cut\0copy\0paste\0xml\0replicate\0remove\0"
    "undo\0redo\0importProjects\0xmlDataList\0"
    "setData\0key_name\0value\0removeData\0"
    "constructNode\0parentID\0centerPoint\0"
    "constructEdge\0srcIDs\0dstID\0edgeClass\0"
    "constructWorkerProcess\0constructConnectedNode\0"
    "nodeKind\0edgeKind\0pos\0destructEdges\0"
    "destructAllEdges\0triggerAction\0"
    "actionName\0ModelNameChanged"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ModelController[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      43,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      22,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,  229,    2, 0x06 /* Public */,
       5,    3,  232,    2, 0x06 /* Public */,
       5,    2,  239,    2, 0x26 /* Public | MethodCloned */,
       5,    1,  244,    2, 0x26 /* Public | MethodCloned */,
       9,    3,  247,    2, 0x06 /* Public */,
      14,    4,  254,    2, 0x06 /* Public */,
      18,    2,  263,    2, 0x06 /* Public */,
      21,    0,  268,    2, 0x06 /* Public */,
      22,    1,  269,    2, 0x06 /* Public */,
      24,    3,  272,    2, 0x06 /* Public */,
      27,    2,  279,    2, 0x06 /* Public */,
      28,    1,  284,    2, 0x06 /* Public */,
      29,    1,  287,    2, 0x06 /* Public */,
      30,    0,  290,    2, 0x06 /* Public */,
      31,    1,  291,    2, 0x06 /* Public */,
      33,    1,  294,    2, 0x06 /* Public */,
      34,    2,  297,    2, 0x06 /* Public */,
      34,    1,  302,    2, 0x26 /* Public | MethodCloned */,
      35,    0,  305,    2, 0x06 /* Public */,
      36,    1,  306,    2, 0x06 /* Public */,
      37,    3,  309,    2, 0x06 /* Public */,
      37,    2,  316,    2, 0x26 /* Public | MethodCloned */,

 // slots: name, argc, parameters, tag, flags
      42,    1,  321,    2, 0x0a /* Public */,
      42,    0,  324,    2, 0x2a /* Public | MethodCloned */,
      44,    1,  325,    2, 0x0a /* Public */,
      45,    1,  328,    2, 0x0a /* Public */,
      46,    2,  331,    2, 0x0a /* Public */,
      48,    1,  336,    2, 0x0a /* Public */,
      49,    1,  339,    2, 0x0a /* Public */,
      50,    0,  342,    2, 0x0a /* Public */,
      51,    0,  343,    2, 0x0a /* Public */,
      52,    1,  344,    2, 0x0a /* Public */,
      54,    3,  347,    2, 0x0a /* Public */,
      57,    2,  354,    2, 0x0a /* Public */,
      58,    3,  359,    2, 0x0a /* Public */,
      61,    3,  366,    2, 0x0a /* Public */,
      65,    3,  373,    2, 0x0a /* Public */,
      66,    5,  380,    2, 0x0a /* Public */,
      66,    4,  391,    2, 0x2a /* Public | MethodCloned */,
      70,    3,  400,    2, 0x0a /* Public */,
      71,    2,  407,    2, 0x0a /* Public */,
      72,    1,  412,    2, 0x0a /* Public */,
      74,    0,  415,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, QMetaType::Bool, QMetaType::Bool, QMetaType::QString,    6,    7,    8,
    QMetaType::Void, QMetaType::Bool, QMetaType::Bool,    6,    7,
    QMetaType::Void, QMetaType::Bool,    6,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, 0x80000000 | 12,   10,   11,   13,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 15, QMetaType::Int, QMetaType::Int,   11,   13,   16,   17,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 20,   19,   13,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   23,
    QMetaType::Void, QMetaType::Int, QMetaType::QString, QMetaType::QVariant,   19,   25,   26,
    QMetaType::Void, QMetaType::Int, QMetaType::QString,   19,   25,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   32,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,    2,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, 0x80000000 | 38, QMetaType::QString, QMetaType::Int,   39,   40,   41,
    QMetaType::Void, 0x80000000 | 38, QMetaType::QString,   39,   40,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,   43,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3, QMetaType::QString,    4,   47,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QStringList,   53,
    QMetaType::Void, QMetaType::Int, QMetaType::QString, QMetaType::QVariant,   11,   55,   56,
    QMetaType::Void, QMetaType::Int, QMetaType::QString,   11,   55,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 12, QMetaType::QPointF,   59,   13,   60,
    QMetaType::Void, 0x80000000 | 3, QMetaType::Int, 0x80000000 | 15,   62,   63,   64,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::QPointF,   10,   17,   60,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 12, QMetaType::Int, 0x80000000 | 15, QMetaType::QPointF,   59,   67,   63,   68,   69,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 12, QMetaType::Int, 0x80000000 | 15,   59,   67,   63,   68,
    QMetaType::Void, 0x80000000 | 3, QMetaType::Int, 0x80000000 | 15,   62,   63,   64,
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 15,   62,   64,
    QMetaType::Void, QMetaType::QString,   73,
    QMetaType::Void,

       0        // eod
};

void ModelController::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ModelController *_t = static_cast<ModelController *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->highlight((*reinterpret_cast< QList<int>(*)>(_a[1]))); break;
        case 1: _t->ActionProcessing((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3]))); break;
        case 2: _t->ActionProcessing((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 3: _t->ActionProcessing((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: _t->NodeConstructed((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< NODE_KIND(*)>(_a[3]))); break;
        case 5: _t->EdgeConstructed((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< EDGE_KIND(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4]))); break;
        case 6: _t->EntityDestructed((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< GRAPHML_KIND(*)>(_a[2]))); break;
        case 7: _t->InitiateTeardown(); break;
        case 8: _t->ProjectModified((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 9: _t->DataChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QVariant(*)>(_a[3]))); break;
        case 10: _t->DataRemoved((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 11: _t->ProjectFileChanged((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 12: _t->ProjectNameChanged((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 13: _t->UndoRedoUpdated(); break;
        case 14: _t->ModelReady((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 15: _t->ProgressChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 16: _t->ShowProgress((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 17: _t->ShowProgress((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 18: _t->ActionFinished(); break;
        case 19: _t->SetClipboardData((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 20: _t->Notification((*reinterpret_cast< MODEL_SEVERITY(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 21: _t->Notification((*reinterpret_cast< MODEL_SEVERITY(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 22: _t->SetupController((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 23: _t->SetupController(); break;
        case 24: _t->cut((*reinterpret_cast< QList<int>(*)>(_a[1]))); break;
        case 25: _t->copy((*reinterpret_cast< QList<int>(*)>(_a[1]))); break;
        case 26: _t->paste((*reinterpret_cast< QList<int>(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 27: _t->replicate((*reinterpret_cast< QList<int>(*)>(_a[1]))); break;
        case 28: _t->remove((*reinterpret_cast< QList<int>(*)>(_a[1]))); break;
        case 29: _t->undo(); break;
        case 30: _t->redo(); break;
        case 31: _t->importProjects((*reinterpret_cast< QStringList(*)>(_a[1]))); break;
        case 32: _t->setData((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QVariant(*)>(_a[3]))); break;
        case 33: _t->removeData((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 34: _t->constructNode((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< NODE_KIND(*)>(_a[2])),(*reinterpret_cast< QPointF(*)>(_a[3]))); break;
        case 35: _t->constructEdge((*reinterpret_cast< QList<int>(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< EDGE_KIND(*)>(_a[3]))); break;
        case 36: _t->constructWorkerProcess((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< QPointF(*)>(_a[3]))); break;
        case 37: _t->constructConnectedNode((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< NODE_KIND(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< EDGE_KIND(*)>(_a[4])),(*reinterpret_cast< QPointF(*)>(_a[5]))); break;
        case 38: _t->constructConnectedNode((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< NODE_KIND(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< EDGE_KIND(*)>(_a[4]))); break;
        case 39: _t->destructEdges((*reinterpret_cast< QList<int>(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< EDGE_KIND(*)>(_a[3]))); break;
        case 40: _t->destructAllEdges((*reinterpret_cast< QList<int>(*)>(_a[1])),(*reinterpret_cast< EDGE_KIND(*)>(_a[2]))); break;
        case 41: _t->triggerAction((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 42: _t->ModelNameChanged(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QList<int> >(); break;
            }
            break;
        case 4:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 2:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< NODE_KIND >(); break;
            }
            break;
        case 5:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< EDGE_KIND >(); break;
            }
            break;
        case 6:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< GRAPHML_KIND >(); break;
            }
            break;
        case 24:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QList<int> >(); break;
            }
            break;
        case 25:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QList<int> >(); break;
            }
            break;
        case 26:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QList<int> >(); break;
            }
            break;
        case 27:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QList<int> >(); break;
            }
            break;
        case 28:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QList<int> >(); break;
            }
            break;
        case 34:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< NODE_KIND >(); break;
            }
            break;
        case 35:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 2:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< EDGE_KIND >(); break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QList<int> >(); break;
            }
            break;
        case 37:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 3:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< EDGE_KIND >(); break;
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< NODE_KIND >(); break;
            }
            break;
        case 38:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 3:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< EDGE_KIND >(); break;
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< NODE_KIND >(); break;
            }
            break;
        case 39:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 2:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< EDGE_KIND >(); break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QList<int> >(); break;
            }
            break;
        case 40:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< EDGE_KIND >(); break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QList<int> >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (ModelController::*_t)(QList<int> );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ModelController::highlight)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (ModelController::*_t)(bool , bool , QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ModelController::ActionProcessing)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (ModelController::*_t)(int , int , NODE_KIND );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ModelController::NodeConstructed)) {
                *result = 4;
                return;
            }
        }
        {
            typedef void (ModelController::*_t)(int , EDGE_KIND , int , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ModelController::EdgeConstructed)) {
                *result = 5;
                return;
            }
        }
        {
            typedef void (ModelController::*_t)(int , GRAPHML_KIND );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ModelController::EntityDestructed)) {
                *result = 6;
                return;
            }
        }
        {
            typedef void (ModelController::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ModelController::InitiateTeardown)) {
                *result = 7;
                return;
            }
        }
        {
            typedef void (ModelController::*_t)(bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ModelController::ProjectModified)) {
                *result = 8;
                return;
            }
        }
        {
            typedef void (ModelController::*_t)(int , QString , QVariant );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ModelController::DataChanged)) {
                *result = 9;
                return;
            }
        }
        {
            typedef void (ModelController::*_t)(int , QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ModelController::DataRemoved)) {
                *result = 10;
                return;
            }
        }
        {
            typedef void (ModelController::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ModelController::ProjectFileChanged)) {
                *result = 11;
                return;
            }
        }
        {
            typedef void (ModelController::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ModelController::ProjectNameChanged)) {
                *result = 12;
                return;
            }
        }
        {
            typedef void (ModelController::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ModelController::UndoRedoUpdated)) {
                *result = 13;
                return;
            }
        }
        {
            typedef void (ModelController::*_t)(bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ModelController::ModelReady)) {
                *result = 14;
                return;
            }
        }
        {
            typedef void (ModelController::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ModelController::ProgressChanged)) {
                *result = 15;
                return;
            }
        }
        {
            typedef void (ModelController::*_t)(bool , QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ModelController::ShowProgress)) {
                *result = 16;
                return;
            }
        }
        {
            typedef void (ModelController::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ModelController::ActionFinished)) {
                *result = 18;
                return;
            }
        }
        {
            typedef void (ModelController::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ModelController::SetClipboardData)) {
                *result = 19;
                return;
            }
        }
        {
            typedef void (ModelController::*_t)(MODEL_SEVERITY , QString , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ModelController::Notification)) {
                *result = 20;
                return;
            }
        }
    }
}

const QMetaObject ModelController::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_ModelController.data,
      qt_meta_data_ModelController,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *ModelController::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ModelController::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_ModelController.stringdata0))
        return static_cast<void*>(const_cast< ModelController*>(this));
    return QObject::qt_metacast(_clname);
}

int ModelController::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 43)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 43;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 43)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 43;
    }
    return _id;
}

// SIGNAL 0
void ModelController::highlight(QList<int> _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ModelController::ActionProcessing(bool _t1, bool _t2, QString _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 4
void ModelController::NodeConstructed(int _t1, int _t2, NODE_KIND _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void ModelController::EdgeConstructed(int _t1, EDGE_KIND _t2, int _t3, int _t4)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void ModelController::EntityDestructed(int _t1, GRAPHML_KIND _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void ModelController::InitiateTeardown()
{
    QMetaObject::activate(this, &staticMetaObject, 7, Q_NULLPTR);
}

// SIGNAL 8
void ModelController::ProjectModified(bool _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void ModelController::DataChanged(int _t1, QString _t2, QVariant _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}

// SIGNAL 10
void ModelController::DataRemoved(int _t1, QString _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 10, _a);
}

// SIGNAL 11
void ModelController::ProjectFileChanged(QString _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 11, _a);
}

// SIGNAL 12
void ModelController::ProjectNameChanged(QString _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 12, _a);
}

// SIGNAL 13
void ModelController::UndoRedoUpdated()
{
    QMetaObject::activate(this, &staticMetaObject, 13, Q_NULLPTR);
}

// SIGNAL 14
void ModelController::ModelReady(bool _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 14, _a);
}

// SIGNAL 15
void ModelController::ProgressChanged(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 15, _a);
}

// SIGNAL 16
void ModelController::ShowProgress(bool _t1, QString _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 16, _a);
}

// SIGNAL 18
void ModelController::ActionFinished()
{
    QMetaObject::activate(this, &staticMetaObject, 18, Q_NULLPTR);
}

// SIGNAL 19
void ModelController::SetClipboardData(QString _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 19, _a);
}

// SIGNAL 20
void ModelController::Notification(MODEL_SEVERITY _t1, QString _t2, int _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 20, _a);
}
QT_END_MOC_NAMESPACE
