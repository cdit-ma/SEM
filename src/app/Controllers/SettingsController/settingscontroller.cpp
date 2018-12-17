#include "settingscontroller.h"
#include "setting.h"
#include <QDebug>
#include <QApplication>
#include <QSettings>
#include <QStringBuilder>
#include <QVariant>

#include "../../theme.h"
#include "../../Widgets/Dialogs/appsettings.h"

SettingsController::SettingsController(QObject *parent) : QObject(parent)
{
    intializeSettings();

    settingsFile = new QSettings(QApplication::applicationDirPath() + "/settings.ini", QSettings::IniFormat);

    
    //Connect to the
    connect(Theme::theme(), &Theme::changeSetting, this, &SettingsController::setSetting);
    connect(this, &SettingsController::settingChanged, Theme::theme(), &Theme::settingChanged);
    

    //Place defaults in case nothing is set.
    emit settingChanged(SETTINGS::THEME_SETTHEME_XMASTHEME, true);
    emit settingChanged(SETTINGS::THEME_SETASPECT_COLORBLIND, true);

    loadSettingsFromFile();
    saveSettings();
}

SettingsController::~SettingsController()
{
    saveSettings();
    settingsFile->deleteLater();
}

QVariant SettingsController::getSetting(SETTINGS ID)
{
    if(settingsHash.contains(ID)){
        return settingsHash[ID]->getValue();
    }
    return QVariant();
}

void SettingsController::setSetting(SETTINGS ID, QVariant value)
{
    _setSetting(_getSetting(ID), value);
}

bool SettingsController::isWriteProtected()
{
    return !settingsFile->isWritable();
}

bool SettingsController::isThemeSetting(SETTINGS key)
{
    Setting* setting = _getSetting(key);
    return setting && setting->isThemeSetting();
}

QList<Setting *> SettingsController::getSettings()
{
    QList<Setting*> s;
    foreach(SETTINGS key, settingsKeys){
       if(settingsHash.contains(key)){
           s.append(settingsHash[key]);
       }
    }
    return s;
}

