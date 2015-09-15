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
#include <QPushButton>
#include <QLineEdit>
#include <QDialog>
#include <QMessageBox>

#define SETTINGS_WIDTH 450
#define SETTINGS_HEIGHT 350



AppSettings::AppSettings(QWidget *parent, QString applicationPath):QDialog(parent)
{
    //Setup Settings.
    settings = new QSettings(applicationPath + "/settings.ini", QSettings::IniFormat);
    settingsLoaded = false;
    setMinimumWidth(SETTINGS_WIDTH);
    setMinimumHeight(SETTINGS_HEIGHT);
    setWindowTitle("Application Settings");

    setWindowFlags(windowFlags() & (~Qt::WindowContextHelpButtonHint));
    this->setWindowIcon(QIcon(":/Actions/Settings.png"));
    setModal(true);

    setupLayout();
}

AppSettings::~AppSettings()
{
}

/**
 * @brief AppSettings::getSettings Gets the QSettings object for this Application.
 * @return The QSettings Object.
 */
QSettings* AppSettings::getSettings()
{
    return settings;
}

/**
 * @brief AppSettings::loadSettings Emits a SIGNAL for each setting contained in the QSettings.
 */
void AppSettings::loadSettings()
{

    foreach(QString group, settings->childGroups()){
        settings->beginGroup(group);


        foreach(QString key, settings->childKeys()){
            //Dont reload window settings.
            if(settingsLoaded && group == "01-Window_Settings"){
                 continue;
            }
            QString value = settings->value(key).toString();
            //Foreach Key in each Group in the settings file, emit the signal that the setting has changed.
            emit settingChanged(group ,key, value);
        }
        settings->endGroup();
    }


    if(!settingsLoaded){
        settingsLoaded = true;
    }
}

/**
 * @brief AppSettings::getSetting Gets the value for a Setting with KeyName provided.
 * @param keyName The name of the key
 * @return The value of the Setting, or "" if no Setting found.
 */
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

/**
 * @brief AppSettings::setSetting Sets the value for a Setting with KeyName provided.
 * @param keyName The name of the key
 * @param value The value to set the Setting
 */
void AppSettings::setSetting(QString keyName, QVariant value)
{
    QString groupName = getGroup(keyName);
    if(groupName != ""){
        settings->beginGroup(groupName);
        settings->setValue(keyName, value);
        settings->endGroup();

        KeyEditWidget* settingWidget = settingsWidgetsHash[keyName];
        if(settingWidget){
            settingWidget->setValue(value);
        }
    }
}



QString AppSettings::getReadableValue(const QString value)
{
    QString returnable = value;

    returnable = returnable.replace("_", " ");
    while(returnable.contains("-")){
        int hypenPos = returnable.indexOf("-") + 1;
        returnable = returnable.mid(hypenPos);
    }
    return returnable+":";
}

void AppSettings::_settingChanged(QString settingGroup, QString settingName, QString settingValue)
{
    //Check if value has changed.

    bool settingChanged = getSetting(settingName) != settingValue;

    if(settingChanged){
        QStringList setting;
        setting << settingGroup << settingName << settingValue;
        changedSettings[settingName] = setting;
    }else{
        changedSettings.remove(settingName);
    }

    //Update the visual state of the Widget.
    if(settingsWidgetsHash.contains(settingName)){
        KeyEditWidget* settingWidget = settingsWidgetsHash[settingName];
        if(settingWidget){
            settingWidget->setHighlighted(settingChanged);
        }
    }

    updateApplyButton();
}


void AppSettings::settingUpdated(QString g , QString n , QString v)
{
    settings->beginGroup(g);
    settings->setValue(n,v);
    settings->endGroup();
    emit settingChanged(g,n,v);
}


