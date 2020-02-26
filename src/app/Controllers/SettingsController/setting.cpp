#include "setting.h"
#include <QStringBuilder>

Setting::Setting(SETTINGS ID, SETTING_TYPE type, const QString& category, const QString& section, const QString& name)
{
    this->ID = ID;
    this->type = type;
    this->category = category;
    this->section = section;
    this->name = name;
    defaultValue = "";
    value = "";
}

SETTINGS Setting::getID() const
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

QPair<QString, QString> Setting::getIcon() const
{
    return icon;
}

bool Setting::gotIcon() const
{
    return icon.first != "" && icon.second != "";
}

void Setting::setIcon(const QString& path, const QString& name)
{
    icon.first = path;
    icon.second = name;
}

void Setting::setIcon(const QPair<QString, QString>& icon)
{
    setIcon(icon.first, icon.second);
}

bool Setting::isThemeSetting() const
{
    return category.toLower() == "theme";
}

void Setting::setDefaultValue(const QVariant& value)
{
    if(defaultValue != value){
        defaultValue = value;
        setValue(defaultValue);
    }
}

bool Setting::setValue(const QVariant& value)
{
   //Update if setting has changed
   if(value != this->value){
       this->value = value;
       return true;
   }else if(type == SETTING_TYPE::BUTTON || type == SETTING_TYPE::NONE){
       return true;
   }
   return false;
}

QVariant Setting::getValue() const
{
    return value;
}
