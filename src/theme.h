#ifndef THEME_H
#define THEME_H

#include <QHash>
#include <QPixmap>
#include <QThread>
#include <QColor>
#include <QIcon>
#include <QReadWriteLock>
#include <QStringBuilder>
#include <QMovie>
#include <QObject>
#include <QFuture>
#include <QProxyStyle>

#include "ModelController/nodekinds.h"
#include "enumerations.h"
#include "Controllers/SettingsController/settingscontroller.h"
#include "Controllers/NotificationManager/notificationenumerations.h"


#define THEME_STYLE_QMENU "THEME_STYLE_QMENU"
#define THEME_STYLE_QPUSHBUTTON "THEME_STYLE_QPUSHBUTTON"
#define THEME_STYLE_QPUSHBUTTON_JENKINS "THEME_STYLE_QPUSHBUTTON_JENKINS"
#define THEME_STYLE_GROUPBOX "THEME_STYLE_GROUPBOX"
#define THEME_STYLE_HIDDEN_TOOLBAR "HIDDEN_TOOLBAR"

typedef QPair<QString, QString> IconPair;
Q_DECLARE_METATYPE(IconPair); 

class Theme: public QObject
{
Q_OBJECT
protected:
    Theme();
    ~Theme();
public:

    enum COLOR_ROLE{CR_NONE, CR_NORMAL, CR_DISABLED, CR_SELECTED};

    QColor white();
    QColor black();

    QColor getBackgroundColor();
    QString getBackgroundColorHex();

    QColor getDisabledBackgroundColor();
    QString getDisabledBackgroundColorHex();

    QColor getAltBackgroundColor();
    QString getAltBackgroundColorHex();

    QColor getHighlightColor();
    QString getHighlightColorHex();

    QColor getPressedColor();
    QString getPressedColorHex();

    QMovie* getGif(QString path, QString name);

    QColor getSelectedItemBorderColor();
    QString getSelectedItemBorderColorHex();

    QColor getActiveWidgetBorderColor();
    QString getActiveWidgetBorderColorHex();

    QColor getTextColor(COLOR_ROLE role = CR_NORMAL);
    QString getTextColorHex(COLOR_ROLE role = CR_NORMAL);

    QColor getMenuIconColor(COLOR_ROLE role = CR_NORMAL);
    QString getMenuIconColorHex(COLOR_ROLE role = CR_NORMAL);

    QColor getAspectBackgroundColor(VIEW_ASPECT aspect);
    QString getAspectBackgroundColorHex(VIEW_ASPECT aspect);

    QColor getDefaultImageTintColor();
    QString getDefaultImageTintColorHex();

    QColor getMainImageColor(QString prefix, QString alias);
    QColor getMainImageColor(IconPair path);

    QIcon getIcon(IconPair icon);
    QIcon getIcon(QString prefix, QString alias);

private:
    QPixmap _getPixmap(QString resourceName, QSize size = QSize(), QColor tintColor = QColor());
public:
    QPixmap getImage(QString prefix, QString alias, QSize size = QSize(), QColor tintColor = QColor());

    QString getPixmapResourceName(QString resource_name, QSize size, QColor tintColor);
    QColor getAltTextColor();
    QString getAltTextColorHex();
    void setAltTextColor(QColor color);

    void setBackgroundColor(QColor color);
    void setDisabledBackgroundColor(QColor color);
    void setAltBackgroundColor(QColor color);
    void setHighlightColor(QColor color);
    void setActiveWidgetBorderColor(QColor color);
    void setTextColor(COLOR_ROLE role, QColor color);
    void setMenuIconColor(COLOR_ROLE role, QColor color);
    void setAspectBackgroundColor(VIEW_ASPECT aspect, QColor color);
    void setIconToggledImage(QString prefix, QString alias, QString toggledOnPrefix, QString toggledOnAlias, QString toggledOffPrefix, QString toggleOffAlias, bool ignore_toggle_coloring = true);

    void setWindowIcon(QString window_title, QString visible_icon_prefix, QString visible_icon_alias);

    void setIconAlias(QString prefix, QString alias, QString icon_prefix, QString icon_alias);
    void setDefaultImageTintColor(QColor color);
    void setDefaultImageTintColor(QString prefix, QString alias, QColor color);

    void setSeverityColor(Notification::Severity severity, QColor color);
    QColor getSeverityColor(Notification::Severity);

    void applyTheme();
    bool isValid() const;

    bool gotImage(IconPair icon);
    bool gotImage(QString, QString);

    QString getBorderWidth();
    QString getCornerRadius();
    QString getSharpCornerRadius();

