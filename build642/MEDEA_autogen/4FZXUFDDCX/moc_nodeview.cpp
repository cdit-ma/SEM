/****************************************************************************
** Meta object code from reading C++ file 'nodeview.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/Views/NodeView/nodeview.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'nodeview.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_NodeView_t {
    QByteArrayData data[83];
    char stringdata0[1359];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_NodeView_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_NodeView_t qt_meta_stringdata_NodeView = {
    {
QT_MOC_LITERAL(0, 0, 8), // "NodeView"
QT_MOC_LITERAL(1, 9, 21), // "trans_InActive2Moving"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 21), // "trans_Moving2InActive"
QT_MOC_LITERAL(4, 54, 23), // "trans_InActive2Resizing"
QT_MOC_LITERAL(5, 78, 23), // "trans_Resizing2InActive"
QT_MOC_LITERAL(6, 102, 29), // "trans_InActive2RubberbandMode"
QT_MOC_LITERAL(7, 132, 29), // "trans_RubberbandMode2InActive"
QT_MOC_LITERAL(8, 162, 45), // "trans_RubberbandMode2Rubberba..."
QT_MOC_LITERAL(9, 208, 45), // "trans_RubberbandMode_Selectin..."
QT_MOC_LITERAL(10, 254, 25), // "trans_InActive2Connecting"
QT_MOC_LITERAL(11, 280, 25), // "trans_Connecting2InActive"
QT_MOC_LITERAL(12, 306, 16), // "sceneRectChanged"
QT_MOC_LITERAL(13, 323, 9), // "sceneRect"
QT_MOC_LITERAL(14, 333, 16), // "toolbarRequested"
QT_MOC_LITERAL(15, 350, 9), // "screenPos"
QT_MOC_LITERAL(16, 360, 7), // "itemPos"
QT_MOC_LITERAL(17, 368, 15), // "viewportChanged"
QT_MOC_LITERAL(18, 384, 4), // "rect"
QT_MOC_LITERAL(19, 389, 4), // "zoom"
QT_MOC_LITERAL(20, 394, 12), // "viewFocussed"
QT_MOC_LITERAL(21, 407, 9), // "NodeView*"
QT_MOC_LITERAL(22, 417, 4), // "view"
QT_MOC_LITERAL(23, 422, 8), // "focussed"
QT_MOC_LITERAL(24, 431, 13), // "triggerAction"
QT_MOC_LITERAL(25, 445, 7), // "setData"
QT_MOC_LITERAL(26, 453, 10), // "removeData"
QT_MOC_LITERAL(27, 464, 8), // "editData"
QT_MOC_LITERAL(28, 473, 4), // "test"
QT_MOC_LITERAL(29, 478, 21), // "viewItem_LabelChanged"
QT_MOC_LITERAL(30, 500, 5), // "label"
QT_MOC_LITERAL(31, 506, 20), // "viewItem_Constructed"
QT_MOC_LITERAL(32, 527, 9), // "ViewItem*"
QT_MOC_LITERAL(33, 537, 8), // "viewItem"
QT_MOC_LITERAL(34, 546, 19), // "viewItem_Destructed"
QT_MOC_LITERAL(35, 566, 2), // "ID"
QT_MOC_LITERAL(36, 569, 37), // "selectionHandler_ItemSelectio..."
QT_MOC_LITERAL(37, 607, 4), // "item"
QT_MOC_LITERAL(38, 612, 8), // "selected"
QT_MOC_LITERAL(39, 621, 43), // "selectionHandler_ItemActiveSe..."
QT_MOC_LITERAL(40, 665, 8), // "isActive"
QT_MOC_LITERAL(41, 674, 10), // "itemsMoved"
QT_MOC_LITERAL(42, 685, 12), // "themeChanged"
QT_MOC_LITERAL(43, 698, 9), // "selectAll"
QT_MOC_LITERAL(44, 708, 14), // "clearSelection"
QT_MOC_LITERAL(45, 723, 11), // "minimap_Pan"
QT_MOC_LITERAL(46, 735, 5), // "delta"
QT_MOC_LITERAL(47, 741, 12), // "minimap_Zoom"
QT_MOC_LITERAL(48, 754, 16), // "node_ConnectMode"
QT_MOC_LITERAL(49, 771, 9), // "NodeItem*"
QT_MOC_LITERAL(50, 781, 22), // "node_PopOutRelatedNode"
QT_MOC_LITERAL(51, 804, 13), // "NodeViewItem*"
QT_MOC_LITERAL(52, 818, 9), // "NODE_KIND"
QT_MOC_LITERAL(53, 828, 4), // "kind"
QT_MOC_LITERAL(54, 833, 13), // "item_EditData"
QT_MOC_LITERAL(55, 847, 7), // "keyName"
QT_MOC_LITERAL(56, 855, 15), // "item_RemoveData"
QT_MOC_LITERAL(57, 871, 13), // "item_Selected"
QT_MOC_LITERAL(58, 885, 6), // "append"
QT_MOC_LITERAL(59, 892, 19), // "item_ActiveSelected"
QT_MOC_LITERAL(60, 912, 16), // "item_SetExpanded"
QT_MOC_LITERAL(61, 929, 11), // "EntityItem*"
QT_MOC_LITERAL(62, 941, 6), // "expand"
QT_MOC_LITERAL(63, 948, 16), // "item_SetCentered"
QT_MOC_LITERAL(64, 965, 18), // "item_MoveSelection"
QT_MOC_LITERAL(65, 984, 11), // "item_Resize"
QT_MOC_LITERAL(66, 996, 11), // "RECT_VERTEX"
QT_MOC_LITERAL(67, 1008, 4), // "vert"
QT_MOC_LITERAL(68, 1013, 10), // "centerItem"
QT_MOC_LITERAL(69, 1024, 17), // "centerConnections"
QT_MOC_LITERAL(70, 1042, 13), // "highlightItem"
QT_MOC_LITERAL(71, 1056, 11), // "highlighted"
QT_MOC_LITERAL(72, 1068, 20), // "state_Moving_Entered"
QT_MOC_LITERAL(73, 1089, 19), // "state_Moving_Exited"
QT_MOC_LITERAL(74, 1109, 22), // "state_Resizing_Entered"
QT_MOC_LITERAL(75, 1132, 21), // "state_Resizing_Exited"
QT_MOC_LITERAL(76, 1154, 28), // "state_RubberbandMode_Entered"
QT_MOC_LITERAL(77, 1183, 27), // "state_RubberbandMode_Exited"
QT_MOC_LITERAL(78, 1211, 38), // "state_RubberbandMode_Selectin..."
QT_MOC_LITERAL(79, 1250, 37), // "state_RubberbandMode_Selectin..."
QT_MOC_LITERAL(80, 1288, 24), // "state_Connecting_Entered"
QT_MOC_LITERAL(81, 1313, 23), // "state_Connecting_Exited"
QT_MOC_LITERAL(82, 1337, 21) // "state_Default_Entered"

    },
    "NodeView\0trans_InActive2Moving\0\0"
    "trans_Moving2InActive\0trans_InActive2Resizing\0"
    "trans_Resizing2InActive\0"
    "trans_InActive2RubberbandMode\0"
    "trans_RubberbandMode2InActive\0"
    "trans_RubberbandMode2RubberbandMode_Selecting\0"
    "trans_RubberbandMode_Selecting2RubberbandMode\0"
    "trans_InActive2Connecting\0"
    "trans_Connecting2InActive\0sceneRectChanged\0"
    "sceneRect\0toolbarRequested\0screenPos\0"
    "itemPos\0viewportChanged\0rect\0zoom\0"
    "viewFocussed\0NodeView*\0view\0focussed\0"
    "triggerAction\0setData\0removeData\0"
    "editData\0test\0viewItem_LabelChanged\0"
    "label\0viewItem_Constructed\0ViewItem*\0"
    "viewItem\0viewItem_Destructed\0ID\0"
    "selectionHandler_ItemSelectionChanged\0"
    "item\0selected\0selectionHandler_ItemActiveSelectionChanged\0"
    "isActive\0itemsMoved\0themeChanged\0"
    "selectAll\0clearSelection\0minimap_Pan\0"
    "delta\0minimap_Zoom\0node_ConnectMode\0"
    "NodeItem*\0node_PopOutRelatedNode\0"
    "NodeViewItem*\0NODE_KIND\0kind\0item_EditData\0"
    "keyName\0item_RemoveData\0item_Selected\0"
    "append\0item_ActiveSelected\0item_SetExpanded\0"
    "EntityItem*\0expand\0item_SetCentered\0"
    "item_MoveSelection\0item_Resize\0"
    "RECT_VERTEX\0vert\0centerItem\0"
    "centerConnections\0highlightItem\0"
    "highlighted\0state_Moving_Entered\0"
    "state_Moving_Exited\0state_Resizing_Entered\0"
    "state_Resizing_Exited\0"
    "state_RubberbandMode_Entered\0"
    "state_RubberbandMode_Exited\0"
    "state_RubberbandMode_Selecting_Entered\0"
    "state_RubberbandMode_Selecting_Exited\0"
    "state_Connecting_Entered\0"
    "state_Connecting_Exited\0state_Default_Entered"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_NodeView[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      54,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      18,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,  284,    2, 0x06 /* Public */,
       3,    0,  285,    2, 0x06 /* Public */,
       4,    0,  286,    2, 0x06 /* Public */,
       5,    0,  287,    2, 0x06 /* Public */,
       6,    0,  288,    2, 0x06 /* Public */,
       7,    0,  289,    2, 0x06 /* Public */,
       8,    0,  290,    2, 0x06 /* Public */,
       9,    0,  291,    2, 0x06 /* Public */,
      10,    0,  292,    2, 0x06 /* Public */,
      11,    0,  293,    2, 0x06 /* Public */,
      12,    1,  294,    2, 0x06 /* Public */,
      14,    2,  297,    2, 0x06 /* Public */,
      17,    2,  302,    2, 0x06 /* Public */,
      20,    2,  307,    2, 0x06 /* Public */,
      24,    1,  312,    2, 0x06 /* Public */,
      25,    3,  315,    2, 0x06 /* Public */,
      26,    2,  322,    2, 0x06 /* Public */,
      27,    2,  327,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      28,    0,  332,    2, 0x08 /* Private */,
      29,    1,  333,    2, 0x08 /* Private */,
      31,    1,  336,    2, 0x08 /* Private */,
      34,    2,  339,    2, 0x08 /* Private */,
      36,    2,  344,    2, 0x08 /* Private */,
      39,    2,  349,    2, 0x08 /* Private */,
      41,    0,  354,    2, 0x08 /* Private */,
      42,    0,  355,    2, 0x08 /* Private */,
      43,    0,  356,    2, 0x0a /* Public */,
      44,    0,  357,    2, 0x0a /* Public */,
      45,    1,  358,    2, 0x0a /* Public */,
      47,    1,  361,    2, 0x0a /* Public */,
      48,    1,  364,    2, 0x08 /* Private */,
      50,    2,  367,    2, 0x08 /* Private */,
      54,    2,  372,    2, 0x08 /* Private */,
      56,    2,  377,    2, 0x08 /* Private */,
      57,    2,  382,    2, 0x08 /* Private */,
      59,    1,  387,    2, 0x08 /* Private */,
      60,    2,  390,    2, 0x08 /* Private */,
      63,    1,  395,    2, 0x08 /* Private */,
      64,    1,  398,    2, 0x08 /* Private */,
      65,    3,  401,    2, 0x08 /* Private */,
      68,    1,  408,    2, 0x08 /* Private */,
      69,    1,  411,    2, 0x08 /* Private */,
      70,    2,  414,    2, 0x08 /* Private */,
      72,    0,  419,    2, 0x08 /* Private */,
      73,    0,  420,    2, 0x08 /* Private */,
      74,    0,  421,    2, 0x08 /* Private */,
      75,    0,  422,    2, 0x08 /* Private */,
      76,    0,  423,    2, 0x08 /* Private */,
      77,    0,  424,    2, 0x08 /* Private */,
      78,    0,  425,    2, 0x08 /* Private */,
      79,    0,  426,    2, 0x08 /* Private */,
      80,    0,  427,    2, 0x08 /* Private */,
      81,    0,  428,    2, 0x08 /* Private */,
      82,    0,  429,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QRectF,   13,
    QMetaType::Void, QMetaType::QPoint, QMetaType::QPointF,   15,   16,
    QMetaType::Void, QMetaType::QRectF, QMetaType::QReal,   18,   19,
    QMetaType::Void, 0x80000000 | 21, QMetaType::Bool,   22,   23,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::Int, QMetaType::QString, QMetaType::QVariant,    2,    2,    2,
    QMetaType::Void, QMetaType::Int, QMetaType::QString,    2,    2,
    QMetaType::Void, QMetaType::Int, QMetaType::QString,    2,    2,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   30,
    QMetaType::Void, 0x80000000 | 32,   33,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 32,   35,   33,
    QMetaType::Void, 0x80000000 | 32, QMetaType::Bool,   37,   38,
    QMetaType::Void, 0x80000000 | 32, QMetaType::Bool,   37,   40,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QPointF,   46,
    QMetaType::Void, QMetaType::Int,   46,
    QMetaType::Void, 0x80000000 | 49,   37,
    QMetaType::Void, 0x80000000 | 51, 0x80000000 | 52,   37,   53,
    QMetaType::Void, 0x80000000 | 32, QMetaType::QString,   37,   55,
    QMetaType::Void, 0x80000000 | 32, QMetaType::QString,   37,   55,
    QMetaType::Void, 0x80000000 | 32, QMetaType::Bool,   37,   58,
    QMetaType::Void, 0x80000000 | 32,   37,
    QMetaType::Void, 0x80000000 | 61, QMetaType::Bool,   37,   62,
    QMetaType::Void, 0x80000000 | 61,   37,
    QMetaType::Void, QMetaType::QPointF,   46,
    QMetaType::Void, 0x80000000 | 49, QMetaType::QSizeF, 0x80000000 | 66,   37,   46,   67,
    QMetaType::Void, QMetaType::Int,   35,
    QMetaType::Void, 0x80000000 | 32,   37,
    QMetaType::Void, QMetaType::Int, QMetaType::Bool,   35,   71,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void NodeView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        NodeView *_t = static_cast<NodeView *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->trans_InActive2Moving(); break;
        case 1: _t->trans_Moving2InActive(); break;
        case 2: _t->trans_InActive2Resizing(); break;
        case 3: _t->trans_Resizing2InActive(); break;
        case 4: _t->trans_InActive2RubberbandMode(); break;
        case 5: _t->trans_RubberbandMode2InActive(); break;
        case 6: _t->trans_RubberbandMode2RubberbandMode_Selecting(); break;
        case 7: _t->trans_RubberbandMode_Selecting2RubberbandMode(); break;
        case 8: _t->trans_InActive2Connecting(); break;
        case 9: _t->trans_Connecting2InActive(); break;
        case 10: _t->sceneRectChanged((*reinterpret_cast< QRectF(*)>(_a[1]))); break;
        case 11: _t->toolbarRequested((*reinterpret_cast< QPoint(*)>(_a[1])),(*reinterpret_cast< QPointF(*)>(_a[2]))); break;
        case 12: _t->viewportChanged((*reinterpret_cast< QRectF(*)>(_a[1])),(*reinterpret_cast< qreal(*)>(_a[2]))); break;
        case 13: _t->viewFocussed((*reinterpret_cast< NodeView*(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 14: _t->triggerAction((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 15: _t->setData((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QVariant(*)>(_a[3]))); break;
        case 16: _t->removeData((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 17: _t->editData((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 18: _t->test(); break;
        case 19: _t->viewItem_LabelChanged((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 20: _t->viewItem_Constructed((*reinterpret_cast< ViewItem*(*)>(_a[1]))); break;
        case 21: _t->viewItem_Destructed((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< ViewItem*(*)>(_a[2]))); break;
        case 22: _t->selectionHandler_ItemSelectionChanged((*reinterpret_cast< ViewItem*(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 23: _t->selectionHandler_ItemActiveSelectionChanged((*reinterpret_cast< ViewItem*(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 24: _t->itemsMoved(); break;
        case 25: _t->themeChanged(); break;
        case 26: _t->selectAll(); break;
        case 27: _t->clearSelection(); break;
        case 28: _t->minimap_Pan((*reinterpret_cast< QPointF(*)>(_a[1]))); break;
        case 29: _t->minimap_Zoom((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 30: _t->node_ConnectMode((*reinterpret_cast< NodeItem*(*)>(_a[1]))); break;
        case 31: _t->node_PopOutRelatedNode((*reinterpret_cast< NodeViewItem*(*)>(_a[1])),(*reinterpret_cast< NODE_KIND(*)>(_a[2]))); break;
        case 32: _t->item_EditData((*reinterpret_cast< ViewItem*(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 33: _t->item_RemoveData((*reinterpret_cast< ViewItem*(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 34: _t->item_Selected((*reinterpret_cast< ViewItem*(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 35: _t->item_ActiveSelected((*reinterpret_cast< ViewItem*(*)>(_a[1]))); break;
        case 36: _t->item_SetExpanded((*reinterpret_cast< EntityItem*(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 37: _t->item_SetCentered((*reinterpret_cast< EntityItem*(*)>(_a[1]))); break;
        case 38: _t->item_MoveSelection((*reinterpret_cast< QPointF(*)>(_a[1]))); break;
        case 39: _t->item_Resize((*reinterpret_cast< NodeItem*(*)>(_a[1])),(*reinterpret_cast< QSizeF(*)>(_a[2])),(*reinterpret_cast< RECT_VERTEX(*)>(_a[3]))); break;
        case 40: _t->centerItem((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 41: _t->centerConnections((*reinterpret_cast< ViewItem*(*)>(_a[1]))); break;
        case 42: _t->highlightItem((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 43: _t->state_Moving_Entered(); break;
        case 44: _t->state_Moving_Exited(); break;
        case 45: _t->state_Resizing_Entered(); break;
        case 46: _t->state_Resizing_Exited(); break;
        case 47: _t->state_RubberbandMode_Entered(); break;
        case 48: _t->state_RubberbandMode_Exited(); break;
        case 49: _t->state_RubberbandMode_Selecting_Entered(); break;
        case 50: _t->state_RubberbandMode_Selecting_Exited(); break;
        case 51: _t->state_Connecting_Entered(); break;
        case 52: _t->state_Connecting_Exited(); break;
        case 53: _t->state_Default_Entered(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 13:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< NodeView* >(); break;
            }
            break;
        case 20:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< ViewItem* >(); break;
            }
            break;
        case 21:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< ViewItem* >(); break;
            }
            break;
        case 22:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< ViewItem* >(); break;
            }
            break;
        case 23:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< ViewItem* >(); break;
            }
            break;
        case 30:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< NodeItem* >(); break;
            }
            break;
        case 31:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< NODE_KIND >(); break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< NodeViewItem* >(); break;
            }
            break;
        case 32:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< ViewItem* >(); break;
            }
            break;
        case 33:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< ViewItem* >(); break;
            }
            break;
        case 34:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< ViewItem* >(); break;
            }
            break;
        case 35:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< ViewItem* >(); break;
            }
            break;
        case 36:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< EntityItem* >(); break;
            }
            break;
        case 37:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< EntityItem* >(); break;
            }
            break;
        case 39:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< NodeItem* >(); break;
            }
            break;
        case 41:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< ViewItem* >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (NodeView::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NodeView::trans_InActive2Moving)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (NodeView::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NodeView::trans_Moving2InActive)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (NodeView::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NodeView::trans_InActive2Resizing)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (NodeView::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NodeView::trans_Resizing2InActive)) {
                *result = 3;
                return;
            }
        }
        {
            typedef void (NodeView::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NodeView::trans_InActive2RubberbandMode)) {
                *result = 4;
                return;
            }
        }
        {
            typedef void (NodeView::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NodeView::trans_RubberbandMode2InActive)) {
                *result = 5;
                return;
            }
        }
        {
            typedef void (NodeView::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NodeView::trans_RubberbandMode2RubberbandMode_Selecting)) {
                *result = 6;
                return;
            }
        }
        {
            typedef void (NodeView::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NodeView::trans_RubberbandMode_Selecting2RubberbandMode)) {
                *result = 7;
                return;
            }
        }
        {
            typedef void (NodeView::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NodeView::trans_InActive2Connecting)) {
                *result = 8;
                return;
            }
        }
        {
            typedef void (NodeView::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NodeView::trans_Connecting2InActive)) {
                *result = 9;
                return;
            }
        }
        {
            typedef void (NodeView::*_t)(QRectF );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NodeView::sceneRectChanged)) {
                *result = 10;
                return;
            }
        }
        {
            typedef void (NodeView::*_t)(QPoint , QPointF );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NodeView::toolbarRequested)) {
                *result = 11;
                return;
            }
        }
        {
            typedef void (NodeView::*_t)(QRectF , qreal );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NodeView::viewportChanged)) {
                *result = 12;
                return;
            }
        }
        {
            typedef void (NodeView::*_t)(NodeView * , bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NodeView::viewFocussed)) {
                *result = 13;
                return;
            }
        }
        {
            typedef void (NodeView::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NodeView::triggerAction)) {
                *result = 14;
                return;
            }
        }
        {
            typedef void (NodeView::*_t)(int , QString , QVariant );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NodeView::setData)) {
                *result = 15;
                return;
            }
        }
        {
            typedef void (NodeView::*_t)(int , QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NodeView::removeData)) {
                *result = 16;
                return;
            }
        }
        {
            typedef void (NodeView::*_t)(int , QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NodeView::editData)) {
                *result = 17;
                return;
            }
        }
    }
}

