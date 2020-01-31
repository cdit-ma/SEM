#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <QDialog>
#include <QHash>
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

struct SettingStruct {
    QString key;
    QString group;
    QVariant value;
};

class AppSettings: public QDialog
{
    Q_OBJECT
    
public:
    AppSettings(QWidget* parent = nullptr);
    ~AppSettings();

signals:
    void setSetting(SETTINGS key, const QVariant& data);
    void settingsApplied();
    
public slots:
    void settingChanged(SETTINGS key, const QVariant& data);

private slots:
    void themeChanged();
    void dataValueChanged(const QVariant& data);

    void applySettings();
    void clearSettings();
    
private:
    QString settingKey(Setting* setting);
    QString settingKey(const QString& category, const QString& section);
    
    void updateLabels();
    void updateButtons();
    
    DataEditWidget* getDataWidget(SETTINGS key);
    
    void setupLayout();
    void setupSettingsLayouts();
    
    QVBoxLayout* getCategoryLayout(const QString& category);
    CustomGroupBox* getSectionBox(const QString& category, const QString& section);
    
    QTabWidget* tabWidget = nullptr;
    QLabel* warningLabel = nullptr;
    QToolBar* toolbar = nullptr;

    QAction* applySettingsAction = nullptr;
    QAction* clearSettingsAction = nullptr;
    QAction* warningAction = nullptr;
    
    QHash<SETTINGS, DataEditWidget*> dataEditWidgets;
    QHash<QString, QVBoxLayout*> categoryLayouts;
    QHash<SETTINGS, QVariant> changedSettings;
    QHash<QString, CustomGroupBox*> sectionBoxes;
};




#endif
