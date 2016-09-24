#include "enumerations.h"
#include <QDebug>

#define SHADE_AMOUNT 130

VIEW_ASPECT_POS GET_ASPECT_POS(VIEW_ASPECT aspect)
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

QString GET_ASPECT_NAME(VIEW_ASPECT aspect)
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

VIEW_ASPECT GET_ASPECT_FROM_KIND(QString aspectKind)
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

QStringList GET_ASPECT_NAMES()
{
    QStringList list;
    foreach(VIEW_ASPECT aspect, GET_VIEW_ASPECTS()){
        list << GET_ASPECT_NAME(aspect);
    }
    return list;
}

QColor GET_ASPECT_COLOR(VIEW_ASPECT aspect)
{
    /*
     * THESE ARE THE ORIGINAL COLOURS
     */
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
}


QColor GET_VIEW_COLOR(VIEW_THEME theme, COLOR_SHADE shade)
{
    // TODO - Use this function to alter the widget colours when the theme's changed.
    QColor color = Qt::white;

    switch (theme) {
    case VT_LIGHT_THEME:
        // not using this theme at the moment
        color = QColor(250,250,250);
        break;
    case VT_NORMAL_THEME:
        color = QColor(170,170,170);
        break;
    case VT_DARK_THEME:
        color = QColor(70,70,70);
        break;
    default:
        return color;
    }

    switch (shade) {
    case LIGHTER_SHADE:
        return color.lighter(SHADE_AMOUNT);
    case NORMAL_SHADE:
        return color;
    case DARKER_SHADE:
        return color.darker(SHADE_AMOUNT);
    default:
        return color;
    }
}

