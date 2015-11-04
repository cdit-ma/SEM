#ifndef GRAPHMLDATA_H
#define GRAPHMLDATA_H

#include "graphmlkey.h"

#include <QStringList>
#include <QString>

class GraphMLData: public GraphML{
    Q_OBJECT
public:
    GraphMLData(GraphMLKey* key, QString value="", bool isProtected=false);
    ~GraphMLData();
    void setParent(GraphML* parent);
    GraphML* getParent();

    void clearValue();
    void setValue(QString value);
    void setProtected(bool setProtected);
    bool isProtected();

    void setParentData(GraphMLData* data, bool protect = true);
    void unsetParentData();
    GraphMLData* getParentData();


    void bindData(GraphMLData* data, bool protect = true);
    void unbindData(GraphMLData* data);
    QList<GraphMLData*> getBoundData();

    bool gotDoubleValue() const;
    qreal getDoubleValue() const;
    QString getValue() const;
    GraphMLKey* getKey();
    QString getKeyName();
    QString toGraphML(qint32 indentationLevel=0);
    QString toString();
    QStringList getValidValues(QString kind);
    QStringList toStringList();
    QList<int> getBoundIDS();
signals:
    void dataChanged(GraphMLData* data);
    void valueChanged(QString value, QString key="");
private:
    void updateDouble();
    GraphML* Parent;

    GraphMLData* parentData;
    QList<GraphMLData*> childData;
    QString value;
    QString keyName;
    QString type;
    qreal valueDbl;
    bool hasDbl;
    bool protectedData;

    GraphMLKey* key;

    // GraphML interface

};

#endif // GRAPHMLDATA_H