const QMetaObject NodeView::staticMetaObject = {
    { &QGraphicsView::staticMetaObject, qt_meta_stringdata_NodeView.data,
      qt_meta_data_NodeView,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *NodeView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *NodeView::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_NodeView.stringdata0))
        return static_cast<void*>(const_cast< NodeView*>(this));
    return QGraphicsView::qt_metacast(_clname);
}

int NodeView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGraphicsView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 54)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 54;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 54)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 54;
    }
    return _id;
}

// SIGNAL 0
void NodeView::trans_InActive2Moving()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}

// SIGNAL 1
void NodeView::trans_Moving2InActive()
{
    QMetaObject::activate(this, &staticMetaObject, 1, Q_NULLPTR);
}

// SIGNAL 2
void NodeView::trans_InActive2Resizing()
{
    QMetaObject::activate(this, &staticMetaObject, 2, Q_NULLPTR);
}

// SIGNAL 3
void NodeView::trans_Resizing2InActive()
{
    QMetaObject::activate(this, &staticMetaObject, 3, Q_NULLPTR);
}

// SIGNAL 4
void NodeView::trans_InActive2RubberbandMode()
{
    QMetaObject::activate(this, &staticMetaObject, 4, Q_NULLPTR);
}

// SIGNAL 5
void NodeView::trans_RubberbandMode2InActive()
{
    QMetaObject::activate(this, &staticMetaObject, 5, Q_NULLPTR);
}