QList<SETTINGS> SettingsController::getSettingsKeys(QString category, QString section, QString name)
{
    QList<SETTINGS> keys;

    bool useCat = !category.isEmpty();
    bool useSect = !section.isEmpty();
    bool useName = !name.isEmpty();

    foreach(SETTINGS key, settingsKeys){
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
    createSetting(SETTINGS::GENERAL_MODEL_PATH, SETTING_TYPE::PATH, "General", "MEDEA", "Default Model path", "Icons", "folder");
    createSetting(SETTINGS::GENERAL_REGEN_PATH, SETTING_TYPE::PATH, "General", "MEDEA", "re_gen Path", "Icons", "folder");

    createSetting(SETTINGS::GENERAL_RE_CONFIGURE_PATH, SETTING_TYPE::FILE, "General", "Runtime Environment", "RE configure script path", "Icons", "file");
    createSetting(SETTINGS::GENERAL_CMAKE_GENERATOR, SETTING_TYPE::STRING, "General", "Runtime Environment", "CMake Generator", "Icons", "file");


    createSetting(SETTINGS::CHARTS_AGGREGATION_SERVER_ENDPOINT, SETTING_TYPE::STRING, "Charts", "Aggregation Server", "Aggregation Server Endpoint", "Icons", "cloudCircle");

    createSetting(SETTINGS::GENERAL_MEDEA_WIKI_URL, SETTING_TYPE::STRING, "General", "MEDEA", "MEDEA Wiki URL", "Icons", "book");
    createSetting(SETTINGS::GENERAL_SAVE_WINDOW_ON_EXIT, SETTING_TYPE::BOOL, "General", "MEDEA", "Save Window State on exit", "Icons", "floppyDisk");
    createSetting(SETTINGS::GENERAL_SAVE_DOCKS_ON_EXIT, SETTING_TYPE::BOOL, "General", "MEDEA", "Save Dock Widgets State on exit", "Icons", "floppyDisk");
    createSetting(SETTINGS::GENERAL_ZOOM_UNDER_MOUSE, SETTING_TYPE::BOOL, "General", "MEDEA", "Zoom to mouse", "Icons", "zoom");
    createSetting(SETTINGS::GENERAL_AUTOSAVE_DURATION, SETTING_TYPE::INT, "General", "MEDEA", "Autosave wait delay (Mins)", "Icons", "clockDark");
    
    createSetting(SETTINGS::GENERAL_RESET_SETTINGS, SETTING_TYPE::BUTTON, "General", "MEDEA", "Reset All Settings", "Icons", "bin");
    

    
    
    createSetting(SETTINGS::GENERAL_RECENT_PROJECTS, SETTING_TYPE::STRINGLIST, "General", "MEDEA", "Recent Projects");

    createSetting(SETTINGS::WINDOW_INNER_GEOMETRY, SETTING_TYPE::BYTEARRAY, "General", "Window", "Central Window Geometry");
    createSetting(SETTINGS::WINDOW_INNER_STATE, SETTING_TYPE::BYTEARRAY, "General", "Window", "Central Window State");
    createSetting(SETTINGS::WINDOW_OUTER_GEOMETRY, SETTING_TYPE::BYTEARRAY, "General", "Window", "Window Geometry");
    createSetting(SETTINGS::WINDOW_OUTER_STATE, SETTING_TYPE::BYTEARRAY, "General", "Window", "Window State");

    createSetting(SETTINGS::WINDOW_RIGHT_GEOMETRY, SETTING_TYPE::BYTEARRAY, "General", "Window", "Right Tool Window Geometry");
    createSetting(SETTINGS::WINDOW_RIGHT_STATE, SETTING_TYPE::BYTEARRAY, "General", "Window", "Right Tool Window State");

    


    /* TO REMOVE */

    //Toolbar - Visible Buttons
    createSetting(SETTINGS::TOOLBAR_CONTEXT, SETTING_TYPE::BOOL, "Toolbar", "Visible Buttons", "Show Context Toolbar", "Icons", "gearDark");
    createSetting(SETTINGS::TOOLBAR_UNDO, SETTING_TYPE::BOOL, "Toolbar", "Visible Buttons", "Undo", "Icons", "arrowUndo");
    createSetting(SETTINGS::TOOLBAR_REDO, SETTING_TYPE::BOOL, "Toolbar", "Visible Buttons", "Redo", "Icons", "arrowRedo");
    createSetting(SETTINGS::TOOLBAR_CUT, SETTING_TYPE::BOOL, "Toolbar", "Visible Buttons", "Cut", "Icons", "scissors");
    createSetting(SETTINGS::TOOLBAR_COPY, SETTING_TYPE::BOOL, "Toolbar", "Visible Buttons", "Copy", "Icons", "copy");
    createSetting(SETTINGS::TOOLBAR_PASTE, SETTING_TYPE::BOOL, "Toolbar", "Visible Buttons", "Paste", "Icons", "clipboard");
    createSetting(SETTINGS::TOOLBAR_REPLICATE, SETTING_TYPE::BOOL, "Toolbar", "Visible Buttons", "Replicate", "Icons", "copyList");
    createSetting(SETTINGS::TOOLBAR_FIT_TO_SCREEN, SETTING_TYPE::BOOL, "Toolbar", "Visible Buttons", "Fit To Screen", "Icons", "screenResize");
    createSetting(SETTINGS::TOOLBAR_CENTER_SELECTION, SETTING_TYPE::BOOL, "Toolbar", "Visible Buttons", "Center Selection", "Icons", "crosshair");
    createSetting(SETTINGS::TOOLBAR_VIEW_IN_NEWWINDOW, SETTING_TYPE::BOOL, "Toolbar", "Visible Buttons", "View In New Window", "Icons", "popOut");
    createSetting(SETTINGS::TOOLBAR_SEARCH, SETTING_TYPE::BOOL, "Toolbar", "Visible Buttons", "Search", "Icons", "zoom");
    createSetting(SETTINGS::TOOLBAR_DELETE, SETTING_TYPE::BOOL, "Toolbar", "Visible Buttons", "Delete", "Icons", "bin");
    createSetting(SETTINGS::TOOLBAR_ALIGN_HORIZONTAL, SETTING_TYPE::BOOL, "Toolbar", "Visible Buttons", "Align Horizontally", "Icons", "alignHorizontal");
    createSetting(SETTINGS::TOOLBAR_ALIGN_VERTICAL, SETTING_TYPE::BOOL, "Toolbar", "Visible Buttons", "Align Vertically", "Icons", "alignVertical");
    createSetting(SETTINGS::TOOLBAR_VALIDATE, SETTING_TYPE::BOOL, "Toolbar", "Visible Buttons", "Validate Model", "Icons", "shieldTick");
    createSetting(SETTINGS::TOOLBAR_CONTRACT, SETTING_TYPE::BOOL, "Toolbar", "Visible Buttons", "Expand Selection", "Icons", "triangleSouthEast");
    createSetting(SETTINGS::TOOLBAR_EXPAND, SETTING_TYPE::BOOL, "Toolbar", "Visible Buttons", "Contract Selection", "Icons", "triangleNorthWest");
     
     //Jenkins - Server
    createSetting(SETTINGS::JENKINS_URL, SETTING_TYPE::STRING, "Jenkins", "Server", "URL", "Icons", "globe");
    createSetting(SETTINGS::JENKINS_JOBNAME, SETTING_TYPE::STRING, "Jenkins", "Server", "Jobname", "Icons", "tag");

    //Jenkins - User
    createSetting(SETTINGS::JENKINS_USER, SETTING_TYPE::STRING, "Jenkins", "User", "Username", "Icons", "personCircle");
    createSetting(SETTINGS::JENKINS_API, SETTING_TYPE::STRING, "Jenkins", "User", "API Token", "Icons", "star");
    createSetting(SETTINGS::JENKINS_REQUEST_USER_JOBS, SETTING_TYPE::BOOL, "Jenkins", "User", "Request only jobs constructed by this user", "Icons", "filterList");
    createSetting(SETTINGS::JENKINS_TEST, SETTING_TYPE::BUTTON, "Jenkins", "User", "Test Settings", "Icons", "circleTickDark");
    

    createSetting(SETTINGS::THEME_SIZE_FONTSIZE, SETTING_TYPE::FONT, "Theme", "Size", "Select Font", "Icons", "format");
    createSetting(SETTINGS::THEME_SIZE_ICONSIZE, SETTING_TYPE::INT, "Theme", "Size", "Set Icon Size", "Icons", "zoomIn");
    createSetting(SETTINGS::THEME_INACTIVE_EDGE_OPACITY, SETTING_TYPE::PERCENTAGE, "Theme", "Size", "Set Inactive Edge Opacity", "Icons", "torch");
    

    
    createSetting(SETTINGS::THEME_SETTHEME_XMASTHEME, SETTING_TYPE::BUTTON, "Theme", "Theme Presets", "Christmas Theme");
    createSetting(SETTINGS::THEME_SETTHEME_DARKTHEME, SETTING_TYPE::BUTTON, "Theme", "Theme Presets", "Dark Theme");
    createSetting(SETTINGS::THEME_SETTHEME_LIGHTHEME, SETTING_TYPE::BUTTON, "Theme", "Theme Presets", "Light Theme");
    createSetting(SETTINGS::THEME_SETTHEME_SOLARIZEDDARKTHEME, SETTING_TYPE::BUTTON, "Theme", "Theme Presets", "Solarised Dark Theme");
    createSetting(SETTINGS::THEME_SETTHEME_SOLARIZEDLIGHTTHEME, SETTING_TYPE::BUTTON, "Theme", "Theme Presets", "Solarised Light Theme");

    //Theme - Default Colors
    createSetting(SETTINGS::THEME_BG_COLOR, SETTING_TYPE::COLOR, "Theme", "Default Colors", "Background");
    createSetting(SETTINGS::THEME_BG_ALT_COLOR, SETTING_TYPE::COLOR, "Theme", "Default Colors", "Alternative Background");
    createSetting(SETTINGS::THEME_TEXT_COLOR, SETTING_TYPE::COLOR, "Theme", "Default Colors", "Text");
    createSetting(SETTINGS::THEME_ALTERNATE_TEXT_COLOR, SETTING_TYPE::COLOR, "Theme", "Default Colors", "Alternate Text");

    createSetting(SETTINGS::THEME_ICON_COLOR, SETTING_TYPE::COLOR, "Theme", "Default Colors", "Icon");

    //Theme - Disabled Colors
    createSetting(SETTINGS::THEME_BG_DISABLED_COLOR, SETTING_TYPE::COLOR, "Theme", "Disabled Colors", "Background");
    createSetting(SETTINGS::THEME_TEXT_DISABLED_COLOR, SETTING_TYPE::COLOR, "Theme", "Disabled Colors", "Text");
    createSetting(SETTINGS::THEME_ICON_DISABLED_COLOR, SETTING_TYPE::COLOR, "Theme", "Disabled Colors", "Icon");

    //Theme - Selected Colors
    createSetting(SETTINGS::THEME_BG_SELECTED_COLOR, SETTING_TYPE::COLOR, "Theme", "Selected Colors", "Background");
    createSetting(SETTINGS::THEME_TEXT_SELECTED_COLOR, SETTING_TYPE::COLOR, "Theme", "Selected Colors", "Text");
    createSetting(SETTINGS::THEME_ICON_SELECTED_COLOR, SETTING_TYPE::COLOR, "Theme", "Selected Colors", "Icon");
    createSetting(SETTINGS::THEME_VIEW_BORDER_SELECTED_COLOR, SETTING_TYPE::COLOR, "Theme", "Selected Colors", "Active View Border");

    //Theme - Aspect Colors
    createSetting(SETTINGS::THEME_ASPECT_BG_INTERFACES_COLOR, SETTING_TYPE::COLOR, "Theme", "Aspect Colors", "Interfaces");
    createSetting(SETTINGS::THEME_ASPECT_BG_BEHAVIOUR_COLOR, SETTING_TYPE::COLOR, "Theme", "Aspect Colors", "Behaviour");
    createSetting(SETTINGS::THEME_ASPECT_BG_ASSEMBLIES_COLOR, SETTING_TYPE::COLOR, "Theme", "Aspect Colors", "Assemblies");
    createSetting(SETTINGS::THEME_ASPECT_BG_HARDWARE_COLOR, SETTING_TYPE::COLOR, "Theme", "Aspect Colors", "Hardware");
    createSetting(SETTINGS::THEME_SETASPECT_CLASSIC, SETTING_TYPE::BUTTON, "Theme", "Aspect Colors", "Classic");
    createSetting(SETTINGS::THEME_SETASPECT_COLORBLIND, SETTING_TYPE::BUTTON, "Theme", "Aspect Colors", "Color Blind");


    
    createSetting(SETTINGS::THEME_SEVERITY_RUNNING_COLOR, SETTING_TYPE::COLOR, "Theme", "Severity Colors", Notification::getSeverityString(Notification::Severity::RUNNING), "Notification", Notification::getSeverityString(Notification::Severity::RUNNING));
    createSetting(SETTINGS::THEME_SEVERITY_INFO_COLOR, SETTING_TYPE::COLOR, "Theme", "Severity Colors", Notification::getSeverityString(Notification::Severity::INFO), "Notification", Notification::getSeverityString(Notification::Severity::INFO));
    createSetting(SETTINGS::THEME_SEVERITY_WARNING_COLOR, SETTING_TYPE::COLOR, "Theme", "Severity Colors",  Notification::getSeverityString(Notification::Severity::WARNING), "Notification", Notification::getSeverityString(Notification::Severity::WARNING));
    createSetting(SETTINGS::THEME_SEVERITY_ERROR_COLOR, SETTING_TYPE::COLOR, "Theme", "Severity Colors",  Notification::getSeverityString(Notification::Severity::ERROR), "Notification", Notification::getSeverityString(Notification::Severity::ERROR));
    createSetting(SETTINGS::THEME_SEVERITY_SUCCESS_COLOR, SETTING_TYPE::COLOR, "Theme", "Severity Colors",  Notification::getSeverityString(Notification::Severity::SUCCESS), "Notification", Notification::getSeverityString(Notification::Severity::SUCCESS));
    

    createSetting(SETTINGS::THEME_APPLY, SETTING_TYPE::NONE, "Theme", "Theme", "Apply Theme");



    _getSetting(SETTINGS::GENERAL_MEDEA_WIKI_URL)->setDefaultValue("https://github.com/cdit-ma/MEDEA/wiki");
    _getSetting(SETTINGS::GENERAL_CMAKE_GENERATOR)->setDefaultValue("Ninja");
    _getSetting(SETTINGS::GENERAL_REGEN_PATH)->setDefaultValue("Resources/re_gen");


    _getSetting(SETTINGS::CHARTS_AGGREGATION_SERVER_ENDPOINT)->setDefaultValue("tcp://192.168.111.249:12345");

    


    _getSetting(SETTINGS::GENERAL_AUTOSAVE_DURATION)->setDefaultValue(3);
    _getSetting(SETTINGS::GENERAL_SAVE_WINDOW_ON_EXIT)->setDefaultValue(true);
    _getSetting(SETTINGS::GENERAL_SAVE_DOCKS_ON_EXIT)->setDefaultValue(false);
    
    
    _getSetting(SETTINGS::GENERAL_ZOOM_UNDER_MOUSE)->setDefaultValue(true);
    
    QFont font("Verdana");
    font.setStyleStrategy(QFont::PreferAntialias);
    font.setPointSizeF(9);
    
    _getSetting(SETTINGS::THEME_SIZE_FONTSIZE)->setDefaultValue(font);
    _getSetting(SETTINGS::THEME_SIZE_ICONSIZE)->setDefaultValue(16);
    _getSetting(SETTINGS::THEME_INACTIVE_EDGE_OPACITY)->setDefaultValue(33);

    _getSetting(SETTINGS::TOOLBAR_CONTEXT)->setDefaultValue(false);
    _getSetting(SETTINGS::TOOLBAR_SEARCH)->setDefaultValue(true);
    _getSetting(SETTINGS::TOOLBAR_UNDO)->setDefaultValue(true);
    _getSetting(SETTINGS::TOOLBAR_REDO)->setDefaultValue(true);
    _getSetting(SETTINGS::TOOLBAR_CUT)->setDefaultValue(true);
    _getSetting(SETTINGS::TOOLBAR_COPY)->setDefaultValue(true);
    _getSetting(SETTINGS::TOOLBAR_PASTE)->setDefaultValue(true);
    _getSetting(SETTINGS::TOOLBAR_REPLICATE)->setDefaultValue(true);
    _getSetting(SETTINGS::TOOLBAR_FIT_TO_SCREEN)->setDefaultValue(true);
    _getSetting(SETTINGS::TOOLBAR_CENTER_SELECTION)->setDefaultValue(true);
    _getSetting(SETTINGS::TOOLBAR_VIEW_IN_NEWWINDOW)->setDefaultValue(true);
    _getSetting(SETTINGS::TOOLBAR_DELETE)->setDefaultValue(true);
    _getSetting(SETTINGS::TOOLBAR_ALIGN_HORIZONTAL)->setDefaultValue(false);
    _getSetting(SETTINGS::TOOLBAR_ALIGN_VERTICAL)->setDefaultValue(false);
    _getSetting(SETTINGS::TOOLBAR_EXPAND)->setDefaultValue(true);
    _getSetting(SETTINGS::TOOLBAR_CONTRACT)->setDefaultValue(true);
    _getSetting(SETTINGS::TOOLBAR_VALIDATE)->setDefaultValue(true);
    

    _getSetting(SETTINGS::GENERAL_RECENT_PROJECTS)->setDefaultValue(QStringList());
    _getSetting(SETTINGS::JENKINS_JOBNAME)->setDefaultValue("deploy_model");
}

QString getSettingKey(Setting* setting){
    QString str;
    if(setting){
        str = QString::number(static_cast<uint>(setting->getID())) % "_" % setting->getName();
        str = str.toUpper();
        str.replace(" ", "_");
    }
    return str;
}

QString getSettingGroupKey(Setting* setting){
    QString str;
    if(setting){
        str = setting->getCategory() % "_" % setting->getSection();
        str = str.toUpper();
        str.replace(" ", "_");
    }
    return str;
}

void SettingsController::writeSetting(Setting* setting, QVariant value){
    if(setting && settingsFile){
        settingsFile->beginGroup(getSettingGroupKey(setting));
        settingsFile->setValue(getSettingKey(setting), value);
        settingsFile->endGroup();
    }
}
void SettingsController::loadSettingsFromFile()
{
    foreach(Setting* setting, settingsHash.values()){
        if(setting->getType() == SETTING_TYPE::BUTTON || setting->getType() == SETTING_TYPE::NONE){
            continue;
        }

        settingsFile->beginGroup(getSettingGroupKey(setting));
        auto file_value = settingsFile->value(getSettingKey(setting));
        settingsFile->endGroup();

        if(!file_value.isNull()){
            if(setting->getType() == SETTING_TYPE::COLOR){
                file_value = QColor(file_value.toString());
            }
            if(!file_value.isNull()){
                _setSetting(setting, file_value);
            }
        }else{
            writeSetting(setting, setting->getDefaultValue());
        }
        emit settingChanged(setting->getID(), setting->getValue());
    }
}

void SettingsController::_setSetting(Setting *setting, QVariant value)
{
    if(setting && setting->setValue(value)){
        emit settingChanged(setting->getID(), setting->getValue());

        if(setting->getID() == SETTINGS::GENERAL_RESET_SETTINGS){
            resetSettings();
        }
    }

}

Setting *SettingsController::createSetting(SETTINGS ID, SETTING_TYPE type, QString category, QString section, QString name, QString iconPath, QString iconName)
{
    if(!settingsHash.contains(ID)){
        Setting* setting = new Setting(ID, type, category, section, name);
        if(iconPath != "" && iconName != ""){
            setting->setIcon(iconPath, iconName);
        }
        settingsHash[ID] = setting;
        settingsKeys.append(ID);
        return setting;
    }else{
        qCritical() << "Duplicate setting created.";
    }
    return 0;
}

Setting *SettingsController::_getSetting(SETTINGS ID)
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
        connect(settingsGUI, &AppSettings::settingsApplied, this, &SettingsController::saveSettings);
        connect(settingsGUI, &AppSettings::settingsApplied, this, &SettingsController::settingsApplied);
    }
    settingsGUI->show();
}

