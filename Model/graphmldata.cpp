#include "graphmldata.h"

GraphMLData::GraphMLData(GraphMLAttribute *type, QString value):GraphML(GraphML::DATA)
{
    this->type = type;
    this->setValue(value);
}

void GraphMLData::setValue(QString value)
{
    this->value = value;
}

QString GraphMLData::getValue() const
{
    return this->value;
}

GraphMLAttribute *GraphMLData::getType()
{
    return this->type;
}

QString GraphMLData::toGraphML(qint32 indentationLevel)
{
    QString tabSpace;
    for(int i=0;i<indentationLevel;i++){
        tabSpace += "\t";
    }

    QString returnable = tabSpace + QString("<data key=\"%1\">%2</data>\n").arg(this->getType()->getID(), this->getValue());
    return returnable;
}

QString GraphMLData::toString()
{
    return "this->";
}
