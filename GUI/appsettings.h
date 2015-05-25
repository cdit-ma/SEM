#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <QDialog>
#include <QSettings>
#include <QHash>
#include "keyeditwidget.h"

class AppSettings: public QDialog
{
    Q_OBJECT
public:
    AppSettings(QWidget *parent = 0, QString applicationPath="");
    QSettings* getSettings();
    void loadSettings();
    QString getSetting(QString keyName="");
    void setSetting(QString keyName, QVariant value);

    QString getReadableValue(const QString value);
signals:
    void settingChanged(QString settingGroup, QString settingName, QString settingValue);
public slots:
    void settingUpdated(QString, QString, QString);
    void launchSettingsUI();
    void updateSetting();

private:
    QString getGroup(QString keyName);
    void setupLayout();
    QSettings* settings;
    QHash<QString, QString> keyToGroupMap;
};


#endif
