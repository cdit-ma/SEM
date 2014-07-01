#ifndef GRAPHMLDATA_H
#define GRAPHMLDATA_H

#include "graphmlattribute.h"

#include <QString>

class GraphMLData: public GraphML{
public:
    GraphMLData(GraphMLAttribute* type, QString value);
    void setValue(QString value);
    QString getValue() const;

    GraphMLAttribute* getType();

    QString toGraphML(qint32 indentationLevel=0);
    QString toString();

private:
    QString value;
    GraphMLAttribute* type;

    // GraphML interface

};

#endif // GRAPHMLDATA_H
