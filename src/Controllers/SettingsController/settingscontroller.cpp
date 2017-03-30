#include "settingscontroller.h"
#include "setting.h"
#include <QDebug>
#include <QApplication>
#include <QSettings>
#include <QStringBuilder>
#include <QVariant>

#include "../../theme.h"
#include "../../Widgets/Dialogs/appsettings.h"

SettingsController* SettingsController::settingsSingleton = 0;

SettingsController::SettingsController(QObject *parent) : QObject(parent)
{
    //Register the settings key.
    qRegisterMetaType<SETTING_KEY>("SETTING_KEY");

    settingsGUI = 0;
    settingsFile = new QSettings(QApplication::applicationDirPath() + "/settings.ini", QSettings::IniFormat);

    intializeSettings();

    //Connect to the
    connect(Theme::theme(), &Theme::changeSetting, this, &SettingsController::setSetting);
    connect(this, &SettingsController::settingChanged, Theme::theme(), &Theme::settingChanged);


    //Place defaults in case nothing is set.
    emit settingChanged(SK_THEME_SETTHEME_DARKTHEME, true);
    emit settingChanged(SK_THEME_SETASPECT_COLORBLIND, true);

    loadSettingsFromFile();
}

SettingsController::~SettingsController()
{
    saveSettings();
    settingsFile->deleteLater();
}

QVariant SettingsController::getSetting(SETTING_KEY ID)
{
    if(settingsHash.contains(ID)){
        return settingsHash[ID]->getValue();
    }
    return QVariant();
}

void SettingsController::setSetting(SETTING_KEY ID, QVariant value)
{
    _setSetting(_getSetting(ID), value);
}

bool SettingsController::isWriteProtected()
{
    return !settingsFile->isWritable();
}

bool SettingsController::isThemeSetting(SETTING_KEY key)
{
    Setting* setting = _getSetting(key);
    return setting && setting->isThemeSetting();
}

QList<Setting *> SettingsController::getSettings()
{
    QList<Setting*> s;
    foreach(SETTING_KEY key, settingsKeys){
       if(settingsHash.contains(key)){
           s.append(settingsHash[key]);
       }
    }
    return s;
}

QList<SETTING_KEY> SettingsController::getSettingsKeys(QString category, QString section, QString name)
{
    QList<SETTING_KEY> keys;

    bool useCat = !category.isEmpty();
    bool useSect = !section.isEmpty();
    bool useName = !name.isEmpty();

    foreach(SETTING_KEY key, settingsKeys){
        Setting* s = _getSetting(key);

        if(s){
            if(useCat && s->getCategory() != category){
                continue;
            }else if(useSect && s->getSection() != section){
                continue;
            }else if(useName && s->getName() != name){
                continue;
            }
            keys.append(key);
        }
    }
    return keys;
}

