#ifndef SETTINGSCONTROLLER_H
#define SETTINGSCONTROLLER_H

#include "../../enumerations.h"


#include <QSettings>
#include <QObject>

enum SETTING_KEY{
    SK_NONE,
    // GENERAL
    //SK_GENERAL_DEBUG_LOGGING,
    SK_GENERAL_MODEL_PATH,
    //SK_GENERAL_CUTS_CONFIGURE_PATH,
    SK_GENERAL_MEDEA_WIKI_URL,
    SK_GENERAL_WIDTH,
    SK_GENERAL_HEIGHT,
    SK_GENERAL_MAXIMIZED,
    SK_GENERAL_SAVE_WINDOW_ON_EXIT,
    //SK_GENERAL_SELECT_ON_CREATION,
    SK_GENERAL_ZOOM_UNDER_MOUSE,
    SK_GENERAL_RECENT_PROJECTS,
    SK_GENERAL_RESET_SETTINGS,

    // WINDOW
    SK_WINDOW_INTERFACES_VISIBLE,
    SK_WINDOW_BEHAVIOUR_VISIBLE,
    SK_WINDOW_ASSEMBLIES_VISIBLE,
    SK_WINDOW_HARDWARE_VISIBLE,
    SK_WINDOW_QOS_VISIBLE,
    SK_WINDOW_TABLE_VISIBLE,
    SK_WINDOW_MINIMAP_VISIBLE,
    SK_WINDOW_VIEW_MANAGER_VISIBLE,
    SK_WINDOW_TOOLBAR_VISIBLE,

    // TOOLBAR
    SK_TOOLBAR_CONTEXT,
    SK_TOOLBAR_UNDO,
    SK_TOOLBAR_REDO,
    SK_TOOLBAR_CUT,
    SK_TOOLBAR_COPY,
    SK_TOOLBAR_PASTE,
    SK_TOOLBAR_REPLICATE,
    SK_TOOLBAR_FIT_TO_SCREEN,
    SK_TOOLBAR_CENTER_SELECTION,
    SK_TOOLBAR_VIEW_IN_NEWWINDOW,
    //SK_TOOLBAR_SORT,
    SK_TOOLBAR_DELETE,
    SK_TOOLBAR_ALIGN_HORIZONTAL,
    SK_TOOLBAR_ALIGN_VERTICAL,
    SK_TOOLBAR_EXPAND,
    SK_TOOLBAR_CONTRACT,
    SK_TOOLBAR_SEARCH,

    // JENKINS
    SK_JENKINC_URL,
    SK_JENKINC_JOBNAME,
    SK_JENKINC_USER,
    SK_JENKINC_API,

    // THEME
    SK_THEME_BG_COLOR,
    SK_THEME_BG_ALT_COLOR,
    SK_THEME_TEXT_COLOR,
    SK_THEME_ALTERNATE_TEXT_COLOR,
    SK_THEME_ICON_COLOR,
    SK_THEME_BG_DISABLED_COLOR,
    SK_THEME_TEXT_DISABLED_COLOR,
    SK_THEME_ICON_DISABLED_COLOR,
    SK_THEME_BG_SELECTED_COLOR,
    SK_THEME_TEXT_SELECTED_COLOR,
    SK_THEME_ICON_SELECTED_COLOR,
    SK_THEME_VIEW_BORDER_SELECTED_COLOR,

    SK_THEME_ASPECT_BG_INTERFACES_COLOR,
    SK_THEME_ASPECT_BG_BEHAVIOUR_COLOR,
    SK_THEME_ASPECT_BG_ASSEMBLIES_COLOR,
    SK_THEME_ASPECT_BG_HARDWARE_COLOR,

    SK_THEME_SETTHEME_DARKTHEME,
    SK_THEME_SETTHEME_LIGHTHEME,
    SK_THEME_SETASPECT_CLASSIC,
    SK_THEME_SETASPECT_COLORBLIND,
    SK_THEME_APPLY
};


enum SETTING_TYPE{ST_NONE, ST_COLOR, ST_STRING, ST_BOOL, ST_INT, ST_PATH, ST_FILE, ST_BUTTON, ST_STRINGLIST};


class Setting;
class AppSettings;
class SettingsController : public QObject
{
    Q_OBJECT
protected:
    SettingsController(QObject *parent = 0);
    ~SettingsController();
public:
    QVariant getSetting(SETTING_KEY ID);
    bool isWriteProtected();
    bool isThemeSetting(SETTING_KEY key);
    QList<Setting*> getSettings();
    QList<SETTING_KEY> getSettingsKeys(QString category="", QString section="", QString name="");

signals:
    void settingsApplied();
    void settingChanged(SETTING_KEY ID, QVariant value);
public slots:
    void setSetting(SETTING_KEY ID, QVariant value);
    void showSettingsWidget();
    void resetSettings();


    void saveSettings();
private:
    void intializeSettings();
    void loadSettingsFromFile();

    void _setSetting(Setting* setting, QVariant value);
    Setting* createSetting(SETTING_KEY ID, SETTING_TYPE type, QString category, QString section, QString name);
    Setting* _getSetting(SETTING_KEY ID);

    QHash<SETTING_KEY, Setting*> settingsHash;
    QList<SETTING_KEY> settingsKeys;
    QSettings* settingsFile;
    AppSettings* settingsGUI;

public:
    static SettingsController* settings();
    static void teardownSettings();
    static void initializeSettings();
private:
    static SettingsController* settingsSingleton;
};

#endif // SETTINGSCONTROLLER_H