QColor GET_INVERT_COLOR(VIEW_THEME theme){

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



QList<VIEW_ASPECT> GET_VIEW_ASPECTS()
{
    QList<VIEW_ASPECT> list;
    list << VA_INTERFACES << VA_BEHAVIOUR << VA_ASSEMBLIES << VA_HARDWARE;
    return list;
}


QString GET_DOCK_LABEL(DOCK_TYPE type)
{
    switch (type) {
    case PARTS_DOCK:
        return "Parts";
    case DEFINITIONS_DOCK:
        return "Definitions";
    case FUNCTIONS_DOCK:
        return "Functions";
    case HARDWARE_DOCK:
        return "Nodes";
    default:
        return "Unknown";
    }
}


QString GET_COLOR_STRING(QColor color, COLOR_SHADE shade)
{
    if (!color.isValid()) {
        return "white";
    }

    switch (shade) {
    case LIGHTER_SHADE:
        color = color.lighter(SHADE_AMOUNT);
    case NORMAL_SHADE:
        break;
    case DARKER_SHADE:
        color = color.darker(SHADE_AMOUNT);
    default:
        break;
    }

    QString colorStr = "rgba(";
    colorStr += QString::number(color.red()) + ",";
    colorStr += QString::number(color.green()) + ",";
    colorStr += QString::number(color.blue()) + ",";
    colorStr += QString::number(color.alpha()) + ")";
    return colorStr;
}


QString GET_VIEW_COLOR_STRING(VIEW_THEME theme, COLOR_SHADE shade)
{
    QColor viewColor = GET_VIEW_COLOR(theme, shade);
    return GET_COLOR_STRING(viewColor);
}


QColor GET_TEXT_COLOR(VIEW_THEME theme, bool invert)
{
    if (invert) {
        /*
        switch (theme) {
        case VT_LIGHT_THEME:
        case VT_NORMAL_THEME:
            return Qt::white;
        case VT_DARK_THEME:
            return Qt::black;
        default:
            return Qt::blue;
        }
        */
        return Qt::black;
    } else {
        switch (theme) {
        case VT_LIGHT_THEME:
        case VT_NORMAL_THEME:
            return Qt::black;
        case VT_DARK_THEME:
            return Qt::white;
        default:
            return Qt::red;
        }
    }
}

QHash<QString, QString> GET_SETTINGS_TOOLTIPS_HASH()
{
    QHash<QString, QString> tooltips;

    tooltips[LOG_DEBUGGING] = "Toggles logging of diagnostic information during MEDEA usage.";
    tooltips[THREAD_LIMIT] = "Changes the number of threads used for generation of model code in local deployment.";
    tooltips[CUTS_CONFIGURE_PATH] = "The absolute file path to the CUTS configure .bat/.sh script";
    tooltips[DEFAULT_DIR_PATH] = "The default directory used for the Open/Save prompt in MEDEA.";
    tooltips[SCREENSHOT_PATH] = "The default directory used to store screenshots taken in MEDEA.";
    tooltips[SCREENSHOT_QUALITY] = "Controls the resolution of the screenshots taken in MEDEA, the larger the number the higher the resolution.";

    tooltips[WINDOW_X] = "The initial X screen position of the main MEDEA window.";
    tooltips[WINDOW_Y] = "The initial Y screen position of the main MEDEA window.";
    tooltips[WINDOW_W] = "The initial width of the main MEDEA window.";
    tooltips[WINDOW_H] = "The initial height of the main MEDEA window.";
    tooltips[WINDOW_MAX_STATE] = "Controls whether MEDEA starts in maximized window state.";
    tooltips[WINDOW_FULL_SCREEN] = "Controls whether MEDEA starts in full-screen window state.";
    tooltips[WINDOW_STORE_SETTINGS] = "Controls whether MEDEA stores the previous window settings on exit.";

    tooltips[AUTO_CENTER_VIEW] = "Toggles auto centralization of the visible aspects upon aspects being changed.";
    tooltips[SELECT_ON_CREATION] = "Toggles auto selection of newly constructed entities.";
    tooltips[ZOOM_ANCHOR_ON_MOUSE] = "Switches between scrolling in on the position the cursor is over or the center of the viewport.";
    tooltips[TOGGLE_GRID] = "Toggles the visual grid drawn during movement of child items.";
    tooltips[SHOW_MANAGEMENT_COMPONENTS] = "Toggles the visibility of Management Components in the Assembly Definitions aspect.";
    tooltips[SHOW_LOCAL_NODE] = "Toggles the visibility of the localhost Hardware Node in the Hardware Definitions aspect.";

    tooltips[ASPECT_I] = "Toggles the initial visibility of the Interface Definitions Aspect";
    tooltips[ASPECT_B] = "Toggles the initial visibility of the Behaviour Definitions Aspect";
    tooltips[ASPECT_A] = "Toggles the initial visibility of the Assembly Definitions Aspect";
    tooltips[ASPECT_H] = "Toggles the initial visibility of the Hardware Definitions Aspect";
    tooltips[ASPECT_I_COLOR] = "Sets the background color of the Interface Definitions Aspect";
    tooltips[ASPECT_B_COLOR] = "Sets the background color of the Behaviour Definitions Aspect";
    tooltips[ASPECT_A_COLOR] = "Sets the background color of the Assembly Definitions Aspect";
    tooltips[ASPECT_H_COLOR] = "Sets the background color of the Hardware Definitions Aspect";

    tooltips[TOOLBAR_VISIBLE] = "Toggles the visibility of the toolbar.";
    tooltips[TOOLBAR_CONTEXT] = "Sets the visibility of the context toolbar toolbutton.";
    tooltips[TOOLBAR_UNDO] = "Sets the visibility of the undo toolbar toolbutton.";
    tooltips[TOOLBAR_REDO] = "Sets the visibility of the redo toolbar toolbutton.";
    tooltips[TOOLBAR_CUT] = "Sets the visibility of the cut toolbar toolbutton.";
    tooltips[TOOLBAR_COPY] = "Sets the visibility of the copy toolbar toolbutton.";
    tooltips[TOOLBAR_PASTE] = "Sets the visibility of the paste toolbar toolbutton.";
    tooltips[TOOLBAR_REPLICATE] = "Sets the visibility of the replicate toolbar toolbutton.";
    tooltips[TOOLBAR_DELETE_ENTITIES] = "Sets the visibility of the delete toolbar toolbutton.";
    tooltips[TOOLBAR_POPUP_SUBVIEW] = "Sets the visibility of the popup subview toolbar toolbutton.";
    tooltips[TOOLBAR_GRID_LINES] = "Sets the visibility of the toggle gridlines toolbar toolbutton.";
    tooltips[TOOLBAR_FIT_TO_SCREEN] = "Sets the visibility of the fit to screen toolbar toolbutton.";
    tooltips[TOOLBAR_CENTER_ON_ENTITY] = "Sets the visibility of the fit to screen toolbar toolbutton.";
    tooltips[TOOLBAR_ZOOM_TO_FIT] = "Sets the visibility of the zoom to fit toolbar toolbutton.";
    tooltips[TOOLBAR_SORT] = "Sets the visibility of the sort toolbar toolbutton.";
    tooltips[TOOLBAR_VERT_ALIGN] = "Sets the visibility of the vertical alignment toolbar toolbutton.";
    tooltips[TOOLBAR_HORIZ_ALIGN] = "Sets the visibility of the horizontal alignment toolbar toolbutton.";

    tooltips[JENKINS_URL] = "Sets the URL of the Jenkins Server to use for Jenkins Deployment of Models.";
    tooltips[JENKINS_USER] = "Set the username used to authenticate on the Jenkins server.";
    tooltips[JENKINS_PASS] = "Sets the password used to authenticate on the Jenkins server.";
    tooltips[JENKINS_JOB] = "Sets the jobname used by the Jenkins Server to execute Models.";
    tooltips[JENKINS_TOKEN] = "Sets the API token used to authenticate API calls on the Jenkins Server.";


    tooltips[THEME_BG_COLOR] = "Sets the background color in MEDEA.";
    tooltips[THEME_BG_ALT_COLOR] = "Sets the alternate background color, used for menus, in MEDEA.";
    tooltips[THEME_DISABLED_BG_COLOR] = "Sets the background color, used for disabled items, in MEDEA.";
    tooltips[THEME_HIGHLIGHT_COLOR] = "Sets the color, used for highlight menus and items, in MEDEA.";
    tooltips[THEME_MENU_TEXT_COLOR] = "Sets the color for text in MEDEA.";
    tooltips[THEME_MENU_TEXT_DISABLED_COLOR] = "Sets the color for disabled text in MEDEA.";
    tooltips[THEME_MENU_TEXT_SELECTED_COLOR] = "Sets the color for selected text in MEDEA.";
    tooltips[THEME_MENU_ICON_COLOR] = "Sets the color tint used for icons in MEDEA.";
    tooltips[THEME_MENU_ICON_DISABLED_COLOR] = "Sets the color tint used for disabled icons in MEDEA.";
    tooltips[THEME_MENU_ICON_SELECTED_COLOR] = "Sets the color tint used for selected icons in MEDEA.";

    return tooltips;
}

QHash<QString, QString> GET_SETTINGS_GROUP_HASH()
{
    QHash<QString, QString> groups;
    groups[THEME_BG_COLOR] = "Colors";
    groups[THEME_BG_ALT_COLOR] = "Colors";
    groups[THEME_DISABLED_BG_COLOR] = "Disabled_Colors";
    groups[THEME_HIGHLIGHT_COLOR] = "Selected_Colors";

    groups[THEME_MENU_ICON_COLOR] = "Colors";
    groups[THEME_MENU_ICON_DISABLED_COLOR] = "Disabled_Colors";
    groups[THEME_MENU_ICON_SELECTED_COLOR] = "Selected_Colors";

    groups[THEME_MENU_TEXT_COLOR] = "Colors";
    groups[THEME_MENU_TEXT_DISABLED_COLOR] = "Disabled_Colors";
    groups[THEME_MENU_TEXT_SELECTED_COLOR] = "Selected_Colors";


    groups[JENKINS_URL] = "Server";
    groups[JENKINS_JOB] = "Server";

    groups[JENKINS_USER] = "User";
    groups[JENKINS_PASS] = "User";
    groups[JENKINS_TOKEN] = "User";

    groups[TOOLBAR_VISIBLE] = "Visible_Buttons";
    groups[TOOLBAR_CONTEXT] = "Visible_Buttons";
    groups[TOOLBAR_UNDO] = "Visible_Buttons";
    groups[TOOLBAR_REDO] = "Visible_Buttons";
    groups[TOOLBAR_CUT] = "Visible_Buttons";
    groups[TOOLBAR_COPY] = "Visible_Buttons";
    groups[TOOLBAR_PASTE] = "Visible_Buttons";
    groups[TOOLBAR_REPLICATE] = "Visible_Buttons";
    groups[TOOLBAR_DELETE_ENTITIES] = "Visible_Buttons";
    groups[TOOLBAR_POPUP_SUBVIEW] = "Visible_Buttons";
    groups[TOOLBAR_GRID_LINES] = "Visible_Buttons";
    groups[TOOLBAR_FIT_TO_SCREEN] = "Visible_Buttons";
    groups[TOOLBAR_CENTER_ON_ENTITY] = "Visible_Buttons";
    groups[TOOLBAR_ZOOM_TO_FIT] = "Visible_Buttons";
    groups[TOOLBAR_SORT] = "Visible_Buttons";
    groups[TOOLBAR_VERT_ALIGN] = "Visible_Buttons";
    groups[TOOLBAR_HORIZ_ALIGN] = "Visible_Buttons";
    groups[TOOLBAR_BACK] = "Visible_Buttons";
    groups[TOOLBAR_FORWARD] = "Visible_Buttons";

    groups[ASPECT_I] = "0-Default_Aspects_Visible";
    groups[ASPECT_B] = "0-Default_Aspects_Visible";
    groups[ASPECT_A] = "0-Default_Aspects_Visible";
    groups[ASPECT_H] = "0-Default_Aspects_Visible";

    groups[ASPECT_I_COLOR] = "1-Aspect_Background_Color";
    groups[ASPECT_B_COLOR] = "1-Aspect_Background_Color";
    groups[ASPECT_A_COLOR] = "1-Aspect_Background_Color";
    groups[ASPECT_H_COLOR] = "1-Aspect_Background_Color";

    groups[SHOW_LOCAL_NODE] = "Show_Entities";
    groups[SHOW_MANAGEMENT_COMPONENTS] = "Show_Entities";

    groups[WINDOW_X] = "Geometry";
    groups[WINDOW_Y] = "Geometry";
    groups[WINDOW_W] = "Geometry";
    groups[WINDOW_H] = "Geometry";

    groups[WINDOW_MAX_STATE] = "State";
    groups[WINDOW_FULL_SCREEN] = "State";
    groups[WINDOW_STORE_SETTINGS] = "State";

    groups[SCREENSHOT_PATH] = "Screenshot";
    groups[SCREENSHOT_QUALITY] = "Screenshot";

    groups[CUTS_CONFIGURE_PATH] = "CUTS";
    return groups;
}


QColor GET_HARDWARE_HIGHLIGHT_COLOR()
{
    //return QColor(153,153,255);
    //return QColor(0,102,102);
    //return QColor(0,76,153);
    //return QColor(102,0,51);
    return QColor(178,34,34);
}


RECT_VERTEX GET_ASPECT_VERTEX(VIEW_ASPECT aspect)
{
    switch(aspect){
    case VA_INTERFACES:
        return RV_TOPLEFT;
    case VA_BEHAVIOUR:
        return RV_TOPRIGHT;
    case VA_ASSEMBLIES:
        return RV_BOTTOMLEFT;
    case VA_HARDWARE:
        return RV_BOTTOMRIGHT;
    default:
        break;
    }
    return RV_NONE;
}


QList<NOTIFICATION_TYPE> getNotificationTypes()
{
    QList<NOTIFICATION_TYPE> list;
    list << NT_INFO;
    list << NT_WARNING;
    list << NT_ERROR;
    list << NT_CRITICAL;
    return list;
}


QString GET_NOTIFICATION_TYPE_STRING(NOTIFICATION_TYPE type)
{
    switch (type) {
    case NT_INFO:
        return "Information";
    case NT_WARNING:
        return "Warning";
    case NT_ERROR:
        return "Error";
    case NT_CRITICAL:
        return "Critical";
    default:
        return "";
    }
}
