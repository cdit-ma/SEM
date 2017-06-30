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
    QByteArrayData data[80];
    char stringdata0[1344];
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
QT_MOC_LITERAL(12, 306, 16), // "toolbarRequested"
QT_MOC_LITERAL(13, 323, 9), // "screenPos"
QT_MOC_LITERAL(14, 333, 7), // "itemPos"
QT_MOC_LITERAL(15, 341, 16), // "viewport_changed"
QT_MOC_LITERAL(16, 358, 12), // "viewportRect"
QT_MOC_LITERAL(17, 371, 11), // "zoom_factor"
QT_MOC_LITERAL(18, 383, 12), // "viewFocussed"
QT_MOC_LITERAL(19, 396, 9), // "NodeView*"
QT_MOC_LITERAL(20, 406, 4), // "view"
QT_MOC_LITERAL(21, 411, 8), // "focussed"
QT_MOC_LITERAL(22, 420, 13), // "triggerAction"
QT_MOC_LITERAL(23, 434, 7), // "setData"
QT_MOC_LITERAL(24, 442, 10), // "removeData"
QT_MOC_LITERAL(25, 453, 8), // "editData"
QT_MOC_LITERAL(26, 462, 21), // "viewItem_LabelChanged"
QT_MOC_LITERAL(27, 484, 5), // "label"
QT_MOC_LITERAL(28, 490, 20), // "viewItem_Constructed"
QT_MOC_LITERAL(29, 511, 9), // "ViewItem*"
QT_MOC_LITERAL(30, 521, 8), // "viewItem"
QT_MOC_LITERAL(31, 530, 19), // "viewItem_Destructed"
QT_MOC_LITERAL(32, 550, 2), // "ID"
QT_MOC_LITERAL(33, 553, 37), // "selectionHandler_ItemSelectio..."
QT_MOC_LITERAL(34, 591, 4), // "item"
QT_MOC_LITERAL(35, 596, 8), // "selected"
QT_MOC_LITERAL(36, 605, 43), // "selectionHandler_ItemActiveSe..."
QT_MOC_LITERAL(37, 649, 8), // "isActive"
QT_MOC_LITERAL(38, 658, 12), // "themeChanged"
QT_MOC_LITERAL(39, 671, 11), // "fitToScreen"
QT_MOC_LITERAL(40, 683, 9), // "selectAll"
QT_MOC_LITERAL(41, 693, 14), // "clearSelection"
QT_MOC_LITERAL(42, 708, 11), // "minimap_Pan"
QT_MOC_LITERAL(43, 720, 5), // "delta"
QT_MOC_LITERAL(44, 726, 12), // "minimap_Zoom"
QT_MOC_LITERAL(45, 739, 16), // "node_ConnectMode"
QT_MOC_LITERAL(46, 756, 9), // "NodeItem*"
QT_MOC_LITERAL(47, 766, 22), // "node_PopOutRelatedNode"
QT_MOC_LITERAL(48, 789, 13), // "NodeViewItem*"
QT_MOC_LITERAL(49, 803, 9), // "NODE_KIND"
QT_MOC_LITERAL(50, 813, 4), // "kind"
QT_MOC_LITERAL(51, 818, 13), // "item_EditData"
QT_MOC_LITERAL(52, 832, 7), // "keyName"
QT_MOC_LITERAL(53, 840, 15), // "item_RemoveData"
QT_MOC_LITERAL(54, 856, 13), // "item_Selected"
QT_MOC_LITERAL(55, 870, 6), // "append"
QT_MOC_LITERAL(56, 877, 19), // "item_ActiveSelected"
QT_MOC_LITERAL(57, 897, 16), // "item_SetExpanded"
QT_MOC_LITERAL(58, 914, 11), // "EntityItem*"
QT_MOC_LITERAL(59, 926, 6), // "expand"
QT_MOC_LITERAL(60, 933, 16), // "item_SetCentered"
QT_MOC_LITERAL(61, 950, 18), // "item_MoveSelection"
QT_MOC_LITERAL(62, 969, 11), // "item_Resize"
QT_MOC_LITERAL(63, 981, 11), // "RECT_VERTEX"
QT_MOC_LITERAL(64, 993, 4), // "vert"
QT_MOC_LITERAL(65, 998, 10), // "centerItem"
QT_MOC_LITERAL(66, 1009, 17), // "centerConnections"
QT_MOC_LITERAL(67, 1027, 13), // "highlightItem"
QT_MOC_LITERAL(68, 1041, 11), // "highlighted"
QT_MOC_LITERAL(69, 1053, 20), // "state_Moving_Entered"
QT_MOC_LITERAL(70, 1074, 19), // "state_Moving_Exited"
QT_MOC_LITERAL(71, 1094, 22), // "state_Resizing_Entered"
QT_MOC_LITERAL(72, 1117, 21), // "state_Resizing_Exited"
QT_MOC_LITERAL(73, 1139, 28), // "state_RubberbandMode_Entered"
QT_MOC_LITERAL(74, 1168, 27), // "state_RubberbandMode_Exited"
QT_MOC_LITERAL(75, 1196, 38), // "state_RubberbandMode_Selectin..."
QT_MOC_LITERAL(76, 1235, 37), // "state_RubberbandMode_Selectin..."
QT_MOC_LITERAL(77, 1273, 24), // "state_Connecting_Entered"
QT_MOC_LITERAL(78, 1298, 23), // "state_Connecting_Exited"
QT_MOC_LITERAL(79, 1322, 21) // "state_Default_Entered"

    },
    "NodeView\0trans_InActive2Moving\0\0"
    "trans_Moving2InActive\0trans_InActive2Resizing\0"
    "trans_Resizing2InActive\0"
    "trans_InActive2RubberbandMode\0"
    "trans_RubberbandMode2InActive\0"
    "trans_RubberbandMode2RubberbandMode_Selecting\0"
    "trans_RubberbandMode_Selecting2RubberbandMode\0"
    "trans_InActive2Connecting\0"
    "trans_Connecting2InActive\0toolbarRequested\0"
    "screenPos\0itemPos\0viewport_changed\0"
    "viewportRect\0zoom_factor\0viewFocussed\0"
    "NodeView*\0view\0focussed\0triggerAction\0"
    "setData\0removeData\0editData\0"
    "viewItem_LabelChanged\0label\0"
    "viewItem_Constructed\0ViewItem*\0viewItem\0"
    "viewItem_Destructed\0ID\0"
    "selectionHandler_ItemSelectionChanged\0"
    "item\0selected\0selectionHandler_ItemActiveSelectionChanged\0"
    "isActive\0themeChanged\0fitToScreen\0"
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
      52,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      17,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,  274,    2, 0x06 /* Public */,
       3,    0,  275,    2, 0x06 /* Public */,
       4,    0,  276,    2, 0x06 /* Public */,
       5,    0,  277,    2, 0x06 /* Public */,
       6,    0,  278,    2, 0x06 /* Public */,
       7,    0,  279,    2, 0x06 /* Public */,
       8,    0,  280,    2, 0x06 /* Public */,
       9,    0,  281,    2, 0x06 /* Public */,
      10,    0,  282,    2, 0x06 /* Public */,
      11,    0,  283,    2, 0x06 /* Public */,
      12,    2,  284,    2, 0x06 /* Public */,
      15,    2,  289,    2, 0x06 /* Public */,
      18,    2,  294,    2, 0x06 /* Public */,
      22,    1,  299,    2, 0x06 /* Public */,
      23,    3,  302,    2, 0x06 /* Public */,
      24,    2,  309,    2, 0x06 /* Public */,
      25,    2,  314,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      26,    1,  319,    2, 0x08 /* Private */,
      28,    1,  322,    2, 0x08 /* Private */,
      31,    2,  325,    2, 0x08 /* Private */,
      33,    2,  330,    2, 0x08 /* Private */,
      36,    2,  335,    2, 0x08 /* Private */,
      38,    0,  340,    2, 0x08 /* Private */,
      39,    0,  341,    2, 0x0a /* Public */,
      40,    0,  342,    2, 0x0a /* Public */,
      41,    0,  343,    2, 0x0a /* Public */,
      42,    1,  344,    2, 0x0a /* Public */,
      44,    1,  347,    2, 0x0a /* Public */,
      45,    1,  350,    2, 0x08 /* Private */,
      47,    2,  353,    2, 0x08 /* Private */,
      51,    2,  358,    2, 0x08 /* Private */,
      53,    2,  363,    2, 0x08 /* Private */,
      54,    2,  368,    2, 0x08 /* Private */,
      56,    1,  373,    2, 0x08 /* Private */,
      57,    2,  376,    2, 0x08 /* Private */,
      60,    1,  381,    2, 0x08 /* Private */,
      61,    1,  384,    2, 0x08 /* Private */,
      62,    3,  387,    2, 0x08 /* Private */,
      65,    1,  394,    2, 0x08 /* Private */,
      66,    1,  397,    2, 0x08 /* Private */,
      67,    2,  400,    2, 0x08 /* Private */,
      69,    0,  405,    2, 0x08 /* Private */,
      70,    0,  406,    2, 0x08 /* Private */,
      71,    0,  407,    2, 0x08 /* Private */,
      72,    0,  408,    2, 0x08 /* Private */,
      73,    0,  409,    2, 0x08 /* Private */,
      74,    0,  410,    2, 0x08 /* Private */,
      75,    0,  411,    2, 0x08 /* Private */,
      76,    0,  412,    2, 0x08 /* Private */,
      77,    0,  413,    2, 0x08 /* Private */,
      78,    0,  414,    2, 0x08 /* Private */,
      79,    0,  415,    2, 0x08 /* Private */,

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
    QMetaType::Void, QMetaType::QPoint, QMetaType::QPointF,   13,   14,
    QMetaType::Void, QMetaType::QRectF, QMetaType::Double,   16,   17,
    QMetaType::Void, 0x80000000 | 19, QMetaType::Bool,   20,   21,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::Int, QMetaType::QString, QMetaType::QVariant,    2,    2,    2,
    QMetaType::Void, QMetaType::Int, QMetaType::QString,    2,    2,
    QMetaType::Void, QMetaType::Int, QMetaType::QString,    2,    2,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,   27,
    QMetaType::Void, 0x80000000 | 29,   30,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 29,   32,   30,
    QMetaType::Void, 0x80000000 | 29, QMetaType::Bool,   34,   35,
    QMetaType::Void, 0x80000000 | 29, QMetaType::Bool,   34,   37,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QPointF,   43,
    QMetaType::Void, QMetaType::Int,   43,
    QMetaType::Void, 0x80000000 | 46,   34,
    QMetaType::Void, 0x80000000 | 48, 0x80000000 | 49,   34,   50,
    QMetaType::Void, 0x80000000 | 29, QMetaType::QString,   34,   52,
    QMetaType::Void, 0x80000000 | 29, QMetaType::QString,   34,   52,
    QMetaType::Void, 0x80000000 | 29, QMetaType::Bool,   34,   55,
    QMetaType::Void, 0x80000000 | 29,   34,
    QMetaType::Void, 0x80000000 | 58, QMetaType::Bool,   34,   59,
    QMetaType::Void, 0x80000000 | 58,   34,
    QMetaType::Void, QMetaType::QPointF,   43,
    QMetaType::Void, 0x80000000 | 46, QMetaType::QSizeF, 0x80000000 | 63,   34,   43,   64,
    QMetaType::Void, QMetaType::Int,   32,
    QMetaType::Void, 0x80000000 | 29,   34,
    QMetaType::Void, QMetaType::Int, QMetaType::Bool,   32,   68,
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
        case 10: _t->toolbarRequested((*reinterpret_cast< QPoint(*)>(_a[1])),(*reinterpret_cast< QPointF(*)>(_a[2]))); break;
        case 11: _t->viewport_changed((*reinterpret_cast< QRectF(*)>(_a[1])),(*reinterpret_cast< double(*)>(_a[2]))); break;
        case 12: _t->viewFocussed((*reinterpret_cast< NodeView*(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 13: _t->triggerAction((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 14: _t->setData((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QVariant(*)>(_a[3]))); break;
        case 15: _t->removeData((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 16: _t->editData((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 17: _t->viewItem_LabelChanged((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 18: _t->viewItem_Constructed((*reinterpret_cast< ViewItem*(*)>(_a[1]))); break;
        case 19: _t->viewItem_Destructed((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< ViewItem*(*)>(_a[2]))); break;
        case 20: _t->selectionHandler_ItemSelectionChanged((*reinterpret_cast< ViewItem*(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 21: _t->selectionHandler_ItemActiveSelectionChanged((*reinterpret_cast< ViewItem*(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 22: _t->themeChanged(); break;
        case 23: _t->fitToScreen(); break;
        case 24: _t->selectAll(); break;
        case 25: _t->clearSelection(); break;
        case 26: _t->minimap_Pan((*reinterpret_cast< QPointF(*)>(_a[1]))); break;
        case 27: _t->minimap_Zoom((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 28: _t->node_ConnectMode((*reinterpret_cast< NodeItem*(*)>(_a[1]))); break;
        case 29: _t->node_PopOutRelatedNode((*reinterpret_cast< NodeViewItem*(*)>(_a[1])),(*reinterpret_cast< NODE_KIND(*)>(_a[2]))); break;
        case 30: _t->item_EditData((*reinterpret_cast< ViewItem*(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 31: _t->item_RemoveData((*reinterpret_cast< ViewItem*(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 32: _t->item_Selected((*reinterpret_cast< ViewItem*(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 33: _t->item_ActiveSelected((*reinterpret_cast< ViewItem*(*)>(_a[1]))); break;
        case 34: _t->item_SetExpanded((*reinterpret_cast< EntityItem*(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 35: _t->item_SetCentered((*reinterpret_cast< EntityItem*(*)>(_a[1]))); break;
        case 36: _t->item_MoveSelection((*reinterpret_cast< QPointF(*)>(_a[1]))); break;
        case 37: _t->item_Resize((*reinterpret_cast< NodeItem*(*)>(_a[1])),(*reinterpret_cast< QSizeF(*)>(_a[2])),(*reinterpret_cast< RECT_VERTEX(*)>(_a[3]))); break;
        case 38: _t->centerItem((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 39: _t->centerConnections((*reinterpret_cast< ViewItem*(*)>(_a[1]))); break;
        case 40: _t->highlightItem((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 41: _t->state_Moving_Entered(); break;
        case 42: _t->state_Moving_Exited(); break;
        case 43: _t->state_Resizing_Entered(); break;
        case 44: _t->state_Resizing_Exited(); break;
        case 45: _t->state_RubberbandMode_Entered(); break;
        case 46: _t->state_RubberbandMode_Exited(); break;
        case 47: _t->state_RubberbandMode_Selecting_Entered(); break;
        case 48: _t->state_RubberbandMode_Selecting_Exited(); break;
        case 49: _t->state_Connecting_Entered(); break;
        case 50: _t->state_Connecting_Exited(); break;
        case 51: _t->state_Default_Entered(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 12:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< NodeView* >(); break;
            }
            break;
        case 18:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< ViewItem* >(); break;
            }
            break;
        case 19:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< ViewItem* >(); break;
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
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< ViewItem* >(); break;
            }
            break;
        case 28:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< NodeItem* >(); break;
            }
            break;
        case 29:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< NODE_KIND >(); break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< NodeViewItem* >(); break;
            }
            break;
        case 30:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< ViewItem* >(); break;
            }
            break;
        case 31:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< ViewItem* >(); break;
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
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< EntityItem* >(); break;
            }
            break;
        case 35:
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
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< NodeItem* >(); break;
            }
            break;
        case 39:
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
            typedef void (NodeView::*_t)(QPoint , QPointF );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NodeView::toolbarRequested)) {
                *result = 10;
                return;
            }
        }
        {
            typedef void (NodeView::*_t)(QRectF , double );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NodeView::viewport_changed)) {
                *result = 11;
                return;
            }
        }
        {
            typedef void (NodeView::*_t)(NodeView * , bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NodeView::viewFocussed)) {
                *result = 12;
                return;
            }
        }
        {
            typedef void (NodeView::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NodeView::triggerAction)) {
                *result = 13;
                return;
            }
        }
        {
            typedef void (NodeView::*_t)(int , QString , QVariant );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NodeView::setData)) {
                *result = 14;
                return;
            }
        }
        {
            typedef void (NodeView::*_t)(int , QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NodeView::removeData)) {
                *result = 15;
                return;
            }
        }
        {
            typedef void (NodeView::*_t)(int , QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NodeView::editData)) {
                *result = 16;
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
        if (_id < 52)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 52;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 52)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 52;
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
void NodeView::toolbarRequested(QPoint _t1, QPointF _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 10, _a);
}

// SIGNAL 11
void NodeView::viewport_changed(QRectF _t1, double _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 11, _a);
}

// SIGNAL 12
void NodeView::viewFocussed(NodeView * _t1, bool _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 12, _a);
}

// SIGNAL 13
void NodeView::triggerAction(QString _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 13, _a);
}

// SIGNAL 14
void NodeView::setData(int _t1, QString _t2, QVariant _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 14, _a);
}

// SIGNAL 15
void NodeView::removeData(int _t1, QString _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 15, _a);
}

// SIGNAL 16
void NodeView::editData(int _t1, QString _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 16, _a);
}
QT_END_MOC_NAMESPACE
