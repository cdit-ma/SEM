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
#include "../Widgets/New/dataeditwidget.h"

#include "../View/theme.h"
#include "../Controller/settingscontroller.h"

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
    void setSetting(SETTING_KEY key, QVariant data);
public slots:
    void settingChanged(SETTING_KEY key, QVariant data);

private slots:
    void themeChanged();
    void dataValueChanged(QString dataKey, QVariant data);

    void applySettings();
    void clearSettings();
private:
    void updateButtons();

    SETTING_KEY getSettingKey(QString key);

    DataEditWidget* getDataWidget(QString key);
    DataEditWidget* getDataWidget(SETTING_KEY key);
    void setupLayout();
    void setupSettingsLayouts();
    QVBoxLayout *getCategoryLayout(QString category);
    QVBoxLayout* getSectionLayout(QString category, QString section);

    QTabWidget* tabWidget;
    QLabel* warningLabel;
    QToolBar* toolbar;
    QHash<QString, SETTING_KEY> settingKeyLookup;
    QHash<SETTING_KEY, DataEditWidget*> dataEditWidgets;
    QHash<QString, QVBoxLayout*> categoryLayouts;
    QHash<QString, QVBoxLayout*> sectionLayouts;
    QHash<SETTING_KEY, QVariant> changedSettings;
    QAction* applySettingsAction;
    QAction* clearSettingsAction;
    QAction* warningAction;

};


#endif
