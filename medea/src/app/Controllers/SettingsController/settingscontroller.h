#ifndef SETTINGSCONTROLLER_H
#define SETTINGSCONTROLLER_H

#include <QSettings>
#include <QObject>

enum class SETTINGS {
    NONE,
    
    // GENERAL
    GENERAL_MODEL_PATH,
    GENERAL_RE_CONFIGURE_PATH,
    GENERAL_REGEN_PATH,
    GENERAL_CMAKE_GENERATOR,
    GENERAL_MEDEA_WIKI_URL,
    GENERAL_AUTOSAVE_DURATION,
    GENERAL_SAVE_WINDOW_ON_EXIT,
    GENERAL_SAVE_DOCKS_ON_EXIT,
    GENERAL_RECENT_PROJECTS,
    GENERAL_RESET_SETTINGS,
    GENERAL_SHOW_WORKER_ASPECT,
    GENERAL_ZOOM_UNDER_MOUSE,
    GENERAL_ON_CONSTRUCTION_CENTER,
    GENERAL_ON_CONSTRUCTION_SELECT,

    CHARTS_AGGREGATION_BROKER_ENDPOINT,

    WINDOW_INNER_GEOMETRY,
    WINDOW_INNER_STATE,
    WINDOW_OUTER_GEOMETRY,
    WINDOW_OUTER_STATE,
    WINDOW_RIGHT_GEOMETRY,
    WINDOW_RIGHT_STATE,

    // TOOLBAR
    TOOLBAR_CONTEXT,
    TOOLBAR_UNDO,
    TOOLBAR_REDO,
    TOOLBAR_CUT,
    TOOLBAR_COPY,
    TOOLBAR_PASTE,
    TOOLBAR_REPLICATE,
    TOOLBAR_FIT_TO_SCREEN,
    TOOLBAR_CENTER_SELECTION,
    TOOLBAR_VIEW_IN_NEWWINDOW,
    TOOLBAR_DELETE,
    TOOLBAR_ALIGN_HORIZONTAL,
    TOOLBAR_ALIGN_VERTICAL,
    TOOLBAR_EXPAND,
    TOOLBAR_CONTRACT,
    TOOLBAR_SEARCH,
    TOOLBAR_VALIDATE,

    // JENKINS
    JENKINS_URL,
    JENKINS_JOBNAME,
    JENKINS_REQUEST_USER_JOBS,
    JENKINS_USER,
    JENKINS_API,
    JENKINS_TEST,

    // THEME
    THEME_SIZE_FONTSIZE,
    THEME_SIZE_ICONSIZE,
    THEME_BG_COLOR,
    THEME_BG_ALT_COLOR,
    THEME_TEXT_COLOR,
    THEME_ALTERNATE_TEXT_COLOR,
    THEME_ICON_COLOR,
    THEME_BG_DISABLED_COLOR,
    THEME_TEXT_DISABLED_COLOR,
    THEME_ICON_DISABLED_COLOR,
    THEME_BG_SELECTED_COLOR,
    THEME_TEXT_SELECTED_COLOR,
    THEME_ICON_SELECTED_COLOR,
    THEME_VIEW_BORDER_SELECTED_COLOR,
    THEME_INACTIVE_EDGE_OPACITY,

    THEME_ASPECT_BG_INTERFACES_COLOR,
    THEME_ASPECT_BG_BEHAVIOUR_COLOR,
    THEME_ASPECT_BG_ASSEMBLIES_COLOR,
    THEME_ASPECT_BG_HARDWARE_COLOR,

    THEME_SEVERITY_RUNNING_COLOR,
    THEME_SEVERITY_INFO_COLOR,
    THEME_SEVERITY_WARNING_COLOR,
    THEME_SEVERITY_ERROR_COLOR,
    THEME_SEVERITY_SUCCESS_COLOR,

    THEME_SETTHEME_DARKTHEME,
    THEME_SETTHEME_LIGHTHEME,
    THEME_SETTHEME_SOLARIZEDDARKTHEME,
    THEME_SETTHEME_SOLARIZEDLIGHTTHEME,
    
    THEME_SETASPECT_CLASSIC,
    THEME_SETASPECT_COLORBLIND,
    THEME_APPLY
};

enum class SETTING_TYPE {
    NONE,
    COLOR,
    STRING,
    BOOL,
    FONT,
    INT,
    PATH,
    FILE,
    BUTTON,
    STRINGLIST,
    BYTEARRAY,
    PERCENTAGE
};

class Setting;
class AppSettings;
class SettingsController : public QObject
{
    Q_OBJECT
    
protected:
    explicit SettingsController(QObject* parent = nullptr);
    ~SettingsController() final;
    
public:
	QVariant getSetting(SETTINGS ID);
	QList<Setting*> getSettings();
	QList<SETTINGS> getSettingsKeys(const QString& category = "", const QString& section = "", const QString& name = "");
	
	bool isWriteProtected();
	bool isThemeSetting(SETTINGS key);
    
    static SettingsController* settings();

signals:
    void settingsApplied();
    void settingChanged(SETTINGS ID, QVariant value);
    
public slots:
    void setSetting(SETTINGS ID, const QVariant& value);
    void showSettingsWidget();
    void resetSettings();
    void saveSettings();
    
private:
    void initializeSettings();
    void loadSettingsFromFile();
    void writeSetting(Setting* setting, const QVariant& value);

    void _setSetting(Setting* setting, const QVariant& value);
    Setting* createSetting(SETTINGS ID, SETTING_TYPE type, const QString& category, const QString& section, const QString& name, const QString& icon_path = "", const QString& icon_name = "");
    Setting* _getSetting(SETTINGS ID);

    QHash<SETTINGS, Setting*> settingsHash;
    QList<SETTINGS> settingsKeys;
    
    QSettings* settingsFile = nullptr;
    AppSettings* settingsGUI = nullptr;
};

inline uint qHash(SETTINGS key, uint seed)
{
    return ::qHash(static_cast<uint>(key), seed);
}
Q_DECLARE_METATYPE(SETTINGS)

#endif // SETTINGSCONTROLLER_H
