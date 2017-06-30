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
    QByteArrayData data[103];
    char stringdata0[1378];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ModelController_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ModelController_t qt_meta_stringdata_ModelController = {
    {
QT_MOC_LITERAL(0, 0, 15), // "ModelController"
QT_MOC_LITERAL(1, 16, 15), // "NodeConstructed"
QT_MOC_LITERAL(2, 32, 0), // ""
QT_MOC_LITERAL(3, 33, 9), // "parent_id"
QT_MOC_LITERAL(4, 43, 2), // "id"
QT_MOC_LITERAL(5, 46, 9), // "NODE_KIND"
QT_MOC_LITERAL(6, 56, 4), // "kind"
QT_MOC_LITERAL(7, 61, 15), // "EdgeConstructed"
QT_MOC_LITERAL(8, 77, 9), // "EDGE_KIND"
QT_MOC_LITERAL(9, 87, 6), // "src_id"
QT_MOC_LITERAL(10, 94, 6), // "dst_id"
QT_MOC_LITERAL(11, 101, 15), // "projectModified"
QT_MOC_LITERAL(12, 117, 8), // "modified"
QT_MOC_LITERAL(13, 126, 16), // "initiateTeardown"
QT_MOC_LITERAL(14, 143, 15), // "controller_dead"
QT_MOC_LITERAL(15, 159, 17), // "entityConstructed"
QT_MOC_LITERAL(16, 177, 2), // "ID"
QT_MOC_LITERAL(17, 180, 12), // "GRAPHML_KIND"
QT_MOC_LITERAL(18, 193, 5), // "eKind"
QT_MOC_LITERAL(19, 199, 4), // "data"
QT_MOC_LITERAL(20, 204, 10), // "properties"
QT_MOC_LITERAL(21, 215, 16), // "entityDestructed"
QT_MOC_LITERAL(22, 232, 11), // "dataChanged"
QT_MOC_LITERAL(23, 244, 7), // "keyName"
QT_MOC_LITERAL(24, 252, 11), // "dataRemoved"
QT_MOC_LITERAL(25, 264, 15), // "propertyChanged"
QT_MOC_LITERAL(26, 280, 12), // "propertyName"
QT_MOC_LITERAL(27, 293, 15), // "propertyRemoved"
QT_MOC_LITERAL(28, 309, 29), // "controller_ProjectFileChanged"
QT_MOC_LITERAL(29, 339, 29), // "controller_ProjectNameChanged"
QT_MOC_LITERAL(30, 369, 15), // "undoRedoChanged"
QT_MOC_LITERAL(31, 385, 23), // "controller_IsModelReady"
QT_MOC_LITERAL(32, 409, 5), // "ready"
QT_MOC_LITERAL(33, 415, 15), // "progressChanged"
QT_MOC_LITERAL(34, 431, 12), // "showProgress"
QT_MOC_LITERAL(35, 444, 32), // "controller_ActionProgressChanged"
QT_MOC_LITERAL(36, 477, 7), // "percent"
QT_MOC_LITERAL(37, 485, 6), // "action"
QT_MOC_LITERAL(38, 492, 25), // "controller_ActionFinished"
QT_MOC_LITERAL(39, 518, 15), // "actionSucceeded"
QT_MOC_LITERAL(40, 534, 9), // "errorCode"
QT_MOC_LITERAL(41, 544, 23), // "controller_OpenFinished"
QT_MOC_LITERAL(42, 568, 7), // "success"
QT_MOC_LITERAL(43, 576, 22), // "controller_AskQuestion"
QT_MOC_LITERAL(44, 599, 5), // "title"
QT_MOC_LITERAL(45, 605, 7), // "message"
QT_MOC_LITERAL(46, 613, 28), // "controller_GotQuestionAnswer"
QT_MOC_LITERAL(47, 642, 23), // "controller_SavedProject"
QT_MOC_LITERAL(48, 666, 8), // "filePath"
QT_MOC_LITERAL(49, 675, 3), // "dat"
QT_MOC_LITERAL(50, 679, 26), // "controller_ExportedProject"
QT_MOC_LITERAL(51, 706, 26), // "controller_ExportedSnippet"
QT_MOC_LITERAL(52, 733, 14), // "snippetXMLData"
QT_MOC_LITERAL(53, 748, 29), // "controller_GraphMLConstructed"
QT_MOC_LITERAL(54, 778, 7), // "Entity*"
QT_MOC_LITERAL(55, 786, 28), // "controller_GraphMLDestructed"
QT_MOC_LITERAL(56, 815, 13), // "test_destruct"
QT_MOC_LITERAL(57, 829, 30), // "controller_ProjectRequiresSave"
QT_MOC_LITERAL(58, 860, 12), // "requiresSave"
QT_MOC_LITERAL(59, 873, 29), // "controller_SetClipboardBuffer"
QT_MOC_LITERAL(60, 903, 25), // "controller_SetViewEnabled"
QT_MOC_LITERAL(61, 929, 15), // "setupController"
QT_MOC_LITERAL(62, 945, 7), // "setData"
QT_MOC_LITERAL(63, 953, 8), // "parentID"
QT_MOC_LITERAL(64, 962, 9), // "dataValue"
QT_MOC_LITERAL(65, 972, 10), // "removeData"
QT_MOC_LITERAL(66, 983, 22), // "constructConnectedNode"
QT_MOC_LITERAL(67, 1006, 8), // "nodeKind"
QT_MOC_LITERAL(68, 1015, 5), // "dstID"
QT_MOC_LITERAL(69, 1021, 8), // "edgeKind"
QT_MOC_LITERAL(70, 1030, 3), // "pos"
QT_MOC_LITERAL(71, 1034, 13), // "constructNode"
QT_MOC_LITERAL(72, 1048, 11), // "centerPoint"
QT_MOC_LITERAL(73, 1060, 22), // "constructWorkerProcess"
QT_MOC_LITERAL(74, 1083, 3), // "cut"
QT_MOC_LITERAL(75, 1087, 10), // "QList<int>"
QT_MOC_LITERAL(76, 1098, 3), // "IDs"
QT_MOC_LITERAL(77, 1102, 4), // "copy"
QT_MOC_LITERAL(78, 1107, 5), // "paste"
QT_MOC_LITERAL(79, 1113, 7), // "xmlData"
QT_MOC_LITERAL(80, 1121, 9), // "replicate"
QT_MOC_LITERAL(81, 1131, 6), // "remove"
QT_MOC_LITERAL(82, 1138, 4), // "undo"
QT_MOC_LITERAL(83, 1143, 4), // "redo"
QT_MOC_LITERAL(84, 1148, 13), // "triggerAction"
QT_MOC_LITERAL(85, 1162, 10), // "actionName"
QT_MOC_LITERAL(86, 1173, 11), // "openProject"
QT_MOC_LITERAL(87, 1185, 8), // "filepath"
QT_MOC_LITERAL(88, 1194, 14), // "importProjects"
QT_MOC_LITERAL(89, 1209, 11), // "xmlDataList"
QT_MOC_LITERAL(90, 1221, 13), // "importSnippet"
QT_MOC_LITERAL(91, 1235, 8), // "fileName"
QT_MOC_LITERAL(92, 1244, 8), // "fileData"
QT_MOC_LITERAL(93, 1253, 13), // "exportSnippet"
QT_MOC_LITERAL(94, 1267, 5), // "clear"
QT_MOC_LITERAL(95, 1273, 13), // "constructEdge"
QT_MOC_LITERAL(96, 1287, 6), // "srcIDs"
QT_MOC_LITERAL(97, 1294, 9), // "edgeClass"
QT_MOC_LITERAL(98, 1304, 13), // "destructEdges"
QT_MOC_LITERAL(99, 1318, 14), // "displayMessage"
QT_MOC_LITERAL(100, 1333, 17), // "gotQuestionAnswer"
QT_MOC_LITERAL(101, 1351, 6), // "answer"
QT_MOC_LITERAL(102, 1358, 19) // "_projectNameChanged"

    },
    "ModelController\0NodeConstructed\0\0"
    "parent_id\0id\0NODE_KIND\0kind\0EdgeConstructed\0"
    "EDGE_KIND\0src_id\0dst_id\0projectModified\0"
    "modified\0initiateTeardown\0controller_dead\0"
    "entityConstructed\0ID\0GRAPHML_KIND\0"
    "eKind\0data\0properties\0entityDestructed\0"
    "dataChanged\0keyName\0dataRemoved\0"
    "propertyChanged\0propertyName\0"
    "propertyRemoved\0controller_ProjectFileChanged\0"
    "controller_ProjectNameChanged\0"
    "undoRedoChanged\0controller_IsModelReady\0"
    "ready\0progressChanged\0showProgress\0"
    "controller_ActionProgressChanged\0"
    "percent\0action\0controller_ActionFinished\0"
    "actionSucceeded\0errorCode\0"
    "controller_OpenFinished\0success\0"
    "controller_AskQuestion\0title\0message\0"
    "controller_GotQuestionAnswer\0"
    "controller_SavedProject\0filePath\0dat\0"
    "controller_ExportedProject\0"
    "controller_ExportedSnippet\0snippetXMLData\0"
    "controller_GraphMLConstructed\0Entity*\0"
    "controller_GraphMLDestructed\0test_destruct\0"
    "controller_ProjectRequiresSave\0"
    "requiresSave\0controller_SetClipboardBuffer\0"
    "controller_SetViewEnabled\0setupController\0"
    "setData\0parentID\0dataValue\0removeData\0"
    "constructConnectedNode\0nodeKind\0dstID\0"
    "edgeKind\0pos\0constructNode\0centerPoint\0"
    "constructWorkerProcess\0cut\0QList<int>\0"
    "IDs\0copy\0paste\0xmlData\0replicate\0"
    "remove\0undo\0redo\0triggerAction\0"
    "actionName\0openProject\0filepath\0"
    "importProjects\0xmlDataList\0importSnippet\0"
    "fileName\0fileData\0exportSnippet\0clear\0"
    "constructEdge\0srcIDs\0edgeClass\0"
    "destructEdges\0displayMessage\0"
    "gotQuestionAnswer\0answer\0_projectNameChanged"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ModelController[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      61,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      36,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    3,  319,    2, 0x06 /* Public */,
       7,    4,  326,    2, 0x06 /* Public */,
      11,    1,  335,    2, 0x06 /* Public */,
      13,    0,  338,    2, 0x06 /* Public */,
      14,    0,  339,    2, 0x06 /* Public */,
      15,    5,  340,    2, 0x06 /* Public */,
      21,    3,  351,    2, 0x06 /* Public */,
      22,    3,  358,    2, 0x06 /* Public */,
      24,    2,  365,    2, 0x06 /* Public */,
      25,    3,  370,    2, 0x06 /* Public */,
      27,    2,  377,    2, 0x06 /* Public */,
      28,    1,  382,    2, 0x06 /* Public */,
      29,    1,  385,    2, 0x06 /* Public */,
      30,    0,  388,    2, 0x06 /* Public */,
      31,    1,  389,    2, 0x06 /* Public */,
      33,    1,  392,    2, 0x06 /* Public */,
      34,    2,  395,    2, 0x06 /* Public */,
      34,    1,  400,    2, 0x26 /* Public | MethodCloned */,
      35,    2,  403,    2, 0x06 /* Public */,
      35,    1,  408,    2, 0x26 /* Public | MethodCloned */,
      38,    2,  411,    2, 0x06 /* Public */,
      38,    1,  416,    2, 0x26 /* Public | MethodCloned */,
      38,    0,  419,    2, 0x26 /* Public | MethodCloned */,
      41,    1,  420,    2, 0x06 /* Public */,
      43,    3,  423,    2, 0x06 /* Public */,
      43,    2,  430,    2, 0x26 /* Public | MethodCloned */,
      46,    0,  435,    2, 0x06 /* Public */,
      47,    2,  436,    2, 0x06 /* Public */,
      50,    1,  441,    2, 0x06 /* Public */,
      51,    1,  444,    2, 0x06 /* Public */,
      53,    1,  447,    2, 0x06 /* Public */,
      55,    2,  450,    2, 0x06 /* Public */,
      56,    1,  455,    2, 0x06 /* Public */,
      57,    1,  458,    2, 0x06 /* Public */,
      59,    1,  461,    2, 0x06 /* Public */,
      60,    1,  464,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      61,    0,  467,    2, 0x0a /* Public */,
      62,    3,  468,    2, 0x0a /* Public */,
      65,    2,  475,    2, 0x0a /* Public */,
      66,    5,  480,    2, 0x0a /* Public */,
      66,    4,  491,    2, 0x2a /* Public | MethodCloned */,
      71,    3,  500,    2, 0x0a /* Public */,
      73,    3,  507,    2, 0x0a /* Public */,
      74,    1,  514,    2, 0x0a /* Public */,
      77,    1,  517,    2, 0x0a /* Public */,
      78,    2,  520,    2, 0x0a /* Public */,
      80,    1,  525,    2, 0x0a /* Public */,
      81,    1,  528,    2, 0x0a /* Public */,
      82,    0,  531,    2, 0x0a /* Public */,
      83,    0,  532,    2, 0x0a /* Public */,
      84,    1,  533,    2, 0x0a /* Public */,
      86,    2,  536,    2, 0x0a /* Public */,
      88,    1,  541,    2, 0x0a /* Public */,
      90,    3,  544,    2, 0x0a /* Public */,
      93,    1,  551,    2, 0x0a /* Public */,
      94,    0,  554,    2, 0x0a /* Public */,
      95,    3,  555,    2, 0x0a /* Public */,
      98,    3,  562,    2, 0x0a /* Public */,
      99,    3,  569,    2, 0x0a /* Public */,
     100,    1,  576,    2, 0x0a /* Public */,
     102,    0,  579,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::Int, 0x80000000 | 5,    3,    4,    6,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 8, QMetaType::Int, QMetaType::Int,    4,    6,    9,   10,
    QMetaType::Void, QMetaType::Bool,   12,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 17, QMetaType::QString, QMetaType::QVariantHash, QMetaType::QVariantHash,   16,   18,    6,   19,   20,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 17, QMetaType::QString,   16,   18,    6,
    QMetaType::Void, QMetaType::Int, QMetaType::QString, QMetaType::QVariant,   16,   23,   19,
    QMetaType::Void, QMetaType::Int, QMetaType::QString,   16,   23,
    QMetaType::Void, QMetaType::Int, QMetaType::QString, QMetaType::QVariant,   16,   26,   19,
    QMetaType::Void, QMetaType::Int, QMetaType::QString,   16,   26,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   32,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,    2,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Int, QMetaType::QString,   36,   37,
    QMetaType::Void, QMetaType::Int,   36,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,   39,   40,
    QMetaType::Void, QMetaType::Bool,   39,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   42,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::Int,   44,   45,   16,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   44,   45,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   48,   49,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString,   52,
    QMetaType::Void, 0x80000000 | 54,    2,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 17,   16,    6,
    QMetaType::Void, QMetaType::Int,   16,
    QMetaType::Void, QMetaType::Bool,   58,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::Bool,    2,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::QString, QMetaType::QVariant,   63,   23,   64,
    QMetaType::Void, QMetaType::Int, QMetaType::QString,   63,   23,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 5, QMetaType::Int, 0x80000000 | 8, QMetaType::QPointF,   63,   67,   68,   69,   70,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 5, QMetaType::Int, 0x80000000 | 8,   63,   67,   68,   69,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 5, QMetaType::QPointF,   63,    6,   72,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::QPointF,    3,   10,   72,
    QMetaType::Void, 0x80000000 | 75,   76,
    QMetaType::Void, 0x80000000 | 75,   76,
    QMetaType::Void, 0x80000000 | 75, QMetaType::QString,   76,   79,
    QMetaType::Void, 0x80000000 | 75,   76,
    QMetaType::Void, 0x80000000 | 75,   76,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   85,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   87,   79,
    QMetaType::Void, QMetaType::QStringList,   89,
    QMetaType::Void, 0x80000000 | 75, QMetaType::QString, QMetaType::QString,   76,   91,   92,
    QMetaType::Void, 0x80000000 | 75,   76,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 75, QMetaType::Int, 0x80000000 | 8,   96,   68,   97,
    QMetaType::Void, 0x80000000 | 75, QMetaType::Int, 0x80000000 | 8,   96,   68,   97,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::Int,   44,   45,   16,
    QMetaType::Void, QMetaType::Bool,  101,
    QMetaType::Void,

       0        // eod
};

void ModelController::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ModelController *_t = static_cast<ModelController *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->NodeConstructed((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< NODE_KIND(*)>(_a[3]))); break;
        case 1: _t->EdgeConstructed((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< EDGE_KIND(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4]))); break;
        case 2: _t->projectModified((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: _t->initiateTeardown(); break;
        case 4: _t->controller_dead(); break;
        case 5: _t->entityConstructed((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< GRAPHML_KIND(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3])),(*reinterpret_cast< QHash<QString,QVariant>(*)>(_a[4])),(*reinterpret_cast< QHash<QString,QVariant>(*)>(_a[5]))); break;
        case 6: _t->entityDestructed((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< GRAPHML_KIND(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3]))); break;
        case 7: _t->dataChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QVariant(*)>(_a[3]))); break;
        case 8: _t->dataRemoved((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 9: _t->propertyChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QVariant(*)>(_a[3]))); break;
        case 10: _t->propertyRemoved((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 11: _t->controller_ProjectFileChanged((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 12: _t->controller_ProjectNameChanged((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 13: _t->undoRedoChanged(); break;
        case 14: _t->controller_IsModelReady((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 15: _t->progressChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 16: _t->showProgress((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 17: _t->showProgress((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 18: _t->controller_ActionProgressChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 19: _t->controller_ActionProgressChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 20: _t->controller_ActionFinished((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 21: _t->controller_ActionFinished((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 22: _t->controller_ActionFinished(); break;
        case 23: _t->controller_OpenFinished((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 24: _t->controller_AskQuestion((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 25: _t->controller_AskQuestion((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 26: _t->controller_GotQuestionAnswer(); break;
        case 27: _t->controller_SavedProject((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 28: _t->controller_ExportedProject((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 29: _t->controller_ExportedSnippet((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 30: _t->controller_GraphMLConstructed((*reinterpret_cast< Entity*(*)>(_a[1]))); break;
        case 31: _t->controller_GraphMLDestructed((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< GRAPHML_KIND(*)>(_a[2]))); break;
        case 32: _t->test_destruct((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 33: _t->controller_ProjectRequiresSave((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 34: _t->controller_SetClipboardBuffer((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 35: _t->controller_SetViewEnabled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 36: _t->setupController(); break;
        case 37: _t->setData((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QVariant(*)>(_a[3]))); break;
        case 38: _t->removeData((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 39: _t->constructConnectedNode((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< NODE_KIND(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< EDGE_KIND(*)>(_a[4])),(*reinterpret_cast< QPointF(*)>(_a[5]))); break;
        case 40: _t->constructConnectedNode((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< NODE_KIND(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< EDGE_KIND(*)>(_a[4]))); break;
        case 41: _t->constructNode((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< NODE_KIND(*)>(_a[2])),(*reinterpret_cast< QPointF(*)>(_a[3]))); break;
        case 42: _t->constructWorkerProcess((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< QPointF(*)>(_a[3]))); break;
        case 43: _t->cut((*reinterpret_cast< QList<int>(*)>(_a[1]))); break;
        case 44: _t->copy((*reinterpret_cast< QList<int>(*)>(_a[1]))); break;
        case 45: _t->paste((*reinterpret_cast< QList<int>(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 46: _t->replicate((*reinterpret_cast< QList<int>(*)>(_a[1]))); break;
        case 47: _t->remove((*reinterpret_cast< QList<int>(*)>(_a[1]))); break;
        case 48: _t->undo(); break;
        case 49: _t->redo(); break;
        case 50: _t->triggerAction((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 51: _t->openProject((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 52: _t->importProjects((*reinterpret_cast< QStringList(*)>(_a[1]))); break;
        case 53: _t->importSnippet((*reinterpret_cast< QList<int>(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3]))); break;
        case 54: _t->exportSnippet((*reinterpret_cast< QList<int>(*)>(_a[1]))); break;
        case 55: _t->clear(); break;
        case 56: _t->constructEdge((*reinterpret_cast< QList<int>(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< EDGE_KIND(*)>(_a[3]))); break;
        case 57: _t->destructEdges((*reinterpret_cast< QList<int>(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< EDGE_KIND(*)>(_a[3]))); break;
        case 58: _t->displayMessage((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 59: _t->gotQuestionAnswer((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 60: _t->_projectNameChanged(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 2:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< NODE_KIND >(); break;
            }
            break;
        case 1:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< EDGE_KIND >(); break;
            }
            break;
        case 5:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< GRAPHML_KIND >(); break;
            }
            break;
        case 6:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< GRAPHML_KIND >(); break;
            }
            break;
        case 31:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< GRAPHML_KIND >(); break;
            }
            break;
        case 39:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 3:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< EDGE_KIND >(); break;
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< NODE_KIND >(); break;
            }
            break;
        case 40:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 3:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< EDGE_KIND >(); break;
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< NODE_KIND >(); break;
            }
            break;
        case 41:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< NODE_KIND >(); break;
            }
            break;
        case 43:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QList<int> >(); break;
            }
            break;
        case 44:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QList<int> >(); break;
            }
            break;
        case 45:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QList<int> >(); break;
            }
            break;
        case 46:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QList<int> >(); break;
            }
            break;
        case 47:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QList<int> >(); break;
            }
            break;
        case 53:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QList<int> >(); break;
            }
            break;
        case 54:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QList<int> >(); break;
            }
            break;
        case 56:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 2:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< EDGE_KIND >(); break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QList<int> >(); break;
            }
            break;
        case 57:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 2:
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
            typedef void (ModelController::*_t)(int , int , NODE_KIND );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ModelController::NodeConstructed)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (ModelController::*_t)(int , EDGE_KIND , int , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ModelController::EdgeConstructed)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (ModelController::*_t)(bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ModelController::projectModified)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (ModelController::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ModelController::initiateTeardown)) {
                *result = 3;
                return;
            }
        }
        {
            typedef void (ModelController::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ModelController::controller_dead)) {
                *result = 4;
                return;
            }
        }
        {
            typedef void (ModelController::*_t)(int , GRAPHML_KIND , QString , QHash<QString,QVariant> , QHash<QString,QVariant> );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ModelController::entityConstructed)) {
                *result = 5;
                return;
            }
        }
        {
            typedef void (ModelController::*_t)(int , GRAPHML_KIND , QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ModelController::entityDestructed)) {
                *result = 6;
                return;
            }
        }
        {
            typedef void (ModelController::*_t)(int , QString , QVariant );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ModelController::dataChanged)) {
                *result = 7;
                return;
            }
        }
        {
            typedef void (ModelController::*_t)(int , QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ModelController::dataRemoved)) {
                *result = 8;
                return;
            }
        }
        {
            typedef void (ModelController::*_t)(int , QString , QVariant );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ModelController::propertyChanged)) {
                *result = 9;
                return;
            }
        }
        {
            typedef void (ModelController::*_t)(int , QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ModelController::propertyRemoved)) {
                *result = 10;
                return;
            }
        }
        {
            typedef void (ModelController::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ModelController::controller_ProjectFileChanged)) {
                *result = 11;
                return;
            }
        }
        {
            typedef void (ModelController::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ModelController::controller_ProjectNameChanged)) {
                *result = 12;
                return;
            }
        }
        {
            typedef void (ModelController::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ModelController::undoRedoChanged)) {
                *result = 13;
                return;
            }
        }
        {
            typedef void (ModelController::*_t)(bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ModelController::controller_IsModelReady)) {
                *result = 14;
                return;
            }
        }
        {
            typedef void (ModelController::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ModelController::progressChanged)) {
                *result = 15;
                return;
            }
        }
        {
            typedef void (ModelController::*_t)(bool , QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ModelController::showProgress)) {
                *result = 16;
                return;
            }
        }
        {
            typedef void (ModelController::*_t)(int , QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ModelController::controller_ActionProgressChanged)) {
                *result = 18;
                return;
            }
        }
        {
            typedef void (ModelController::*_t)(bool , QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ModelController::controller_ActionFinished)) {
                *result = 20;
                return;
            }
        }
        {
            typedef void (ModelController::*_t)(bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ModelController::controller_OpenFinished)) {
                *result = 23;
                return;
            }
        }
        {
            typedef void (ModelController::*_t)(QString , QString , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ModelController::controller_AskQuestion)) {
                *result = 24;
                return;
            }
        }
        {
            typedef void (ModelController::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ModelController::controller_GotQuestionAnswer)) {
                *result = 26;
                return;
            }
        }
        {
            typedef void (ModelController::*_t)(QString , QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ModelController::controller_SavedProject)) {
                *result = 27;
                return;
            }
        }
        {
            typedef void (ModelController::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ModelController::controller_ExportedProject)) {
                *result = 28;
                return;
            }
        }
        {
            typedef void (ModelController::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ModelController::controller_ExportedSnippet)) {
                *result = 29;
                return;
            }
        }
        {
            typedef void (ModelController::*_t)(Entity * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ModelController::controller_GraphMLConstructed)) {
                *result = 30;
                return;
            }
        }
        {
            typedef void (ModelController::*_t)(int , GRAPHML_KIND );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ModelController::controller_GraphMLDestructed)) {
                *result = 31;
                return;
            }
        }
        {
            typedef void (ModelController::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ModelController::test_destruct)) {
                *result = 32;
                return;
            }
        }
        {
            typedef void (ModelController::*_t)(bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ModelController::controller_ProjectRequiresSave)) {
                *result = 33;
                return;
            }
        }
        {
            typedef void (ModelController::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ModelController::controller_SetClipboardBuffer)) {
                *result = 34;
                return;
            }
        }
        {
            typedef void (ModelController::*_t)(bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ModelController::controller_SetViewEnabled)) {
                *result = 35;
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
        if (_id < 61)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 61;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 61)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 61;
    }
    return _id;
}

