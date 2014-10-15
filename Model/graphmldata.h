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
    //void setValue(QString value);

    void setValue(QString value);
    void setProtected(bool setProtected);
    bool getProtected();

    QString getValue() const;
    GraphMLKey* getKey();
    QString toGraphML(qint32 indentationLevel=0);
    QString toString();

    QStringList toStringList();
signals:
    void dataChanged(GraphMLData* data);
private:
    QString value;
    bool isProtected;
    GraphMLKey* key;

    // GraphML interface

};

#endif // GRAPHMLDATA_H
