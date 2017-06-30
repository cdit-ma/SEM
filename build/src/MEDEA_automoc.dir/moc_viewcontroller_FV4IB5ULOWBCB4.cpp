/****************************************************************************
** Meta object code from reading C++ file 'viewcontroller.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/Controllers/ViewController/viewcontroller.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QList>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'viewcontroller.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_ViewController_t {
    QByteArrayData data[181];
    char stringdata0[2585];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ViewController_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ViewController_t qt_meta_stringdata_ViewController = {
    {
QT_MOC_LITERAL(0, 0, 14), // "ViewController"
QT_MOC_LITERAL(1, 15, 20), // "vc_showWelcomeScreen"
QT_MOC_LITERAL(2, 36, 0), // ""
QT_MOC_LITERAL(3, 37, 15), // "vc_JenkinsReady"
QT_MOC_LITERAL(4, 53, 12), // "vc_JavaReady"
QT_MOC_LITERAL(5, 66, 18), // "vc_controllerReady"
QT_MOC_LITERAL(6, 85, 22), // "vc_viewItemConstructed"
QT_MOC_LITERAL(7, 108, 9), // "ViewItem*"
QT_MOC_LITERAL(8, 118, 8), // "viewItem"
QT_MOC_LITERAL(9, 127, 22), // "vc_viewItemDestructing"
QT_MOC_LITERAL(10, 150, 2), // "ID"
QT_MOC_LITERAL(11, 153, 4), // "item"
QT_MOC_LITERAL(12, 158, 14), // "vc_showToolbar"
QT_MOC_LITERAL(13, 173, 9), // "globalPos"
QT_MOC_LITERAL(14, 183, 7), // "itemPos"
QT_MOC_LITERAL(15, 191, 23), // "vc_gotSearchSuggestions"
QT_MOC_LITERAL(16, 215, 11), // "suggestions"
QT_MOC_LITERAL(17, 227, 16), // "vc_editTableCell"
QT_MOC_LITERAL(18, 244, 7), // "keyName"
QT_MOC_LITERAL(19, 252, 16), // "vc_projectClosed"
QT_MOC_LITERAL(20, 269, 13), // "vc_setupModel"
QT_MOC_LITERAL(21, 283, 7), // "vc_undo"
QT_MOC_LITERAL(22, 291, 7), // "vc_redo"
QT_MOC_LITERAL(23, 299, 16), // "vc_triggerAction"
QT_MOC_LITERAL(24, 316, 10), // "vc_setData"
QT_MOC_LITERAL(25, 327, 13), // "vc_removeData"
QT_MOC_LITERAL(26, 341, 17), // "vc_deleteEntities"
QT_MOC_LITERAL(27, 359, 10), // "QList<int>"
QT_MOC_LITERAL(28, 370, 3), // "IDs"
QT_MOC_LITERAL(29, 374, 14), // "vc_cutEntities"
QT_MOC_LITERAL(30, 389, 15), // "vc_copyEntities"
QT_MOC_LITERAL(31, 405, 8), // "vc_paste"
QT_MOC_LITERAL(32, 414, 4), // "data"
QT_MOC_LITERAL(33, 419, 20), // "vc_replicateEntities"
QT_MOC_LITERAL(34, 440, 16), // "vc_constructNode"
QT_MOC_LITERAL(35, 457, 8), // "parentID"
QT_MOC_LITERAL(36, 466, 9), // "NODE_KIND"
QT_MOC_LITERAL(37, 476, 8), // "nodeKind"
QT_MOC_LITERAL(38, 485, 3), // "pos"
QT_MOC_LITERAL(39, 489, 16), // "vc_constructEdge"
QT_MOC_LITERAL(40, 506, 9), // "sourceIDs"
QT_MOC_LITERAL(41, 516, 5), // "dstID"
QT_MOC_LITERAL(42, 522, 9), // "EDGE_KIND"
QT_MOC_LITERAL(43, 532, 8), // "edgeKind"
QT_MOC_LITERAL(44, 541, 16), // "vc_destructEdges"
QT_MOC_LITERAL(45, 558, 25), // "vc_constructConnectedNode"
QT_MOC_LITERAL(46, 584, 25), // "vc_constructWorkerProcess"
QT_MOC_LITERAL(47, 610, 5), // "point"
QT_MOC_LITERAL(48, 616, 17), // "vc_importProjects"
QT_MOC_LITERAL(49, 634, 8), // "fileData"
QT_MOC_LITERAL(50, 643, 14), // "vc_openProject"
QT_MOC_LITERAL(51, 658, 8), // "fileName"
QT_MOC_LITERAL(52, 667, 8), // "filePath"
QT_MOC_LITERAL(53, 676, 15), // "vc_projectSaved"
QT_MOC_LITERAL(54, 692, 21), // "vc_projectPathChanged"
QT_MOC_LITERAL(55, 714, 17), // "vc_answerQuestion"
QT_MOC_LITERAL(56, 732, 16), // "vc_exportSnippet"
QT_MOC_LITERAL(57, 749, 16), // "vc_importSnippet"
QT_MOC_LITERAL(58, 766, 15), // "mc_showProgress"
QT_MOC_LITERAL(59, 782, 18), // "mc_progressChanged"
QT_MOC_LITERAL(60, 801, 13), // "mc_modelReady"
QT_MOC_LITERAL(61, 815, 18), // "mc_projectModified"
QT_MOC_LITERAL(62, 834, 18), // "mc_undoRedoUpdated"
QT_MOC_LITERAL(63, 853, 17), // "vc_actionFinished"
QT_MOC_LITERAL(64, 871, 20), // "vc_executeJenkinsJob"
QT_MOC_LITERAL(65, 892, 13), // "vc_centerItem"
QT_MOC_LITERAL(66, 906, 35), // "vc_selectAndCenterConnectedEn..."
QT_MOC_LITERAL(67, 942, 14), // "vc_fitToScreen"
QT_MOC_LITERAL(68, 957, 29), // "vc_addProjectToRecentProjects"
QT_MOC_LITERAL(69, 987, 22), // "vc_getCodeForComponent"
QT_MOC_LITERAL(70, 1010, 11), // "graphmlPath"
QT_MOC_LITERAL(71, 1022, 13), // "componentName"
QT_MOC_LITERAL(72, 1036, 16), // "vc_validateModel"
QT_MOC_LITERAL(73, 1053, 10), // "reportPath"
QT_MOC_LITERAL(74, 1064, 17), // "vc_modelValidated"
QT_MOC_LITERAL(75, 1082, 6), // "report"
QT_MOC_LITERAL(76, 1089, 24), // "vc_launchLocalDeployment"
QT_MOC_LITERAL(77, 1114, 20), // "vc_backgroundProcess"
QT_MOC_LITERAL(78, 1135, 10), // "inProgress"
QT_MOC_LITERAL(79, 1146, 18), // "BACKGROUND_PROCESS"
QT_MOC_LITERAL(80, 1165, 7), // "process"
QT_MOC_LITERAL(81, 1173, 19), // "vc_importXMEProject"
QT_MOC_LITERAL(82, 1193, 7), // "xmePath"
QT_MOC_LITERAL(83, 1201, 19), // "vc_importXMIProject"
QT_MOC_LITERAL(84, 1221, 7), // "XMIPath"
QT_MOC_LITERAL(85, 1229, 16), // "vc_highlightItem"
QT_MOC_LITERAL(86, 1246, 9), // "highlight"
QT_MOC_LITERAL(87, 1256, 13), // "projectOpened"
QT_MOC_LITERAL(88, 1270, 7), // "success"
QT_MOC_LITERAL(89, 1278, 18), // "gotExportedSnippet"
QT_MOC_LITERAL(90, 1297, 11), // "snippetData"
QT_MOC_LITERAL(91, 1309, 11), // "askQuestion"
QT_MOC_LITERAL(92, 1321, 5), // "title"
QT_MOC_LITERAL(93, 1327, 7), // "message"
QT_MOC_LITERAL(94, 1335, 14), // "modelValidated"
QT_MOC_LITERAL(95, 1350, 17), // "importGraphMLFile"
QT_MOC_LITERAL(96, 1368, 20), // "importGraphMLExtract"
QT_MOC_LITERAL(97, 1389, 14), // "showCodeViewer"
QT_MOC_LITERAL(98, 1404, 7), // "tabName"
QT_MOC_LITERAL(99, 1412, 7), // "content"
QT_MOC_LITERAL(100, 1420, 32), // "jenkinsManager_SettingsValidated"
QT_MOC_LITERAL(101, 1453, 11), // "errorString"
QT_MOC_LITERAL(102, 1465, 22), // "jenkinsManager_GotJava"
QT_MOC_LITERAL(103, 1488, 4), // "java"
QT_MOC_LITERAL(104, 1493, 11), // "javaVersion"
QT_MOC_LITERAL(105, 1505, 34), // "jenkinsManager_GotJenkinsNode..."
QT_MOC_LITERAL(106, 1540, 11), // "graphmlData"
QT_MOC_LITERAL(107, 1552, 19), // "getCodeForComponent"
QT_MOC_LITERAL(108, 1572, 13), // "validateModel"
QT_MOC_LITERAL(109, 1586, 11), // "selectModel"
QT_MOC_LITERAL(110, 1598, 21), // "launchLocalDeployment"
QT_MOC_LITERAL(111, 1620, 14), // "actionFinished"
QT_MOC_LITERAL(112, 1635, 2), // "gg"
QT_MOC_LITERAL(113, 1638, 21), // "model_NodeConstructed"
QT_MOC_LITERAL(114, 1660, 9), // "parent_id"
QT_MOC_LITERAL(115, 1670, 2), // "id"
QT_MOC_LITERAL(116, 1673, 4), // "kind"
QT_MOC_LITERAL(117, 1678, 21), // "model_EdgeConstructed"
QT_MOC_LITERAL(118, 1700, 6), // "src_id"
QT_MOC_LITERAL(119, 1707, 6), // "dst_id"
QT_MOC_LITERAL(120, 1714, 27), // "controller_entityDestructed"
QT_MOC_LITERAL(121, 1742, 12), // "GRAPHML_KIND"
QT_MOC_LITERAL(122, 1755, 5), // "eKind"
QT_MOC_LITERAL(123, 1761, 22), // "controller_dataChanged"
QT_MOC_LITERAL(124, 1784, 3), // "key"
QT_MOC_LITERAL(125, 1788, 22), // "controller_dataRemoved"
QT_MOC_LITERAL(126, 1811, 16), // "setClipboardData"
QT_MOC_LITERAL(127, 1828, 10), // "newProject"
QT_MOC_LITERAL(128, 1839, 11), // "openProject"
QT_MOC_LITERAL(129, 1851, 19), // "openExistingProject"
QT_MOC_LITERAL(130, 1871, 4), // "file"
QT_MOC_LITERAL(131, 1876, 14), // "importProjects"
QT_MOC_LITERAL(132, 1891, 16), // "importXMEProject"
QT_MOC_LITERAL(133, 1908, 16), // "importXMIProject"
QT_MOC_LITERAL(134, 1925, 13), // "importSnippet"
QT_MOC_LITERAL(135, 1939, 13), // "exportSnippet"
QT_MOC_LITERAL(136, 1953, 11), // "saveProject"
QT_MOC_LITERAL(137, 1965, 13), // "saveAsProject"
QT_MOC_LITERAL(138, 1979, 12), // "closeProject"
QT_MOC_LITERAL(139, 1992, 10), // "closeMEDEA"
QT_MOC_LITERAL(140, 2003, 17), // "generateWorkspace"
QT_MOC_LITERAL(141, 2021, 17), // "executeJenkinsJob"
QT_MOC_LITERAL(142, 2039, 7), // "fitView"
QT_MOC_LITERAL(143, 2047, 11), // "fitAllViews"
QT_MOC_LITERAL(144, 2059, 15), // "centerSelection"
QT_MOC_LITERAL(145, 2075, 22), // "alignSelectionVertical"
QT_MOC_LITERAL(146, 2098, 24), // "alignSelectionHorizontal"
QT_MOC_LITERAL(147, 2123, 32), // "selectAndCenterConnectedEntities"
QT_MOC_LITERAL(148, 2156, 10), // "centerOnID"
QT_MOC_LITERAL(149, 2167, 8), // "showWiki"
QT_MOC_LITERAL(150, 2176, 9), // "reportBug"
QT_MOC_LITERAL(151, 2186, 23), // "showWikiForSelectedItem"
QT_MOC_LITERAL(152, 2210, 10), // "centerImpl"
QT_MOC_LITERAL(153, 2221, 16), // "centerDefinition"
QT_MOC_LITERAL(154, 2238, 15), // "popupDefinition"
QT_MOC_LITERAL(155, 2254, 9), // "popupImpl"
QT_MOC_LITERAL(156, 2264, 14), // "popupSelection"
QT_MOC_LITERAL(157, 2279, 9), // "popupItem"
QT_MOC_LITERAL(158, 2289, 7), // "aboutQt"
QT_MOC_LITERAL(159, 2297, 10), // "aboutMEDEA"
QT_MOC_LITERAL(160, 2308, 3), // "cut"
QT_MOC_LITERAL(161, 2312, 4), // "copy"
QT_MOC_LITERAL(162, 2317, 5), // "paste"
QT_MOC_LITERAL(163, 2323, 9), // "replicate"
QT_MOC_LITERAL(164, 2333, 15), // "deleteSelection"
QT_MOC_LITERAL(165, 2349, 15), // "expandSelection"
QT_MOC_LITERAL(166, 2365, 17), // "contractSelection"
QT_MOC_LITERAL(167, 2383, 9), // "editLabel"
QT_MOC_LITERAL(168, 2393, 20), // "editReplicationCount"
QT_MOC_LITERAL(169, 2414, 20), // "setSelectionReadOnly"
QT_MOC_LITERAL(170, 2435, 6), // "locked"
QT_MOC_LITERAL(171, 2442, 22), // "constructDDSQOSProfile"
QT_MOC_LITERAL(172, 2465, 24), // "requestSearchSuggestions"
QT_MOC_LITERAL(173, 2490, 13), // "setModelReady"
QT_MOC_LITERAL(174, 2504, 4), // "okay"
QT_MOC_LITERAL(175, 2509, 18), // "setControllerReady"
QT_MOC_LITERAL(176, 2528, 5), // "ready"
QT_MOC_LITERAL(177, 2534, 7), // "openURL"
QT_MOC_LITERAL(178, 2542, 3), // "url"
QT_MOC_LITERAL(179, 2546, 20), // "initializeController"
QT_MOC_LITERAL(180, 2567, 17) // "table_dataChanged"

    },
    "ViewController\0vc_showWelcomeScreen\0"
    "\0vc_JenkinsReady\0vc_JavaReady\0"
    "vc_controllerReady\0vc_viewItemConstructed\0"
    "ViewItem*\0viewItem\0vc_viewItemDestructing\0"
    "ID\0item\0vc_showToolbar\0globalPos\0"
    "itemPos\0vc_gotSearchSuggestions\0"
    "suggestions\0vc_editTableCell\0keyName\0"
    "vc_projectClosed\0vc_setupModel\0vc_undo\0"
    "vc_redo\0vc_triggerAction\0vc_setData\0"
    "vc_removeData\0vc_deleteEntities\0"
    "QList<int>\0IDs\0vc_cutEntities\0"
    "vc_copyEntities\0vc_paste\0data\0"
    "vc_replicateEntities\0vc_constructNode\0"
    "parentID\0NODE_KIND\0nodeKind\0pos\0"
    "vc_constructEdge\0sourceIDs\0dstID\0"
    "EDGE_KIND\0edgeKind\0vc_destructEdges\0"
    "vc_constructConnectedNode\0"
    "vc_constructWorkerProcess\0point\0"
    "vc_importProjects\0fileData\0vc_openProject\0"
    "fileName\0filePath\0vc_projectSaved\0"
    "vc_projectPathChanged\0vc_answerQuestion\0"
    "vc_exportSnippet\0vc_importSnippet\0"
    "mc_showProgress\0mc_progressChanged\0"
    "mc_modelReady\0mc_projectModified\0"
    "mc_undoRedoUpdated\0vc_actionFinished\0"
    "vc_executeJenkinsJob\0vc_centerItem\0"
    "vc_selectAndCenterConnectedEntities\0"
    "vc_fitToScreen\0vc_addProjectToRecentProjects\0"
    "vc_getCodeForComponent\0graphmlPath\0"
    "componentName\0vc_validateModel\0"
    "reportPath\0vc_modelValidated\0report\0"
    "vc_launchLocalDeployment\0vc_backgroundProcess\0"
    "inProgress\0BACKGROUND_PROCESS\0process\0"
    "vc_importXMEProject\0xmePath\0"
    "vc_importXMIProject\0XMIPath\0"
    "vc_highlightItem\0highlight\0projectOpened\0"
    "success\0gotExportedSnippet\0snippetData\0"
    "askQuestion\0title\0message\0modelValidated\0"
    "importGraphMLFile\0importGraphMLExtract\0"
    "showCodeViewer\0tabName\0content\0"
    "jenkinsManager_SettingsValidated\0"
    "errorString\0jenkinsManager_GotJava\0"
    "java\0javaVersion\0jenkinsManager_GotJenkinsNodesList\0"
    "graphmlData\0getCodeForComponent\0"
    "validateModel\0selectModel\0"
    "launchLocalDeployment\0actionFinished\0"
    "gg\0model_NodeConstructed\0parent_id\0"
    "id\0kind\0model_EdgeConstructed\0src_id\0"
    "dst_id\0controller_entityDestructed\0"
    "GRAPHML_KIND\0eKind\0controller_dataChanged\0"
    "key\0controller_dataRemoved\0setClipboardData\0"
    "newProject\0openProject\0openExistingProject\0"
    "file\0importProjects\0importXMEProject\0"
    "importXMIProject\0importSnippet\0"
    "exportSnippet\0saveProject\0saveAsProject\0"
    "closeProject\0closeMEDEA\0generateWorkspace\0"
    "executeJenkinsJob\0fitView\0fitAllViews\0"
    "centerSelection\0alignSelectionVertical\0"
    "alignSelectionHorizontal\0"
    "selectAndCenterConnectedEntities\0"
    "centerOnID\0showWiki\0reportBug\0"
    "showWikiForSelectedItem\0centerImpl\0"
    "centerDefinition\0popupDefinition\0"
    "popupImpl\0popupSelection\0popupItem\0"
    "aboutQt\0aboutMEDEA\0cut\0copy\0paste\0"
    "replicate\0deleteSelection\0expandSelection\0"
    "contractSelection\0editLabel\0"
    "editReplicationCount\0setSelectionReadOnly\0"
    "locked\0constructDDSQOSProfile\0"
    "requestSearchSuggestions\0setModelReady\0"
    "okay\0setControllerReady\0ready\0openURL\0"
    "url\0initializeController\0table_dataChanged"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ViewController[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
     126,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      56,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,  644,    2, 0x06 /* Public */,
       3,    1,  647,    2, 0x06 /* Public */,
       4,    1,  650,    2, 0x06 /* Public */,
       5,    1,  653,    2, 0x06 /* Public */,
       6,    1,  656,    2, 0x06 /* Public */,
       9,    2,  659,    2, 0x06 /* Public */,
      12,    2,  664,    2, 0x06 /* Public */,
      12,    1,  669,    2, 0x26 /* Public | MethodCloned */,
      15,    1,  672,    2, 0x06 /* Public */,
      17,    2,  675,    2, 0x06 /* Public */,
      19,    0,  680,    2, 0x06 /* Public */,
      20,    0,  681,    2, 0x06 /* Public */,
      21,    0,  682,    2, 0x06 /* Public */,
      22,    0,  683,    2, 0x06 /* Public */,
      23,    1,  684,    2, 0x06 /* Public */,
      24,    3,  687,    2, 0x06 /* Public */,
      25,    2,  694,    2, 0x06 /* Public */,
      26,    1,  699,    2, 0x06 /* Public */,
      29,    1,  702,    2, 0x06 /* Public */,
      30,    1,  705,    2, 0x06 /* Public */,
      31,    2,  708,    2, 0x06 /* Public */,
      33,    1,  713,    2, 0x06 /* Public */,
      34,    3,  716,    2, 0x06 /* Public */,
      34,    2,  723,    2, 0x26 /* Public | MethodCloned */,
      39,    3,  728,    2, 0x06 /* Public */,
      44,    3,  735,    2, 0x06 /* Public */,
      45,    5,  742,    2, 0x06 /* Public */,
      45,    4,  753,    2, 0x26 /* Public | MethodCloned */,
      46,    3,  762,    2, 0x06 /* Public */,
      48,    1,  769,    2, 0x06 /* Public */,
      50,    2,  772,    2, 0x06 /* Public */,
      53,    1,  777,    2, 0x06 /* Public */,
      54,    1,  780,    2, 0x06 /* Public */,
      55,    1,  783,    2, 0x06 /* Public */,
      56,    1,  786,    2, 0x06 /* Public */,
      57,    3,  789,    2, 0x06 /* Public */,
      58,    2,  796,    2, 0x06 /* Public */,
      59,    1,  801,    2, 0x06 /* Public */,
      60,    1,  804,    2, 0x06 /* Public */,
      61,    1,  807,    2, 0x06 /* Public */,
      62,    0,  810,    2, 0x06 /* Public */,
      63,    0,  811,    2, 0x06 /* Public */,
      64,    1,  812,    2, 0x06 /* Public */,
      65,    1,  815,    2, 0x06 /* Public */,
      66,    1,  818,    2, 0x06 /* Public */,
      67,    0,  821,    2, 0x06 /* Public */,
      68,    1,  822,    2, 0x06 /* Public */,
      69,    2,  825,    2, 0x06 /* Public */,
      72,    2,  830,    2, 0x06 /* Public */,
      74,    1,  835,    2, 0x06 /* Public */,
      76,    1,  838,    2, 0x06 /* Public */,
      77,    2,  841,    2, 0x06 /* Public */,
      77,    1,  846,    2, 0x26 /* Public | MethodCloned */,
      81,    2,  849,    2, 0x06 /* Public */,
      83,    1,  854,    2, 0x06 /* Public */,
      85,    2,  857,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      87,    1,  862,    2, 0x0a /* Public */,
      89,    1,  865,    2, 0x0a /* Public */,
      91,    3,  868,    2, 0x0a /* Public */,
      94,    1,  875,    2, 0x0a /* Public */,
      95,    1,  878,    2, 0x0a /* Public */,
      96,    1,  881,    2, 0x0a /* Public */,
      97,    2,  884,    2, 0x0a /* Public */,
     100,    2,  889,    2, 0x0a /* Public */,
     102,    2,  894,    2, 0x0a /* Public */,
     105,    1,  899,    2, 0x0a /* Public */,
     107,    0,  902,    2, 0x0a /* Public */,
     108,    0,  903,    2, 0x0a /* Public */,
     109,    0,  904,    2, 0x0a /* Public */,
     110,    0,  905,    2, 0x0a /* Public */,
     111,    2,  906,    2, 0x0a /* Public */,
     113,    3,  911,    2, 0x0a /* Public */,
     117,    4,  918,    2, 0x0a /* Public */,
     120,    3,  927,    2, 0x0a /* Public */,
     123,    3,  934,    2, 0x0a /* Public */,
     125,    2,  941,    2, 0x0a /* Public */,
     126,    1,  946,    2, 0x0a /* Public */,
     127,    0,  949,    2, 0x0a /* Public */,
     128,    0,  950,    2, 0x0a /* Public */,
     129,    1,  951,    2, 0x0a /* Public */,
     131,    0,  954,    2, 0x0a /* Public */,
     132,    0,  955,    2, 0x0a /* Public */,
     133,    0,  956,    2, 0x0a /* Public */,
     134,    0,  957,    2, 0x0a /* Public */,
     135,    0,  958,    2, 0x0a /* Public */,
     136,    0,  959,    2, 0x0a /* Public */,
     137,    0,  960,    2, 0x0a /* Public */,
     138,    0,  961,    2, 0x0a /* Public */,
     139,    0,  962,    2, 0x0a /* Public */,
     140,    0,  963,    2, 0x0a /* Public */,
     141,    0,  964,    2, 0x0a /* Public */,
     142,    0,  965,    2, 0x0a /* Public */,
     143,    0,  966,    2, 0x0a /* Public */,
     144,    0,  967,    2, 0x0a /* Public */,
     145,    0,  968,    2, 0x0a /* Public */,
     146,    0,  969,    2, 0x0a /* Public */,
     147,    0,  970,    2, 0x0a /* Public */,
     148,    1,  971,    2, 0x0a /* Public */,
     149,    0,  974,    2, 0x0a /* Public */,
     150,    0,  975,    2, 0x0a /* Public */,
     151,    0,  976,    2, 0x0a /* Public */,
     152,    0,  977,    2, 0x0a /* Public */,
     153,    0,  978,    2, 0x0a /* Public */,
     154,    0,  979,    2, 0x0a /* Public */,
     155,    0,  980,    2, 0x0a /* Public */,
     156,    0,  981,    2, 0x0a /* Public */,
     157,    1,  982,    2, 0x0a /* Public */,
     158,    0,  985,    2, 0x0a /* Public */,
     159,    0,  986,    2, 0x0a /* Public */,
     160,    0,  987,    2, 0x0a /* Public */,
     161,    0,  988,    2, 0x0a /* Public */,
     162,    0,  989,    2, 0x0a /* Public */,
     163,    0,  990,    2, 0x0a /* Public */,
     164,    0,  991,    2, 0x0a /* Public */,
     165,    0,  992,    2, 0x0a /* Public */,
     166,    0,  993,    2, 0x0a /* Public */,
     167,    0,  994,    2, 0x0a /* Public */,
     168,    0,  995,    2, 0x0a /* Public */,
     169,    1,  996,    2, 0x0a /* Public */,
     171,    0,  999,    2, 0x0a /* Public */,
     172,    0, 1000,    2, 0x0a /* Public */,
     173,    1, 1001,    2, 0x0a /* Public */,
     175,    1, 1004,    2, 0x0a /* Public */,
     177,    1, 1007,    2, 0x0a /* Public */,
     179,    0, 1010,    2, 0x08 /* Private */,
     180,    3, 1011,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, 0x80000000 | 7,    8,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 7,   10,   11,
    QMetaType::Void, QMetaType::QPoint, QMetaType::QPointF,   13,   14,
    QMetaType::Void, QMetaType::QPoint,   13,
    QMetaType::Void, QMetaType::QStringList,   16,
    QMetaType::Void, QMetaType::Int, QMetaType::QString,   10,   18,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::Int, QMetaType::QString, QMetaType::QVariant,    2,    2,    2,
    QMetaType::Void, QMetaType::Int, QMetaType::QString,    2,    2,
    QMetaType::Void, 0x80000000 | 27,   28,
    QMetaType::Void, 0x80000000 | 27,   28,
    QMetaType::Void, 0x80000000 | 27,   28,
    QMetaType::Void, 0x80000000 | 27, QMetaType::QString,   28,   32,
    QMetaType::Void, 0x80000000 | 27,   28,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 36, QMetaType::QPointF,   35,   37,   38,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 36,   35,   37,
    QMetaType::Void, 0x80000000 | 27, QMetaType::Int, 0x80000000 | 42,   40,   41,   43,
    QMetaType::Void, 0x80000000 | 27, QMetaType::Int, 0x80000000 | 42,   40,   41,   43,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 36, QMetaType::Int, 0x80000000 | 42, QMetaType::QPointF,   35,   37,   41,   43,   38,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 36, QMetaType::Int, 0x80000000 | 42,   35,   37,   41,   43,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::QPointF,   35,   41,   47,
    QMetaType::Void, QMetaType::QStringList,   49,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   51,   52,
    QMetaType::Void, QMetaType::QString,   52,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, 0x80000000 | 27,   28,
    QMetaType::Void, 0x80000000 | 27, QMetaType::QString, QMetaType::QString,   28,   51,   49,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,    2,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   52,
    QMetaType::Void, QMetaType::Int,   10,
    QMetaType::Void, 0x80000000 | 7,   11,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   52,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   70,   71,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   70,   73,
    QMetaType::Void, QMetaType::QStringList,   75,
    QMetaType::Void, QMetaType::QString,   70,
    QMetaType::Void, QMetaType::Bool, 0x80000000 | 79,   78,   80,
    QMetaType::Void, QMetaType::Bool,   78,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   82,   70,
    QMetaType::Void, QMetaType::QString,   84,
    QMetaType::Void, QMetaType::Int, QMetaType::Bool,   10,   86,

 // slots: parameters
    QMetaType::Void, QMetaType::Bool,   88,
    QMetaType::Void, QMetaType::QString,   90,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::Int,   92,   93,   10,
    QMetaType::Void, QMetaType::QString,   73,
    QMetaType::Void, QMetaType::QString,   70,
    QMetaType::Void, QMetaType::QString,   32,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   98,   99,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,   88,  101,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,  103,  104,
    QMetaType::Void, QMetaType::QString,  106,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,   88,  112,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, 0x80000000 | 36,  114,  115,  116,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 42, QMetaType::Int, QMetaType::Int,  115,  116,  118,  119,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 121, QMetaType::QString,   10,  122,  116,
    QMetaType::Void, QMetaType::Int, QMetaType::QString, QMetaType::QVariant,   10,  124,   32,
    QMetaType::Void, QMetaType::Int, QMetaType::QString,   10,  124,
    QMetaType::Void, QMetaType::QString,   32,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,  130,
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
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   10,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   10,
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
    QMetaType::Void, QMetaType::Bool,  170,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,  174,
    QMetaType::Void, QMetaType::Bool,  176,
    QMetaType::Void, QMetaType::QString,  178,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::QString, QMetaType::QVariant,   10,  124,   32,

       0        // eod
};

