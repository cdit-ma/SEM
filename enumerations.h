#ifndef ENUMERATIONS_H
#define ENUMERATIONS_H

#include <QStringList>
#include <QString>
#include <QColor>

enum VIEW_ASPECT{VA_NONE, VA_INTERFACES, VA_BEHAVIOUR, VA_ASSEMBLIES, VA_HARDWARE};
enum VIEW_ASPECT_POS{VAP_NONE, VAP_TOPLEFT, VAP_TOPRIGHT, VAP_BOTTOMRIGHT, VAP_BOTTOMLEFT};
const static QList<VIEW_ASPECT> VIEW_ASPECTS = QList<VIEW_ASPECT>() << VA_INTERFACES << VA_BEHAVIOUR << VA_ASSEMBLIES << VA_HARDWARE;

enum VIEW_THEME{VT_NO_THEME, VT_LIGHT_THEME, VT_NORMAL_THEME, VT_DARK_THEME};
enum DOCK_TYPE{PARTS_DOCK, DEFINITIONS_DOCK, FUNCTIONS_DOCK, HARDWARE_DOCK};

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

static QColor GET_ASPECT_COLOR(VIEW_ASPECT aspect)
{
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
        break;
    }
    return Qt::white;
}

static QStringList GET_ASPECT_NAMES()
{
    QStringList list;
    foreach(VIEW_ASPECT aspect, VIEW_ASPECTS){
        list << GET_ASPECT_NAME(aspect);
    }
    return list;
}


#endif // ENUMERATIONS_H
