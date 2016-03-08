#ifndef ENUMERATIONS_H
#define ENUMERATIONS_H

#include <QStringList>
#include <QString>
#include <QColor>
#include <QList>


enum ERROR_TYPE{ET_OKAY, ET_WARNING, ET_CRITICAL};
enum VIEW_STATE{VS_NONE, VS_SELECTED, VS_RUBBERBAND, VS_RUBBERBANDING, VS_CONNECT, VS_MOVING, VS_RESIZING, VS_CONNECTING, VS_PAN, VS_PANNING};
enum VIEW_ASPECT{VA_NONE, VA_INTERFACES, VA_BEHAVIOUR, VA_ASSEMBLIES, VA_HARDWARE};
enum VIEW_ASPECT_POS{VAP_NONE, VAP_TOPLEFT, VAP_TOPRIGHT, VAP_BOTTOMRIGHT, VAP_BOTTOMLEFT};

enum VIEW_THEME{VT_NO_THEME, VT_LIGHT_THEME, VT_NORMAL_THEME, VT_DARK_THEME};
enum DOCK_TYPE{UNKNOWN_DOCK, PARTS_DOCK, DEFINITIONS_DOCK, FUNCTIONS_DOCK, HARDWARE_DOCK};

enum NODE_CLASS{NC_NONE, NC_DEPLOYMENT_DEFINITIONS, NC_WORKER_DEFINITIONS};

enum COLOR_SHADE{LIGHTER_SHADE, NORMAL_SHADE, DARKER_SHADE};

#define GENERAL_SETTINGS "00-General"
#define LOG_DEBUGGING "00-01-Log_Debug_Information"
#define THREAD_LIMIT "00-02-Thread_Limit"
#define CUTS_CONFIGURE_PATH "00-03-CUTS_Configure_Script_Path"
#define DEFAULT_DIR_PATH "00-04-Default_Open_Path"
#define SCREENSHOT_PATH "00-05-Default_Screenshot_Path"
#define SCREENSHOT_QUALITY "00-06-Screenshot_Quality"

#define WINDOW_SETTINGS "01-Window"
#define WINDOW_X "01-01-Position_X"
#define WINDOW_Y "01-02-Position_Y"
#define WINDOW_W "01-03-Width"
#define WINDOW_H "01-04-Height"
#define WINDOW_MAX_STATE "01-05-Maximized"
#define WINDOW_FULL_SCREEN "01-06-Full_Screen_Mode"
#define WINDOW_STORE_SETTINGS "01-07-Save_Position_On_Exit"

#define VIEW_SETTINGS "02-View"
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

#define TOOLBAR_SETTINGS "05-Toolbar"
#define TOOLBAR_VISIBLE "05-00-00-Hide_Toolbar"
#define TOOLBAR_EXPANDED "05-00-01-Expand_Toolbar"
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

#define JENKINS_SETTINGS "06-Jenkins"
#define JENKINS_URL "06-01-URL"
#define JENKINS_USER "06-02-Username"
#define JENKINS_PASS "06-03-Password"
#define JENKINS_JOB "06-04-MEDEA_Jobname"
#define JENKINS_TOKEN "06-05-API_Token"

#define THEME_SETTINGS "07-Theme"
#define THEME_SET_LIGHT_THEME "07-00-Set_Light_Theme"
#define THEME_SET_DARK_THEME "07-00-Set_Dark_Theme"
#define THEME_BG_COLOR "07-01-Background_Color"
#define THEME_BG_ALT_COLOR "07-02-Alternate_Background_Color"
#define THEME_HIGHLIGHT_COLOR "07-03-Highlight_Color"
#define THEME_MENU_TEXT_COLOR "07-04-Menu_Text_Color"
#define THEME_MENU_TEXT_DISABLED_COLOR "07-05-Menu_Text_Disabled_Color"
#define THEME_MENU_TEXT_SELECTED_COLOR "07-06-Menu_Text_Selected_Color"
#define THEME_MENU_ICON_COLOR "07-07-Menu_Icon_Tint_Color"
#define THEME_MENU_ICON_DISABLED_COLOR "07-08-Menu_Icon_Disabled_Tint_Color"
#define THEME_MENU_ICON_SELECTED_COLOR "07-09-Menu_Icon_Selected_Tint_Color"
#define THEME_ICON_TINT_COLOR "07-10-Default_Icon_Tint_Color"

#define MINIMUM_TEXT_SIZE 6


extern QList<VIEW_ASPECT> GET_VIEW_ASPECTS();
extern VIEW_ASPECT_POS GET_ASPECT_POS(VIEW_ASPECT aspect);
extern QString GET_ASPECT_NAME(VIEW_ASPECT aspect);
extern VIEW_ASPECT GET_ASPECT_FROM_KIND(QString aspectKind);
extern QStringList GET_ASPECT_NAMES();

extern QColor GET_INVERT_COLOR(VIEW_THEME theme);
extern QColor GET_TEXT_COLOR(VIEW_THEME theme, bool invert = false);
extern QColor GET_ASPECT_COLOR(VIEW_ASPECT aspect);
extern QColor GET_VIEW_COLOR(VIEW_THEME theme, COLOR_SHADE shade = NORMAL_SHADE);
extern QString GET_VIEW_COLOR_STRING(VIEW_THEME theme, COLOR_SHADE shade = NORMAL_SHADE);
extern QString GET_COLOR_STRING(QColor color, COLOR_SHADE shade = NORMAL_SHADE);
extern QString GET_DOCK_LABEL(DOCK_TYPE type);


#endif // ENUMERATIONS_H
