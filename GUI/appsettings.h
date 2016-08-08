#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <QDialog>
#include <QHash>
#include <QGroupBox>
#include <QTabWidget>
#include <QToolBar>
#include <QLabel>
#include <QScrollArea>

#include "keyeditwidget.h"

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
    void setSetting(QString, QVariant);
private slots:
    void themeChanged();
private:
    void setupLayout();
    void setupSettingsLayouts();
    QVBoxLayout *getCategoryLayout(QString category);
    QVBoxLayout* getSectionLayout(QString category, QString section);

    QTabWidget* tabWidget;
    QLabel* warningLabel;
    QToolBar* toolbar;
    QHash<SETTING_KEY, KeyEditWidget*> keyEdits;
    QHash<QString, QVBoxLayout*> categoryLayouts;
    QHash<QString, QVBoxLayout*> sectionLayouts;
    QAction* applySettings;
    QAction* clearSettings;

};


#endif