// SIGNAL 6
void NodeView::trans_RubberbandMode2RubberbandMode_Selecting()
{
    QMetaObject::activate(this, &staticMetaObject, 6, Q_NULLPTR);
}

// SIGNAL 7
void NodeView::trans_RubberbandMode_Selecting2RubberbandMode()
{
    QMetaObject::activate(this, &staticMetaObject, 7, Q_NULLPTR);
}

// SIGNAL 8
void NodeView::trans_InActive2Connecting()
{
    QMetaObject::activate(this, &staticMetaObject, 8, Q_NULLPTR);
}

// SIGNAL 9
void NodeView::trans_Connecting2InActive()
{
    QMetaObject::activate(this, &staticMetaObject, 9, Q_NULLPTR);
}

// SIGNAL 10
void NodeView::sceneRectChanged(QRectF _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 10, _a);
}

// SIGNAL 11
void NodeView::toolbarRequested(QPoint _t1, QPointF _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 11, _a);
}

// SIGNAL 12
void NodeView::viewportChanged(QRectF _t1, qreal _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 12, _a);
}

// SIGNAL 13
void NodeView::viewFocussed(NodeView * _t1, bool _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 13, _a);
}

// SIGNAL 14
void NodeView::triggerAction(QString _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 14, _a);
}

// SIGNAL 15
void NodeView::setData(int _t1, QString _t2, QVariant _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 15, _a);
}

// SIGNAL 16
void NodeView::removeData(int _t1, QString _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 16, _a);
}

// SIGNAL 17
void NodeView::editData(int _t1, QString _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 17, _a);
}
QT_END_MOC_NAMESPACE
