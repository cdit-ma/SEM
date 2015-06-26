#ifndef GRAPHMLKEY_H
#define GRAPHMLKEY_H
#include <QString>
#include "graphml.h"
#include <QStringList>
#include <QMap>
#include <QHash>
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

    bool isNumber();
    void appendValidValues(QStringList nodeKinds, QStringList values);
    void appendValidValues(QStringList values, QString nodeKind="ALL");

    void setDefaultValue(QString value);
    QString getDefaultValue() const;

    bool equals(GraphMLKey* key);
    bool gotSelectableValues(QString nodeKind="ALL");
    QStringList getSelectableValues(QString nodeKind="ALL");

    QString validateDataChange(GraphMLData* data, QString newValue);

    GraphML::KIND getForKind() const;
    QString toGraphML(qint32 indentationLevel=0);
    TYPE getType() const;
    QString getTypeString();
    QString getForKindString();
private:
    bool isValidValue(QString number, TYPE type);

    void addValidValue(QString nodeKind, QString value);

    GraphML::KIND forKind;
    QString forKindStr;
    bool protectedKey;

    QList<GraphMLData*> keysData;

    QMultiMap<QString, QString> validValues;

    QHash<QString, QStringList> invalidCharacters;

    TYPE type;
    QString typeStr;
    QString defaultValue;

    static int _Did;

    // GraphML interface
public:
    QString toString();
};

#endif // GRAPHMLKEY_H