    // Default StyleSheets
    QString getWindowStyleSheet();
    QString getScrollBarStyleSheet();
    QString getDialogStyleSheet();
    QString getSplitterStyleSheet();
    QString getWidgetStyleSheet(QString widgetName="QWidget");
    QString getTabbedWidgetStyleSheet();
    QString getNodeViewStyleSheet(bool isActive=false);
    QString getDockWidgetStyleSheet();
    QString getViewDockWidgetStyleSheet(bool isActive=false);
    QString getDockTitleBarStyleSheet(bool isActive=false, QString widgetName="DockTitleBar");
    QString getToolDockWidgetTitleBarStyleSheet();
    QString getMenuBarStyleSheet();
    QString getMenuStyleSheet(int icon_size = 32);
    QString getToolBarStyleSheet();
    QString getAbstractItemViewStyleSheet();
    QString getAltAbstractItemViewStyleSheet();
    QString getComboBoxStyleSheet();
    QString getGroupBoxStyleSheet();
    QString getPushButtonStyleSheet();
    QString getToolButtonStyleSheet();
    QString getLineEditStyleSheet(QString widget_name="QLineEdit");
    QString getTextEditStyleSheet();
    QString getRadioButtonStyleSheet();
    QString getMessageBoxStyleSheet();
    QString getPopupWidgetStyleSheet();
    QString getProgressBarStyleSheet();
    QString getLabelStyleSheet();
    QString getTitleLabelStyleSheet();
    QString getAspectButtonStyleSheet(VIEW_ASPECT aspect);

signals:
    void theme_Changed();
    void changeSetting(SETTINGS setting, QVariant value);
public slots:
    void settingChanged(SETTINGS setting, QVariant value);
private:
    void clearIconMap();
    void preloadImages();
    QColor calculateImageColor(QImage image);
    QString getResourceName(QString prefix, QString alias) const;
    QString getResourceName(IconPair icon) const;
    void resetTheme(VIEW_THEME themePreset);
    void resetAspectTheme(bool colorBlind);
    void setupToggledIcons();
    void setupAliasIcons();
    void updateValid();

    QImage getImage(QString resource_name);
    QColor getTintColor(QString resource_name);
    QSize getOriginalSize(QString resource_name);

    IconPair splitImagePath(QString path);

    bool tintIcon(IconPair pair);
    bool tintIcon(QString prefix, QString alias);
    bool tintIcon(QSize size);
    bool tintIcon(QString resource_name);

    QSet<QString> image_names;

    QHash<QString, QMovie*> gifLookup;
    QHash<QString, QImage> imageLookup;
    QHash<QString, QPixmap> pixmapLookup;
    QHash<QString, QIcon> iconLookup;

    QHash<QString, QSize> pixmapSizeLookup;
    QHash<QString, QColor> pixmapTintLookup;

    struct IconToggle{
        IconPair on;
        IconPair off;
        bool got_toggle = false;

        COLOR_ROLE on_normal = CR_SELECTED;
        COLOR_ROLE on_active = CR_SELECTED;
        COLOR_ROLE on_disabled = CR_DISABLED;

        COLOR_ROLE off_normal = CR_NORMAL;
        COLOR_ROLE off_active = CR_SELECTED;
        COLOR_ROLE off_disabled = CR_DISABLED;
    };

    QHash<QString, QPair<IconPair, IconPair> > iconToggledLookup;
    QHash<QString, IconToggle > iconToggledLookup2;
    QHash<QString, QColor> pixmapMainColorLookup;

    QHash<VIEW_ASPECT, QColor> aspectColor;

    QHash<Notification::Severity, QColor> severityColor;

    QHash<COLOR_ROLE, QColor> textColor;
    QHash<COLOR_ROLE, QColor> menuIconColor;

    QHash<QString, QString> icon_alias_lookup;

    QColor highlightColor;
    QColor backgroundColor;
    QColor altBackgroundColor;
    QColor disabledBackgroundColor;
    QColor iconColor;
    QColor selectedItemBorderColor;
    QColor selectedWidgetBorderColor;
    QColor altTextColor;

    QReadWriteLock lock_;


    bool themeChanged = false;
    bool valid = false;

    QFuture<void> preloadThread;
public:
    static QColor blendColors(const QColor color1, const QColor color2, qreal blendRatio=0.5);
    static QString QColorToHex(const QColor color);
    static Theme* theme();
    static void teardownTheme();
    static QSize roundQSize(QSize size);
    static IconPair getIconPair(QString prefix, QString alias);

    static void UpdateActionIcon(QAction* action, Theme* theme = 0);
    static void StoreActionIcon(QAction* action, QString alias, QString name);
    static void StoreActionIcon(QAction* action, IconPair icon);
private:
    static Theme* themeSingleton;
};


class CustomMenuStyle : public QProxyStyle{
    public:
        CustomMenuStyle(int icon_size = 32);
        int pixelMetric(PixelMetric metric, const QStyleOption* option = 0, const QWidget* widget = 0) const;
    private:
        int icon_size = 32;
};

#endif // THEME_H
