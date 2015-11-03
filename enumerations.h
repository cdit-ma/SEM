#ifndef ENUMERATIONS_H
#define ENUMERATIONS_H

#include <QStringList>
#include <QString>
#include <QColor>

enum VIEW_ASPECT{VA_NONE, VA_INTERFACES, VA_BEHAVIOUR, VA_ASSEMBLIES, VA_HARDWARE};
enum VIEW_ASPECT_POS{VAP_NONE, VAP_TOPLEFT, VAP_TOPRIGHT, VAP_BOTTOMRIGHT, VAP_BOTTOMLEFT};

const static QStringList VIEW_ASPECT_LIST = QStringList() << "InterfaceDefinitions" << "BehaviourDefinitions" << "AssemblyDefinitions" << "HardwareDefinitions";



static VIEW_ASPECT_POS getAspectPosition(VIEW_ASPECT aspect){
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

static QString getAspectName(VIEW_ASPECT aspect){
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

static VIEW_ASPECT getViewAspectFromAspectNodeKind(QString nodeKind){
    if(nodeKind == "InterfaceDefinitions"){
        return VA_INTERFACES;
    }else if(nodeKind == "BehaviourDefinitions"){
        return VA_BEHAVIOUR;
    }else if(nodeKind == "HardwareDefinitions"){
        return VA_HARDWARE;
    }else if(nodeKind == "AssemblyDefinitions"){
        return VA_ASSEMBLIES;
    }
    return VA_NONE;
}

static QColor getAspectColor(VIEW_ASPECT aspect){
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

#endif // ENUMERATIONS_H