void SettingsController::intializeSettings()
{
    //General
    createSetting(SK_GENERAL_DEBUG_LOGGING, ST_BOOL, "General", "", "Log debugging information");

    createSetting(SK_GENERAL_MODEL_PATH, ST_PATH, "General", "", "Default Model path");
    createSetting(SK_GENERAL_CUTS_CONFIGURE_PATH, ST_FILE, "General", "", "CUTS configure script file path");

    createSetting(SK_GENERAL_MEDEA_WIKI_URL, ST_STRING, "General", "", "MEDEA Wiki URL");



    //General - Window
    createSetting(SK_GENERAL_WIDTH, ST_INT, "General", "Window", "Initial MEDEA window width");
    createSetting(SK_GENERAL_HEIGHT, ST_INT, "General", "Window", "Initial MEDEA window height");
    createSetting(SK_GENERAL_MAXIMIZED, ST_BOOL, "General", "Window", "Launch MEDEA maximized");
    createSetting(SK_GENERAL_SAVE_WINDOW_ON_EXIT, ST_BOOL, "General", "Window", "Save window state on exit");



    //General - Controls
    createSetting(SK_GENERAL_SELECT_ON_CREATION, ST_BOOL, "General", "Controls", "Select Entity on creation");
    createSetting(SK_GENERAL_ZOOM_UNDER_MOUSE, ST_BOOL, "General", "Controls", "Zoom to mouse");

    createSetting(SK_GENERAL_RECENT_PROJECTS, ST_STRINGLIST, "General", "", "Recent Projects");

    createSetting(SK_GENERAL_RESET_SETTINGS, ST_BUTTON, "General", "", "Reset All Settings");



    //Window - Views
    createSetting(SK_WINDOW_INTERFACES_VISIBLE, ST_BOOL, "Window", "Views", "Show Interfaces on launch");
    createSetting(SK_WINDOW_BEHAVIOUR_VISIBLE, ST_BOOL, "Window", "Views", "Show Behaviour on launch");
    createSetting(SK_WINDOW_ASSEMBLIES_VISIBLE, ST_BOOL, "Window", "Views", "Show Assemblies on launch");
    createSetting(SK_WINDOW_HARDWARE_VISIBLE, ST_BOOL, "Window", "Views", "Show Hardware on launch");
    createSetting(SK_WINDOW_QOS_VISIBLE, ST_BOOL, "Window", "Views", "Show QOS Browser on launch");






    //Window - Tools
    createSetting(SK_WINDOW_TABLE_VISIBLE, ST_BOOL, "Window", "Tools", "Show Table on launch");
    createSetting(SK_WINDOW_MINIMAP_VISIBLE, ST_BOOL, "Window", "Tools", "Show Minimap on launch");
    createSetting(SK_WINDOW_VIEW_MANAGER_VISIBLE, ST_BOOL, "Window", "Tools", "Show View Manager on launch");
    createSetting(SK_WINDOW_TOOLBAR_VISIBLE, ST_BOOL, "Window", "Tools", "Show Toolbar on launch");






    //Toolbar - Visible Buttons
    createSetting(SK_TOOLBAR_CONTEXT, ST_BOOL, "Toolbar", "Visible Buttons", "Show Context Toolbar");
    createSetting(SK_TOOLBAR_UNDO, ST_BOOL, "Toolbar", "Visible Buttons", "Undo");
    createSetting(SK_TOOLBAR_REDO, ST_BOOL, "Toolbar", "Visible Buttons", "Redo");
    createSetting(SK_TOOLBAR_CUT, ST_BOOL, "Toolbar", "Visible Buttons", "Cut");
    createSetting(SK_TOOLBAR_COPY, ST_BOOL, "Toolbar", "Visible Buttons", "Copy");
    createSetting(SK_TOOLBAR_PASTE, ST_BOOL, "Toolbar", "Visible Buttons", "Paste");
    createSetting(SK_TOOLBAR_REPLICATE, ST_BOOL, "Toolbar", "Visible Buttons", "Replicate");
    createSetting(SK_TOOLBAR_FIT_TO_SCREEN, ST_BOOL, "Toolbar", "Visible Buttons", "Fit To Screen");
    createSetting(SK_TOOLBAR_CENTER_SELECTION, ST_BOOL, "Toolbar", "Visible Buttons", "Center Selection");
    createSetting(SK_TOOLBAR_VIEW_IN_NEWWINDOW, ST_BOOL, "Toolbar", "Visible Buttons", "View In New Window");
    createSetting(SK_TOOLBAR_SORT, ST_BOOL, "Toolbar", "Visible Buttons", "Sort");
    createSetting(SK_TOOLBAR_SEARCH, ST_BOOL, "Toolbar", "Visible Buttons", "Search");
    createSetting(SK_TOOLBAR_DELETE, ST_BOOL, "Toolbar", "Visible Buttons", "Delete");
    createSetting(SK_TOOLBAR_ALIGN_HORIZONTAL, ST_BOOL, "Toolbar", "Visible Buttons", "Align Horizontally");
    createSetting(SK_TOOLBAR_ALIGN_VERTICAL, ST_BOOL, "Toolbar", "Visible Buttons", "Align Vertically");

    createSetting(SK_TOOLBAR_CONTRACT, ST_BOOL, "Toolbar", "Visible Buttons", "Expand Selection");
    createSetting(SK_TOOLBAR_EXPAND, ST_BOOL, "Toolbar", "Visible Buttons", "Contract Selection");





     //Jenkins - Server
    createSetting(SK_JENKINS_URL, ST_STRING, "Jenkins", "Server", "URL");
    createSetting(SK_JENKINS_JOBNAME, ST_STRING, "Jenkins", "Server", "Jobname");

    //Jenkins - User
    createSetting(SK_JENKINS_USER, ST_STRING, "Jenkins", "User", "Username");
    createSetting(SK_JENKINS_PASSWORD, ST_STRING, "Jenkins", "User", "Password");
    createSetting(SK_JENKINS_API, ST_STRING, "Jenkins", "User", "API Token");

    createSetting(SK_THEME_SETTHEME_DARKTHEME, ST_BUTTON, "Theme", "Theme Presets", "Dark Theme");
    createSetting(SK_THEME_SETTHEME_LIGHTHEME, ST_BUTTON, "Theme", "Theme Presets", "Light Theme");

    //Theme - Default Colors
    createSetting(SK_THEME_BG_COLOR, ST_COLOR, "Theme", "Default Colors", "Background");
    createSetting(SK_THEME_BG_ALT_COLOR, ST_COLOR, "Theme", "Default Colors", "Alternative Background");
    createSetting(SK_THEME_TEXT_COLOR, ST_COLOR, "Theme", "Default Colors", "Text");
    createSetting(SK_THEME_ALTERNATE_TEXT_COLOR, ST_COLOR, "Theme", "Default Colors", "Alternate Text");

    createSetting(SK_THEME_ICON_COLOR, ST_COLOR, "Theme", "Default Colors", "Icon");

    //Theme - Disabled Colors
    createSetting(SK_THEME_BG_DISABLED_COLOR, ST_COLOR, "Theme", "Disabled Colors", "Background");
    createSetting(SK_THEME_TEXT_DISABLED_COLOR, ST_COLOR, "Theme", "Disabled Colors", "Text");
    createSetting(SK_THEME_ICON_DISABLED_COLOR, ST_COLOR, "Theme", "Disabled Colors", "Icon");

    //Theme - Selected Colors
    createSetting(SK_THEME_BG_SELECTED_COLOR, ST_COLOR, "Theme", "Selected Colors", "Background");
    createSetting(SK_THEME_TEXT_SELECTED_COLOR, ST_COLOR, "Theme", "Selected Colors", "Text");
    createSetting(SK_THEME_ICON_SELECTED_COLOR, ST_COLOR, "Theme", "Selected Colors", "Icon");
    createSetting(SK_THEME_VIEW_BORDER_SELECTED_COLOR, ST_COLOR, "Theme", "Selected Colors", "Active View Border");

    //Theme - Aspect Colors
    createSetting(SK_THEME_ASPECT_BG_INTERFACES_COLOR, ST_COLOR, "Theme", "Aspect Colors", "Interfaces");
    createSetting(SK_THEME_ASPECT_BG_BEHAVIOUR_COLOR, ST_COLOR, "Theme", "Aspect Colors", "Behaviour");
    createSetting(SK_THEME_ASPECT_BG_ASSEMBLIES_COLOR, ST_COLOR, "Theme", "Aspect Colors", "Assemblies");
    createSetting(SK_THEME_ASPECT_BG_HARDWARE_COLOR, ST_COLOR, "Theme", "Aspect Colors", "Hardware");
    createSetting(SK_THEME_SETASPECT_CLASSIC, ST_BUTTON, "Theme", "Aspect Colors", "Classic");
    createSetting(SK_THEME_SETASPECT_COLORBLIND, ST_BUTTON, "Theme", "Aspect Colors", "Color Blind");




    createSetting(SK_THEME_APPLY, ST_NONE, "Theme", "Theme", "Apply Theme");



    _getSetting(SK_GENERAL_WIDTH)->setDefaultValue(1200);
    _getSetting(SK_GENERAL_MEDEA_WIKI_URL)->setDefaultValue("https://github.com/cdit-ma/MEDEA/wiki");


    _getSetting(SK_GENERAL_HEIGHT)->setDefaultValue(700);
    _getSetting(SK_GENERAL_SAVE_WINDOW_ON_EXIT)->setDefaultValue(true);
    _getSetting(SK_GENERAL_SELECT_ON_CREATION)->setDefaultValue(true);
    _getSetting(SK_GENERAL_ZOOM_UNDER_MOUSE)->setDefaultValue(true);
    _getSetting(SK_GENERAL_DEBUG_LOGGING)->setDefaultValue(false);
    _getSetting(SK_GENERAL_MAXIMIZED)->setDefaultValue(false);


    _getSetting(SK_WINDOW_INTERFACES_VISIBLE)->setDefaultValue(true);
    _getSetting(SK_WINDOW_BEHAVIOUR_VISIBLE)->setDefaultValue(true);
    _getSetting(SK_WINDOW_ASSEMBLIES_VISIBLE)->setDefaultValue(true);
    _getSetting(SK_WINDOW_HARDWARE_VISIBLE)->setDefaultValue(true);
    _getSetting(SK_WINDOW_QOS_VISIBLE)->setDefaultValue(false);


    _getSetting(SK_WINDOW_TABLE_VISIBLE)->setDefaultValue(true);
    _getSetting(SK_WINDOW_MINIMAP_VISIBLE)->setDefaultValue(true);
    _getSetting(SK_WINDOW_VIEW_MANAGER_VISIBLE)->setDefaultValue(true);
    _getSetting(SK_WINDOW_TOOLBAR_VISIBLE)->setDefaultValue(true);

    _getSetting(SK_TOOLBAR_CONTEXT)->setDefaultValue(false);
    _getSetting(SK_TOOLBAR_SEARCH)->setDefaultValue(true);
    _getSetting(SK_TOOLBAR_UNDO)->setDefaultValue(true);
    _getSetting(SK_TOOLBAR_REDO)->setDefaultValue(true);
    _getSetting(SK_TOOLBAR_CUT)->setDefaultValue(true);
    _getSetting(SK_TOOLBAR_COPY)->setDefaultValue(true);
    _getSetting(SK_TOOLBAR_PASTE)->setDefaultValue(true);
    _getSetting(SK_TOOLBAR_REPLICATE)->setDefaultValue(true);
    _getSetting(SK_TOOLBAR_FIT_TO_SCREEN)->setDefaultValue(true);
    _getSetting(SK_TOOLBAR_CENTER_SELECTION)->setDefaultValue(true);
    _getSetting(SK_TOOLBAR_VIEW_IN_NEWWINDOW)->setDefaultValue(true);
    _getSetting(SK_TOOLBAR_SORT)->setDefaultValue(false);
    _getSetting(SK_TOOLBAR_DELETE)->setDefaultValue(true);
    _getSetting(SK_TOOLBAR_ALIGN_HORIZONTAL)->setDefaultValue(true);
    _getSetting(SK_TOOLBAR_ALIGN_VERTICAL)->setDefaultValue(true);
    _getSetting(SK_TOOLBAR_EXPAND)->setDefaultValue(true);
    _getSetting(SK_TOOLBAR_CONTRACT)->setDefaultValue(true);

    _getSetting(SK_GENERAL_RECENT_PROJECTS)->setDefaultValue(QStringList());

    _getSetting(SK_JENKINS_JOBNAME)->setDefaultValue("MEDEA-SEM");

}

