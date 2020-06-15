#ifndef KINDS_H
#define KINDS_H
#include <QObject>

enum class GRAPHML_KIND
    {
        NONE,
        DATA,
        KEY,
        NODE,
        EDGE,
    };
Q_DECLARE_METATYPE(GRAPHML_KIND)


inline GRAPHML_KIND getGraphMLKindFromString(const QStringRef str){
    //Process the Tags
    GRAPHML_KIND kind = GRAPHML_KIND::NONE;

    if(str == "edge"){
        kind = GRAPHML_KIND::EDGE;
    }else if(str == "node"){
        kind = GRAPHML_KIND::NODE;
    }else if(str == "data"){
        kind = GRAPHML_KIND::DATA;
    }else if(str == "key"){
        kind = GRAPHML_KIND::KEY;
    }
    return kind;
};

inline QString getGraphMLKindString(GRAPHML_KIND kind){
    switch(kind){
        case GRAPHML_KIND::EDGE:
            return "edge";
        case GRAPHML_KIND::NODE:
            return "node";
        case GRAPHML_KIND::DATA:
            return "data";
        case GRAPHML_KIND::KEY:
            return "key";
        default:
            return "none";
    }
};
#endif