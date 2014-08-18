#ifndef GRAPHMLDATA_H
#define GRAPHMLDATA_H

#include "graphmlkey.h"

#include <QString>

class GraphMLData: public GraphML{
        Q_OBJECT
public:
    GraphMLData(GraphMLKey* key, QString value);
    ~GraphMLData();
    void setValue(QString value);
    QString getValue() const;
    GraphMLKey* getKey();
    QString toGraphML(qint32 indentationLevel=0);
    QString toString();

private:
    QString value;
    GraphMLKey* key;

    // GraphML interface

};

#endif // GRAPHMLDATA_H
