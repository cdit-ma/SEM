#ifndef GRAPHMLKEY_H
#define GRAPHMLKEY_H
#include <QString>
#include "graphml.h"

class GraphMLData;
class GraphMLKey: public GraphML
{
        Q_OBJECT
public:
    //TODO: match all types for GRAPHML.
    enum TYPE {BOOLEAN, INT, LONG, FLOAT, DOUBLE, STRING};

    GraphMLKey(QString name, QString typeStr, QString forStr);
    ~GraphMLKey();

    void setDefaultProtected(bool setProtected);
    bool isProtected();

    void setDefaultValue(QString value);
    QString getDefaultValue() const;

    bool equals(GraphMLKey* key);

    QString validateDataChange(GraphMLData* data, QString newValue);

    GraphML::KIND getForKind() const;
    QString toGraphML(qint32 indentationLevel=0);
    TYPE getType() const;
    QString getTypeString();
    QString getForKindString();
private:

    GraphML::KIND forKind;
    QString forKindStr;
    bool protectedKey;

    QList<GraphMLData*> keysData;

    QList<QChar> invalidLabelCharacters;

    TYPE type;
    QString typeStr;
    QString defaultValue;

    static int _Did;

    // GraphML interface
public:
    QString toString();
};

#endif // GRAPHMLKEY_H
