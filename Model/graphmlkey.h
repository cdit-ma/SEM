#ifndef GRAPHMLKEY_H
#define GRAPHMLKEY_H
#include <QString>
#include "graphml.h"
#include <QMap>
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

    void appendValidValues(QStringList nodeKinds, QStringList values);
    void appendValidValues(QString nodeKind, QStringList values);

    void setDefaultValue(QString value);
    QString getDefaultValue() const;

    bool equals(GraphMLKey* key);
    bool gotSelectableValues(QString nodeKind);
    QStringList getSelectableValues(QString nodeKind);

    QString validateDataChange(GraphMLData* data, QString newValue);

    GraphML::KIND getForKind() const;
    QString toGraphML(qint32 indentationLevel=0);
    TYPE getType() const;
    QString getTypeString();
    QString getForKindString();
private:

    void addValidValue(QString nodeKind, QString value);

    GraphML::KIND forKind;
    QString forKindStr;
    bool protectedKey;

    QList<GraphMLData*> keysData;

    QMultiMap<QString, QString> validValues;

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
