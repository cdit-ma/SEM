#include "setting.h"
#include <QStringBuilder>
Setting::Setting(SETTING_KEY ID, SETTING_TYPE type, QString category, QString section, QString name)
{
    this->ID = ID;
    this->type = type;
    this->category = category;
    this->section = section;
    this->name = name;
    this->defaultValue = "";
    this->value = "";
}


SETTING_KEY Setting::getID() const
{
    return ID;
}

SETTING_TYPE Setting::getType() const
{
    return type;
}

QString Setting::getSection() const
{
    return section;
}

QString Setting::getCategory() const
{
    return category;
}

QString Setting::getName() const
{
    return name;
}

QVariant Setting::getDefaultValue() const
{
    return defaultValue;
}

QString Setting::getTooltip() const
{
    return tooltip;
}

QPair<QString, QString> Setting::getIcon() const
{
    return icon;
}

bool Setting::gotIcon() const{
    return icon.first != "" && icon.second != "";
}

void Setting::setIcon(QString path, QString name){
    icon.first = path;
    icon.second = name;
}
void Setting::setIcon(QPair<QString, QString> icon){
    setIcon(icon.first, icon.second);
}

bool Setting::isThemeSetting() const
{
    return getSettingString().contains("theme", Qt::CaseInsensitive);
}

QString Setting::getSettingString() const
{
    QString setting = QString::number(ID) % "_" % category % "_" % section % "_" % name;
    setting = setting.toUpper();
    setting.replace(" ", "_");
    return setting;
}

void Setting::setDefaultValue(QVariant value)
{
    if(defaultValue != value){
        defaultValue = value;
        setValue(defaultValue);
    }
}

bool Setting::resetValue()
{
    return setValue(defaultValue);
}

bool Setting::setValue(QVariant value)
{
   //Update if setting has changed
   if(value != this->value){
       this->value = value;
       return true;
   }else if(type == ST_BUTTON || type == ST_NONE){
       return true;
   }
   return false;
}

QVariant Setting::getValue() const
{
    return value;
}
