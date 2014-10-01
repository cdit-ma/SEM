#ifndef GRAPHMLKEY_H
#define GRAPHMLKEY_H
#include <QString>
#include "graphml.h"

class GraphMLKey: public GraphML
{
        Q_OBJECT
public:
    enum TYPE {BOOLEAN, INT, LONG, FLOAT, DOUBLE, STRING};
    GraphMLKey(QString name, QString typeStr, QString forStr);
    ~GraphMLKey();

    void setDefaultValue(QString value);
    QString getDefaultValue() const;
    bool operator==(const GraphMLKey &other) const;

    GraphML::KIND getForKind() const;
    QString toGraphML(qint32 indentationLevel=0);
    TYPE getType() const;
    QString getTypeString();
    QString getForKindString();
private:

    GraphML::KIND forKind;
    QString forKindStr;

    TYPE type;
    QString typeStr;
    QString defaultValue;

    static int _Did;

    // GraphML interface
public:
    QString toString();
};

#endif // GRAPHMLKEY_H