void ViewController::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ViewController *_t = static_cast<ViewController *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->vc_showWelcomeScreen((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->vc_JenkinsReady((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: _t->vc_JavaReady((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: _t->vc_controllerReady((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: _t->vc_viewItemConstructed((*reinterpret_cast< ViewItem*(*)>(_a[1]))); break;
        case 5: _t->vc_viewItemDestructing((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< ViewItem*(*)>(_a[2]))); break;
        case 6: _t->vc_showToolbar((*reinterpret_cast< QPoint(*)>(_a[1])),(*reinterpret_cast< QPointF(*)>(_a[2]))); break;
        case 7: _t->vc_showToolbar((*reinterpret_cast< QPoint(*)>(_a[1]))); break;
        case 8: _t->vc_gotSearchSuggestions((*reinterpret_cast< QStringList(*)>(_a[1]))); break;
        case 9: _t->vc_editTableCell((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 10: _t->vc_projectClosed(); break;
        case 11: _t->vc_setupModel(); break;
        case 12: _t->vc_undo(); break;
        case 13: _t->vc_redo(); break;
        case 14: _t->vc_triggerAction((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 15: _t->vc_setData((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QVariant(*)>(_a[3]))); break;
        case 16: _t->vc_removeData((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 17: _t->vc_deleteEntities((*reinterpret_cast< QList<int>(*)>(_a[1]))); break;
        case 18: _t->vc_cutEntities((*reinterpret_cast< QList<int>(*)>(_a[1]))); break;
        case 19: _t->vc_copyEntities((*reinterpret_cast< QList<int>(*)>(_a[1]))); break;
        case 20: _t->vc_paste((*reinterpret_cast< QList<int>(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 21: _t->vc_replicateEntities((*reinterpret_cast< QList<int>(*)>(_a[1]))); break;
        case 22: _t->vc_constructNode((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< NODE_KIND(*)>(_a[2])),(*reinterpret_cast< QPointF(*)>(_a[3]))); break;
        case 23: _t->vc_constructNode((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< NODE_KIND(*)>(_a[2]))); break;
        case 24: _t->vc_constructEdge((*reinterpret_cast< QList<int>(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< EDGE_KIND(*)>(_a[3]))); break;
        case 25: _t->vc_destructEdges((*reinterpret_cast< QList<int>(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< EDGE_KIND(*)>(_a[3]))); break;
        case 26: _t->vc_constructConnectedNode((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< NODE_KIND(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< EDGE_KIND(*)>(_a[4])),(*reinterpret_cast< QPointF(*)>(_a[5]))); break;
        case 27: _t->vc_constructConnectedNode((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< NODE_KIND(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< EDGE_KIND(*)>(_a[4]))); break;
        case 28: _t->vc_constructWorkerProcess((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< QPointF(*)>(_a[3]))); break;
        case 29: _t->vc_importProjects((*reinterpret_cast< QStringList(*)>(_a[1]))); break;
        case 30: _t->vc_openProject((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 31: _t->vc_projectSaved((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 32: _t->vc_projectPathChanged((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 33: _t->vc_answerQuestion((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 34: _t->vc_exportSnippet((*reinterpret_cast< QList<int>(*)>(_a[1]))); break;
        case 35: _t->vc_importSnippet((*reinterpret_cast< QList<int>(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3]))); break;
        case 36: _t->mc_showProgress((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 37: _t->mc_progressChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 38: _t->mc_modelReady((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 39: _t->mc_projectModified((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 40: _t->mc_undoRedoUpdated(); break;
        case 41: _t->vc_actionFinished(); break;
        case 42: _t->vc_executeJenkinsJob((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 43: _t->vc_centerItem((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 44: _t->vc_selectAndCenterConnectedEntities((*reinterpret_cast< ViewItem*(*)>(_a[1]))); break;
        case 45: _t->vc_fitToScreen(); break;
        case 46: _t->vc_addProjectToRecentProjects((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 47: _t->vc_getCodeForComponent((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 48: _t->vc_validateModel((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 49: _t->vc_modelValidated((*reinterpret_cast< QStringList(*)>(_a[1]))); break;
        case 50: _t->vc_launchLocalDeployment((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 51: _t->vc_backgroundProcess((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< BACKGROUND_PROCESS(*)>(_a[2]))); break;
        case 52: _t->vc_backgroundProcess((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 53: _t->vc_importXMEProject((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 54: _t->vc_importXMIProject((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 55: _t->vc_highlightItem((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 56: _t->projectOpened((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 57: _t->gotExportedSnippet((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 58: _t->askQuestion((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 59: _t->modelValidated((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 60: _t->importGraphMLFile((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 61: _t->importGraphMLExtract((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 62: _t->showCodeViewer((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 63: _t->jenkinsManager_SettingsValidated((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 64: _t->jenkinsManager_GotJava((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 65: _t->jenkinsManager_GotJenkinsNodesList((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 66: _t->getCodeForComponent(); break;
        case 67: _t->validateModel(); break;
        case 68: _t->selectModel(); break;
        case 69: _t->launchLocalDeployment(); break;
        case 70: _t->actionFinished((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 71: _t->model_NodeConstructed((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< NODE_KIND(*)>(_a[3]))); break;
        case 72: _t->model_EdgeConstructed((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< EDGE_KIND(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4]))); break;
        case 73: _t->controller_entityDestructed((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< GRAPHML_KIND(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3]))); break;
        case 74: _t->controller_dataChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QVariant(*)>(_a[3]))); break;
        case 75: _t->controller_dataRemoved((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 76: _t->setClipboardData((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 77: _t->newProject(); break;
        case 78: _t->openProject(); break;
        case 79: _t->openExistingProject((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 80: _t->importProjects(); break;
        case 81: _t->importXMEProject(); break;
        case 82: _t->importXMIProject(); break;
        case 83: _t->importSnippet(); break;
        case 84: _t->exportSnippet(); break;
        case 85: _t->saveProject(); break;
        case 86: _t->saveAsProject(); break;
        case 87: _t->closeProject(); break;
        case 88: _t->closeMEDEA(); break;
        case 89: _t->generateWorkspace(); break;
        case 90: _t->executeJenkinsJob(); break;
        case 91: _t->fitView(); break;
        case 92: _t->fitAllViews(); break;
        case 93: _t->centerSelection(); break;
        case 94: _t->alignSelectionVertical(); break;
        case 95: _t->alignSelectionHorizontal(); break;
        case 96: _t->selectAndCenterConnectedEntities(); break;
        case 97: _t->centerOnID((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 98: _t->showWiki(); break;
        case 99: _t->reportBug(); break;
        case 100: _t->showWikiForSelectedItem(); break;
        case 101: _t->centerImpl(); break;
        case 102: _t->centerDefinition(); break;
        case 103: _t->popupDefinition(); break;
        case 104: _t->popupImpl(); break;
        case 105: _t->popupSelection(); break;
        case 106: _t->popupItem((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 107: _t->aboutQt(); break;
        case 108: _t->aboutMEDEA(); break;
        case 109: _t->cut(); break;
        case 110: _t->copy(); break;
        case 111: _t->paste(); break;
        case 112: _t->replicate(); break;
        case 113: _t->deleteSelection(); break;
        case 114: _t->expandSelection(); break;
        case 115: _t->contractSelection(); break;
        case 116: _t->editLabel(); break;
        case 117: _t->editReplicationCount(); break;
        case 118: _t->setSelectionReadOnly((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 119: _t->constructDDSQOSProfile(); break;
        case 120: _t->requestSearchSuggestions(); break;
        case 121: _t->setModelReady((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 122: _t->setControllerReady((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 123: _t->openURL((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 124: _t->initializeController(); break;
        case 125: _t->table_dataChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QVariant(*)>(_a[3]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 4:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< ViewItem* >(); break;
            }
            break;
        case 5:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< ViewItem* >(); break;
            }
            break;
        case 17:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QList<int> >(); break;
            }
            break;
        case 18:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QList<int> >(); break;
            }
            break;
        case 19:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QList<int> >(); break;
            }
            break;
        case 20:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QList<int> >(); break;
            }
            break;
        case 21:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QList<int> >(); break;
            }
            break;
        case 22:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< NODE_KIND >(); break;
            }
            break;
        case 23:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< NODE_KIND >(); break;
            }
            break;
        case 24:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 2:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< EDGE_KIND >(); break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QList<int> >(); break;
            }
            break;
        case 25:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 2:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< EDGE_KIND >(); break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QList<int> >(); break;
            }
            break;
        case 26:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 3:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< EDGE_KIND >(); break;
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< NODE_KIND >(); break;
            }
            break;
        case 27:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 3:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< EDGE_KIND >(); break;
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< NODE_KIND >(); break;
            }
            break;
        case 34:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QList<int> >(); break;
            }
            break;
        case 35:
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
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< ViewItem* >(); break;
            }
            break;
        case 71:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 2:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< NODE_KIND >(); break;
            }
            break;
        case 72:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< EDGE_KIND >(); break;
            }
            break;
        case 73:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< GRAPHML_KIND >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (ViewController::*_t)(bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_showWelcomeScreen)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_JenkinsReady)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_JavaReady)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_controllerReady)) {
                *result = 3;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(ViewItem * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_viewItemConstructed)) {
                *result = 4;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(int , ViewItem * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_viewItemDestructing)) {
                *result = 5;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(QPoint , QPointF );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_showToolbar)) {
                *result = 6;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(QStringList );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_gotSearchSuggestions)) {
                *result = 8;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(int , QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_editTableCell)) {
                *result = 9;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_projectClosed)) {
                *result = 10;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_setupModel)) {
                *result = 11;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_undo)) {
                *result = 12;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_redo)) {
                *result = 13;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_triggerAction)) {
                *result = 14;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(int , QString , QVariant );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_setData)) {
                *result = 15;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(int , QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_removeData)) {
                *result = 16;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(QList<int> );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_deleteEntities)) {
                *result = 17;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(QList<int> );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_cutEntities)) {
                *result = 18;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(QList<int> );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_copyEntities)) {
                *result = 19;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(QList<int> , QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_paste)) {
                *result = 20;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(QList<int> );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_replicateEntities)) {
                *result = 21;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(int , NODE_KIND , QPointF );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_constructNode)) {
                *result = 22;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(QList<int> , int , EDGE_KIND );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_constructEdge)) {
                *result = 24;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(QList<int> , int , EDGE_KIND );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_destructEdges)) {
                *result = 25;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(int , NODE_KIND , int , EDGE_KIND , QPointF );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_constructConnectedNode)) {
                *result = 26;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(int , int , QPointF );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_constructWorkerProcess)) {
                *result = 28;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(QStringList );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_importProjects)) {
                *result = 29;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(QString , QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_openProject)) {
                *result = 30;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_projectSaved)) {
                *result = 31;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_projectPathChanged)) {
                *result = 32;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_answerQuestion)) {
                *result = 33;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(QList<int> );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_exportSnippet)) {
                *result = 34;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(QList<int> , QString , QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_importSnippet)) {
                *result = 35;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(bool , QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::mc_showProgress)) {
                *result = 36;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::mc_progressChanged)) {
                *result = 37;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::mc_modelReady)) {
                *result = 38;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::mc_projectModified)) {
                *result = 39;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::mc_undoRedoUpdated)) {
                *result = 40;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_actionFinished)) {
                *result = 41;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_executeJenkinsJob)) {
                *result = 42;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_centerItem)) {
                *result = 43;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(ViewItem * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_selectAndCenterConnectedEntities)) {
                *result = 44;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_fitToScreen)) {
                *result = 45;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_addProjectToRecentProjects)) {
                *result = 46;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(QString , QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_getCodeForComponent)) {
                *result = 47;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(QString , QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_validateModel)) {
                *result = 48;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(QStringList );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_modelValidated)) {
                *result = 49;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_launchLocalDeployment)) {
                *result = 50;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(bool , BACKGROUND_PROCESS );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_backgroundProcess)) {
                *result = 51;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(QString , QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_importXMEProject)) {
                *result = 53;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_importXMIProject)) {
                *result = 54;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(int , bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_highlightItem)) {
                *result = 55;
                return;
            }
        }
    }
}

const QMetaObject ViewController::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_ViewController.data,
      qt_meta_data_ViewController,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *ViewController::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ViewController::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_ViewController.stringdata0))
        return static_cast<void*>(const_cast< ViewController*>(this));
    return QObject::qt_metacast(_clname);
}

int ViewController::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 126)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 126;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 126)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 126;
    }
    return _id;
}

// SIGNAL 0
void ViewController::vc_showWelcomeScreen(bool _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ViewController::vc_JenkinsReady(bool _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void ViewController::vc_JavaReady(bool _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void ViewController::vc_controllerReady(bool _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void ViewController::vc_viewItemConstructed(ViewItem * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void ViewController::vc_viewItemDestructing(int _t1, ViewItem * _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void ViewController::vc_showToolbar(QPoint _t1, QPointF _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 8
void ViewController::vc_gotSearchSuggestions(QStringList _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void ViewController::vc_editTableCell(int _t1, QString _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}

// SIGNAL 10
void ViewController::vc_projectClosed()
{
    QMetaObject::activate(this, &staticMetaObject, 10, Q_NULLPTR);
}

// SIGNAL 11
void ViewController::vc_setupModel()
{
    QMetaObject::activate(this, &staticMetaObject, 11, Q_NULLPTR);
}

// SIGNAL 12
void ViewController::vc_undo()
{
    QMetaObject::activate(this, &staticMetaObject, 12, Q_NULLPTR);
}

// SIGNAL 13
void ViewController::vc_redo()
{
    QMetaObject::activate(this, &staticMetaObject, 13, Q_NULLPTR);
}

// SIGNAL 14
void ViewController::vc_triggerAction(QString _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 14, _a);
}

// SIGNAL 15
void ViewController::vc_setData(int _t1, QString _t2, QVariant _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 15, _a);
}

// SIGNAL 16
void ViewController::vc_removeData(int _t1, QString _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 16, _a);
}

// SIGNAL 17
void ViewController::vc_deleteEntities(QList<int> _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 17, _a);
}

// SIGNAL 18
void ViewController::vc_cutEntities(QList<int> _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 18, _a);
}

// SIGNAL 19
void ViewController::vc_copyEntities(QList<int> _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 19, _a);
}

// SIGNAL 20
void ViewController::vc_paste(QList<int> _t1, QString _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 20, _a);
}

// SIGNAL 21
void ViewController::vc_replicateEntities(QList<int> _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 21, _a);
}

// SIGNAL 22
void ViewController::vc_constructNode(int _t1, NODE_KIND _t2, QPointF _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 22, _a);
}

// SIGNAL 24
void ViewController::vc_constructEdge(QList<int> _t1, int _t2, EDGE_KIND _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 24, _a);
}

// SIGNAL 25
void ViewController::vc_destructEdges(QList<int> _t1, int _t2, EDGE_KIND _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 25, _a);
}

// SIGNAL 26
void ViewController::vc_constructConnectedNode(int _t1, NODE_KIND _t2, int _t3, EDGE_KIND _t4, QPointF _t5)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)), const_cast<void*>(reinterpret_cast<const void*>(&_t5)) };
    QMetaObject::activate(this, &staticMetaObject, 26, _a);
}

// SIGNAL 28
void ViewController::vc_constructWorkerProcess(int _t1, int _t2, QPointF _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 28, _a);
}

// SIGNAL 29
void ViewController::vc_importProjects(QStringList _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 29, _a);
}

// SIGNAL 30
void ViewController::vc_openProject(QString _t1, QString _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 30, _a);
}

// SIGNAL 31
void ViewController::vc_projectSaved(QString _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 31, _a);
}

// SIGNAL 32
void ViewController::vc_projectPathChanged(QString _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 32, _a);
}

// SIGNAL 33
void ViewController::vc_answerQuestion(bool _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 33, _a);
}

// SIGNAL 34
void ViewController::vc_exportSnippet(QList<int> _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 34, _a);
}

// SIGNAL 35
void ViewController::vc_importSnippet(QList<int> _t1, QString _t2, QString _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 35, _a);
}

// SIGNAL 36
void ViewController::mc_showProgress(bool _t1, QString _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 36, _a);
}

// SIGNAL 37
void ViewController::mc_progressChanged(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 37, _a);
}

// SIGNAL 38
void ViewController::mc_modelReady(bool _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 38, _a);
}

// SIGNAL 39
void ViewController::mc_projectModified(bool _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 39, _a);
}

// SIGNAL 40
void ViewController::mc_undoRedoUpdated()
{
    QMetaObject::activate(this, &staticMetaObject, 40, Q_NULLPTR);
}

// SIGNAL 41
void ViewController::vc_actionFinished()
{
    QMetaObject::activate(this, &staticMetaObject, 41, Q_NULLPTR);
}

// SIGNAL 42
void ViewController::vc_executeJenkinsJob(QString _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 42, _a);
}

// SIGNAL 43
void ViewController::vc_centerItem(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 43, _a);
}

// SIGNAL 44
void ViewController::vc_selectAndCenterConnectedEntities(ViewItem * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 44, _a);
}

// SIGNAL 45
void ViewController::vc_fitToScreen()
{
    QMetaObject::activate(this, &staticMetaObject, 45, Q_NULLPTR);
}

// SIGNAL 46
void ViewController::vc_addProjectToRecentProjects(QString _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 46, _a);
}

// SIGNAL 47
void ViewController::vc_getCodeForComponent(QString _t1, QString _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 47, _a);
}

// SIGNAL 48
void ViewController::vc_validateModel(QString _t1, QString _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 48, _a);
}

// SIGNAL 49
void ViewController::vc_modelValidated(QStringList _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 49, _a);
}

// SIGNAL 50
void ViewController::vc_launchLocalDeployment(QString _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 50, _a);
}

// SIGNAL 51
void ViewController::vc_backgroundProcess(bool _t1, BACKGROUND_PROCESS _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 51, _a);
}

// SIGNAL 53
void ViewController::vc_importXMEProject(QString _t1, QString _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 53, _a);
}

// SIGNAL 54
void ViewController::vc_importXMIProject(QString _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 54, _a);
}

// SIGNAL 55
void ViewController::vc_highlightItem(int _t1, bool _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 55, _a);
}
QT_END_MOC_NAMESPACE
