#include "appsettings.h"
#include <QWidget>
#include <QObject>
#include <QSettings>
#include <QString>
#include <QDebug>
#include <QVBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QLineEdit>


AppSettings::AppSettings(QString applicationPath, QWidget *parent):QObject(parent)
{
    settings = new QSettings(applicationPath+"/settings.ini", QSettings::IniFormat);

}

QSettings* AppSettings::getSettings()
{
    return settings;
}

void AppSettings::settingUpdated(QString g , QString n , QString v)
{
    settings->beginGroup(g);
    settings->setValue(n,v);
    settings->endGroup();
    settingChanged(g,n,v);
}


void AppSettings::launchSettingsUI()
{
    QWidget* settingsGui = new QWidget(0);
    //QWidget* settingsGui = new QWidget((QWidget*)parent());


    QVBoxLayout *settingsLayout = new QVBoxLayout();
    settingsGui->setLayout(settingsLayout);

    foreach(QString group, settings->childGroups()){

        settings->beginGroup(group);


        QGroupBox* groupBox = new QGroupBox();
        groupBox->setTitle(group);
        settingsLayout->addWidget(groupBox);

        QVBoxLayout* groupLayout = new QVBoxLayout();
        groupBox->setLayout(groupLayout);

        foreach(QString key, settings->childKeys()){
            KeyEditWidget* keyEdit = new KeyEditWidget(0,group, key, settings->value(key));
            connect(keyEdit, SIGNAL(valueChanged(QString,QString,QString)), this, SLOT(settingUpdated(QString,QString,QString)));
            groupLayout->addLayout(keyEdit->getLayout());
        }
        settings->endGroup();
    }


    //Read settings.



    settingsGui->show();

}

void AppSettings::updateSetting()
{

}
