#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <QDialog>
#include <QSettings>
#include <QScrollArea>
#include <QHash>
#include <QGroupBox>
#include "keyeditwidget.h"

class AppSettings: public QDialog
{
    Q_OBJECT
public:
    AppSettings(QWidget *parent = 0, QString applicationPath="");
    ~AppSettings();



    QSettings* getSettings();

    void loadSettings();

    void setSetting(QString keyName, QVariant value);
    QString getSetting(QString keyName);

    QString getReadableValue(const QString value);
signals:
    void settingChanged(QString settingGroup, QString settingName, QString setti5ngValue);
private slots:
    void _settingChanged(QString settingGroup, QString settingName, QString settingValue);
    void settingUpdated(QString, QString, QString);


    void groupToggled(bool toggled);

    void clearSettings(bool applySettings=true);


private:
    void updateApplyButton();
    QString getGroup(QString keyName);
    void setupLayout();
    QSettings* settings;

    QHash<QString, QString> keyToGroupMap;
    //QList<KeyEditWidget*> settingsWidgets;
    bool settingsLoaded;
    bool settingFileWriteable;

    QPushButton* applyButton;
    QHash<QString, KeyEditWidget*> settingsWidgetsHash;
    QHash<QString, QStringList> changedSettings;

    QScrollArea* scrollArea;


    // QDialog interface
public slots:
    void reject();
};


#endif
