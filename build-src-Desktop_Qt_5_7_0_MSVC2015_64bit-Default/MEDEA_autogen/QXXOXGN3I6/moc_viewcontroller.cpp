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
    QByteArrayData data[171];
    char stringdata0[2502];
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
QT_MOC_LITERAL(18, 244, 17), // "vc_ActionFinished"
QT_MOC_LITERAL(19, 262, 16), // "vc_editTableCell"
QT_MOC_LITERAL(20, 279, 7), // "keyName"
QT_MOC_LITERAL(21, 287, 16), // "vc_projectClosed"
QT_MOC_LITERAL(22, 304, 23), // "vc_SetupModelController"
QT_MOC_LITERAL(23, 328, 9), // "file_path"
QT_MOC_LITERAL(24, 338, 7), // "vc_undo"
QT_MOC_LITERAL(25, 346, 7), // "vc_redo"
QT_MOC_LITERAL(26, 354, 16), // "vc_triggerAction"
QT_MOC_LITERAL(27, 371, 10), // "vc_setData"
QT_MOC_LITERAL(28, 382, 13), // "vc_removeData"
QT_MOC_LITERAL(29, 396, 17), // "vc_deleteEntities"
QT_MOC_LITERAL(30, 414, 10), // "QList<int>"
QT_MOC_LITERAL(31, 425, 3), // "IDs"
QT_MOC_LITERAL(32, 429, 14), // "vc_cutEntities"
QT_MOC_LITERAL(33, 444, 15), // "vc_copyEntities"
QT_MOC_LITERAL(34, 460, 8), // "vc_paste"
QT_MOC_LITERAL(35, 469, 4), // "data"
QT_MOC_LITERAL(36, 474, 20), // "vc_replicateEntities"
QT_MOC_LITERAL(37, 495, 16), // "vc_constructNode"
QT_MOC_LITERAL(38, 512, 8), // "parentID"
QT_MOC_LITERAL(39, 521, 9), // "NODE_KIND"
QT_MOC_LITERAL(40, 531, 8), // "nodeKind"
QT_MOC_LITERAL(41, 540, 3), // "pos"
QT_MOC_LITERAL(42, 544, 16), // "vc_constructEdge"
QT_MOC_LITERAL(43, 561, 9), // "sourceIDs"
QT_MOC_LITERAL(44, 571, 5), // "dstID"
QT_MOC_LITERAL(45, 577, 9), // "EDGE_KIND"
QT_MOC_LITERAL(46, 587, 8), // "edgeKind"
QT_MOC_LITERAL(47, 596, 16), // "vc_destructEdges"
QT_MOC_LITERAL(48, 613, 19), // "vc_destructAllEdges"
QT_MOC_LITERAL(49, 633, 25), // "vc_constructConnectedNode"
QT_MOC_LITERAL(50, 659, 25), // "vc_constructWorkerProcess"
QT_MOC_LITERAL(51, 685, 5), // "point"
QT_MOC_LITERAL(52, 691, 17), // "vc_importProjects"
QT_MOC_LITERAL(53, 709, 8), // "fileData"
QT_MOC_LITERAL(54, 718, 15), // "vc_projectSaved"
QT_MOC_LITERAL(55, 734, 8), // "filePath"
QT_MOC_LITERAL(56, 743, 21), // "vc_projectPathChanged"
QT_MOC_LITERAL(57, 765, 17), // "vc_answerQuestion"
QT_MOC_LITERAL(58, 783, 15), // "mc_showProgress"
QT_MOC_LITERAL(59, 799, 18), // "mc_progressChanged"
QT_MOC_LITERAL(60, 818, 13), // "mc_modelReady"
QT_MOC_LITERAL(61, 832, 18), // "mc_projectModified"
QT_MOC_LITERAL(62, 851, 18), // "mc_undoRedoUpdated"
QT_MOC_LITERAL(63, 870, 20), // "vc_executeJenkinsJob"
QT_MOC_LITERAL(64, 891, 13), // "vc_centerItem"
QT_MOC_LITERAL(65, 905, 35), // "vc_selectAndCenterConnectedEn..."
QT_MOC_LITERAL(66, 941, 14), // "vc_fitToScreen"
QT_MOC_LITERAL(67, 956, 29), // "vc_addProjectToRecentProjects"
QT_MOC_LITERAL(68, 986, 22), // "vc_getCodeForComponent"
QT_MOC_LITERAL(69, 1009, 11), // "graphmlPath"
QT_MOC_LITERAL(70, 1021, 13), // "componentName"
QT_MOC_LITERAL(71, 1035, 16), // "vc_validateModel"
QT_MOC_LITERAL(72, 1052, 10), // "reportPath"
QT_MOC_LITERAL(73, 1063, 17), // "vc_modelValidated"
QT_MOC_LITERAL(74, 1081, 6), // "report"
QT_MOC_LITERAL(75, 1088, 24), // "vc_launchLocalDeployment"
QT_MOC_LITERAL(76, 1113, 20), // "vc_backgroundProcess"
QT_MOC_LITERAL(77, 1134, 10), // "inProgress"
QT_MOC_LITERAL(78, 1145, 18), // "BACKGROUND_PROCESS"
QT_MOC_LITERAL(79, 1164, 7), // "process"
QT_MOC_LITERAL(80, 1172, 19), // "vc_importXMEProject"
QT_MOC_LITERAL(81, 1192, 7), // "xmePath"
QT_MOC_LITERAL(82, 1200, 19), // "vc_importXMIProject"
QT_MOC_LITERAL(83, 1220, 7), // "XMIPath"
QT_MOC_LITERAL(84, 1228, 16), // "vc_highlightItem"
QT_MOC_LITERAL(85, 1245, 9), // "highlight"
QT_MOC_LITERAL(86, 1255, 3), // "ids"
QT_MOC_LITERAL(87, 1259, 14), // "modelValidated"
QT_MOC_LITERAL(88, 1274, 17), // "importGraphMLFile"
QT_MOC_LITERAL(89, 1292, 20), // "importGraphMLExtract"
QT_MOC_LITERAL(90, 1313, 14), // "showCodeViewer"
QT_MOC_LITERAL(91, 1328, 7), // "tabName"
QT_MOC_LITERAL(92, 1336, 7), // "content"
QT_MOC_LITERAL(93, 1344, 32), // "jenkinsManager_SettingsValidated"
QT_MOC_LITERAL(94, 1377, 7), // "success"
QT_MOC_LITERAL(95, 1385, 11), // "errorString"
QT_MOC_LITERAL(96, 1397, 22), // "jenkinsManager_GotJava"
QT_MOC_LITERAL(97, 1420, 4), // "java"
QT_MOC_LITERAL(98, 1425, 11), // "javaVersion"
QT_MOC_LITERAL(99, 1437, 34), // "jenkinsManager_GotJenkinsNode..."
QT_MOC_LITERAL(100, 1472, 11), // "graphmlData"
QT_MOC_LITERAL(101, 1484, 19), // "getCodeForComponent"
QT_MOC_LITERAL(102, 1504, 13), // "validateModel"
QT_MOC_LITERAL(103, 1518, 11), // "selectModel"
QT_MOC_LITERAL(104, 1530, 21), // "launchLocalDeployment"
QT_MOC_LITERAL(105, 1552, 21), // "model_NodeConstructed"
QT_MOC_LITERAL(106, 1574, 9), // "parent_id"
QT_MOC_LITERAL(107, 1584, 2), // "id"
QT_MOC_LITERAL(108, 1587, 4), // "kind"
QT_MOC_LITERAL(109, 1592, 21), // "model_EdgeConstructed"
QT_MOC_LITERAL(110, 1614, 6), // "src_id"
QT_MOC_LITERAL(111, 1621, 6), // "dst_id"
QT_MOC_LITERAL(112, 1628, 27), // "controller_entityDestructed"
QT_MOC_LITERAL(113, 1656, 12), // "GRAPHML_KIND"
QT_MOC_LITERAL(114, 1669, 22), // "controller_dataChanged"
QT_MOC_LITERAL(115, 1692, 3), // "key"
QT_MOC_LITERAL(116, 1696, 22), // "controller_dataRemoved"
QT_MOC_LITERAL(117, 1719, 16), // "setClipboardData"
QT_MOC_LITERAL(118, 1736, 10), // "newProject"
QT_MOC_LITERAL(119, 1747, 11), // "OpenProject"
QT_MOC_LITERAL(120, 1759, 19), // "OpenExistingProject"
QT_MOC_LITERAL(121, 1779, 14), // "importProjects"
QT_MOC_LITERAL(122, 1794, 16), // "importXMEProject"
QT_MOC_LITERAL(123, 1811, 16), // "importXMIProject"
QT_MOC_LITERAL(124, 1828, 11), // "saveProject"
QT_MOC_LITERAL(125, 1840, 13), // "saveAsProject"
QT_MOC_LITERAL(126, 1854, 12), // "closeProject"
QT_MOC_LITERAL(127, 1867, 10), // "closeMEDEA"
QT_MOC_LITERAL(128, 1878, 13), // "importIdlFile"
QT_MOC_LITERAL(129, 1892, 17), // "generateWorkspace"
QT_MOC_LITERAL(130, 1910, 17), // "executeJenkinsJob"
QT_MOC_LITERAL(131, 1928, 7), // "fitView"
QT_MOC_LITERAL(132, 1936, 11), // "fitAllViews"
QT_MOC_LITERAL(133, 1948, 15), // "centerSelection"
QT_MOC_LITERAL(134, 1964, 22), // "alignSelectionVertical"
QT_MOC_LITERAL(135, 1987, 24), // "alignSelectionHorizontal"
QT_MOC_LITERAL(136, 2012, 32), // "selectAndCenterConnectedEntities"
QT_MOC_LITERAL(137, 2045, 10), // "centerOnID"
QT_MOC_LITERAL(138, 2056, 8), // "showWiki"
QT_MOC_LITERAL(139, 2065, 9), // "reportBug"
QT_MOC_LITERAL(140, 2075, 23), // "showWikiForSelectedItem"
QT_MOC_LITERAL(141, 2099, 10), // "centerImpl"
QT_MOC_LITERAL(142, 2110, 16), // "centerDefinition"
QT_MOC_LITERAL(143, 2127, 15), // "popupDefinition"
QT_MOC_LITERAL(144, 2143, 9), // "popupImpl"
QT_MOC_LITERAL(145, 2153, 14), // "popupSelection"
QT_MOC_LITERAL(146, 2168, 9), // "popupItem"
QT_MOC_LITERAL(147, 2178, 7), // "aboutQt"
QT_MOC_LITERAL(148, 2186, 10), // "aboutMEDEA"
QT_MOC_LITERAL(149, 2197, 3), // "cut"
QT_MOC_LITERAL(150, 2201, 4), // "copy"
QT_MOC_LITERAL(151, 2206, 5), // "paste"
QT_MOC_LITERAL(152, 2212, 9), // "replicate"
QT_MOC_LITERAL(153, 2222, 15), // "deleteSelection"
QT_MOC_LITERAL(154, 2238, 15), // "expandSelection"
QT_MOC_LITERAL(155, 2254, 17), // "contractSelection"
QT_MOC_LITERAL(156, 2272, 9), // "editLabel"
QT_MOC_LITERAL(157, 2282, 20), // "editReplicationCount"
QT_MOC_LITERAL(158, 2303, 22), // "constructDDSQOSProfile"
QT_MOC_LITERAL(159, 2326, 24), // "requestSearchSuggestions"
QT_MOC_LITERAL(160, 2351, 18), // "setControllerReady"
QT_MOC_LITERAL(161, 2370, 5), // "ready"
QT_MOC_LITERAL(162, 2376, 7), // "openURL"
QT_MOC_LITERAL(163, 2384, 3), // "url"
QT_MOC_LITERAL(164, 2388, 20), // "ModelControllerReady"
QT_MOC_LITERAL(165, 2409, 20), // "initializeController"
QT_MOC_LITERAL(166, 2430, 17), // "table_dataChanged"
QT_MOC_LITERAL(167, 2448, 17), // "modelNotification"
QT_MOC_LITERAL(168, 2466, 14), // "MODEL_SEVERITY"
QT_MOC_LITERAL(169, 2481, 8), // "severity"
QT_MOC_LITERAL(170, 2490, 11) // "description"

    },
    "ViewController\0vc_showWelcomeScreen\0"
    "\0vc_JenkinsReady\0vc_JavaReady\0"
    "vc_controllerReady\0vc_ProjectLoaded\0"
    "vc_viewItemConstructed\0ViewItem*\0"
    "viewItem\0vc_viewItemDestructing\0ID\0"
    "item\0vc_showToolbar\0globalPos\0itemPos\0"
    "vc_gotSearchSuggestions\0suggestions\0"
    "vc_ActionFinished\0vc_editTableCell\0"
    "keyName\0vc_projectClosed\0"
    "vc_SetupModelController\0file_path\0"
    "vc_undo\0vc_redo\0vc_triggerAction\0"
    "vc_setData\0vc_removeData\0vc_deleteEntities\0"
    "QList<int>\0IDs\0vc_cutEntities\0"
    "vc_copyEntities\0vc_paste\0data\0"
    "vc_replicateEntities\0vc_constructNode\0"
    "parentID\0NODE_KIND\0nodeKind\0pos\0"
    "vc_constructEdge\0sourceIDs\0dstID\0"
    "EDGE_KIND\0edgeKind\0vc_destructEdges\0"
    "vc_destructAllEdges\0vc_constructConnectedNode\0"
    "vc_constructWorkerProcess\0point\0"
    "vc_importProjects\0fileData\0vc_projectSaved\0"
    "filePath\0vc_projectPathChanged\0"
    "vc_answerQuestion\0mc_showProgress\0"
    "mc_progressChanged\0mc_modelReady\0"
    "mc_projectModified\0mc_undoRedoUpdated\0"
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
    "vc_highlightItem\0highlight\0ids\0"
    "modelValidated\0importGraphMLFile\0"
    "importGraphMLExtract\0showCodeViewer\0"
    "tabName\0content\0jenkinsManager_SettingsValidated\0"
    "success\0errorString\0jenkinsManager_GotJava\0"
    "java\0javaVersion\0jenkinsManager_GotJenkinsNodesList\0"
    "graphmlData\0getCodeForComponent\0"
    "validateModel\0selectModel\0"
    "launchLocalDeployment\0model_NodeConstructed\0"
    "parent_id\0id\0kind\0model_EdgeConstructed\0"
    "src_id\0dst_id\0controller_entityDestructed\0"
    "GRAPHML_KIND\0controller_dataChanged\0"
    "key\0controller_dataRemoved\0setClipboardData\0"
    "newProject\0OpenProject\0OpenExistingProject\0"
    "importProjects\0importXMEProject\0"
    "importXMIProject\0saveProject\0saveAsProject\0"
    "closeProject\0closeMEDEA\0importIdlFile\0"
    "generateWorkspace\0executeJenkinsJob\0"
    "fitView\0fitAllViews\0centerSelection\0"
    "alignSelectionVertical\0alignSelectionHorizontal\0"
    "selectAndCenterConnectedEntities\0"
    "centerOnID\0showWiki\0reportBug\0"
    "showWikiForSelectedItem\0centerImpl\0"
    "centerDefinition\0popupDefinition\0"
    "popupImpl\0popupSelection\0popupItem\0"
    "aboutQt\0aboutMEDEA\0cut\0copy\0paste\0"
    "replicate\0deleteSelection\0expandSelection\0"
    "contractSelection\0editLabel\0"
    "editReplicationCount\0constructDDSQOSProfile\0"
    "requestSearchSuggestions\0setControllerReady\0"
    "ready\0openURL\0url\0ModelControllerReady\0"
    "initializeController\0table_dataChanged\0"
    "modelNotification\0MODEL_SEVERITY\0"
    "severity\0description"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ViewController[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
     121,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      55,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,  619,    2, 0x06 /* Public */,
       3,    1,  622,    2, 0x06 /* Public */,
       4,    1,  625,    2, 0x06 /* Public */,
       5,    1,  628,    2, 0x06 /* Public */,
       6,    1,  631,    2, 0x06 /* Public */,
       7,    1,  634,    2, 0x06 /* Public */,
      10,    2,  637,    2, 0x06 /* Public */,
      13,    2,  642,    2, 0x06 /* Public */,
      13,    1,  647,    2, 0x26 /* Public | MethodCloned */,
      16,    1,  650,    2, 0x06 /* Public */,
      18,    0,  653,    2, 0x06 /* Public */,
      19,    2,  654,    2, 0x06 /* Public */,
      21,    0,  659,    2, 0x06 /* Public */,
      22,    1,  660,    2, 0x06 /* Public */,
      24,    0,  663,    2, 0x06 /* Public */,
      25,    0,  664,    2, 0x06 /* Public */,
      26,    1,  665,    2, 0x06 /* Public */,
      27,    3,  668,    2, 0x06 /* Public */,
      28,    2,  675,    2, 0x06 /* Public */,
      29,    1,  680,    2, 0x06 /* Public */,
      32,    1,  683,    2, 0x06 /* Public */,
      33,    1,  686,    2, 0x06 /* Public */,
      34,    2,  689,    2, 0x06 /* Public */,
      36,    1,  694,    2, 0x06 /* Public */,
      37,    3,  697,    2, 0x06 /* Public */,
      37,    2,  704,    2, 0x26 /* Public | MethodCloned */,
      42,    3,  709,    2, 0x06 /* Public */,
      47,    3,  716,    2, 0x06 /* Public */,
      48,    2,  723,    2, 0x06 /* Public */,
      49,    5,  728,    2, 0x06 /* Public */,
      49,    4,  739,    2, 0x26 /* Public | MethodCloned */,
      50,    3,  748,    2, 0x06 /* Public */,
      52,    1,  755,    2, 0x06 /* Public */,
      54,    1,  758,    2, 0x06 /* Public */,
      56,    1,  761,    2, 0x06 /* Public */,
      57,    1,  764,    2, 0x06 /* Public */,
      58,    2,  767,    2, 0x06 /* Public */,
      59,    1,  772,    2, 0x06 /* Public */,
      60,    1,  775,    2, 0x06 /* Public */,
      61,    1,  778,    2, 0x06 /* Public */,
      62,    0,  781,    2, 0x06 /* Public */,
      63,    1,  782,    2, 0x06 /* Public */,
      64,    1,  785,    2, 0x06 /* Public */,
      65,    1,  788,    2, 0x06 /* Public */,
      66,    0,  791,    2, 0x06 /* Public */,
      67,    1,  792,    2, 0x06 /* Public */,
      68,    2,  795,    2, 0x06 /* Public */,
      71,    2,  800,    2, 0x06 /* Public */,
      73,    1,  805,    2, 0x06 /* Public */,
      75,    1,  808,    2, 0x06 /* Public */,
      76,    2,  811,    2, 0x06 /* Public */,
      76,    1,  816,    2, 0x26 /* Public | MethodCloned */,
      80,    2,  819,    2, 0x06 /* Public */,
      82,    1,  824,    2, 0x06 /* Public */,
      84,    2,  827,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      85,    1,  832,    2, 0x0a /* Public */,
      87,    1,  835,    2, 0x0a /* Public */,
      88,    1,  838,    2, 0x0a /* Public */,
      89,    1,  841,    2, 0x0a /* Public */,
      90,    2,  844,    2, 0x0a /* Public */,
      93,    2,  849,    2, 0x0a /* Public */,
      96,    2,  854,    2, 0x0a /* Public */,
      99,    1,  859,    2, 0x0a /* Public */,
     101,    0,  862,    2, 0x0a /* Public */,
     102,    0,  863,    2, 0x0a /* Public */,
     103,    0,  864,    2, 0x0a /* Public */,
     104,    0,  865,    2, 0x0a /* Public */,
     105,    3,  866,    2, 0x0a /* Public */,
     109,    4,  873,    2, 0x0a /* Public */,
     112,    2,  882,    2, 0x0a /* Public */,
     114,    3,  887,    2, 0x0a /* Public */,
     116,    2,  894,    2, 0x0a /* Public */,
     117,    1,  899,    2, 0x0a /* Public */,
     118,    0,  902,    2, 0x0a /* Public */,
     119,    0,  903,    2, 0x0a /* Public */,
     120,    1,  904,    2, 0x0a /* Public */,
     121,    0,  907,    2, 0x0a /* Public */,
     122,    0,  908,    2, 0x0a /* Public */,
     123,    0,  909,    2, 0x0a /* Public */,
     124,    0,  910,    2, 0x0a /* Public */,
     125,    0,  911,    2, 0x0a /* Public */,
     126,    0,  912,    2, 0x0a /* Public */,
     127,    0,  913,    2, 0x0a /* Public */,
     128,    0,  914,    2, 0x0a /* Public */,
     129,    0,  915,    2, 0x0a /* Public */,
     130,    0,  916,    2, 0x0a /* Public */,
     131,    0,  917,    2, 0x0a /* Public */,
     132,    0,  918,    2, 0x0a /* Public */,
     133,    0,  919,    2, 0x0a /* Public */,
     134,    0,  920,    2, 0x0a /* Public */,
     135,    0,  921,    2, 0x0a /* Public */,
     136,    0,  922,    2, 0x0a /* Public */,
     137,    1,  923,    2, 0x0a /* Public */,
     138,    0,  926,    2, 0x0a /* Public */,
     139,    0,  927,    2, 0x0a /* Public */,
     140,    0,  928,    2, 0x0a /* Public */,
     141,    0,  929,    2, 0x0a /* Public */,
     142,    0,  930,    2, 0x0a /* Public */,
     143,    0,  931,    2, 0x0a /* Public */,
     144,    0,  932,    2, 0x0a /* Public */,
     145,    0,  933,    2, 0x0a /* Public */,
     146,    1,  934,    2, 0x0a /* Public */,
     147,    0,  937,    2, 0x0a /* Public */,
     148,    0,  938,    2, 0x0a /* Public */,
     149,    0,  939,    2, 0x0a /* Public */,
     150,    0,  940,    2, 0x0a /* Public */,
     151,    0,  941,    2, 0x0a /* Public */,
     152,    0,  942,    2, 0x0a /* Public */,
     153,    0,  943,    2, 0x0a /* Public */,
     154,    0,  944,    2, 0x0a /* Public */,
     155,    0,  945,    2, 0x0a /* Public */,
     156,    0,  946,    2, 0x0a /* Public */,
     157,    0,  947,    2, 0x0a /* Public */,
     158,    0,  948,    2, 0x0a /* Public */,
     159,    0,  949,    2, 0x0a /* Public */,
     160,    1,  950,    2, 0x0a /* Public */,
     162,    1,  953,    2, 0x0a /* Public */,
     164,    1,  956,    2, 0x08 /* Private */,
     165,    0,  959,    2, 0x08 /* Private */,
     166,    3,  960,    2, 0x08 /* Private */,
     167,    3,  967,    2, 0x08 /* Private */,

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
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::QString,   11,   20,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   23,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::Int, QMetaType::QString, QMetaType::QVariant,    2,    2,    2,
    QMetaType::Void, QMetaType::Int, QMetaType::QString,    2,    2,
    QMetaType::Void, 0x80000000 | 30,   31,
    QMetaType::Void, 0x80000000 | 30,   31,
    QMetaType::Void, 0x80000000 | 30,   31,
    QMetaType::Void, 0x80000000 | 30, QMetaType::QString,   31,   35,
    QMetaType::Void, 0x80000000 | 30,   31,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 39, QMetaType::QPointF,   38,   40,   41,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 39,   38,   40,
    QMetaType::Void, 0x80000000 | 30, QMetaType::Int, 0x80000000 | 45,   43,   44,   46,
    QMetaType::Void, 0x80000000 | 30, QMetaType::Int, 0x80000000 | 45,   43,   44,   46,
    QMetaType::Void, 0x80000000 | 30, 0x80000000 | 45,   43,   46,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 39, QMetaType::Int, 0x80000000 | 45, QMetaType::QPointF,   38,   40,   44,   46,   41,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 39, QMetaType::Int, 0x80000000 | 45,   38,   40,   44,   46,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::QPointF,   38,   44,   51,
    QMetaType::Void, QMetaType::QStringList,   53,
    QMetaType::Void, QMetaType::QString,   55,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,    2,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   55,
    QMetaType::Void, QMetaType::Int,   11,
    QMetaType::Void, 0x80000000 | 8,   12,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   55,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   69,   70,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   69,   72,
    QMetaType::Void, QMetaType::QStringList,   74,
    QMetaType::Void, QMetaType::QString,   69,
    QMetaType::Void, QMetaType::Bool, 0x80000000 | 78,   77,   79,
    QMetaType::Void, QMetaType::Bool,   77,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   81,   69,
    QMetaType::Void, QMetaType::QString,   83,
    QMetaType::Void, QMetaType::Int, QMetaType::Bool,   11,   85,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 30,   86,
    QMetaType::Void, QMetaType::QString,   72,
    QMetaType::Void, QMetaType::QString,   69,
    QMetaType::Void, QMetaType::QString,   35,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   91,   92,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,   94,   95,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,   97,   98,
    QMetaType::Void, QMetaType::QString,  100,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, 0x80000000 | 39,  106,  107,  108,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 45, QMetaType::Int, QMetaType::Int,  107,  108,  110,  111,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 113,   11,  108,
    QMetaType::Void, QMetaType::Int, QMetaType::QString, QMetaType::QVariant,   11,  115,   35,
    QMetaType::Void, QMetaType::Int, QMetaType::QString,   11,  115,
    QMetaType::Void, QMetaType::QString,   35,
    QMetaType::Void,
    QMetaType::Bool,
    QMetaType::Bool, QMetaType::QString,   23,
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
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,  161,
    QMetaType::Void, QMetaType::QString,  163,
    QMetaType::Void, QMetaType::Bool,  161,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::QString, QMetaType::QVariant,   11,  115,   35,
    QMetaType::Void, 0x80000000 | 168, QMetaType::QString, QMetaType::Int,  169,  170,   11,

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
        case 10: _t->vc_ActionFinished(); break;
        case 11: _t->vc_editTableCell((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 12: _t->vc_projectClosed(); break;
        case 13: _t->vc_SetupModelController((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 14: _t->vc_undo(); break;
        case 15: _t->vc_redo(); break;
        case 16: _t->vc_triggerAction((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 17: _t->vc_setData((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QVariant(*)>(_a[3]))); break;
        case 18: _t->vc_removeData((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 19: _t->vc_deleteEntities((*reinterpret_cast< QList<int>(*)>(_a[1]))); break;
        case 20: _t->vc_cutEntities((*reinterpret_cast< QList<int>(*)>(_a[1]))); break;
        case 21: _t->vc_copyEntities((*reinterpret_cast< QList<int>(*)>(_a[1]))); break;
        case 22: _t->vc_paste((*reinterpret_cast< QList<int>(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 23: _t->vc_replicateEntities((*reinterpret_cast< QList<int>(*)>(_a[1]))); break;
        case 24: _t->vc_constructNode((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< NODE_KIND(*)>(_a[2])),(*reinterpret_cast< QPointF(*)>(_a[3]))); break;
        case 25: _t->vc_constructNode((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< NODE_KIND(*)>(_a[2]))); break;
        case 26: _t->vc_constructEdge((*reinterpret_cast< QList<int>(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< EDGE_KIND(*)>(_a[3]))); break;
        case 27: _t->vc_destructEdges((*reinterpret_cast< QList<int>(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< EDGE_KIND(*)>(_a[3]))); break;
        case 28: _t->vc_destructAllEdges((*reinterpret_cast< QList<int>(*)>(_a[1])),(*reinterpret_cast< EDGE_KIND(*)>(_a[2]))); break;
        case 29: _t->vc_constructConnectedNode((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< NODE_KIND(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< EDGE_KIND(*)>(_a[4])),(*reinterpret_cast< QPointF(*)>(_a[5]))); break;
        case 30: _t->vc_constructConnectedNode((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< NODE_KIND(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< EDGE_KIND(*)>(_a[4]))); break;
        case 31: _t->vc_constructWorkerProcess((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< QPointF(*)>(_a[3]))); break;
        case 32: _t->vc_importProjects((*reinterpret_cast< QStringList(*)>(_a[1]))); break;
        case 33: _t->vc_projectSaved((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 34: _t->vc_projectPathChanged((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 35: _t->vc_answerQuestion((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 36: _t->mc_showProgress((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 37: _t->mc_progressChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 38: _t->mc_modelReady((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 39: _t->mc_projectModified((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 40: _t->mc_undoRedoUpdated(); break;
        case 41: _t->vc_executeJenkinsJob((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 42: _t->vc_centerItem((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 43: _t->vc_selectAndCenterConnectedEntities((*reinterpret_cast< ViewItem*(*)>(_a[1]))); break;
        case 44: _t->vc_fitToScreen(); break;
        case 45: _t->vc_addProjectToRecentProjects((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 46: _t->vc_getCodeForComponent((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 47: _t->vc_validateModel((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 48: _t->vc_modelValidated((*reinterpret_cast< QStringList(*)>(_a[1]))); break;
        case 49: _t->vc_launchLocalDeployment((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 50: _t->vc_backgroundProcess((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< BACKGROUND_PROCESS(*)>(_a[2]))); break;
        case 51: _t->vc_backgroundProcess((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 52: _t->vc_importXMEProject((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 53: _t->vc_importXMIProject((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 54: _t->vc_highlightItem((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 55: _t->highlight((*reinterpret_cast< QList<int>(*)>(_a[1]))); break;
        case 56: _t->modelValidated((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 57: _t->importGraphMLFile((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 58: _t->importGraphMLExtract((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 59: _t->showCodeViewer((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 60: _t->jenkinsManager_SettingsValidated((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 61: _t->jenkinsManager_GotJava((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 62: _t->jenkinsManager_GotJenkinsNodesList((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 63: _t->getCodeForComponent(); break;
        case 64: _t->validateModel(); break;
        case 65: _t->selectModel(); break;
        case 66: _t->launchLocalDeployment(); break;
        case 67: _t->model_NodeConstructed((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< NODE_KIND(*)>(_a[3]))); break;
        case 68: _t->model_EdgeConstructed((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< EDGE_KIND(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4]))); break;
        case 69: _t->controller_entityDestructed((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< GRAPHML_KIND(*)>(_a[2]))); break;
        case 70: _t->controller_dataChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QVariant(*)>(_a[3]))); break;
        case 71: _t->controller_dataRemoved((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 72: _t->setClipboardData((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 73: _t->newProject(); break;
        case 74: { bool _r = _t->OpenProject();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 75: { bool _r = _t->OpenExistingProject((*reinterpret_cast< QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 76: _t->importProjects(); break;
        case 77: _t->importXMEProject(); break;
        case 78: _t->importXMIProject(); break;
        case 79: _t->saveProject(); break;
        case 80: _t->saveAsProject(); break;
        case 81: _t->closeProject(); break;
        case 82: _t->closeMEDEA(); break;
        case 83: _t->importIdlFile(); break;
        case 84: _t->generateWorkspace(); break;
        case 85: _t->executeJenkinsJob(); break;
        case 86: _t->fitView(); break;
        case 87: _t->fitAllViews(); break;
        case 88: _t->centerSelection(); break;
        case 89: _t->alignSelectionVertical(); break;
        case 90: _t->alignSelectionHorizontal(); break;
        case 91: _t->selectAndCenterConnectedEntities(); break;
        case 92: _t->centerOnID((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 93: _t->showWiki(); break;
        case 94: _t->reportBug(); break;
        case 95: _t->showWikiForSelectedItem(); break;
        case 96: _t->centerImpl(); break;
        case 97: _t->centerDefinition(); break;
        case 98: _t->popupDefinition(); break;
        case 99: _t->popupImpl(); break;
        case 100: _t->popupSelection(); break;
        case 101: _t->popupItem((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 102: _t->aboutQt(); break;
        case 103: _t->aboutMEDEA(); break;
        case 104: _t->cut(); break;
        case 105: _t->copy(); break;
        case 106: _t->paste(); break;
        case 107: _t->replicate(); break;
        case 108: _t->deleteSelection(); break;
        case 109: _t->expandSelection(); break;
        case 110: _t->contractSelection(); break;
        case 111: _t->editLabel(); break;
        case 112: _t->editReplicationCount(); break;
        case 113: _t->constructDDSQOSProfile(); break;
        case 114: _t->requestSearchSuggestions(); break;
        case 115: _t->setControllerReady((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 116: _t->openURL((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 117: _t->ModelControllerReady((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 118: _t->initializeController(); break;
        case 119: _t->table_dataChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QVariant(*)>(_a[3]))); break;
        case 120: _t->modelNotification((*reinterpret_cast< MODEL_SEVERITY(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
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
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QList<int> >(); break;
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
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< NODE_KIND >(); break;
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
            case 2:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< EDGE_KIND >(); break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QList<int> >(); break;
            }
            break;
        case 28:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< EDGE_KIND >(); break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QList<int> >(); break;
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
        case 30:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 3:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< EDGE_KIND >(); break;
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< NODE_KIND >(); break;
            }
            break;
        case 43:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< ViewItem* >(); break;
            }
            break;
        case 55:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QList<int> >(); break;
            }
            break;
        case 67:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 2:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< NODE_KIND >(); break;
            }
            break;
        case 68:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< EDGE_KIND >(); break;
            }
            break;
        case 69:
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
            typedef void (ViewController::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_ActionFinished)) {
                *result = 10;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(int , QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_editTableCell)) {
                *result = 11;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_projectClosed)) {
                *result = 12;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_SetupModelController)) {
                *result = 13;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_undo)) {
                *result = 14;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_redo)) {
                *result = 15;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_triggerAction)) {
                *result = 16;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(int , QString , QVariant );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_setData)) {
                *result = 17;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(int , QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_removeData)) {
                *result = 18;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(QList<int> );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_deleteEntities)) {
                *result = 19;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(QList<int> );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_cutEntities)) {
                *result = 20;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(QList<int> );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_copyEntities)) {
                *result = 21;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(QList<int> , QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_paste)) {
                *result = 22;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(QList<int> );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_replicateEntities)) {
                *result = 23;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(int , NODE_KIND , QPointF );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_constructNode)) {
                *result = 24;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(QList<int> , int , EDGE_KIND );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_constructEdge)) {
                *result = 26;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(QList<int> , int , EDGE_KIND );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_destructEdges)) {
                *result = 27;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(QList<int> , EDGE_KIND );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_destructAllEdges)) {
                *result = 28;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(int , NODE_KIND , int , EDGE_KIND , QPointF );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_constructConnectedNode)) {
                *result = 29;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(int , int , QPointF );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_constructWorkerProcess)) {
                *result = 31;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(QStringList );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_importProjects)) {
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
            typedef void (ViewController::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_executeJenkinsJob)) {
                *result = 41;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_centerItem)) {
                *result = 42;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(ViewItem * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_selectAndCenterConnectedEntities)) {
                *result = 43;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_fitToScreen)) {
                *result = 44;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_addProjectToRecentProjects)) {
                *result = 45;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(QString , QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_getCodeForComponent)) {
                *result = 46;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(QString , QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_validateModel)) {
                *result = 47;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(QStringList );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_modelValidated)) {
                *result = 48;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_launchLocalDeployment)) {
                *result = 49;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(bool , BACKGROUND_PROCESS );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_backgroundProcess)) {
                *result = 50;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(QString , QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_importXMEProject)) {
                *result = 52;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_importXMIProject)) {
                *result = 53;
                return;
            }
        }
        {
            typedef void (ViewController::*_t)(int , bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewController::vc_highlightItem)) {
                *result = 54;
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
        if (_id < 121)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 121;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 121)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 121;
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
void ViewController::vc_ActionFinished()
{
    QMetaObject::activate(this, &staticMetaObject, 10, Q_NULLPTR);
}

// SIGNAL 11
void ViewController::vc_editTableCell(int _t1, QString _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 11, _a);
}

// SIGNAL 12
void ViewController::vc_projectClosed()
{
    QMetaObject::activate(this, &staticMetaObject, 12, Q_NULLPTR);
}

// SIGNAL 13
void ViewController::vc_SetupModelController(QString _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 13, _a);
}

// SIGNAL 14
void ViewController::vc_undo()
{
    QMetaObject::activate(this, &staticMetaObject, 14, Q_NULLPTR);
}

// SIGNAL 15
void ViewController::vc_redo()
{
    QMetaObject::activate(this, &staticMetaObject, 15, Q_NULLPTR);
}

// SIGNAL 16
void ViewController::vc_triggerAction(QString _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 16, _a);
}

// SIGNAL 17
void ViewController::vc_setData(int _t1, QString _t2, QVariant _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 17, _a);
}

// SIGNAL 18
void ViewController::vc_removeData(int _t1, QString _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 18, _a);
}

// SIGNAL 19
void ViewController::vc_deleteEntities(QList<int> _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 19, _a);
}

// SIGNAL 20
void ViewController::vc_cutEntities(QList<int> _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 20, _a);
}

// SIGNAL 21
void ViewController::vc_copyEntities(QList<int> _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 21, _a);
}

// SIGNAL 22
void ViewController::vc_paste(QList<int> _t1, QString _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 22, _a);
}

// SIGNAL 23
void ViewController::vc_replicateEntities(QList<int> _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 23, _a);
}

// SIGNAL 24
void ViewController::vc_constructNode(int _t1, NODE_KIND _t2, QPointF _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 24, _a);
}

// SIGNAL 26
void ViewController::vc_constructEdge(QList<int> _t1, int _t2, EDGE_KIND _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 26, _a);
}

// SIGNAL 27
void ViewController::vc_destructEdges(QList<int> _t1, int _t2, EDGE_KIND _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 27, _a);
}

// SIGNAL 28
void ViewController::vc_destructAllEdges(QList<int> _t1, EDGE_KIND _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 28, _a);
}

// SIGNAL 29
void ViewController::vc_constructConnectedNode(int _t1, NODE_KIND _t2, int _t3, EDGE_KIND _t4, QPointF _t5)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)), const_cast<void*>(reinterpret_cast<const void*>(&_t5)) };
    QMetaObject::activate(this, &staticMetaObject, 29, _a);
}

// SIGNAL 31
void ViewController::vc_constructWorkerProcess(int _t1, int _t2, QPointF _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 31, _a);
}

// SIGNAL 32
void ViewController::vc_importProjects(QStringList _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
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
void ViewController::vc_executeJenkinsJob(QString _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 41, _a);
}

// SIGNAL 42
void ViewController::vc_centerItem(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 42, _a);
}

// SIGNAL 43
void ViewController::vc_selectAndCenterConnectedEntities(ViewItem * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 43, _a);
}

// SIGNAL 44
void ViewController::vc_fitToScreen()
{
    QMetaObject::activate(this, &staticMetaObject, 44, Q_NULLPTR);
}

// SIGNAL 45
void ViewController::vc_addProjectToRecentProjects(QString _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 45, _a);
}

// SIGNAL 46
void ViewController::vc_getCodeForComponent(QString _t1, QString _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 46, _a);
}

// SIGNAL 47
void ViewController::vc_validateModel(QString _t1, QString _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 47, _a);
}

// SIGNAL 48
void ViewController::vc_modelValidated(QStringList _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 48, _a);
}

// SIGNAL 49
void ViewController::vc_launchLocalDeployment(QString _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 49, _a);
}

// SIGNAL 50
void ViewController::vc_backgroundProcess(bool _t1, BACKGROUND_PROCESS _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 50, _a);
}

// SIGNAL 52
void ViewController::vc_importXMEProject(QString _t1, QString _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 52, _a);
}

// SIGNAL 53
void ViewController::vc_importXMIProject(QString _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 53, _a);
}

// SIGNAL 54
void ViewController::vc_highlightItem(int _t1, bool _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 54, _a);
}
QT_END_MOC_NAMESPACE