void SettingsController::resetSettings()
{
    foreach(SETTINGS sk, settingsKeys){
        Setting* setting = _getSetting(sk);
        if(setting){
            if(setting->getType() == SETTING_TYPE::BUTTON || setting->getType() == SETTING_TYPE::NONE){
                continue;
            }
            //Reset to default
            _setSetting(setting, setting->getDefaultValue());
        }
    }
    //Reset Settings
    emit settingChanged(SETTINGS::THEME_SETTHEME_DARKTHEME, true);
    emit settingChanged(SETTINGS::THEME_SETASPECT_COLORBLIND, true);
    emit settingChanged(SETTINGS::THEME_APPLY, true);
    emit settingsApplied();
    //Write settings
    saveSettings();
}

void SettingsController::saveSettings()
{
    foreach(Setting* setting, settingsHash.values()){
        //Ignore writing Button and None Type settings
        if(setting->getType() == SETTING_TYPE::BUTTON || setting->getType() == SETTING_TYPE::NONE){
            continue;
        }

        QVariant value = setting->getValue();

        //Convert QColor to a String'd hex
        if(setting->getType() == SETTING_TYPE::COLOR){
            QColor color = value.value<QColor>();
            value = Theme::QColorToHex(color);
        }
        writeSetting(setting, value);
    }
    settingsFile->sync();
}

SettingsController *SettingsController::settings()
{
    static SettingsController controller;
    return &controller;
}

void SettingsController::initializeSettings()
{
    settings();
}

