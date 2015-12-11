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


#endif // ENUMERATIONS_H
