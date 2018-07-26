#ifndef SETTING_H
#define SETTING_H


#include "settingscontroller.h"

class Setting{
public:
    Setting(SETTINGS ID, SETTING_TYPE type, QString category, QString section, QString name);
    SETTINGS getID() const;
    SETTING_TYPE getType() const;
    QString getSection() const;
    QString getCategory() const;
    QString getName() const;
    QVariant getDefaultValue() const;
    QPair<QString, QString> getIcon() const;
    bool gotIcon() const;
    void setIcon(QString path, QString name);
    void setIcon(QPair<QString, QString> icon);

    bool isThemeSetting() const;

    void setDefaultValue(QVariant value);
    bool setValue(QVariant value);
    QVariant getValue() const;
private:
    SETTINGS ID;
    SETTING_TYPE type;
    QString category;
    QString section;
    QString name;
    QVariant defaultValue;
    QVariant value;
    QPair<QString, QString> icon;
};
#endif // SETTING_H
