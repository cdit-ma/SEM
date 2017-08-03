#ifndef SETTING_H
#define SETTING_H


#include "settingscontroller.h"

class Setting{
public:
    Setting(SETTING_KEY ID, SETTING_TYPE type, QString category, QString section, QString name);
    SETTING_KEY getID() const;
    SETTING_TYPE getType() const;
    QString getSection() const;
    QString getCategory() const;
    QString getName() const;
    QVariant getDefaultValue() const;
    QString getTooltip() const;
    QPair<QString, QString> getIcon() const;
    bool gotIcon() const;
    void setIcon(QString path, QString name);
    void setIcon(QPair<QString, QString> icon);

    bool isThemeSetting() const;
    QString getSettingString() const;


    void setDefaultValue(QVariant value);
    bool resetValue();
    bool setValue(QVariant value);
    QVariant getValue() const;
private:
    SETTING_KEY ID;
    SETTING_TYPE type;
    QString category;
    QString section;
    QString name;
    QString tooltip;
    QVariant defaultValue;
    QVariant value;
    QPair<QString, QString> icon;
};
#endif // SETTING_H
