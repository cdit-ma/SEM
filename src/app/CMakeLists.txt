set(PROJ_NAME MEDEA)
project(${PROJ_NAME})
#find_package(Qt5 COMPONENTS Widgets Network Charts Concurrent REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Widgets Network Concurrent REQUIRED)

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/Controllers/ActionController/actioncontroller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Controllers/ExecutionManager/executionmanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Controllers/JenkinsManager/jenkinsmanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Controllers/NotificationManager/notificationenumerations.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Controllers/NotificationManager/notificationmanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Controllers/NotificationManager/notificationobject.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Controllers/SearchManager/searchmanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Controllers/SelectionController/selectioncontroller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Controllers/SelectionController/selectionhandler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Controllers/SettingsController/setting.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Controllers/SettingsController/settingscontroller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Controllers/ViewController/edgeviewitem.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Controllers/ViewController/nodeviewitem.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Controllers/ViewController/viewcontroller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Controllers/ViewController/viewitem.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Controllers/WindowManager/windowmanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Controllers/AggregationProxy/aggregationproxy.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/theme.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Utils/actiongroup.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Utils/filehandler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Utils/processrunner.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Utils/qobjectregistrar.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Utils/rootaction.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/ContextMenu/contextmenu.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/Dock/docktabwidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/NodeView/nodeview.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/NodeView/nodeviewminimap.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/NodeView/SceneItems/arrowline.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/NodeView/SceneItems/entityitem.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/NodeView/SceneItems/Edge/edgeitem.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/NodeView/SceneItems/Node/basicnodeitem.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/NodeView/SceneItems/Node/compactnodeitem.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/NodeView/SceneItems/Node/defaultnodeitem.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/NodeView/SceneItems/Node/deploymentcontainernodeitem.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/NodeView/SceneItems/Node/hardwarenodeitem.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/NodeView/SceneItems/Node/membernodeitem.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/NodeView/SceneItems/Node/nodeitem.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/NodeView/SceneItems/Node/stacknodeitem.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/NodeView/SceneItems/statictextitem.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/Notification/notificationdialog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/Notification/notificationitem.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/Notification/notificationpopup.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/Notification/notificationtoolbar.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/QOSBrowser/qosbrowser.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/QOSBrowser/qosprofilemodel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/Search/gotopopup.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/Search/searchdialog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/Search/searchitemwidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/Search/searchpopup.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/Table/datatabledelegate.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/Table/datatablemodel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/Table/datatableview.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/Table/datatablewidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/CodeEditor/codebrowser.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/CodeEditor/codeeditor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/CodeEditor/syntaxhighlighter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/optiongroupbox.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/customgroupbox.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/dataeditwidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Dialogs/appsettings.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Dialogs/popupwidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Dialogs/progresspopup.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Dialogs/shortcutdialog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Dialogs/variabledialog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/DockWidgets/basedockwidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/DockWidgets/defaultdockwidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/DockWidgets/docktitlebar.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/DockWidgets/invisibledockwidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/DockWidgets/tooldockwidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/DockWidgets/viewdockwidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/filterwidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/iconpicker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/loadmorewidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Monitors/consolemonitor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Monitors/jenkinsmonitor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Monitors/jobmonitor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/ViewManager/dockitem.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/ViewManager/dockreparenterpopup.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/ViewManager/viewmanagerwidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/ViewManager/windowitem.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Windows/basewindow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Windows/centralwindow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Windows/invisiblewindow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Windows/mainwindow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Windows/subwindow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Windows/viewwindow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Windows/welcomescreenwidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Panel/overlaysplitter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Panel/panelwidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Charts/Timeline/chartdialog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Charts/Timeline/chartinputpopup.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Charts/Timeline/Chart/hoverpopup.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Charts/Timeline/Chart/timelinechartview.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Charts/Timeline/Chart/chartlist.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Charts/Timeline/Chart/chart.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Charts/Timeline/Axis/chartlabellist.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Charts/Timeline/Axis/chartlabel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Charts/Timeline/Axis/axisslider.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Charts/Timeline/Axis/axisdisplay.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Charts/Timeline/Axis/axiswidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Charts/Data/Requests/request.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Charts/Data/Requests/eventrequest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Charts/Data/Requests/utilisationrequest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Charts/Data/Requests/portlifecyclerequest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Charts/Data/Requests/porteventrequest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Charts/Data/Requests/workloadrequest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Charts/Data/Requests/cpuutilisationrequest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Charts/Data/Requests/memoryutilisationrequest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Charts/Data/Requests/markerrequest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Charts/Data/Events/event.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Charts/Data/Events/portlifecycleevent.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Charts/Data/Events/portevent.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Charts/Data/Events/workloadevent.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Charts/Data/Events/cpuutilisationevent.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Charts/Data/Events/memoryutilisationevent.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Charts/Data/Events/markerevent.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Charts/Data/Series/eventseries.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Charts/Data/Series/portlifecycleeventseries.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Charts/Data/Series/porteventseries.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Charts/Data/Series/workloadeventseries.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Charts/Data/Series/cpuutilisationeventseries.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Charts/Data/Series/memoryutilisationeventseries.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Charts/Data/Series/markereventseries.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Charts/ChartManager/chartmanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Charts/ChartManager/requestbuilder.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Dataflow/dataflowdialog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Dataflow/dataflowgraphicsview.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Dataflow/GraphicsLayoutItems/graphicslayoutitem.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Dataflow/GraphicsLayoutItems/pixmapgraphicsitem.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Dataflow/GraphicsLayoutItems/textgraphicsitem.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Dataflow/EntityItems/componentinstancegraphicsitem.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Dataflow/EntityItems/portinstancegraphicsitem.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Dataflow/GraphicsItems/edgeitem.cpp
)

