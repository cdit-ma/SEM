#include "enumerations.h"
#include <QDebug>
#include <QColor>
#include "ModelController/nodekinds.h"
#define SHADE_AMOUNT 130

//Auto Generated File
#include "cmakevars.h"

QString APP_URL(){
    #ifdef MEDEA_URL
        return MEDEA_URL "/";
    #else
        return "?";
    #endif
}
QString APP_VERSION(){
    #ifdef MEDEA_VERSION
        return MEDEA_VERSION;
    #else
        return "?";
    #endif
}

VIEW_ASPECT_POS GET_ASPECT_POS(VIEW_ASPECT aspect)
{
    switch(aspect){
    case VIEW_ASPECT::INTERFACES:
        return VAP_TOPLEFT;
    case VIEW_ASPECT::BEHAVIOUR:
        return VAP_TOPRIGHT;
    case VIEW_ASPECT::ASSEMBLIES:
        return VAP_BOTTOMLEFT;
    case VIEW_ASPECT::HARDWARE:
        return VAP_BOTTOMRIGHT;
    default:
        break;
    }
    return VAP_NONE;
}

QString GET_ASPECT_NAME(VIEW_ASPECT aspect)
{
    switch(aspect){
    case VIEW_ASPECT::INTERFACES:
        return "Interfaces";
    case VIEW_ASPECT::BEHAVIOUR:
        return "Behaviour";
    case VIEW_ASPECT::ASSEMBLIES:
        return "Assemblies";
    case VIEW_ASPECT::HARDWARE:
        return "Hardware";
    default:
        return "";
    }
}

VIEW_ASPECT GET_ASPECT_FROM_KIND(QString aspectKind)
{
    if(aspectKind == "InterfaceDefinitions"){
        return VIEW_ASPECT::INTERFACES;
    }else if(aspectKind == "BehaviourDefinitions"){
        return VIEW_ASPECT::BEHAVIOUR;
    }else if(aspectKind == "HardwareDefinitions"){
        return VIEW_ASPECT::HARDWARE;
    }else if(aspectKind == "AssemblyDefinitions"){
        return VIEW_ASPECT::ASSEMBLIES;
    }
    return VIEW_ASPECT::NONE;
}

QStringList GET_ASPECT_NAMES()
{
    QStringList list;
    foreach(VIEW_ASPECT aspect, GET_VIEW_ASPECTS()){
        list << GET_ASPECT_NAME(aspect);
    }
    return list;
}

/*

QColor GET_ASPECT_COLOR(VIEW_ASPECT aspect)
{
    
    switch(aspect){
    case VIEW_ASPECT::INTERFACES:
        return QColor(110,210,210);
    case VIEW_ASPECT::BEHAVIOUR:
        return QColor(254,184,126);
    case VIEW_ASPECT::ASSEMBLIES:
        return QColor(255,160,160);
    case VIEW_ASPECT::HARDWARE:
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
*/


QList<VIEW_ASPECT> GET_VIEW_ASPECTS()
{
    QList<VIEW_ASPECT> list;
    list << VIEW_ASPECT::INTERFACES << VIEW_ASPECT::BEHAVIOUR << VIEW_ASPECT::ASSEMBLIES << VIEW_ASPECT::HARDWARE;
    return list;
}


QString GET_DOCK_LABEL(DOCK_TYPE type)
{
    switch (type) {
    case PARTS_DOCK:
        return "Parts";
    case DEFINITIONC_DOCK:
        return "Definitions";
    case FUNCTIONC_DOCK:
        return "Functions";
    case HARDWARE_DOCK:
        return "Nodes";
    default:
        return "Unknown";
    }
}

/*
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
}*/

/*
QColor GET_HARDWARE_HIGHLIGHT_COLOR()
{
    //return QColor(153,153,255);
    //return QColor(0,102,102);
    //return QColor(0,76,153);
    //return QColor(102,0,51);
    return QColor(178,34,34);
}*/


RECT_VERTEX GET_ASPECT_VERTEX(VIEW_ASPECT aspect)
{
    switch(aspect){
    case VIEW_ASPECT::INTERFACES:
        return RV_TOPLEFT;
    case VIEW_ASPECT::BEHAVIOUR:
        return RV_TOPRIGHT;
    case VIEW_ASPECT::ASSEMBLIES:
        return RV_BOTTOMLEFT;
    case VIEW_ASPECT::HARDWARE:
        return RV_BOTTOMRIGHT;
    default:
        break;
    }
    return RV_NONE;
}

QString GET_ASPECT_ICON(VIEW_ASPECT aspect)
{
    switch(aspect){
    case VIEW_ASPECT::INTERFACES:
        return "InterfaceDefinitions";
    case VIEW_ASPECT::BEHAVIOUR:
        return "BehaviourDefinitions";
    case VIEW_ASPECT::ASSEMBLIES:
        return "AssemblyDefinitions";
    case VIEW_ASPECT::HARDWARE:
        return "HardwareDefinitions";
    default:
        return "";
    }
}
