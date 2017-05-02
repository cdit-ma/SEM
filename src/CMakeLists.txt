cmake_minimum_required (VERSION 2.6)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

set(LIB_NAME MEDEA_LIB)
find_package(Qt5Gui)
find_package(Qt5Network)
find_package(Qt5Widgets)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/Controllers/ActionController/actioncontroller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Controllers/SelectionController/selectioncontroller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Controllers/SelectionController/selectionhandler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Controllers/ToolbarController/nodeviewitemaction.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Controllers/ToolbarController/toolbarcontroller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Controllers/ViewController/edgeviewitem.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Controllers/ViewController/nodeviewitem.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Controllers/ViewController/viewcontroller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Controllers/ViewController/viewitem.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Controllers/WindowManager/windowmanager.cpp
    
    ${CMAKE_CURRENT_SOURCE_DIR}/Controllers/SettingsController/settingscontroller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Controllers/SettingsController/setting.cpp
    #${CMAKE_CURRENT_SOURCE_DIR}/Plugins/XMI/Widgets/XMITreeViewDialog.cpp
    #${CMAKE_CURRENT_SOURCE_DIR}/Plugins/XMI/xmiimporter.cpp
    #${CMAKE_CURRENT_SOURCE_DIR}/Plugins/XMI/xmitreemodel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Utils/actiongroup.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Utils/filehandler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Utils/qobjectregistrar.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/ContextToolbar/contexttoolbar.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/Dock/docktabwidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/Dock/dockwidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/Dock/dockwidgetactionitem.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/Dock/dockwidgetitem.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/Dock/dockwidgetparentactionitem.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/NodeView/SceneItems/Node/basicnodeitem.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/NodeView/SceneItems/Node/defaultnodeitem.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/NodeView/SceneItems/Node/hardwarenodeitem.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/NodeView/SceneItems/Node/managementcomponentnodeitem.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/NodeView/SceneItems/Node/nodeitem.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/NodeView/SceneItems/Node/stacknodeitem.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/NodeView/SceneItems/entityitem.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/NodeView/nodeview.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/NodeView/nodeviewminimap.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/QOSBrowser/qosbrowser.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/QOSBrowser/qosprofilemodel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/Search/searchdialog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/Search/searchitemwidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/Table/datatablemodel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/Table/datatableview.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/Table/datatablewidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/CodeEditor/codebrowser.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/CodeEditor/codeeditor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/CodeEditor/syntaxhighlighter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Dialogs/appsettings.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Dialogs/popupwidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Dialogs/shortcutdialog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/ViewManager/viewmanagerwidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Windows/welcomescreenwidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/dataeditwidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/enumerations.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/theme.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Windows/centralwindow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Windows/mainwindow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Windows/subwindow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Windows/viewwindow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Windows/basewindow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/DockWidgets/docktitlebar.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/DockWidgets/basedockwidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/DockWidgets/nodeviewdockwidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/DockWidgets/viewdockwidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/DockWidgets/tooldockwidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Utils/rootaction.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/Table/datatabledelegate.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/ViewManager/windowitem.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/ViewManager/dockitem.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Controllers/NotificationManager/notificationmanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/NodeView/SceneItems/Edge/edgeitem.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/Notification/notificationdialog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/Notification/notificationobject.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/Notification/notificationtoolbar.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/Notification/notificationitem.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Utils/filtergroup.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Utils/processrunner.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Controllers/JenkinsManager/jenkinsmanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Controllers/JenkinsManager/jenkinsrequest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Jenkins/jenkinsjobmonitorwidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Jenkins/jenkinsstartjobwidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Controllers/ExecutionManager/executionmanager.cpp
)