// SIGNAL 0
void ModelController::NodeConstructed(int _t1, int _t2, NODE_KIND _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ModelController::EdgeConstructed(int _t1, EDGE_KIND _t2, int _t3, int _t4)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void ModelController::projectModified(bool _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void ModelController::initiateTeardown()
{
    QMetaObject::activate(this, &staticMetaObject, 3, Q_NULLPTR);
}

// SIGNAL 4
void ModelController::controller_dead()
{
    QMetaObject::activate(this, &staticMetaObject, 4, Q_NULLPTR);
}

// SIGNAL 5
void ModelController::entityConstructed(int _t1, GRAPHML_KIND _t2, QString _t3, QHash<QString,QVariant> _t4, QHash<QString,QVariant> _t5)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)), const_cast<void*>(reinterpret_cast<const void*>(&_t5)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void ModelController::entityDestructed(int _t1, GRAPHML_KIND _t2, QString _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void ModelController::dataChanged(int _t1, QString _t2, QVariant _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void ModelController::dataRemoved(int _t1, QString _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void ModelController::propertyChanged(int _t1, QString _t2, QVariant _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}

// SIGNAL 10
void ModelController::propertyRemoved(int _t1, QString _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 10, _a);
}

// SIGNAL 11
void ModelController::controller_ProjectFileChanged(QString _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 11, _a);
}

// SIGNAL 12
void ModelController::controller_ProjectNameChanged(QString _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 12, _a);
}

// SIGNAL 13
void ModelController::undoRedoChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 13, Q_NULLPTR);
}

