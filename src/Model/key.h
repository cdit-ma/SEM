#ifndef KEY_H
#define KEY_H
#include "graphml.h"
#include <QVariant>
#include "entity.h"
class Entity;
class Key : public GraphML
{
    Q_OBJECT
public:
    static QHash<QString, Key*> keyLookup_;
    static Key* GetKey(QString key_name);
    static Key* GetKey(int key_id);
    static Key* GetKey(QString key_name, QVariant::Type type);

    static QString getGraphMLTypeName(const QVariant::Type type);

    static QVariant::Type getTypeFromGraphML(const QString typeString);

    Key(QString keyName, QVariant::Type type, Entity::ENTITY_KIND entityKind);
    ~Key();

    void setProtected(bool protect);
    bool isProtected();

    void setIsVisualData(bool visual);
    bool isVisualData();
    QString getName() const;

    bool setDefaultValue(const QVariant value);
    QVariant getDefaultValue() const;

    Entity::ENTITY_KIND getEntityKind() const;
    QVariant::Type getType() const;


    void addValidValues(QStringList validValues, QStringList entityKinds = QStringList("ALL"));
    void addValidRange(QPair<qreal, qreal> range, QStringList entityKinds = QStringList("ALL"));
    void addInvalidCharacters(QStringList invalidCharacters, QStringList entityKinds = QStringList("ALL"));

    void setAllowAllValues(QString nodeKind);

    QPair<qreal, qreal> getValidRange(QString entityKinds = "ALL");
    bool gotValidRange(QString entityKinds = "ALL");

    QStringList getValidValues(QString entityKinds = "ALL");
    bool gotValidValues(QString entityKinds = "ALL");

    QStringList getInvalidCharacters(QString entityKinds = "ALL");
    bool gotInvalidCharacters(QString entityKinds = "ALL");

    QVariant validateDataChange(Data* data, QVariant dataValue);

    QString toGraphML(int indentDepth);
    QString toString();
    bool equals(const Key* key) const;
signals:
    void validateError(QString, QString, int);
private:
    void addValidValue(QString nodeKind, QString value);

    void addInvalidCharacters(QString nodeKind, QString invalidCharacter);
    QString _keyName;
    QVariant::Type _keyType;
    Entity::ENTITY_KIND _entityKind;
    bool _isVisual;

    bool _isProtected;
    QVariant defaultValue;

    QMap<QString, bool> ignoreValues;
    QMap<QString, QPair<qreal, qreal> > validRanges;
    QMap<QString, QStringList> validValues;
    QMap<QString, QStringList> invalidCharacters;

};

#endif // KEY_H
