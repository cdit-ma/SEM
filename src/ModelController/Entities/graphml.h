#ifndef GRAPHML_H
#define GRAPHML_H
#include <QObject>
#include "../kinds.h"
class GraphML: public QObject{
    Q_OBJECT
public:
    static void resetID();

    GraphML(GRAPHML_KIND kind);

    GRAPHML_KIND getGraphMLKind() const;
    void setID();
    int getID() const;

    //Pure Virtual
    virtual QString toGraphML(int indentDepth=0)=0;
    virtual QString toString()=0;

private:
    int id;

    GRAPHML_KIND kind;
    static int _IDCounter;
};
#endif // GRAPHML_H


