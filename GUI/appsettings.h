
#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <QSettings>
#include <QObject>
#include "keyeditwidget.h"

class AppSettings: public QObject
{
    Q_OBJECT
public:
    AppSettings(QString applicationPath, QWidget* parent);
    QSettings* getSettings();

signals:
    void settingChanged(QString settingGroup, QString settingName, QString settingValue);
public slots:
    void settingUpdated(QString, QString, QString);
    void launchSettingsUI();
    void updateSetting();

private:
    QSettings* settings;
};


#endif
