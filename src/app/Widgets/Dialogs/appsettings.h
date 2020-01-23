#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <QDialog>
#include <QHash>
#include <QGroupBox>
#include <QTabWidget>
#include <QToolBar>
#include <QLabel>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QAction>
#include "../customgroupbox.h"

#include "../dataeditwidget.h"
#include "../../theme.h"
#include "../../Controllers/SettingsController/settingscontroller.h"

struct SettingStruct{
    QString key;
    QString group;
    QVariant value;
};

class AppSettings: public QDialog
{
    Q_OBJECT
public:
    AppSettings(QWidget *parent = 0);
    ~AppSettings();


    QVariant getSetting(QString);
signals:
    void setSetting(SETTINGS key, QVariant data);
    void settingsApplied();
public slots:
    void settingChanged(SETTINGS key, QVariant data);

private slots:
    void themeChanged();
    void dataValueChanged(QVariant data);

    void applySettings();
    void clearSettings();
private:
    QString settingKey(Setting* setting);
    QString settingKey(QString category, QString section);
    void updateLabels();
    void updateButtons();
    DataEditWidget* getDataWidget(SETTINGS key);
    void setupLayout();
    void setupSettingsLayouts();
    QVBoxLayout *getCategoryLayout(QString category);
    CustomGroupBox* getSectionBox(QString category, QString section);
    
    void showEvent(QShowEvent* event);
    bool theme_initialised_ = false;

    QTabWidget* tabWidget;
    QLabel* warningLabel;
    QToolBar* toolbar;
    QHash<SETTINGS, DataEditWidget*> dataEditWidgets;
    QHash<QString, QVBoxLayout*> categoryLayouts;
    QHash<SETTINGS, QVariant> changedSettings;
    QHash<QString, CustomGroupBox*> sectionBoxes;
    QAction* applySettingsAction;
    QAction* clearSettingsAction;
    QAction* warningAction;
    QAction* resetSettings;

};




#endif
