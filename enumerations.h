#ifndef ENUMERATIONS_H
#define ENUMERATIONS_H

#include <QStringList>
#include <QString>
#include <QColor>

enum VIEW_STATE{VS_NONE, VS_SELECTED, VS_RUBBERBAND, VS_RUBBERBANDING, VS_CONNECT, VS_MOVING, VS_RESIZING, VS_CONNECTING, VS_PAN, VS_PANNING};
enum VIEW_ASPECT{VA_NONE, VA_INTERFACES, VA_BEHAVIOUR, VA_ASSEMBLIES, VA_HARDWARE};
enum VIEW_ASPECT_POS{VAP_NONE, VAP_TOPLEFT, VAP_TOPRIGHT, VAP_BOTTOMRIGHT, VAP_BOTTOMLEFT};
const static QList<VIEW_ASPECT> VIEW_ASPECTS = QList<VIEW_ASPECT>() << VA_INTERFACES << VA_BEHAVIOUR << VA_ASSEMBLIES << VA_HARDWARE;

enum VIEW_THEME{VT_NO_THEME, VT_LIGHT_THEME, VT_NORMAL_THEME, VT_DARK_THEME};
enum DOCK_TYPE{UNKNOWN_DOCK, PARTS_DOCK, DEFINITIONS_DOCK, FUNCTIONS_DOCK, HARDWARE_DOCK};

enum NODE_CLASS{NC_NONE, NC_DEPLOYMENT_DEFINITIONS, NC_WORKER_DEFINITIONS};
enum EDGE_CLASS{EC_NONE};

#define LOG_DEBUGGING "00-01-Log_Debug_Information"
#define THREAD_LIMIT "00-02-Thread_Limit"
#define CUTS_CONFIGURE_PATH "00-03-CUTS_Configure_Script_Path"
#define DEFAULT_DIR_PATH "00-04-Default_Open_Path"

#define WINDOW_SETTINGS "01-Window_Settings"
#define WINDOW_X "01-01-Position_X"
#define WINDOW_Y "01-02-Position_Y"
#define WINDOW_W "01-03-Width"
#define WINDOW_H "01-04-Height"
#define WINDOW_MAX_STATE "01-05-Maximized"
#define WINDOW_FULL_SCREEN "01-06-Full_Screen_Mode"
#define WINDOW_STORE_SETTINGS "01-07-Save_Position_On_Exit"



#define AUTO_CENTER_VIEW "02-01-Auto_Center_View"
#define SELECT_ON_CREATION "02-02-Select_Entity_On_Creation"
#define ZOOM_ANCHOR_ON_MOUSE "02-03-Zoom_View_Under_Mouse"
#define TOGGLE_GRID "02-04-Toggle_Grid_Lines"
#define SHOW_MANAGEMENT_COMPONENTS "02-05-Show_Management_Components"
#define SHOW_LOCAL_NODE "02-06-Show_Local_Node"
#define DARK_THEME_ON "02-07-Dark_Theme_On"

#define ASPECT_SETTINGS "03-Default_Aspects"
#define ASPECT_I "03-01-Interfaces"
#define ASPECT_B "03-02-Behaviour"
#define ASPECT_A "03-03-Assemblies"
#define ASPECT_H "03-04-Hardware"
#define DOCK_VISIBLE "04-01-Hide_Dock"
#define TOOLBAR_VISIBLE "05-00-00-Hide_Toolbar"
#define TOOLBAR_EXPANDED "05-00-01-Expand_Toolbar"

#define JENKINS_URL "06-01-URL"
#define JENKINS_USER "06-02-Username"
#define JENKINS_PASS "06-03-Password"
#define JENKINS_JOB "06-04-MEDEA_Jobname"
#define JENKINS_TOKEN "06-05-API_Token"


#define TOOLBAR_SETTINGS "05-Toolbar_Settings"
#define TOOLBAR_CONTEXT "05-01-Context_Toolbar"
#define TOOLBAR_UNDO "05-02-Undo"
#define TOOLBAR_REDO "05-03-Redo"
#define TOOLBAR_CUT "05-04-Cut"
#define TOOLBAR_COPY "05-05-Copy"
#define TOOLBAR_PASTE "05-06-Paste"
#define TOOLBAR_REPLICATE "05-07-Replicate"
#define TOOLBAR_DELETE_ENTITIES "05-08-Delete_Entities"
#define TOOLBAR_POPUP_SUBVIEW "05-09-Popup_SubView"
#define TOOLBAR_GRID_LINES "05-10-Grid_Lines"
#define TOOLBAR_FIT_TO_SCREEN "05-11-Fit_To_Screen"
#define TOOLBAR_CENTER_ON_ENTITY "05-12-Center_On_Entity"
#define TOOLBAR_ZOOM_TO_FIT "05-13-Zoom_To_Fit"
#define TOOLBAR_SORT "05-14-Sort"
#define TOOLBAR_VERT_ALIGN "05-15-Vertical_Align_Entities"
#define TOOLBAR_HORIZ_ALIGN "05-16-Horizontal_Align_Entities"
#define TOOLBAR_BACK "05-17-Back"
#define TOOLBAR_FORWARD "05-18-Forward"

#define MINIMUM_TEXT_SIZE 6

