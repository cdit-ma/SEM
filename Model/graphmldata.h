#ifndef GRAPHMLDATA_H
#define GRAPHMLDATA_H

#include "graphmlkey.h"

#include <QStringList>
#include <QString>

class GraphMLData: public GraphML{
    Q_OBJECT
public:
    GraphMLData(GraphMLKey* key, QString value);
    ~GraphMLData();
    void setParent(GraphML* parent);
    GraphML* getParent();

    void setValue(QString value);
    void setProtected(bool setProtected);
    bool isProtected();

    void setParentData(GraphMLData* data);
    void unsetParentData();
    GraphMLData* getParentData();

    void bindData(GraphMLData* data);
    void unbindData(GraphMLData* data);
    QList<GraphMLData*> getBoundData();

    QString getValue() const;
    GraphMLKey* getKey();
    QString getKeyName();
    QString toGraphML(qint32 indentationLevel=0);
    QString toString();

    QStringList toStringList();
    QStringList getBoundIDS();
signals:
    void dataChanged(GraphMLData* data);
    void valueChanged(QString value, QString key="");
private:
    GraphML* Parent;

    GraphMLData* parentData;
    QList<GraphMLData*> childData;
    QString value;
    QString keyName;
    QString type;
    bool protectedData;

    GraphMLKey* key;

    // GraphML interface

};

#endif // GRAPHMLDATA_H