void SettingsController::loadSettingsFromFile()
{
    foreach(Setting* setting, settingsHash.values()){
        if(setting->getType() == ST_BUTTON || setting->getType() == ST_NONE){
            continue;
        }
        QString settingKey = setting->getSettingString();

        if(settingsFile->contains(settingKey)){
            QVariant fileValue = settingsFile->value(settingKey);
            if(setting->getType() == ST_COLOR){
                fileValue = QColor(fileValue.toString());
            }
            if(!fileValue.isNull()){
                _setSetting(setting, fileValue);
            }
        }else{
            settingsFile->setValue(settingKey, setting->getDefaultValue());
        }
    }
}

void SettingsController::_setSetting(Setting *setting, QVariant value)
{
    if(setting && setting->setValue(value)){
        emit settingChanged(setting->getID(), setting->getValue());

        if(setting->getID() == SK_GENERAL_RESET_SETTINGS){
            resetSettings();
        }
    }

}

Setting *SettingsController::createSetting(SETTING_KEY ID, SETTING_TYPE type, QString category, QString section, QString name)
{
    if(!settingsHash.contains(ID)){
        Setting* setting = new Setting(ID, type, category, section, name);
        settingsHash[ID] = setting;
        settingsKeys.append(ID);
        return setting;
    }else{
        qCritical() << "Duplicate setting created.";
    }
    return 0;
}

