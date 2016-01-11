#ifndef GRAPHML_H
#define GRAPHML_H
#include <QObject>
class GraphML: public QObject{
    Q_OBJECT
public:
    static void resetID();
    enum GRAPHML_KIND{GK_NONE, GK_DATA, GK_KEY, GK_ENTITY};

    GraphML(GRAPHML_KIND kind);

    GraphML::GRAPHML_KIND getGraphMLKind() const;
    int getID();

    //Pure Virtual
    virtual QString toGraphML(int indentDepth=0)=0;
    virtual QString toString()=0;

private:
    int id;

    GRAPHML_KIND kind;
    static int _IDCounter;
};
#endif // GRAPHML_H