if(WIN32)
    set(SOURCE
        ${SOURCE}
        ${CMAKE_CURRENT_SOURCE_DIR}/medea.rc
    )
endif()

SET(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/Controllers/ActionController/actioncontroller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Controllers/ExecutionManager/executionmanager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Controllers/JenkinsManager/jenkinsmanager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Controllers/NotificationManager/notificationenumerations.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Controllers/NotificationManager/notificationmanager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Controllers/NotificationManager/notificationobject.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Controllers/SearchManager/searchmanager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Controllers/SelectionController/selectioncontroller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Controllers/SelectionController/selectionhandler.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Controllers/SettingsController/setting.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Controllers/SettingsController/settingscontroller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Controllers/ViewController/edgeviewitem.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Controllers/ViewController/nodeviewitem.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Controllers/ViewController/viewcontroller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Controllers/ViewController/viewitem.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Controllers/WindowManager/windowmanager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Controllers/AggregationProxy/aggregationproxy.h
    ${CMAKE_CURRENT_SOURCE_DIR}/theme.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Utils/actiongroup.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Utils/filehandler.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Utils/processrunner.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Utils/qobjectregistrar.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Utils/rootaction.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/ContextMenu/contextmenu.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/Dock/docktabwidget.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/NodeView/nodeview.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/NodeView/nodeviewminimap.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/NodeView/SceneItems/arrowline.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/NodeView/SceneItems/Edge/edgeitem.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/NodeView/SceneItems/entityitem.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/NodeView/SceneItems/Node/basicnodeitem.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/NodeView/SceneItems/Node/compactnodeitem.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/NodeView/SceneItems/Node/defaultnodeitem.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/NodeView/SceneItems/Node/hardwarenodeitem.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/NodeView/SceneItems/Node/membernodeitem.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/NodeView/SceneItems/Node/deploymentcontainernodeitem.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/NodeView/SceneItems/Node/nodeitem.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/NodeView/SceneItems/Node/stacknodeitem.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/NodeView/SceneItems/statictextitem.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/Notification/notificationdialog.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/Notification/notificationitem.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/Notification/notificationpopup.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/Notification/notificationtoolbar.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/QOSBrowser/qosbrowser.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/QOSBrowser/qosprofilemodel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/Search/gotopopup.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/Search/searchdialog.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/Search/searchitemwidget.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/Search/searchpopup.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/Table/datatabledelegate.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/Table/datatablemodel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/Table/datatableview.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Views/Table/datatablewidget.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/CodeEditor/codebrowser.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/CodeEditor/codeeditor.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/CodeEditor/syntaxhighlighter.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/optiongroupbox.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/customgroupbox.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/dataeditwidget.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Dialogs/appsettings.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Dialogs/popupwidget.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Dialogs/progresspopup.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Dialogs/shortcutdialog.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Dialogs/variabledialog.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/DockWidgets/basedockwidget.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/DockWidgets/defaultdockwidget.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/DockWidgets/docktitlebar.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/DockWidgets/invisibledockwidget.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/DockWidgets/tooldockwidget.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/DockWidgets/viewdockwidget.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/filterwidget.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/iconpicker.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/loadmorewidget.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/ViewManager/windowitem.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/ViewManager/dockreparenterpopup.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/ViewManager/dockitem.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Monitors/consolemonitor.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Monitors/jenkinsmonitor.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Monitors/jobmonitor.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Monitors/monitor.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/ViewManager/viewmanagerwidget.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Windows/basewindow.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Windows/centralwindow.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Windows/invisiblewindow.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Windows/mainwindow.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Windows/subwindow.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Windows/viewwindow.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Windows/welcomescreenwidget.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Panel/overlaysplitter.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Panel/panelwidget.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Charts/Timeline/chartdialog.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Charts/Timeline/chartinputpopup.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Charts/Timeline/Chart/hoverpopup.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Charts/Timeline/Chart/timelinechartview.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Charts/Timeline/Chart/chartlist.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Charts/Timeline/Chart/chart.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Charts/Timeline/Axis/chartlabellist.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Charts/Timeline/Axis/chartlabel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Charts/Timeline/Axis/axisslider.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Charts/Timeline/Axis/axisdisplay.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Charts/Timeline/Axis/axiswidget.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Charts/Data/Requests/request.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Charts/Data/Requests/eventrequest.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Charts/Data/Requests/utilisationrequest.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Charts/Data/Requests/portlifecyclerequest.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Charts/Data/Requests/porteventrequest.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Charts/Data/Requests/workloadrequest.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Charts/Data/Requests/cpuutilisationrequest.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Charts/Data/Requests/memoryutilisationrequest.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Charts/Data/Requests/markerrequest.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Charts/Data/Events/protomessagestructs.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Charts/Data/Events/event.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Charts/Data/Events/portlifecycleevent.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Charts/Data/Events/portevent.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Charts/Data/Events/workloadevent.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Charts/Data/Events/cpuutilisationevent.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Charts/Data/Events/memoryutilisationevent.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Charts/Data/Events/markerevent.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Charts/Data/Series/eventseries.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Charts/Data/Series/portlifecycleeventseries.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Charts/Data/Series/porteventseries.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Charts/Data/Series/workloadeventseries.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Charts/Data/Series/cpuutilisationeventseries.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Charts/Data/Series/memoryutilisationeventseries.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Charts/Data/Series/markereventseries.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Charts/ChartManager/chartmanager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Charts/ChartManager/requestbuilder.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Dataflow/dataflowdialog.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Dataflow/dataflowgraphicsview.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Dataflow/GraphicsLayoutItems/graphicslayoutitem.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Dataflow/GraphicsLayoutItems/pixmapgraphicsitem.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Dataflow/GraphicsLayoutItems/textgraphicsitem.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Dataflow/EntityItems/componentinstancegraphicsitem.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Dataflow/EntityItems/portinstancegraphicsitem.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/Dataflow/GraphicsItems/edgeitem.h
)