SET(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/Controllers/ActionController/actioncontroller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Controllers/SelectionController/selectioncontroller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Controllers/SelectionController/selectionhandler.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Controllers/ToolbarController/nodeviewitemaction.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Controllers/ToolbarController/toolbarcontroller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Controllers/ViewController/edgeviewitem.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Controllers/ViewController/nodeviewitem.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Controllers/ViewController/viewcontroller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Controllers/ViewController/viewitem.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Controllers/SettingsController/settingscontroller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Controllers/SettingsController/setting.h
    #${CMAKE_CURRENT_SOURCE_DIR}/#Plugins/XMI/Widgets/XMITreeViewDialog.h
    #${CMAKE_CURRENT_SOURCE_DIR}/#Plugins/XMI/xmiimporter.h
    #${CMAKE_CURRENT_SOURCE_DIR}/#Plugins/XMI/xmitreemodel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Utils/actiongroup.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Utils/filehandler.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Utils/qobjectregistrar.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/ContextToolbar/contexttoolbar.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/Dock/docktabwidget.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/Dock/dockwidget.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/Dock/dockwidgetactionitem.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/Dock/dockwidgetitem.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/Dock/dockwidgetparentactionitem.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/NodeView/SceneItems/Node/basicnodeitem.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/NodeView/SceneItems/Node/defaultnodeitem.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/NodeView/SceneItems/Node/hardwarenodeitem.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/NodeView/SceneItems/Node/managementcomponentnodeitem.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/NodeView/SceneItems/Node/nodeitem.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/NodeView/SceneItems/Node/stacknodeitem.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/NodeView/SceneItems/entityitem.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/NodeView/nodeview.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/NodeView/nodeviewminimap.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/QOSBrowser/qosbrowser.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/QOSBrowser/qosprofilemodel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/Search/searchdialog.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/Search/searchitemwidget.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/Table/datatablemodel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/Table/datatableview.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/Table/datatablewidget.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/CodeEditor/codebrowser.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/CodeEditor/codeeditor.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/CodeEditor/syntaxhighlighter.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Dialogs/appsettings.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Dialogs/popupwidget.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Dialogs/shortcutdialog.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/ViewManager/viewmanagerwidget.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Windows/welcomescreenwidget.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/dataeditwidget.h
    ${CMAKE_CURRENT_SOURCE_DIR}/enumerations.h
    ${CMAKE_CURRENT_SOURCE_DIR}/theme.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Windows/centralwindow.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Windows/mainwindow.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Windows/subwindow.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Windows/basewindow.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Windows/viewwindow.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/DockWidgets/basedockwidget.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/DockWidgets/docktitlebar.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/DockWidgets/nodeviewdockwidget.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/DockWidgets/tooldockwidget.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/DockWidgets/viewdockwidget.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Utils/rootaction.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Utils/doublehash.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Controllers/WindowManager/windowmanager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/Table/datatabledelegate.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/ViewManager/windowitem.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/ViewManager/dockitem.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Controllers/NotificationManager/notificationmanager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/NodeView/SceneItems/Edge/edgeitem.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/Notification/notificationdialog.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/Notification/notificationobject.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/Notification/notificationtoolbar.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/Notification/notificationitem.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Utils/filtergroup.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Utils/processrunner.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Controllers/JenkinsManager/jenkinsmanager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Controllers/JenkinsManager/jenkinsrequest.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Jenkins/jenkinsjobmonitorwidget.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Jenkins/jenkinsstartjobwidget.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Controllers/ExecutionManager/executionmanager.h
)

set(RESOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/images.qrc
    ${CMAKE_CURRENT_SOURCE_DIR}/workers.qrc
    )
add_subdirectory("Model")
add_library(${LIB_NAME} STATIC ${SOURCE} ${HEADERS} ${RESOURCES})


target_link_libraries(${LIB_NAME} Qt5::Widgets)
target_link_libraries(${LIB_NAME} Qt5::Gui)
target_link_libraries(${LIB_NAME} Qt5::Network)

target_link_libraries(${LIB_NAME} MEDEA_MODELCONTROLLER)