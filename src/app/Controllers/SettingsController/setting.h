#ifndef SETTING_H
#define SETTING_H

#include "settingscontroller.h"

class Setting {
public:
    Setting(SETTINGS ID, SETTING_TYPE type, const QString& category, const QString& section, const QString& name);
    SETTINGS getID() const;
    SETTING_TYPE getType() const;
    QString getSection() const;
    QString getCategory() const;
    QString getName() const;
    QVariant getDefaultValue() const;
    QPair<QString, QString> getIcon() const;
    bool gotIcon() const;
    void setIcon(const QString& path, const QString& name);
    void setIcon(const QPair<QString, QString>& icon);

    bool isThemeSetting() const;

    void setDefaultValue(const QVariant& value);
    bool setValue(const QVariant& value);
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