set(RESOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/images.qrc
)

if(WIN32)
    SET(QT_USE_QTMAIN TRUE)
    set(GUI_TYPE WIN32)
endif()

set(COPY_DEST ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

if(APPLE)
    set(MACOSX_BUNDLE_ICON_FILE medea.icns)
    set(MACOSX_BUNDLE_LONG_VERSION_STRING "${PROJ_NAME} v${VERSION}")
    set(MACOSX_BUNDLE_BUNDLE_NAME ${PROJ_NAME})
    set(MACOSX_BUNDLE_SHORT_VERSION_STRING ${VERSION})
    set(MACOSX_BUNDLE_BUNDLE_VERSION ${VERSION})
    set(MACOSX_BUNDLE_COPYRIGHT "Copyright 2016. All Rights Reserved.")

    # Bundle the MEDEA icon (icns) for mac
    set(MAC_ICON ${CMAKE_CURRENT_SOURCE_DIR}/Resources/medea.icns)
    set_source_files_properties(${MAC_ICON} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
    
    # Copy Resources into .app file
    set(COPY_DEST "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJ_NAME}.app/Contents/MacOS/")

    # Construct a .app file
    add_executable(${PROJ_NAME} MACOSX_BUNDLE ${SOURCE} ${HEADERS} ${RESOURCES} ${MAC_ICON})
else()
    # Construct the binary
    add_executable(${PROJ_NAME} ${GUI_TYPE} ${SOURCE} ${HEADERS} ${RESOURCES})
endif()

# Copy files
set(REGEN_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../re/re_gen")
set(REGEN_DST_DIR "${COPY_DEST}/Resources/re_gen")

set(SCRIPTS_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../scripts")
set(SCRIPTS_DST_DIR "${COPY_DEST}/Resources/scripts")

set(CHANGELOG_SRC "${CMAKE_CURRENT_SOURCE_DIR}/../../changelog.md")
set(CHANGELOG_DST "${COPY_DEST}/changelog.md")

# Copy RE_GEN
add_custom_command(TARGET ${PROJ_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${REGEN_SRC_DIR} ${REGEN_DST_DIR} COMMENT "Copying re_gen")
# Copy Scripts
add_custom_command(TARGET ${PROJ_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${SCRIPTS_SRC_DIR} ${SCRIPTS_DST_DIR} COMMENT "Copying scripts")
# Copy Changelog
add_custom_command(TARGET ${PROJ_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CHANGELOG_SRC} ${CHANGELOG_DST} COMMENT "Copying changelog.md")

if(WIN32)
    # Link against the Win Main Launcher
    target_link_libraries(${PROJ_NAME} PRIVATE Qt5::WinMain)
endif()


target_include_directories(${PROJ_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../../re_common/")
target_include_directories(${PROJ_NAME} PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/../../re_common/")

# Link against things
target_link_libraries(${PROJ_NAME} PRIVATE aggregation_requester_lib)
target_link_libraries(${PROJ_NAME} PRIVATE medea_modelcontroller_lib)
target_link_libraries(${PROJ_NAME} PRIVATE Qt5::Widgets)
target_link_libraries(${PROJ_NAME} PRIVATE Qt5::Network)
target_link_libraries(${PROJ_NAME} PRIVATE Qt5::Concurrent)

# Install should copy the MEDEA app into the bin folder
INSTALL(TARGETS ${PROJ_NAME} BUNDLE DESTINATION . COMPONENT ${PROJ_NAME} RUNTIME DESTINATION . COMPONENT ${PROJ_NAME})

if(WIN32)
	# Copy Resources as part of install for windows
    INSTALL(DIRECTORY ${REGEN_SRC_DIR} DESTINATION "./Resources" COMPONENT RE_GEN)
    INSTALL(DIRECTORY ${SCRIPTS_SRC_DIR} DESTINATION "./Resources" COMPONENT SCRIPTS)
    INSTALL(FILES ${CHANGELOG_SRC} DESTINATION "./" COMPONENT ${PROJ_NAME})
    INSTALL(DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/ DESTINATION "./" COMPONENT QT_LIBS FILES_MATCHING PATTERN "*.dll" PATTERN "*.qm")
endif()

# Run the qt deploy program to copy required binaries into the directories
if(DEFINED ENV{Qt5_DIR})
    if(WIN32)
        add_custom_command(TARGET ${PROJ_NAME} POST_BUILD
            COMMAND $ENV{Qt5_DIR}/bin/windeployqt ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJ_NAME}.exe
            COMMENT "Running windeployqt..."
        )
    elseif(APPLE)
        add_custom_command(TARGET ${PROJ_NAME} POST_BUILD
            COMMAND $ENV{Qt5_DIR}/bin/macdeployqt ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJ_NAME}.app
            COMMENT "Running macdeployqt..."
        )
    endif()
else()
    message("Environment Variable: 'Qt5_DIR' not set!")
endif()
