#ifndef GRAPHMLDATA_H
#define GRAPHMLDATA_H

#include "graphmlattribute.h"

#include <QString>

class GraphMLData
{
public:
    GraphMLData(GraphMLAttribute* type, QString value);
    void setValue(QString value);
    QString getValue() const;

    GraphMLAttribute* getType();

     QString toGraphML(qint32 indentationLevel=0);
private:
    QString value;
    GraphMLAttribute* type;
};

#endif // GRAPHMLDATA_H