Setting *SettingsController::_getSetting(SETTING_KEY ID)
{
    if(settingsHash.contains(ID)){
        return settingsHash[ID];
    }
    return 0;
}

void SettingsController::showSettingsWidget()
{
    if(!settingsGUI){
        settingsGUI = new AppSettings(0);
        connect(settingsGUI, &AppSettings::setSetting, this, &SettingsController::setSetting);
        connect(this, &SettingsController::settingChanged, settingsGUI, &AppSettings::settingChanged);
        connect(settingsGUI, &AppSettings::settingsApplied, this, &SettingsController::settingsApplied);
    }
    settingsGUI->show();
}

void SettingsController::resetSettings()
{
    foreach(SETTING_KEY sk, settingsKeys){
        Setting* setting = _getSetting(sk);
        if(setting){
            if(setting->getType() == ST_BUTTON || setting->getType() == ST_NONE){
                continue;
            }
            //Reset to default
            _setSetting(setting, setting->getDefaultValue());
        }
    }
    //Reset Settings
    emit settingChanged(SK_THEME_SETTHEME_DARKTHEME, true);
    emit settingChanged(SK_THEME_SETASPECT_COLORBLIND, true);
    emit settingChanged(SK_THEME_APPLY, true);
    emit settingsApplied();
}

void SettingsController::saveSettings()
{
    foreach(Setting* setting, settingsHash.values()){
        //Ignore writing Button and None Type settings
        if(setting->getType() == ST_BUTTON || setting->getType() == ST_NONE){
            continue;
        }

        QVariant value = setting->getValue();

        //Convert QColor to a String'd hex
        if(setting->getType() == ST_COLOR){
            QColor color = value.value<QColor>();
            value = Theme::QColorToHex(color);
        }
        settingsFile->setValue(setting->getSettingString(), value);
    }
    settingsFile->sync();
}

SettingsController *SettingsController::settings()
{
    if(!settingsSingleton){
        settingsSingleton = new SettingsController();
    }
    return settingsSingleton;
}

void SettingsController::teardownSettings()
{
    if(settingsSingleton){
        delete settingsSingleton;
    }
}

void SettingsController::initializeSettings()
{
    settings();
}