// SIGNAL 14
void ModelController::controller_IsModelReady(bool _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 14, _a);
}

// SIGNAL 15
void ModelController::progressChanged(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 15, _a);
}

// SIGNAL 16
void ModelController::showProgress(bool _t1, QString _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 16, _a);
}

// SIGNAL 18
void ModelController::controller_ActionProgressChanged(int _t1, QString _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 18, _a);
}

// SIGNAL 20
void ModelController::controller_ActionFinished(bool _t1, QString _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 20, _a);
}

// SIGNAL 23
void ModelController::controller_OpenFinished(bool _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 23, _a);
}

// SIGNAL 24
void ModelController::controller_AskQuestion(QString _t1, QString _t2, int _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 24, _a);
}

// SIGNAL 26
void ModelController::controller_GotQuestionAnswer()
{
    QMetaObject::activate(this, &staticMetaObject, 26, Q_NULLPTR);
}

// SIGNAL 27
void ModelController::controller_SavedProject(QString _t1, QString _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 27, _a);
}

// SIGNAL 28
void ModelController::controller_ExportedProject(QString _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 28, _a);
}

// SIGNAL 29
void ModelController::controller_ExportedSnippet(QString _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 29, _a);
}

// SIGNAL 30
void ModelController::controller_GraphMLConstructed(Entity * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 30, _a);
}

// SIGNAL 31
void ModelController::controller_GraphMLDestructed(int _t1, GRAPHML_KIND _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 31, _a);
}

// SIGNAL 32
void ModelController::test_destruct(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 32, _a);
}

// SIGNAL 33
void ModelController::controller_ProjectRequiresSave(bool _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 33, _a);
}

// SIGNAL 34
void ModelController::controller_SetClipboardBuffer(QString _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 34, _a);
}

// SIGNAL 35
void ModelController::controller_SetViewEnabled(bool _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 35, _a);
}
QT_END_MOC_NAMESPACE
