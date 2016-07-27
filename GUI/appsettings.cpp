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

#define SETTINGS_WIDTH 600
#define SETTINGS_HEIGHT 400
#include "../enumerations.h"


#include <QTabWidget>

AppSettings::AppSettings(QWidget *parent, QString applicationPath, QHash<QString, QString> visualGroups, QHash<QString, QString> tooltips):QDialog(parent)
{
    //Setup Settings.
    settings = new QSettings(applicationPath + "/settings.ini", QSettings::IniFormat);
    settingFileWriteable = settings->isWritable();
    settingsLoaded = false;
    setMinimumWidth(SETTINGS_WIDTH);
    setMinimumHeight(SETTINGS_HEIGHT);

    QString title = "Application Settings";
    if(!settingFileWriteable){
        title += " [READ-ONLY]";
    }

    setWindowTitle(title);


    keysTooltips = tooltips;
    keysVisualGroups = visualGroups;



    setWindowFlags(windowFlags() & (~Qt::WindowContextHelpButtonHint));
    this->setWindowIcon(QIcon(":/Actions/Settings.png"));
    setModal(true);

    setupLayout();
    updateApplyButton();

    setAspectColor_Blind();
}


AppSettings::~AppSettings()
{
    settings->sync();
    delete settings;
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
            if(settingsLoaded && (group == WINDOW_SETTINGS || group == TOOLBAR_SETTINGS)){
                 continue;
            }
            QVariant variant;
            bool gotValue = false;

            //Get value from Widget first.
            if(settingsWidgetsHash.contains(key)){
                KeyEditWidget* settingWidget = settingsWidgetsHash[key];
                if(settingWidget){
                    variant = settingWidget->getValue();
                    gotValue = true;
                }
            }

            if(!gotValue){
                variant = settings->value(key);
            }

            if(variant.isValid()){
                //Foreach Key in each Group in the settings file, emit the signal that the setting has changed.
                emit settingChanged(group ,key, variant);
            }
        }
        settings->endGroup();
    }


    if(!settingsLoaded){
        settingsLoaded = true;
    }

    emit settingsApplied();
}

bool AppSettings::areSettingsLoaded()
{
    return settingsLoaded;
}

/**
 * @brief AppSettings::getSetting Gets the value for a Setting with KeyName provided.
 * @param keyName The name of the key
 * @return The value of the Setting, or "" if no Setting found.
 */
