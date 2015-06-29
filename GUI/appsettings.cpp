#include "appsettings.h"

#include <QWidget>
#include <QObject>
#include <QSettings>
#include <QString>
#include <QDebug>
#include <QVBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QScrollArea>
#include <QLineEdit>
#include <QDialog>


AppSettings::AppSettings(QWidget *parent, QString applicationPath):QDialog(parent)
{
    settings = new QSettings(applicationPath+"/settings.ini", QSettings::IniFormat);
    setupLayout();

    setModal(true);
    setWindowFlags(windowFlags() & (~Qt::WindowContextHelpButtonHint));


    scrollArea = new QScrollArea();
    scrollArea->setWidget(this);
    scrollArea->setMinimumWidth(350);
    scrollArea->setMinimumHeight(350);
    scrollArea->setWidgetResizable(true);
    scrollArea->setWindowTitle("MEDEA - Settings");
    connect(this, SIGNAL(destroyed()), scrollArea, SLOT(deleteLater()));
}

AppSettings::~AppSettings()
{
    this->close();
}

QSettings* AppSettings::getSettings()
{
    return settings;
}

/**
 * @brief AppSettings::loadSettings Emits all settings as updated to initialize state.
 */
void AppSettings::loadSettings()
{
    foreach(QString group, settings->childGroups()){
        settings->beginGroup(group);


      foreach(QString key, settings->childKeys()){
          QString value = settings->value(key).toString();
          settingChanged(group,key,value);
      }
      settings->endGroup();
    }
}

QString AppSettings::getSetting(QString keyName)
{
    QString value = "";
    QString groupName = getGroup(keyName);
    if(groupName != ""){
        settings->beginGroup(groupName);
        value = settings->value(keyName).toString();
        settings->endGroup();
    }
    return value;
}

void AppSettings::setSetting(QString keyName, QVariant value)
{
    QString groupName = getGroup(keyName);
    if(groupName != ""){
        settings->beginGroup(groupName);
        settings->setValue(keyName, value);
        settings->endGroup();
    }
}

void AppSettings::show()
{
    scrollArea->show();
}

QString AppSettings::getReadableValue(const QString value)
{
    QString returnable = value;

    returnable = returnable.replace("_", " ");
    while(returnable.contains("-")){
        int hypenPos = returnable.indexOf("-") + 1;
        returnable = returnable.mid(hypenPos);
    }
    return returnable;
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
    show();
}

void AppSettings::updateSetting()
{

}

void AppSettings::groupToggled(bool toggled)
{

    QGroupBox* groupBox = dynamic_cast<QGroupBox*>(sender());
    if(groupBox){
        QString groupTitle = groupBox->title();


        int groupBoxPadding = groupBox->layout()->spacing();

        int initialHeight = 20;
        int expandedHeight = initialHeight + groupBox->layout()->margin() * 2;

        foreach(KeyEditWidget* edit, settingsWidgets){
            if(getReadableValue(edit->getGroupName()) == groupTitle){
                expandedHeight += groupBoxPadding + edit->height();
                edit->setVisible(toggled);
            }
        }
        if(toggled){
            groupBox->setFixedHeight(expandedHeight);
        }else{
            groupBox->setFixedHeight(initialHeight);
        }
    }
}


QString AppSettings::getGroup(QString keyName)
{
    return keyToGroupMap[keyName];

}


void AppSettings::setupLayout()
{

    QVBoxLayout *vLayout = new QVBoxLayout();



    setLayout(vLayout);

    //For each group in Settings.ini
    foreach(QString group, settings->childGroups()){
        //Open Group
        settings->beginGroup(group);



        //Construct Group Box to group Items
        QGroupBox* groupBox = new QGroupBox();
        groupBox->setCheckable(true);
        groupBox->setChecked(false);
        groupBox->setTitle(getReadableValue(group));
        connect(groupBox, SIGNAL(toggled(bool)), this, SLOT(groupToggled(bool)));

        QFont keyFont = font();
        keyFont.setBold(false);

        //Set font to Bold
        QFont groupTitleFont = font();
        groupTitleFont.setBold(true);
        groupBox->setFont(groupTitleFont);
        //Add to Layout.
        vLayout->addWidget(groupBox);

        QVBoxLayout* groupVLayout = new QVBoxLayout();
        groupBox->setLayout(groupVLayout);


        //For each key, construct a KeyEditWidget to change the setting of that key.
        foreach(QString key, settings->childKeys()){

            KeyEditWidget* keyEdit = new KeyEditWidget( group, key, getReadableValue(key), settings->value(key));

            if(!keyToGroupMap.contains(key)){
                keyToGroupMap.insert(key, group);
            }
            settingsWidgets.append(keyEdit);

            //Connect the valueChanged signal to this, to update the settings.ini file.
            connect(keyEdit, SIGNAL(valueChanged(QString,QString,QString)), this, SLOT(settingUpdated(QString,QString,QString)));

            groupVLayout->addWidget(keyEdit);

            keyEdit->setFont(keyFont);
        }
        settings->endGroup();
        emit groupBox->toggled(false);
    }
    vLayout->addStretch();
}
