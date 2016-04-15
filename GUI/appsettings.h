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
    AppSettings(QWidget *parent = 0, QString applicationPath="", QHash<QString, QString> visualGroups = QHash<QString, QString>(), QHash<QString, QString> tooltips = QHash<QString, QString>());
    ~AppSettings();



    QSettings* getSettings();

    void loadSettings();
    bool areSettingsLoaded();

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

    void setAspectColor_Blind();
    void setAspectColor_Default();

private:
    void updateApplyButton();
    QString getGroup(QString keyName);
    void setupLayout();
    QSettings* settings;

    QHash<QString, QString> keysTooltips;
    QHash<QString, QString> keysVisualGroups;
    QHash<QString, KeyEditWidget*> settingsWidgetsHash;
    QHash<QString, SettingStruct> changedSettings;


    QHash<QString, QVBoxLayout*> groupLayouts;
    QHash<QString, QString> keyToGroupMap;

    bool settingsLoaded;
    bool settingFileWriteable;

    QPushButton* clearChangesButton;
    QPushButton* applyButton;

    QScrollArea* scrollArea;


    // QDialog interface
public slots:
    void reject();
};


#endif