void AppSettings::groupToggled(bool toggled)
{
    QGroupBox* groupBox = dynamic_cast<QGroupBox*>(sender());
    if(groupBox){
        QString groupTitle = groupBox->title();


        int groupBoxPadding = groupBox->layout()->spacing();

        int initialHeight = 20;
        int expandedHeight = initialHeight + groupBox->layout()->margin() * 2;

        foreach(KeyEditWidget* edit, settingsWidgetsHash.values()){
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

void AppSettings::clearSettings(bool applySettings)
{
    //While we still have setttings which need updating.
    while(!changedSettings.isEmpty()){
        QString settingKey = changedSettings.keys().first();
        QStringList setting = changedSettings.take(settingKey);

        //If we are meant to apply the settings we need to update the QSettings.
        if(applySettings){
            if(setting.size() == 3){
                QString setting_group = setting[0];
                QString setting_name = setting[1];
                QString setting_value = setting[2];
                //Update the QSetting
                settingUpdated(setting_group, setting_name, setting_value);
            }
        }

        //Clear the GUI.
        KeyEditWidget* settingWidget = settingsWidgetsHash[settingKey];
        if(settingWidget){
            settingWidget->setHighlighted(false);
            //Reset Previous Value
            settingWidget->setValue(getSetting(settingKey));
        }
    }
    updateApplyButton();
}

void AppSettings::updateApplyButton()
{
    if(applyButton){
        applyButton->setEnabled(!changedSettings.isEmpty());
    }
}




QString AppSettings::getGroup(QString keyName)
{
    return keyToGroupMap[keyName];

}


void AppSettings::setupLayout()
{
    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->setSpacing(0);
    mainLayout->setMargin(0);
    setLayout(mainLayout);

    QVBoxLayout *vLayout = new QVBoxLayout();

    int longestFont = 0;
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

        QFontMetrics keyFontMetric(keyFont);


        //For each key, construct a KeyEditWidget to change the setting of that key.
        foreach(QString key, settings->childKeys()){
            if(!settingsWidgetsHash.contains(key)){
                QString humanReadableKey =  getReadableValue(key);

                int fontWidth = keyFontMetric.width(humanReadableKey);


                if(fontWidth > longestFont){
                    longestFont = fontWidth;
                }

                KeyEditWidget* keyEdit = new KeyEditWidget( group, key, humanReadableKey, settings->value(key));

                if(!keyToGroupMap.contains(key)){
                    keyToGroupMap.insert(key, group);
                }

                settingsWidgetsHash[key] = keyEdit;

                //Connect the valueChanged signal to this, to update the settings.ini file.
                connect(keyEdit, SIGNAL(valueChanged(QString,QString,QString)), this, SLOT(_settingChanged(QString,QString,QString)));

                groupVLayout->addWidget(keyEdit);

                keyEdit->setFont(keyFont);

            }else{
                QMessageBox::critical(this, "Settings Error", "Settings file has 2 settings in .ini file with same Key Name! Ignoring duplicate Settings", QMessageBox::Ok);
            }
        }
        settings->endGroup();
        emit groupBox->toggled(false);
    }


    foreach(QString key, settingsWidgetsHash.keys()){
        KeyEditWidget* keyEdit = settingsWidgetsHash[key];
        if(keyEdit){
            keyEdit->setLabelWidth(longestFont);
        }
    }

    vLayout->addStretch();


    QWidget* widgetContainer = new QWidget();
    widgetContainer->setLayout(vLayout);

    scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(widgetContainer);
    mainLayout->addWidget(scrollArea,1);

    QHBoxLayout* buttonLayout = new QHBoxLayout();


    applyButton = new QPushButton("Apply");

    QPushButton* cancelButton = new QPushButton("Cancel");


    buttonLayout->setSpacing(2);
    buttonLayout->setMargin(2);

    buttonLayout->addStretch();
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addWidget(applyButton);

    mainLayout->addLayout(buttonLayout);


    scrollArea->setStyleSheet("QScrollArea{border: none;}");

    connect(applyButton, SIGNAL(clicked()), this, SLOT(clearSettings()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

}

void AppSettings::reject()
{
    clearSettings(false);
    QDialog::reject();
}