QVariant AppSettings::getSetting(QString keyName)
{
    QVariant value;
    QString groupName = getGroup(keyName);
    if(groupName != ""){
        settings->beginGroup(groupName);
        value = settings->value(keyName);
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
    return returnable;
}

void AppSettings::_settingChanged(QString settingGroup, QString settingName, QVariant settingValue)
{
    //Check if value has changed.
    QVariant oldValue = getSetting(settingName);
    bool settingChanged = oldValue != settingValue;

    if(settingChanged){
        SettingStruct setting;
        setting.group = settingGroup;
        setting.key = settingName;
        setting.value = settingValue;

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


void AppSettings::settingUpdated(QString g , QString n , QVariant v)
{
    settings->beginGroup(g);
    settings->setValue(n, v);
    settings->endGroup();
    emit settingChanged(g, n, v);
}

void AppSettings::clearSettings(bool applySettings)
{
    //While we still have setttings which need updating.
    while(!changedSettings.isEmpty()){

        QString settingKey = changedSettings.keys().first();

        SettingStruct setting = changedSettings.take(settingKey);

        QVariant newGUIVal = getSetting(settingKey);
        //If we are meant to apply the settings we need to update the QSettings.
        if(applySettings){
            settingUpdated(setting.group, setting.key, setting.value);
            newGUIVal = setting.value;
        }

        //Clear the GUI.
        KeyEditWidget* settingWidget = settingsWidgetsHash[settingKey];
        if(settingWidget){
            settingWidget->setHighlighted(false);
            //Reset Previous Value
            settingWidget->setValue(newGUIVal);
        }
    }
    updateApplyButton();

    if(applySettings){
        emit settingsApplied();
    }
}

void AppSettings::clearChanges()
{
    clearSettings(false);
}

void AppSettings::setDarkTheme()
{
    clearSettings(false);
    emit settingChanged(THEME_SETTINGS, THEME_SET_DARK_THEME, true);
}

void AppSettings::setLightTheme()
{
    clearSettings(false);
    emit settingChanged(THEME_SETTINGS, THEME_SET_LIGHT_THEME, true);
}

void AppSettings::setAspectColor_Blind()
{
    clearSettings(false);
    emit settingChanged(ASPECT_SETTINGS, ASPECT_COLOR_DEFAULT, true);
}

void AppSettings::setAspectColor_Default()
{
    clearSettings(false);
    emit settingChanged(ASPECT_SETTINGS, ASPECT_COLOR_BLIND, true);
}

void AppSettings::updateApplyButton()
{
    if(applyButton){
        applyButton->setEnabled(!changedSettings.isEmpty());
    }
    if(clearChangesButton){
        clearChangesButton->setEnabled(!changedSettings.isEmpty());
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
    QTabWidget* tabWidget = new QTabWidget();



    if(!settingFileWriteable){
        QLabel* label = new QLabel("settings.ini file is read-only! Settings changed won't persist!");
        label->setStyleSheet("font-style:italic; color:orange; font-weight:bold;");
        label->setAlignment(Qt::AlignCenter);
        vLayout->addWidget(label);
    }

    vLayout->addWidget(tabWidget, 1);

    //For each group in Settings.ini
    foreach(QString group, settings->childGroups()){
        //Open Group
        settings->beginGroup(group);

        //Setup the GroupWidget
        QWidget* groupWidget = new QWidget();
        QVBoxLayout* groupLayout = new QVBoxLayout();
        groupWidget->setLayout(groupLayout);

        groupLayouts[group] = groupLayout;



        //Make all of the layouts.


        QStringList visualGroups;
        foreach(QString keyName, settings->childKeys()){
            QString visualGroupName = "";
            if(keysVisualGroups.contains(keyName)){
                visualGroupName = keysVisualGroups[keyName];
            }
            if(!visualGroups.contains(visualGroupName)){
                visualGroups.append(visualGroupName);
            }
        }
        //Sort the groups.
        qSort(visualGroups.begin(), visualGroups.end(), qLess<QString>());

        //Construct Visual Group Boxes.
        foreach(QString visualGroup, visualGroups){
            QString visualGroupName = getReadableValue(visualGroup);
            QGroupBox* groupBox = new QGroupBox(visualGroupName, this);
            QVBoxLayout* visualLayout = new QVBoxLayout();
            visualLayout->setSpacing(0);
            groupBox->setLayout(visualLayout);
            groupLayout->addWidget(groupBox);
            //Insert into Hash.
            groupLayouts[group + "/" + visualGroup] = visualLayout;
        }
        groupLayout->addStretch();


        tabWidget->addTab(groupWidget, getReadableValue(group));


        int maxSize = -1;
        //For each key, construct a KeyEditWidget to change the setting of that key.
        foreach(QString key, settings->childKeys()){
            if(!settingsWidgetsHash.contains(key)){
                QString humanReadableKey =  getReadableValue(key);

                KeyEditWidget* keyEdit = new KeyEditWidget( group, key, humanReadableKey, settings->value(key));

                int labelWidth = keyEdit->getLabelWidth();
                if(labelWidth > maxSize){
                    maxSize = labelWidth;
                }
                QString tooltip = keysTooltips[key];
                if(tooltip != ""){
                    keyEdit->setToolTip(tooltip);
                }

                if(!keyToGroupMap.contains(key)){
                    keyToGroupMap.insert(key, group);
                }

                settingsWidgetsHash[key] = keyEdit;

                //Connect the valueChanged signal to this, to update the settings.ini file.
                connect(keyEdit, SIGNAL(valueChanged(QString,QString,QVariant)), this, SLOT(_settingChanged(QString,QString,QVariant)));


                //Check for groupName.
                QString visualGroup = keysVisualGroups[key];
                QVBoxLayout* vGroupLayout = groupLayouts[group + "/" + visualGroup];
                if(vGroupLayout){
                    vGroupLayout->addWidget(keyEdit);
                }
            }else{
                QMessageBox::critical(this, "Settings Error", "Settings file has 2 settings in .ini file with same Key Name! Ignoring duplicate Settings", QMessageBox::Ok);
            }
        }

        if(maxSize > 0){
            foreach(QString key, settings->childKeys()){
                 KeyEditWidget* keyEdit = settingsWidgetsHash[key];
                 if(keyEdit){
                     keyEdit->setLabelWidth(maxSize);
                 }
             }
        }

        //ADD A RESET THEME BUTTON.
        if(group == THEME_SETTINGS){
            QGroupBox* groupBox = new QGroupBox("Theme Presets");
            QHBoxLayout* groupBoxLayout = new QHBoxLayout();
            groupBox->setLayout(groupBoxLayout);
            QPushButton* darkTheme = new QPushButton("Dark Theme");
            QPushButton* lightTheme = new QPushButton("Light Theme");
            groupBoxLayout->addWidget(lightTheme, 1);
            groupBoxLayout->addWidget(darkTheme, 1);
            darkTheme->setStyleSheet(
                        "QPushButton{background:rgb(70,70,70); color: rgb(255,255,255);}"
                        "QPushButton:hover{background:rgb(255,165,70); color: rgb(0,0,0);}"
                        );
            lightTheme->setStyleSheet(
                        "QPushButton{background:rgb(170,170,170); color: rgb(0,0,0);}"
                        "QPushButton:hover{background:rgb(75,110,175); color: rgb(255,255,255);}"
                        );

            connect(darkTheme, SIGNAL(clicked()), this, SLOT(setDarkTheme()));
            connect(lightTheme, SIGNAL(clicked()), this, SLOT(setLightTheme()));
            groupLayout->insertWidget(groupLayout->count() - 1, groupBox);
        }else if(group == ASPECT_SETTINGS){
            QGroupBox* groupBox = new QGroupBox("Aspect Color Schemes");
            QHBoxLayout* groupBoxLayout = new QHBoxLayout();
            groupBox->setLayout(groupBoxLayout);

            QPushButton* defaultButton = new QPushButton("Default");
            QPushButton* colorBlindButton = new QPushButton("Color Blind");
            groupBoxLayout->addWidget(defaultButton,1);
            groupBoxLayout->addWidget(colorBlindButton,1);
            connect(defaultButton, SIGNAL(clicked()), this, SLOT(setAspectColor_Blind()));
            connect(colorBlindButton, SIGNAL(clicked()), this, SLOT(setAspectColor_Default()));

            groupLayout->insertWidget(groupLayout->count() - 1, groupBox);
        }

        settings->endGroup();
    }


    QWidget* widgetContainer = new QWidget();
    widgetContainer->setLayout(vLayout);

    scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(widgetContainer);
    mainLayout->addWidget(scrollArea,1);

    QHBoxLayout* buttonLayout = new QHBoxLayout();


    clearChangesButton = new QPushButton("Clear Changes");
    clearChangesButton->setObjectName("original");

    applyButton = new QPushButton("Apply Changes");
    applyButton->setObjectName("original");
    if(settingFileWriteable){
        applyButton->setToolTip("Updates settings.ini file.");
    }else{
        applyButton->setToolTip("Updates local setting for this apps lifecycle.");
    }

   // QPushButton* cancelButton = new QPushButton("Cancel");


    buttonLayout->setSpacing(2);
    buttonLayout->setMargin(2);

    buttonLayout->addStretch();
    buttonLayout->addWidget(clearChangesButton);
    buttonLayout->addWidget(applyButton);

    mainLayout->addLayout(buttonLayout);


    scrollArea->setStyleSheet("QScrollArea{border: none;}");

    connect(applyButton, SIGNAL(clicked()), this, SLOT(clearSettings()));
    connect(clearChangesButton, SIGNAL(clicked()), this, SLOT(clearChanges()));
}

void AppSettings::reject()
{
    clearSettings(false);
    QDialog::reject();
}
