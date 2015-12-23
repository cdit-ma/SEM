#include "key.h"
#include "entity.h"
#include "data.h"
#include <QDebug>

QString Key::getGraphMLTypeName(const QVariant::Type type)
{
    if(type == QVariant::Bool){
        return "boolean";
    }else if(type == QVariant::Int){
        return "int";
    }else if(type == QVariant::LongLong){
        return "longlong";
    }else if(type == QVariant::Double){
        return "double";
    }
    return "string";
}

QVariant::Type Key::getTypeFromGraphML(const QString typeString)
{
    QString typeStringLow = typeString.toLower();

    if(typeString == "boolean"){
        return QVariant::Bool;
    }else if(typeString == "int"){
        return QVariant::Int;
    }else if(typeString == "long"){
        return QVariant::LongLong;
    }else if(typeString == "float" || typeString == "double"){
        return QVariant::Double;
    }else if(typeString == "string"){
        return QVariant::String;
    }

    return QVariant::nameToType(typeStringLow.toStdString().c_str());
}

Key::Key(QString keyName, QVariant::Type type, Entity::ENTITY_KIND entityKind):GraphML(GK_KEY)
{
    _keyName = keyName;
    _keyType = type;
    _entityKind = entityKind;
}

void Key::setProtected(bool protect)
{
    _isProtected = protect;
}

bool Key::isProtected()
{
    return _isProtected;
}

QString Key::getName() const
{
    return _keyName;
}

bool Key::setDefaultValue(const QVariant value)
{
    QVariant copy = value;

    //If Value can be converted to this type.
    if(copy.canConvert(_keyType) && copy.convert(_keyType)){
        defaultValue = value;
        return true;
    }
    return false;
}

QVariant Key::getDefaultValue() const
{
    return defaultValue;
}

Entity::ENTITY_KIND Key::getEntityKind() const
{
    return _entityKind;
}

QVariant::Type Key::getType() const
{
    return _keyType;
}

void Key::addValidValues(QStringList validValues, QStringList entityKinds)
{
    foreach(QString nodeKind, entityKinds){
        foreach(QString validValue, validValues){
            addValidValue(nodeKind, validValue);
        }
    }
}

void Key::addValidRange(QPair<qreal, qreal> range, QStringList entityKinds)
{
    foreach(QString nodeKind, entityKinds){
        validRanges[nodeKind] = range;
    }
}

void Key::addInvalidCharacters(QStringList invalidCharacters, QStringList nodeKinds)
{
    foreach(QString nodeKind, nodeKinds){
        foreach(QString invalidCharacter, invalidCharacters){
            addInvalidCharacters(nodeKind, invalidCharacter);
        }
    }
}

QPair<qreal, qreal> Key::getValidRange(QString entityKinds)
{
    if(validRanges.contains(entityKinds)){
        return validRanges[entityKinds];
    }else if(validRanges.contains("ALL")){
        return validRanges["ALL"];
    }
    return QPair<qreal, qreal>();
}

bool Key::gotValidRange(QString entityKinds)
{
    return validRanges.contains(entityKinds) || validRanges.contains("ALL");
}

QStringList Key::getValidValues(QString entityKinds)
{
    if(validValues.contains(entityKinds)){
        return validValues[entityKinds];
    }else if(validValues.contains("ALL")){
        return validValues["ALL"];
    }
    return QStringList();
}

bool Key::gotValidValues(QString entityKinds)
{
    return validValues.contains(entityKinds) || validValues.contains("ALL");
}

QStringList Key::getInvalidCharacters(QString nodeKind)
{
    if(invalidCharacters.contains(nodeKind)){
        return invalidCharacters[nodeKind];
    }else if(invalidCharacters.contains("ALL")){
        return invalidCharacters["ALL"];
    }
    return QStringList();
}

bool Key::gotInvalidCharacters(QString entityKinds)
{
    return invalidCharacters.contains(entityKinds) || invalidCharacters.contains("ALL");
}

QVariant Key::validateDataChange(Data *data, QVariant dataValue)
{
    if(data->getKey() != this){
        //Return a blank QVariant
        return dataValue;
    }

    Entity* parentEntity = data->getParent();
    if(!parentEntity){
        qCritical() << "NO PARENT";
        //If no parent we can't validate the data, so assume correct
        return dataValue;
    }

    QString entityKind = parentEntity->getEntityName();

    bool okay = false;
    switch(_keyType){
        case QVariant::String:{
            if(dataValue.canConvert(QVariant::String)){
                //Check for valid values
                if(gotValidValues(entityKind)){
                    QStringList values = getValidValues(entityKind);
                    if(values.contains(dataValue.toString())){
                        okay = true;
                    }
                }else{
                    okay = true;
                }
                if(okay){
                    //Check for invalid Characters.
                    if(gotInvalidCharacters(entityKind)){
                        QStringList badChars = getInvalidCharacters(entityKind);
                        foreach(QChar qChar, dataValue.toString()){
                            if(badChars.contains(qChar)){
                                qCritical() << "GOT BAD CHARACTERS";
                                okay = false;
                            }
                        }
                    }
                }
            }
            break;
        }
    default:{
        //Could be a number.

        if(dataValue.canConvert(QVariant::Double)){
            double newValue = dataValue.toDouble(&okay);
            if(okay){
                if(gotValidRange(entityKind)){
                    QPair<qreal, qreal> range = getValidRange(entityKind);
                    if(range.first > newValue || range.second < newValue){
                        okay = false;
                    }
                }else{
                    okay = true;
                }
            }
        }
        break;
    }
    }

    if(okay){
        qCritical() << "NEW IS OKAY";
        //Return new Value
        return dataValue;
    }else{
        qCritical() << "OLD WAS OKAY";
        //Return old value.
        return data->getValue();
    }
}

QString Key::toGraphML(int indentDepth)
{
    QString tab("\t");
    //Construct the desired tab width.
    QString indent = tab.repeated(indentDepth);

    QString xml;
    xml += indent;
    xml += QString("<key attr.name=\"%1\" attr.type=\"%2\" for=\"%3\" id=\"%4\"").arg(getName(), getGraphMLTypeName(_keyType), Entity::getEntityKindName(_entityKind), QString::number(getID()));

    if(!defaultValue.isNull()){
        xml += ">\n";
        xml += indent + QString("\t<default>%1</default>\n").arg(getDefaultValue().toString());
        xml += indent + "</key>\n";
    }else{
        xml += "/>\n";
    }
    return xml;
}

QString Key::toString()
{
    return QString("[" + QString::number(getID()) + "]Key" + getName() + " For: " + Entity::getEntityKindName(_entityKind) + " Type: " + QVariant::typeToName(_keyType));
}

bool Key::equals(const Key *key) const
{
    if(_keyType != key->getType()){
        return false;
    }
    if(_keyName != key->getName()){
        return false;
    }
    if(_entityKind != key->getEntityKind()){
        return false;
    }
    return true;
}

/**
 * @brief Key::addValidValue
 * Adds a value to the list of valid values for the provided nodeKind.
 * @param nodeKind The nodeKind of the valid value
 * @param value The valid value
 */
void Key::addValidValue(QString nodeKind, QString value)
{
    QStringList values = validValues[nodeKind];
    if(!values.contains(value)){
        values.append(value);
        validValues[nodeKind] = values;
    }
}

void Key::addInvalidCharacters(QString nodeKind, QString invalidCharacter)
{
    QStringList values = invalidCharacters[nodeKind];
    if(!values.contains(invalidCharacter)){
        values.append(invalidCharacter);
        invalidCharacters[nodeKind] = values;
    }
}