static VIEW_ASPECT_POS GET_ASPECT_POS(VIEW_ASPECT aspect)
{
    switch(aspect){
    case VA_INTERFACES:
        return VAP_TOPLEFT;
    case VA_BEHAVIOUR:
        return VAP_TOPRIGHT;
    case VA_ASSEMBLIES:
        return VAP_BOTTOMLEFT;
    case VA_HARDWARE:
        return VAP_BOTTOMRIGHT;
    default:
        break;
    }
    return VAP_NONE;
}

static QString GET_ASPECT_NAME(VIEW_ASPECT aspect)
{
    switch(aspect){
    case VA_INTERFACES:
        return "Interfaces";
    case VA_BEHAVIOUR:
        return "Behaviour";
    case VA_ASSEMBLIES:
        return "Assemblies";
    case VA_HARDWARE:
        return "Hardware";
    default:
        break;
    }
    return "";
}

static VIEW_ASPECT GET_ASPECT_FROM_KIND(QString aspectKind)
{
    if(aspectKind == "InterfaceDefinitions"){
        return VA_INTERFACES;
    }else if(aspectKind == "BehaviourDefinitions"){
        return VA_BEHAVIOUR;
    }else if(aspectKind == "HardwareDefinitions"){
        return VA_HARDWARE;
    }else if(aspectKind == "AssemblyDefinitions"){
        return VA_ASSEMBLIES;
    }
    return VA_NONE;
}

static QStringList GET_ASPECT_NAMES()
{
    QStringList list;
    foreach(VIEW_ASPECT aspect, VIEW_ASPECTS){
        list << GET_ASPECT_NAME(aspect);
    }
    return list;
}

static QColor GET_ASPECT_COLOR(VIEW_ASPECT aspect)
{
    /*
     * THESE ARE THE ORIGINAL COLOURS
     */
    //*
    switch(aspect){
    case VA_INTERFACES:
        return QColor(110,210,210);
    case VA_BEHAVIOUR:
        return QColor(254,184,126);
    case VA_ASSEMBLIES:
        return QColor(255,160,160);
    case VA_HARDWARE:
        return QColor(110,170,220);
    default:
        return Qt::white;
    }
    //*/
    /*
    switch(aspect){
    case VA_INTERFACES:
        return QColor(87,139,139);
    case VA_BEHAVIOUR:
        return QColor(231,203,146);
    case VA_ASSEMBLIES:
        return QColor(231,184,146);
    case VA_HARDWARE:
        return QColor(104,119,155);
    default:
        return Qt::white;
    }
    */
    /*
    switch(aspect){
    case VA_INTERFACES:
        return QColor(102,153,153);
    case VA_BEHAVIOUR:
        return QColor(255,209,170);
    case VA_ASSEMBLIES:
        return QColor(255,170,170);
    case VA_HARDWARE:
        return QColor(136,204,136);
    default:
        return Qt::white;
    }
    */
    /*
    switch(aspect){
    case VA_INTERFACES:
        return QColor(111,182,150);
    case VA_BEHAVIOUR:
        return QColor(255,209,156);
    case VA_ASSEMBLIES:
        return QColor(255,183,156);
    case VA_HARDWARE:
        return QColor(106,148,168);
    default:
        return Qt::white;
    }
    */
    /*
    switch(aspect){
    case VA_INTERFACES:
        return QColor(71,154,116);
    case VA_BEHAVIOUR:
        return QColor(223,167,103);
    case VA_ASSEMBLIES:
        return QColor(223,135,103);
    case VA_HARDWARE:
        return QColor(69,117,141);
    default:
        return Qt::white;
    }
    */
    /*
    switch(aspect){
    case VA_INTERFACES:
        return QColor(58,161,117);
    case VA_BEHAVIOUR:
        return QColor(235,165,85);
    case VA_ASSEMBLIES:
        return QColor(235,127,85);
    case VA_HARDWARE:
        return QColor(58,119,149);
    default:
        return Qt::white;
    }
    */
    /*
     * This is preferred.
     */
    /*
    switch(aspect){
    case VA_INTERFACES:
        return QColor(24,148,184);
    case VA_BEHAVIOUR:
        return QColor(110,110,110);
    case VA_ASSEMBLIES:
        return QColor(175,175,175);
    case VA_HARDWARE:
        return QColor(207,107,100);
    default:
        return Qt::white;
    }
    */
}


static QColor GET_VIEW_COLOR(VIEW_THEME theme)
{
    // TODO - Use this function to alter the widget colours when the theme's changed.
    switch (theme) {
    case VT_LIGHT_THEME:
        // not using this theme at the moment
    case VT_NORMAL_THEME:
        return QColor(170,170,170);
    case VT_DARK_THEME:
        return QColor(70,70,70);
        //return QColor(50,50,50);
    default:
        return Qt::white;
    }
}

static QColor GET_INVERT_COLOR(VIEW_THEME theme){

    switch (theme) {
    case VT_LIGHT_THEME:
        // not using this theme at the moment
    case VT_NORMAL_THEME:
        return GET_VIEW_COLOR(VT_DARK_THEME);
    case VT_DARK_THEME:
        return GET_VIEW_COLOR(VT_NORMAL_THEME);
    default:
        return Qt::white;
    }

}


#endif // ENUMERATIONS_H
