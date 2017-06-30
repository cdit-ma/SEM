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
    QByteArrayData data[183];
    char stringdata0[2622];
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
QT_MOC_LITERAL(6, 85, 16), // "vc_ProjectLoaded"
QT_MOC_LITERAL(7, 102, 22), // "vc_viewItemConstructed"
QT_MOC_LITERAL(8, 125, 9), // "ViewItem*"
QT_MOC_LITERAL(9, 135, 8), // "viewItem"
QT_MOC_LITERAL(10, 144, 22), // "vc_viewItemDestructing"
QT_MOC_LITERAL(11, 167, 2), // "ID"
QT_MOC_LITERAL(12, 170, 4), // "item"
QT_MOC_LITERAL(13, 175, 14), // "vc_showToolbar"
QT_MOC_LITERAL(14, 190, 9), // "globalPos"
QT_MOC_LITERAL(15, 200, 7), // "itemPos"
QT_MOC_LITERAL(16, 208, 23), // "vc_gotSearchSuggestions"
QT_MOC_LITERAL(17, 232, 11), // "suggestions"
QT_MOC_LITERAL(18, 244, 16), // "vc_editTableCell"
QT_MOC_LITERAL(19, 261, 7), // "keyName"
QT_MOC_LITERAL(20, 269, 16), // "vc_projectClosed"
QT_MOC_LITERAL(21, 286, 13), // "vc_setupModel"
QT_MOC_LITERAL(22, 300, 7), // "vc_undo"
QT_MOC_LITERAL(23, 308, 7), // "vc_redo"
QT_MOC_LITERAL(24, 316, 16), // "vc_triggerAction"
QT_MOC_LITERAL(25, 333, 10), // "vc_setData"
QT_MOC_LITERAL(26, 344, 13), // "vc_removeData"
QT_MOC_LITERAL(27, 358, 17), // "vc_deleteEntities"
QT_MOC_LITERAL(28, 376, 10), // "QList<int>"
QT_MOC_LITERAL(29, 387, 3), // "IDs"
QT_MOC_LITERAL(30, 391, 14), // "vc_cutEntities"
QT_MOC_LITERAL(31, 406, 15), // "vc_copyEntities"
QT_MOC_LITERAL(32, 422, 8), // "vc_paste"
QT_MOC_LITERAL(33, 431, 4), // "data"
QT_MOC_LITERAL(34, 436, 20), // "vc_replicateEntities"
QT_MOC_LITERAL(35, 457, 16), // "vc_constructNode"
QT_MOC_LITERAL(36, 474, 8), // "parentID"
QT_MOC_LITERAL(37, 483, 9), // "NODE_KIND"
QT_MOC_LITERAL(38, 493, 8), // "nodeKind"
QT_MOC_LITERAL(39, 502, 3), // "pos"
QT_MOC_LITERAL(40, 506, 16), // "vc_constructEdge"
QT_MOC_LITERAL(41, 523, 9), // "sourceIDs"
QT_MOC_LITERAL(42, 533, 5), // "dstID"
QT_MOC_LITERAL(43, 539, 9), // "EDGE_KIND"
QT_MOC_LITERAL(44, 549, 8), // "edgeKind"
QT_MOC_LITERAL(45, 558, 16), // "vc_destructEdges"
QT_MOC_LITERAL(46, 575, 19), // "vc_destructAllEdges"
QT_MOC_LITERAL(47, 595, 25), // "vc_constructConnectedNode"
QT_MOC_LITERAL(48, 621, 25), // "vc_constructWorkerProcess"
QT_MOC_LITERAL(49, 647, 5), // "point"
QT_MOC_LITERAL(50, 653, 17), // "vc_importProjects"
QT_MOC_LITERAL(51, 671, 8), // "fileData"
QT_MOC_LITERAL(52, 680, 14), // "vc_openProject"
QT_MOC_LITERAL(53, 695, 8), // "fileName"
QT_MOC_LITERAL(54, 704, 8), // "filePath"
QT_MOC_LITERAL(55, 713, 15), // "vc_projectSaved"
QT_MOC_LITERAL(56, 729, 21), // "vc_projectPathChanged"
QT_MOC_LITERAL(57, 751, 17), // "vc_answerQuestion"
QT_MOC_LITERAL(58, 769, 16), // "vc_exportSnippet"
QT_MOC_LITERAL(59, 786, 16), // "vc_importSnippet"
QT_MOC_LITERAL(60, 803, 15), // "mc_showProgress"
QT_MOC_LITERAL(61, 819, 18), // "mc_progressChanged"
QT_MOC_LITERAL(62, 838, 13), // "mc_modelReady"
QT_MOC_LITERAL(63, 852, 18), // "mc_projectModified"
QT_MOC_LITERAL(64, 871, 18), // "mc_undoRedoUpdated"
QT_MOC_LITERAL(65, 890, 17), // "vc_actionFinished"
QT_MOC_LITERAL(66, 908, 20), // "vc_executeJenkinsJob"
QT_MOC_LITERAL(67, 929, 13), // "vc_centerItem"
QT_MOC_LITERAL(68, 943, 35), // "vc_selectAndCenterConnectedEn..."
QT_MOC_LITERAL(69, 979, 14), // "vc_fitToScreen"
QT_MOC_LITERAL(70, 994, 29), // "vc_addProjectToRecentProjects"
QT_MOC_LITERAL(71, 1024, 22), // "vc_getCodeForComponent"
QT_MOC_LITERAL(72, 1047, 11), // "graphmlPath"
QT_MOC_LITERAL(73, 1059, 13), // "componentName"
QT_MOC_LITERAL(74, 1073, 16), // "vc_validateModel"
QT_MOC_LITERAL(75, 1090, 10), // "reportPath"
QT_MOC_LITERAL(76, 1101, 17), // "vc_modelValidated"
QT_MOC_LITERAL(77, 1119, 6), // "report"
QT_MOC_LITERAL(78, 1126, 24), // "vc_launchLocalDeployment"
QT_MOC_LITERAL(79, 1151, 20), // "vc_backgroundProcess"
QT_MOC_LITERAL(80, 1172, 10), // "inProgress"
QT_MOC_LITERAL(81, 1183, 18), // "BACKGROUND_PROCESS"
QT_MOC_LITERAL(82, 1202, 7), // "process"
QT_MOC_LITERAL(83, 1210, 19), // "vc_importXMEProject"
QT_MOC_LITERAL(84, 1230, 7), // "xmePath"
QT_MOC_LITERAL(85, 1238, 19), // "vc_importXMIProject"
QT_MOC_LITERAL(86, 1258, 7), // "XMIPath"
QT_MOC_LITERAL(87, 1266, 16), // "vc_highlightItem"
QT_MOC_LITERAL(88, 1283, 9), // "highlight"
QT_MOC_LITERAL(89, 1293, 13), // "projectOpened"
QT_MOC_LITERAL(90, 1307, 7), // "success"
QT_MOC_LITERAL(91, 1315, 18), // "gotExportedSnippet"
QT_MOC_LITERAL(92, 1334, 11), // "snippetData"
QT_MOC_LITERAL(93, 1346, 11), // "askQuestion"
QT_MOC_LITERAL(94, 1358, 5), // "title"
QT_MOC_LITERAL(95, 1364, 7), // "message"
QT_MOC_LITERAL(96, 1372, 14), // "modelValidated"
QT_MOC_LITERAL(97, 1387, 17), // "importGraphMLFile"
QT_MOC_LITERAL(98, 1405, 20), // "importGraphMLExtract"
QT_MOC_LITERAL(99, 1426, 14), // "showCodeViewer"
QT_MOC_LITERAL(100, 1441, 7), // "tabName"
QT_MOC_LITERAL(101, 1449, 7), // "content"
QT_MOC_LITERAL(102, 1457, 32), // "jenkinsManager_SettingsValidated"
QT_MOC_LITERAL(103, 1490, 11), // "errorString"
QT_MOC_LITERAL(104, 1502, 22), // "jenkinsManager_GotJava"
QT_MOC_LITERAL(105, 1525, 4), // "java"
QT_MOC_LITERAL(106, 1530, 11), // "javaVersion"
QT_MOC_LITERAL(107, 1542, 34), // "jenkinsManager_GotJenkinsNode..."
QT_MOC_LITERAL(108, 1577, 11), // "graphmlData"
QT_MOC_LITERAL(109, 1589, 19), // "getCodeForComponent"
QT_MOC_LITERAL(110, 1609, 13), // "validateModel"
QT_MOC_LITERAL(111, 1623, 11), // "selectModel"
QT_MOC_LITERAL(112, 1635, 21), // "launchLocalDeployment"
QT_MOC_LITERAL(113, 1657, 14), // "actionFinished"
QT_MOC_LITERAL(114, 1672, 2), // "gg"
QT_MOC_LITERAL(115, 1675, 21), // "model_NodeConstructed"
QT_MOC_LITERAL(116, 1697, 9), // "parent_id"
QT_MOC_LITERAL(117, 1707, 2), // "id"
QT_MOC_LITERAL(118, 1710, 4), // "kind"
QT_MOC_LITERAL(119, 1715, 21), // "model_EdgeConstructed"
QT_MOC_LITERAL(120, 1737, 6), // "src_id"
QT_MOC_LITERAL(121, 1744, 6), // "dst_id"
QT_MOC_LITERAL(122, 1751, 27), // "controller_entityDestructed"
QT_MOC_LITERAL(123, 1779, 12), // "GRAPHML_KIND"
QT_MOC_LITERAL(124, 1792, 5), // "eKind"
QT_MOC_LITERAL(125, 1798, 22), // "controller_dataChanged"
QT_MOC_LITERAL(126, 1821, 3), // "key"
QT_MOC_LITERAL(127, 1825, 22), // "controller_dataRemoved"
QT_MOC_LITERAL(128, 1848, 16), // "setClipboardData"
QT_MOC_LITERAL(129, 1865, 10), // "newProject"
QT_MOC_LITERAL(130, 1876, 11), // "openProject"
QT_MOC_LITERAL(131, 1888, 19), // "openExistingProject"
QT_MOC_LITERAL(132, 1908, 4), // "file"
QT_MOC_LITERAL(133, 1913, 14), // "importProjects"
QT_MOC_LITERAL(134, 1928, 16), // "importXMEProject"
QT_MOC_LITERAL(135, 1945, 16), // "importXMIProject"
QT_MOC_LITERAL(136, 1962, 13), // "importSnippet"
QT_MOC_LITERAL(137, 1976, 13), // "exportSnippet"
QT_MOC_LITERAL(138, 1990, 11), // "saveProject"
QT_MOC_LITERAL(139, 2002, 13), // "saveAsProject"
QT_MOC_LITERAL(140, 2016, 12), // "closeProject"
QT_MOC_LITERAL(141, 2029, 10), // "closeMEDEA"
QT_MOC_LITERAL(142, 2040, 17), // "generateWorkspace"
QT_MOC_LITERAL(143, 2058, 17), // "executeJenkinsJob"
QT_MOC_LITERAL(144, 2076, 7), // "fitView"
QT_MOC_LITERAL(145, 2084, 11), // "fitAllViews"
QT_MOC_LITERAL(146, 2096, 15), // "centerSelection"
QT_MOC_LITERAL(147, 2112, 22), // "alignSelectionVertical"
QT_MOC_LITERAL(148, 2135, 24), // "alignSelectionHorizontal"
QT_MOC_LITERAL(149, 2160, 32), // "selectAndCenterConnectedEntities"
QT_MOC_LITERAL(150, 2193, 10), // "centerOnID"
QT_MOC_LITERAL(151, 2204, 8), // "showWiki"
QT_MOC_LITERAL(152, 2213, 9), // "reportBug"
QT_MOC_LITERAL(153, 2223, 23), // "showWikiForSelectedItem"
QT_MOC_LITERAL(154, 2247, 10), // "centerImpl"
QT_MOC_LITERAL(155, 2258, 16), // "centerDefinition"
QT_MOC_LITERAL(156, 2275, 15), // "popupDefinition"
QT_MOC_LITERAL(157, 2291, 9), // "popupImpl"
QT_MOC_LITERAL(158, 2301, 14), // "popupSelection"
QT_MOC_LITERAL(159, 2316, 9), // "popupItem"
QT_MOC_LITERAL(160, 2326, 7), // "aboutQt"
QT_MOC_LITERAL(161, 2334, 10), // "aboutMEDEA"
QT_MOC_LITERAL(162, 2345, 3), // "cut"
QT_MOC_LITERAL(163, 2349, 4), // "copy"
QT_MOC_LITERAL(164, 2354, 5), // "paste"
QT_MOC_LITERAL(165, 2360, 9), // "replicate"
QT_MOC_LITERAL(166, 2370, 15), // "deleteSelection"
QT_MOC_LITERAL(167, 2386, 15), // "expandSelection"
QT_MOC_LITERAL(168, 2402, 17), // "contractSelection"
QT_MOC_LITERAL(169, 2420, 9), // "editLabel"
QT_MOC_LITERAL(170, 2430, 20), // "editReplicationCount"
QT_MOC_LITERAL(171, 2451, 20), // "setSelectionReadOnly"
QT_MOC_LITERAL(172, 2472, 6), // "locked"
QT_MOC_LITERAL(173, 2479, 22), // "constructDDSQOSProfile"
QT_MOC_LITERAL(174, 2502, 24), // "requestSearchSuggestions"
QT_MOC_LITERAL(175, 2527, 13), // "setModelReady"
QT_MOC_LITERAL(176, 2541, 4), // "okay"
QT_MOC_LITERAL(177, 2546, 18), // "setControllerReady"
QT_MOC_LITERAL(178, 2565, 5), // "ready"
QT_MOC_LITERAL(179, 2571, 7), // "openURL"
QT_MOC_LITERAL(180, 2579, 3), // "url"
QT_MOC_LITERAL(181, 2583, 20), // "initializeController"
QT_MOC_LITERAL(182, 2604, 17) // "table_dataChanged"

    },
    "ViewController\0vc_showWelcomeScreen\0"
    "\0vc_JenkinsReady\0vc_JavaReady\0"
    "vc_controllerReady\0vc_ProjectLoaded\0"
    "vc_viewItemConstructed\0ViewItem*\0"
    "viewItem\0vc_viewItemDestructing\0ID\0"
    "item\0vc_showToolbar\0globalPos\0itemPos\0"
    "vc_gotSearchSuggestions\0suggestions\0"
    "vc_editTableCell\0keyName\0vc_projectClosed\0"
    "vc_setupModel\0vc_undo\0vc_redo\0"
    "vc_triggerAction\0vc_setData\0vc_removeData\0"
    "vc_deleteEntities\0QList<int>\0IDs\0"
    "vc_cutEntities\0vc_copyEntities\0vc_paste\0"
    "data\0vc_replicateEntities\0vc_constructNode\0"
    "parentID\0NODE_KIND\0nodeKind\0pos\0"
    "vc_constructEdge\0sourceIDs\0dstID\0"
    "EDGE_KIND\0edgeKind\0vc_destructEdges\0"
    "vc_destructAllEdges\0vc_constructConnectedNode\0"
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
     128,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      58,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,  654,    2, 0x06 /* Public */,
       3,    1,  657,    2, 0x06 /* Public */,
       4,    1,  660,    2, 0x06 /* Public */,
       5,    1,  663,    2, 0x06 /* Public */,
       6,    1,  666,    2, 0x06 /* Public */,
       7,    1,  669,    2, 0x06 /* Public */,
      10,    2,  672,    2, 0x06 /* Public */,
      13,    2,  677,    2, 0x06 /* Public */,
      13,    1,  682,    2, 0x26 /* Public | MethodCloned */,
      16,    1,  685,    2, 0x06 /* Public */,
      18,    2,  688,    2, 0x06 /* Public */,
      20,    0,  693,    2, 0x06 /* Public */,
      21,    0,  694,    2, 0x06 /* Public */,
      22,    0,  695,    2, 0x06 /* Public */,
      23,    0,  696,    2, 0x06 /* Public */,
      24,    1,  697,    2, 0x06 /* Public */,
      25,    3,  700,    2, 0x06 /* Public */,
      26,    2,  707,    2, 0x06 /* Public */,
      27,    1,  712,    2, 0x06 /* Public */,
      30,    1,  715,    2, 0x06 /* Public */,
      31,    1,  718,    2, 0x06 /* Public */,
      32,    2,  721,    2, 0x06 /* Public */,
      34,    1,  726,    2, 0x06 /* Public */,
      35,    3,  729,    2, 0x06 /* Public */,
      35,    2,  736,    2, 0x26 /* Public | MethodCloned */,
      40,    3,  741,    2, 0x06 /* Public */,
      45,    3,  748,    2, 0x06 /* Public */,
      46,    2,  755,    2, 0x06 /* Public */,
      47,    5,  760,    2, 0x06 /* Public */,
      47,    4,  771,    2, 0x26 /* Public | MethodCloned */,
      48,    3,  780,    2, 0x06 /* Public */,
      50,    1,  787,    2, 0x06 /* Public */,
      52,    2,  790,    2, 0x06 /* Public */,
      55,    1,  795,    2, 0x06 /* Public */,
      56,    1,  798,    2, 0x06 /* Public */,
      57,    1,  801,    2, 0x06 /* Public */,
      58,    1,  804,    2, 0x06 /* Public */,
      59,    3,  807,    2, 0x06 /* Public */,
      60,    2,  814,    2, 0x06 /* Public */,
      61,    1,  819,    2, 0x06 /* Public */,
      62,    1,  822,    2, 0x06 /* Public */,
      63,    1,  825,    2, 0x06 /* Public */,
      64,    0,  828,    2, 0x06 /* Public */,
      65,    0,  829,    2, 0x06 /* Public */,
      66,    1,  830,    2, 0x06 /* Public */,
      67,    1,  833,    2, 0x06 /* Public */,
      68,    1,  836,    2, 0x06 /* Public */,
      69,    0,  839,    2, 0x06 /* Public */,
      70,    1,  840,    2, 0x06 /* Public */,
      71,    2,  843,    2, 0x06 /* Public */,
      74,    2,  848,    2, 0x06 /* Public */,
      76,    1,  853,    2, 0x06 /* Public */,
      78,    1,  856,    2, 0x06 /* Public */,
      79,    2,  859,    2, 0x06 /* Public */,
      79,    1,  864,    2, 0x26 /* Public | MethodCloned */,
      83,    2,  867,    2, 0x06 /* Public */,
      85,    1,  872,    2, 0x06 /* Public */,
      87,    2,  875,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      89,    1,  880,    2, 0x0a /* Public */,
      91,    1,  883,    2, 0x0a /* Public */,
      93,    3,  886,    2, 0x0a /* Public */,
      96,    1,  893,    2, 0x0a /* Public */,
      97,    1,  896,    2, 0x0a /* Public */,
      98,    1,  899,    2, 0x0a /* Public */,
      99,    2,  902,    2, 0x0a /* Public */,
     102,    2,  907,    2, 0x0a /* Public */,
     104,    2,  912,    2, 0x0a /* Public */,
     107,    1,  917,    2, 0x0a /* Public */,
     109,    0,  920,    2, 0x0a /* Public */,
     110,    0,  921,    2, 0x0a /* Public */,
     111,    0,  922,    2, 0x0a /* Public */,
     112,    0,  923,    2, 0x0a /* Public */,
     113,    2,  924,    2, 0x0a /* Public */,
     115,    3,  929,    2, 0x0a /* Public */,
     119,    4,  936,    2, 0x0a /* Public */,
     122,    3,  945,    2, 0x0a /* Public */,
     125,    3,  952,    2, 0x0a /* Public */,
     127,    2,  959,    2, 0x0a /* Public */,
     128,    1,  964,    2, 0x0a /* Public */,
     129,    0,  967,    2, 0x0a /* Public */,
     130,    0,  968,    2, 0x0a /* Public */,
     131,    1,  969,    2, 0x0a /* Public */,
     133,    0,  972,    2, 0x0a /* Public */,
     134,    0,  973,    2, 0x0a /* Public */,
     135,    0,  974,    2, 0x0a /* Public */,
     136,    0,  975,    2, 0x0a /* Public */,
     137,    0,  976,    2, 0x0a /* Public */,
     138,    0,  977,    2, 0x0a /* Public */,
     139,    0,  978,    2, 0x0a /* Public */,
     140,    0,  979,    2, 0x0a /* Public */,
     141,    0,  980,    2, 0x0a /* Public */,
     142,    0,  981,    2, 0x0a /* Public */,
     143,    0,  982,    2, 0x0a /* Public */,
     144,    0,  983,    2, 0x0a /* Public */,
     145,    0,  984,    2, 0x0a /* Public */,
     146,    0,  985,    2, 0x0a /* Public */,
     147,    0,  986,    2, 0x0a /* Public */,
     148,    0,  987,    2, 0x0a /* Public */,
     149,    0,  988,    2, 0x0a /* Public */,
     150,    1,  989,    2, 0x0a /* Public */,
     151,    0,  992,    2, 0x0a /* Public */,
     152,    0,  993,    2, 0x0a /* Public */,
     153,    0,  994,    2, 0x0a /* Public */,
     154,    0,  995,    2, 0x0a /* Public */,
     155,    0,  996,    2, 0x0a /* Public */,
     156,    0,  997,    2, 0x0a /* Public */,
     157,    0,  998,    2, 0x0a /* Public */,
     158,    0,  999,    2, 0x0a /* Public */,
     159,    1, 1000,    2, 0x0a /* Public */,
     160,    0, 1003,    2, 0x0a /* Public */,
     161,    0, 1004,    2, 0x0a /* Public */,
     162,    0, 1005,    2, 0x0a /* Public */,
     163,    0, 1006,    2, 0x0a /* Public */,
     164,    0, 1007,    2, 0x0a /* Public */,
     165,    0, 1008,    2, 0x0a /* Public */,
     166,    0, 1009,    2, 0x0a /* Public */,
     167,    0, 1010,    2, 0x0a /* Public */,
     168,    0, 1011,    2, 0x0a /* Public */,
     169,    0, 1012,    2, 0x0a /* Public */,
     170,    0, 1013,    2, 0x0a /* Public */,
     171,    1, 1014,    2, 0x0a /* Public */,
     173,    0, 1017,    2, 0x0a /* Public */,
     174,    0, 1018,    2, 0x0a /* Public */,
     175,    1, 1019,    2, 0x0a /* Public */,
     177,    1, 1022,    2, 0x0a /* Public */,
     179,    1, 1025,    2, 0x0a /* Public */,
     181,    0, 1028,    2, 0x08 /* Private */,
     182,    3, 1029,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, 0x80000000 | 8,    9,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 8,   11,   12,
    QMetaType::Void, QMetaType::QPoint, QMetaType::QPointF,   14,   15,
    QMetaType::Void, QMetaType::QPoint,   14,
    QMetaType::Void, QMetaType::QStringList,   17,
    QMetaType::Void, QMetaType::Int, QMetaType::QString,   11,   19,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::Int, QMetaType::QString, QMetaType::QVariant,    2,    2,    2,
    QMetaType::Void, QMetaType::Int, QMetaType::QString,    2,    2,
    QMetaType::Void, 0x80000000 | 28,   29,
    QMetaType::Void, 0x80000000 | 28,   29,
    QMetaType::Void, 0x80000000 | 28,   29,
    QMetaType::Void, 0x80000000 | 28, QMetaType::QString,   29,   33,
    QMetaType::Void, 0x80000000 | 28,   29,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 37, QMetaType::QPointF,   36,   38,   39,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 37,   36,   38,
    QMetaType::Void, 0x80000000 | 28, QMetaType::Int, 0x80000000 | 43,   41,   42,   44,
    QMetaType::Void, 0x80000000 | 28, QMetaType::Int, 0x80000000 | 43,   41,   42,   44,
    QMetaType::Void, 0x80000000 | 28, 0x80000000 | 43,   41,   44,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 37, QMetaType::Int, 0x80000000 | 43, QMetaType::QPointF,   36,   38,   42,   44,   39,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 37, QMetaType::Int, 0x80000000 | 43,   36,   38,   42,   44,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::QPointF,   36,   42,   49,
    QMetaType::Void, QMetaType::QStringList,   51,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   53,   54,
    QMetaType::Void, QMetaType::QString,   54,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, 0x80000000 | 28,   29,
    QMetaType::Void, 0x80000000 | 28, QMetaType::QString, QMetaType::QString,   29,   53,   51,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,    2,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   54,
    QMetaType::Void, QMetaType::Int,   11,
    QMetaType::Void, 0x80000000 | 8,   12,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   54,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   72,   73,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   72,   75,
    QMetaType::Void, QMetaType::QStringList,   77,
    QMetaType::Void, QMetaType::QString,   72,
    QMetaType::Void, QMetaType::Bool, 0x80000000 | 81,   80,   82,
    QMetaType::Void, QMetaType::Bool,   80,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   84,   72,
    QMetaType::Void, QMetaType::QString,   86,
    QMetaType::Void, QMetaType::Int, QMetaType::Bool,   11,   88,

 // slots: parameters
    QMetaType::Void, QMetaType::Bool,   90,
    QMetaType::Void, QMetaType::QString,   92,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::Int,   94,   95,   11,
    QMetaType::Void, QMetaType::QString,   75,
    QMetaType::Void, QMetaType::QString,   72,
    QMetaType::Void, QMetaType::QString,   33,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,  100,  101,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,   90,  103,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,  105,  106,
    QMetaType::Void, QMetaType::QString,  108,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,   90,  114,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, 0x80000000 | 37,  116,  117,  118,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 43, QMetaType::Int, QMetaType::Int,  117,  118,  120,  121,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 123, QMetaType::QString,   11,  124,  118,
    QMetaType::Void, QMetaType::Int, QMetaType::QString, QMetaType::QVariant,   11,  126,   33,
    QMetaType::Void, QMetaType::Int, QMetaType::QString,   11,  126,
    QMetaType::Void, QMetaType::QString,   33,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,  132,
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
    QMetaType::Void, QMetaType::Int,   11,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   11,
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
    QMetaType::Void, QMetaType::Bool,  172,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,  176,
    QMetaType::Void, QMetaType::Bool,  178,
    QMetaType::Void, QMetaType::QString,  180,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::QString, QMetaType::QVariant,   11,  126,   33,

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
        case 4: _t->vc_ProjectLoaded((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 5: _t->vc_viewItemConstructed((*reinterpret_cast< ViewItem*(*)>(_a[1]))); break;
        case 6: _t->vc_viewItemDestructing((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< ViewItem*(*)>(_a[2]))); break;
        case 7: _t->vc_showToolbar((*reinterpret_cast< QPoint(*)>(_a[1])),(*reinterpret_cast< QPointF(*)>(_a[2]))); break;
        case 8: _t->vc_showToolbar((*reinterpret_cast< QPoint(*)>(_a[1]))); break;
        case 9: _t->vc_gotSearchSuggestions((*reinterpret_cast< QStringList(*)>(_a[1]))); break;
        case 10: _t->vc_editTableCell((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 11: _t->vc_projectClosed(); break;
        case 12: _t->vc_setupModel(); break;
        case 13: _t->vc_undo(); break;
        case 14: _t->vc_redo(); break;
        case 15: _t->vc_triggerAction((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 16: _t->vc_setData((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QVariant(*)>(_a[3]))); break;
        case 17: _t->vc_removeData((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 18: _t->vc_deleteEntities((*reinterpret_cast< QList<int>(*)>(_a[1]))); break;
        case 19: _t->vc_cutEntities((*reinterpret_cast< QList<int>(*)>(_a[1]))); break;
        case 20: _t->vc_copyEntities((*reinterpret_cast< QList<int>(*)>(_a[1]))); break;
        case 21: _t->vc_paste((*reinterpret_cast< QList<int>(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 22: _t->vc_replicateEntities((*reinterpret_cast< QList<int>(*)>(_a[1]))); break;
        case 23: _t->vc_constructNode((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< NODE_KIND(*)>(_a[2])),(*reinterpret_cast< QPointF(*)>(_a[3]))); break;
        case 24: _t->vc_constructNode((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< NODE_KIND(*)>(_a[2]))); break;
        case 25: _t->vc_constructEdge((*reinterpret_cast< QList<int>(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< EDGE_KIND(*)>(_a[3]))); break;
        case 26: _t->vc_destructEdges((*reinterpret_cast< QList<int>(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< EDGE_KIND(*)>(_a[3]))); break;
        case 27: _t->vc_destructAllEdges((*reinterpret_cast< QList<int>(*)>(_a[1])),(*reinterpret_cast< EDGE_KIND(*)>(_a[2]))); break;
        case 28: _t->vc_constructConnectedNode((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< NODE_KIND(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< EDGE_KIND(*)>(_a[4])),(*reinterpret_cast< QPointF(*)>(_a[5]))); break;
        case 29: _t->vc_constructConnectedNode((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< NODE_KIND(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< EDGE_KIND(*)>(_a[4]))); break;
        case 30: _t->vc_constructWorkerProcess((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< QPointF(*)>(_a[3]))); break;
        case 31: _t->vc_importProjects((*reinterpret_cast< QStringList(*)>(_a[1]))); break;
        case 32: _t->vc_openProject((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 33: _t->vc_projectSaved((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 34: _t->vc_projectPathChanged((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 35: _t->vc_answerQuestion((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 36: _t->vc_exportSnippet((*reinterpret_cast< QList<int>(*)>(_a[1]))); break;
        case 37: _t->vc_importSnippet((*reinterpret_cast< QList<int>(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3]))); break;
        case 38: _t->mc_showProgress((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 39: _t->mc_progressChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 40: _t->mc_modelReady((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 41: _t->mc_projectModified((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 42: _t->mc_undoRedoUpdated(); break;
        case 43: _t->vc_actionFinished(); break;
        case 44: _t->vc_executeJenkinsJob((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 45: _t->vc_centerItem((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 46: _t->vc_selectAndCenterConnectedEntities((*reinterpret_cast< ViewItem*(*)>(_a[1]))); break;
        case 47: _t->vc_fitToScreen(); break;
        case 48: _t->vc_addProjectToRecentProjects((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 49: _t->vc_getCodeForComponent((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 50: _t->vc_validateModel((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 51: _t->vc_modelValidated((*reinterpret_cast< QStringList(*)>(_a[1]))); break;
        case 52: _t->vc_launchLocalDeployment((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 53: _t->vc_backgroundProcess((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< BACKGROUND_PROCESS(*)>(_a[2]))); break;
        case 54: _t->vc_backgroundProcess((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 55: _t->vc_importXMEProject((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 56: _t->vc_importXMIProject((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 57: _t->vc_highlightItem((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 58: _t->projectOpened((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 59: _t->gotExportedSnippet((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 60: _t->askQuestion((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 61: _t->modelValidated((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 62: _t->importGraphMLFile((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 63: _t->importGraphMLExtract((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 64: _t->showCodeViewer((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 65: _t->jenkinsManager_SettingsValidated((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 66: _t->jenkinsManager_GotJava((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 67: _t->jenkinsManager_GotJenkinsNodesList((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 68: _t->getCodeForComponent(); break;
        case 69: _t->validateModel(); break;
        case 70: _t->selectModel(); break;
        case 71: _t->launchLocalDeployment(); break;
        case 72: _t->actionFinished((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 73: _t->model_NodeConstructed((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< NODE_KIND(*)>(_a[3]))); break;
        case 74: _t->model_EdgeConstructed((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< EDGE_KIND(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4]))); break;
        case 75: _t->controller_entityDestructed((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< GRAPHML_KIND(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3]))); break;
        case 76: _t->controller_dataChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QVariant(*)>(_a[3]))); break;
        case 77: _t->controller_dataRemoved((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 78: _t->setClipboardData((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 79: _t->newProject(); break;
        case 80: _t->openProject(); break;
        case 81: _t->openExistingProject((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 82: _t->importProjects(); break;
        case 83: _t->importXMEProject(); break;
        case 84: _t->importXMIProject(); break;
        case 85: _t->importSnippet(); break;
        case 86: _t->exportSnippet(); break;
        case 87: _t->saveProject(); break;
        case 88: _t->saveAsProject(); break;
        case 89: _t->closeProject(); break;
        case 90: _t->closeMEDEA(); break;
        case 91: _t->generateWorkspace(); break;
        case 92: _t->executeJenkinsJob(); break;
        case 93: _t->fitView(); break;
        case 94: _t->fitAllViews(); break;
        case 95: _t->centerSelection(); break;
        case 96: _t->alignSelectionVertical(); break;
        case 97: _t->alignSelectionHorizontal(); break;
        case 98: _t->selectAndCenterConnectedEntities(); break;
        case 99: _t->centerOnID((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 100: _t->showWiki(); break;
        case 101: _t->reportBug(); break;
        case 102: _t->showWikiForSelectedItem(); break;
        case 103: _t->centerImpl(); break;
        case 104: _t->centerDefinition(); break;
        case 105: _t->popupDefinition(); break;
        case 106: _t->popupImpl(); break;
        case 107: _t->popupSelection(); break;
        case 108: _t->popupItem((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 109: _t->aboutQt(); break;
        case 110: _t->aboutMEDEA(); break;
        case 111: _t->cut(); break;
        case 112: _t->copy(); break;
        case 113: _t->paste(); break;
        case 114: _t->replicate(); break;
        case 115: _t->deleteSelection(); break;
        case 116: _t->expandSelection(); break;
        case 117: _t->contractSelection(); break;
        case 118: _t->editLabel(); break;
        case 119: _t->editReplicationCount(); break;
        case 120: _t->setSelectionReadOnly((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 121: _t->constructDDSQOSProfile(); break;
        case 122: _t->requestSearchSuggestions(); break;
        case 123: _t->setModelReady((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 124: _t->setControllerReady((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 125: _t->openURL((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 126: _t->initializeController(); break;
        case 127: _t->table_dataChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QVariant(*)>(_a[3]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 5:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< ViewItem* >(); break;
            }
            break;
        case 6:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< ViewItem* >(); break;
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
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QList<int> >(); break;
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
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< NODE_KIND >(); break;
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
            case 2:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< EDGE_KIND >(); break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QList<int> >(); break;
            }
            break;
        case 27:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< EDGE_KIND >(); break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QList<int> >(); break;
            }
            break;
        case 28:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 3:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< EDGE_KIND >(); break;
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< NODE_KIND >(); break;
            }
            break;
        case 29:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 3:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< EDGE_KIND >(); break;
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< NODE_KIND >(); break;
            }
            break;
        case 36:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QList<int> >(); break;
            }
            break;
        case 37:
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
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< ViewItem* >(); break;
            }
            break;
        case 73:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 2:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< NODE_KIND >(); break;
            }
            break;
        case 74:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< EDGE_KIND >(); break;
            }
            break;
        case 75:
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
            typedef void (ViewController::*_t)(bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_ProjectLoaded)) {
                *result = 4;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(ViewItem * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_viewItemConstructed)) {
                *result = 5;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(int , ViewItem * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_viewItemDestructing)) {
                *result = 6;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(QPoint , QPointF );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_showToolbar)) {
                *result = 7;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(QStringList );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_gotSearchSuggestions)) {
                *result = 9;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(int , QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_editTableCell)) {
                *result = 10;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_projectClosed)) {
                *result = 11;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_setupModel)) {
                *result = 12;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_undo)) {
                *result = 13;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_redo)) {
                *result = 14;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_triggerAction)) {
                *result = 15;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(int , QString , QVariant );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_setData)) {
                *result = 16;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(int , QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_removeData)) {
                *result = 17;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(QList<int> );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_deleteEntities)) {
                *result = 18;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(QList<int> );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_cutEntities)) {
                *result = 19;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(QList<int> );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_copyEntities)) {
                *result = 20;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(QList<int> , QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_paste)) {
                *result = 21;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(QList<int> );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_replicateEntities)) {
                *result = 22;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(int , NODE_KIND , QPointF );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_constructNode)) {
                *result = 23;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(QList<int> , int , EDGE_KIND );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_constructEdge)) {
                *result = 25;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(QList<int> , int , EDGE_KIND );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_destructEdges)) {
                *result = 26;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(QList<int> , EDGE_KIND );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_destructAllEdges)) {
                *result = 27;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(int , NODE_KIND , int , EDGE_KIND , QPointF );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_constructConnectedNode)) {
                *result = 28;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(int , int , QPointF );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_constructWorkerProcess)) {
                *result = 30;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(QStringList );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_importProjects)) {
                *result = 31;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(QString , QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_openProject)) {
                *result = 32;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_projectSaved)) {
                *result = 33;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_projectPathChanged)) {
                *result = 34;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_answerQuestion)) {
                *result = 35;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(QList<int> );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_exportSnippet)) {
                *result = 36;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(QList<int> , QString , QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_importSnippet)) {
                *result = 37;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(bool , QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::mc_showProgress)) {
                *result = 38;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::mc_progressChanged)) {
                *result = 39;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::mc_modelReady)) {
                *result = 40;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::mc_projectModified)) {
                *result = 41;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::mc_undoRedoUpdated)) {
                *result = 42;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_actionFinished)) {
                *result = 43;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_executeJenkinsJob)) {
                *result = 44;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_centerItem)) {
                *result = 45;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(ViewItem * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_selectAndCenterConnectedEntities)) {
                *result = 46;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_fitToScreen)) {
                *result = 47;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_addProjectToRecentProjects)) {
                *result = 48;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(QString , QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_getCodeForComponent)) {
                *result = 49;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(QString , QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_validateModel)) {
                *result = 50;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(QStringList );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_modelValidated)) {
                *result = 51;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_launchLocalDeployment)) {
                *result = 52;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(bool , BACKGROUND_PROCESS );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_backgroundProcess)) {
                *result = 53;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(QString , QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_importXMEProject)) {
                *result = 55;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_importXMIProject)) {
                *result = 56;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(int , bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_highlightItem)) {
                *result = 57;
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
        if (_id < 128)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 128;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 128)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 128;
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
void ViewController::vc_ProjectLoaded(bool _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void ViewController::vc_viewItemConstructed(ViewItem * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void ViewController::vc_viewItemDestructing(int _t1, ViewItem * _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void ViewController::vc_showToolbar(QPoint _t1, QPointF _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 9
void ViewController::vc_gotSearchSuggestions(QStringList _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}

// SIGNAL 10
void ViewController::vc_editTableCell(int _t1, QString _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 10, _a);
}

// SIGNAL 11
void ViewController::vc_projectClosed()
{
    QMetaObject::activate(this, &staticMetaObject, 11, Q_NULLPTR);
}

// SIGNAL 12
void ViewController::vc_setupModel()
{
    QMetaObject::activate(this, &staticMetaObject, 12, Q_NULLPTR);
}

// SIGNAL 13
void ViewController::vc_undo()
{
    QMetaObject::activate(this, &staticMetaObject, 13, Q_NULLPTR);
}

// SIGNAL 14
void ViewController::vc_redo()
{
    QMetaObject::activate(this, &staticMetaObject, 14, Q_NULLPTR);
}

// SIGNAL 15
void ViewController::vc_triggerAction(QString _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 15, _a);
}

// SIGNAL 16
void ViewController::vc_setData(int _t1, QString _t2, QVariant _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 16, _a);
}

// SIGNAL 17
void ViewController::vc_removeData(int _t1, QString _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 17, _a);
}

// SIGNAL 18
void ViewController::vc_deleteEntities(QList<int> _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 18, _a);
}

// SIGNAL 19
void ViewController::vc_cutEntities(QList<int> _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 19, _a);
}

// SIGNAL 20
void ViewController::vc_copyEntities(QList<int> _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 20, _a);
}

// SIGNAL 21
void ViewController::vc_paste(QList<int> _t1, QString _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 21, _a);
}

// SIGNAL 22
void ViewController::vc_replicateEntities(QList<int> _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 22, _a);
}

// SIGNAL 23
void ViewController::vc_constructNode(int _t1, NODE_KIND _t2, QPointF _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 23, _a);
}

// SIGNAL 25
void ViewController::vc_constructEdge(QList<int> _t1, int _t2, EDGE_KIND _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 25, _a);
}

// SIGNAL 26
void ViewController::vc_destructEdges(QList<int> _t1, int _t2, EDGE_KIND _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 26, _a);
}

// SIGNAL 27
void ViewController::vc_destructAllEdges(QList<int> _t1, EDGE_KIND _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 27, _a);
}

// SIGNAL 28
void ViewController::vc_constructConnectedNode(int _t1, NODE_KIND _t2, int _t3, EDGE_KIND _t4, QPointF _t5)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)), const_cast<void*>(reinterpret_cast<const void*>(&_t5)) };
    QMetaObject::activate(this, &staticMetaObject, 28, _a);
}

// SIGNAL 30
void ViewController::vc_constructWorkerProcess(int _t1, int _t2, QPointF _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 30, _a);
}

// SIGNAL 31
void ViewController::vc_importProjects(QStringList _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 31, _a);
}

// SIGNAL 32
void ViewController::vc_openProject(QString _t1, QString _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 32, _a);
}

// SIGNAL 33
void ViewController::vc_projectSaved(QString _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 33, _a);
}

// SIGNAL 34
void ViewController::vc_projectPathChanged(QString _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 34, _a);
}

// SIGNAL 35
void ViewController::vc_answerQuestion(bool _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 35, _a);
}

// SIGNAL 36
void ViewController::vc_exportSnippet(QList<int> _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 36, _a);
}

// SIGNAL 37
void ViewController::vc_importSnippet(QList<int> _t1, QString _t2, QString _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 37, _a);
}

// SIGNAL 38
void ViewController::mc_showProgress(bool _t1, QString _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 38, _a);
}

// SIGNAL 39
void ViewController::mc_progressChanged(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 39, _a);
}

// SIGNAL 40
void ViewController::mc_modelReady(bool _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 40, _a);
}

// SIGNAL 41
void ViewController::mc_projectModified(bool _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 41, _a);
}

// SIGNAL 42
void ViewController::mc_undoRedoUpdated()
{
    QMetaObject::activate(this, &staticMetaObject, 42, Q_NULLPTR);
}

// SIGNAL 43
void ViewController::vc_actionFinished()
{
    QMetaObject::activate(this, &staticMetaObject, 43, Q_NULLPTR);
}

// SIGNAL 44
void ViewController::vc_executeJenkinsJob(QString _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 44, _a);
}

// SIGNAL 45
void ViewController::vc_centerItem(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 45, _a);
}

// SIGNAL 46
void ViewController::vc_selectAndCenterConnectedEntities(ViewItem * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 46, _a);
}

// SIGNAL 47
void ViewController::vc_fitToScreen()
{
    QMetaObject::activate(this, &staticMetaObject, 47, Q_NULLPTR);
}

// SIGNAL 48
void ViewController::vc_addProjectToRecentProjects(QString _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 48, _a);
}

// SIGNAL 49
void ViewController::vc_getCodeForComponent(QString _t1, QString _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 49, _a);
}

// SIGNAL 50
void ViewController::vc_validateModel(QString _t1, QString _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 50, _a);
}

// SIGNAL 51
void ViewController::vc_modelValidated(QStringList _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 51, _a);
}

// SIGNAL 52
void ViewController::vc_launchLocalDeployment(QString _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 52, _a);
}

// SIGNAL 53
void ViewController::vc_backgroundProcess(bool _t1, BACKGROUND_PROCESS _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 53, _a);
}

// SIGNAL 55
void ViewController::vc_importXMEProject(QString _t1, QString _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 55, _a);
}

// SIGNAL 56
void ViewController::vc_importXMIProject(QString _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 56, _a);
}

// SIGNAL 57
void ViewController::vc_highlightItem(int _t1, bool _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 57, _a);
}
QT_END_MOC_NAMESPACE
