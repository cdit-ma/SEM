#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <QDialog>
#include <QSettings>
#include <QScrollArea>
#include <QHash>
#include <QGroupBox>
#include "keyeditwidget.h"

struct SettingStruct{
    QString key;
    QString group;
    QVariant value;
};

class AppSettings: public QDialog
{
    Q_OBJECT
public:
    AppSettings(QWidget *parent = 0, QString applicationPath="");
    ~AppSettings();



    QSettings* getSettings();

    void loadSettings();

    void setSetting(QString keyName, QVariant value);
    QVariant getSetting(QString keyName);

    QString getReadableValue(const QString value);
signals:
    void settingChanged(QString settingGroup, QString settingName, QVariant settingValue);
    void settingsApplied();
private slots:
    void _settingChanged(QString settingGroup, QString settingName, QVariant settingValue);
    void settingUpdated(QString, QString, QVariant);



    void clearSettings(bool applySettings=true);
    void clearChanges();

    void setDarkTheme();
    void setLightTheme();

private:
    void updateApplyButton();
    QString getGroup(QString keyName);
    void setupLayout();
    QSettings* settings;

    QHash<QString, QString> keyToGroupMap;
    //QList<KeyEditWidget*> settingsWidgets;
    bool settingsLoaded;
    bool settingFileWriteable;

    QPushButton* clearChangesButton;
    QPushButton* applyButton;
    QHash<QString, KeyEditWidget*> settingsWidgetsHash;
    QHash<QString, SettingStruct> changedSettings;

    QScrollArea* scrollArea;


    // QDialog interface
public slots:
    void reject();
};


#endif
