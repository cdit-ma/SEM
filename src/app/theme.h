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
#include <QFont>
#include <QCache>

#include "../modelcontroller/nodekinds.h"
#include "Controllers/SettingsController/settingscontroller.h"
#include "Controllers/NotificationManager/notificationenumerations.h"


#define THEME_STYLE_QMENU "THEME_STYLE_QMENU"
#define THEME_STYLE_QPUSHBUTTON "THEME_STYLE_QPUSHBUTTON"
#define THEME_STYLE_QPUSHBUTTON_JENKINS "THEME_STYLE_QPUSHBUTTON_JENKINS"
#define THEME_STYLE_GROUPBOX "THEME_STYLE_GROUPBOX"
#define THEME_STYLE_HIDDEN_TOOLBAR "HIDDEN_TOOLBAR"

typedef QPair<QString, QString> IconPair;
Q_DECLARE_METATYPE(IconPair); 

enum class ThemePreset{XMAS_THEME, LIGHT_THEME, DARK_THEME, SOLARIZED_DARK_THEME, SOLARIZED_LIGHT_THEME};
enum class ColorRole{NONE, NORMAL, DISABLED, SELECTED};



class Theme: public QObject
{
    Q_OBJECT
protected:
    Theme();
    ~Theme();
public:

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

    QMovie* getGif(const QString& path, const QString& name);


    QColor getActiveWidgetBorderColor();
    QString getActiveWidgetBorderColorHex();

    QColor getTextColor(ColorRole role = ColorRole::NORMAL);
    QString getTextColorHex(ColorRole role = ColorRole::NORMAL);

    QColor getMenuIconColor(ColorRole role = ColorRole::NORMAL);
    QString getMenuIconColorHex(ColorRole role = ColorRole::NORMAL);

    QColor getAspectBackgroundColor(VIEW_ASPECT aspect);

    QColor getMainImageColor(const QString& prefix, const QString& alias);
    QColor getMainImageColor(IconPair path);

    QIcon getIcon(IconPair icon);
    QIcon getIcon(const QString& prefix, const QString& alias);

private:
    QPixmap _getPixmap(const QString& resourceName, QSize size = QSize(), QColor tintColor = QColor());
public:
    QPixmap getImage(const QString& prefix, const QString& alias, QSize size = QSize(), QColor tintColor = QColor());

    QString getPixmapResourceName(QString resource_name, QSize size, QColor tintColor);
    QColor getAltTextColor();
    QString getAltTextColorHex();
    void setAltTextColor(QColor color);

    void setBackgroundColor(QColor color);
    void setDisabledBackgroundColor(QColor color);
    void setAltBackgroundColor(QColor color);
    void setHighlightColor(QColor color);
    void setActiveWidgetBorderColor(QColor color);
    void setTextColor(ColorRole role, QColor color);
    void setMenuIconColor(ColorRole role, QColor color);
    void setAspectBackgroundColor(VIEW_ASPECT aspect, QColor color);
    void setIconToggledImage(const QString& prefix, const QString& alias, const QString& toggledOnPrefix, const QString& toggledOnAlias, const QString& toggledOffPrefix, const QString& toggleOffAlias, bool ignore_toggle_coloring = true);
    void setInactiveEdgeOpacity(qreal opacity);

    void setWindowIcon(const QString& window_title, const QString& visible_icon_prefix, const QString& visible_icon_alias);

    void setIconAlias(const QString& prefix, const QString& alias, const QString& icon_prefix, const QString& icon_alias);
    void setDefaultImageTintColor(const QString& prefix, const QString& alias, QColor color);

    void setSeverityColor(Notification::Severity severity, QColor color);
    QColor getSeverityColor(Notification::Severity);

    void applyTheme();
    bool isValid() const;

    bool gotImage(IconPair icon);
    bool gotImage(const QString&, const QString&);
    bool gotImage(const QString&);

    void setIconSize(int size);

    QStringList getIconPrefixs();
    QStringList getIcons(const QString& icon_prefix);

    QSize getIconSize();
    QSize getLargeIconSize();

    QString getCornerRadius();
    QString getSharpCornerRadius();

    qreal getInactiveEdgeOpacity();

    // Default StyleSheets
    QString getWindowStyleSheet(bool show_background_image);
    QString getScrollAreaStyleSheet();
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
    QString getTabbedToolBarStyleSheet();
    QString getAbstractItemViewStyleSheet();
    QString getGroupBoxStyleSheet();
    QString getPushButtonStyleSheet();
    QString getLineEditStyleSheet(QString widget_name="QLineEdit");
    QString getProgressBarStyleSheet();
    QString getLabelStyleSheet();
    QString getSliderStyleSheet();
    QString getSpinBoxStyleSheet();

    QString getTitleLabelStyleSheet();
    QString getAspectButtonStyleSheet(VIEW_ASPECT aspect);
    QString getToolTipStyleSheet();

    QFont getFont() const;
    QFont getSmallFont() const;
    QFont getLargeFont() const;

signals:
    void theme_Changed();
    void changeSetting(SETTINGS setting, QVariant value);
public slots:
    void settingChanged(SETTINGS setting, QVariant value);
private:
    void preloadImages();

struct ImageLoad{
    QString resource_name;
    QPair<QString, QString> image_path;
    QImage image;
    QColor color;
};
    static ImageLoad LoadImage(const QString& resource_name);
    static QColor CalculateImageColor(const QImage& image);

    QString getResourceName(QString prefix, QString alias) const;
    QString getResourceName(IconPair icon) const;
    void resetTheme(ThemePreset themePreset);
    void resetAspectTheme(bool colorBlind);
    void setupToggledIcons();
    void setupAliasIcons();
    void updateValid();
    
    void setFont(QFont size);

    QImage getImage(const QString& resource_name);

    static IconPair SplitImagePath(const QString& path);

    bool tintIcon(IconPair pair);
    bool tintIcon(const QString& prefix, const QString& alias);
    bool tintIcon(QSize size);
    bool tintIcon(const QString& resource_name);

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

        ColorRole on_normal = ColorRole::SELECTED;
        ColorRole on_active = ColorRole::SELECTED;
        ColorRole on_disabled = ColorRole::DISABLED;

        ColorRole off_normal = ColorRole::NORMAL;
        ColorRole off_active = ColorRole::SELECTED;
        ColorRole off_disabled = ColorRole::DISABLED;
    };

    QHash<QString, IconToggle > iconToggledLookup;
    QHash<QString, QColor> pixmapMainColorLookup;

    QHash<VIEW_ASPECT, QColor> aspectColor;

    QHash<Notification::Severity, QColor> severityColor;

    QHash<ColorRole, QColor> textColor;
    QHash<ColorRole, QColor> menuIconColor;

    QHash<QString, QString> icon_alias_lookup;

    QHash<QString, QSet<QString> > icon_prefix_lookup;

    QColor highlightColor;
    QColor backgroundColor;
    QColor altBackgroundColor;
    QColor disabledBackgroundColor;
    QColor selectedWidgetBorderColor;
    QColor altTextColor;

    QSize icon_size = QSize(16,16);

    qreal inactive_opacity_ = 0.50;

    QFont font;

    QReadWriteLock lock_;
    QReadWriteLock pixmap_lock_;

    ThemePreset current_theme = ThemePreset::DARK_THEME;
    bool themeChanged = false;
    bool valid = false;
    QFuture<void> entity_icons_load_future;
public:
    static QString QColorToHex(const QColor& color);
    static Theme* theme();
    static void roundQSize(QSize& size);
    static IconPair getIconPair(const QString& prefix, const QString& alias);

    static void UpdateActionIcon(QAction* action, Theme* theme = 0);
    static void StoreActionIcon(QAction* action, QString alias, QString name);
    static void StoreActionIcon(QAction* action, IconPair icon);
};


class CustomMenuStyle : public QProxyStyle{
    public:
        CustomMenuStyle(int icon_size = 32);
        int pixelMetric(PixelMetric metric, const QStyleOption* option = 0, const QWidget* widget = 0) const;
    private:
        int icon_size = 32;
};

inline uint qHash(ColorRole key, uint seed)
{
    return ::qHash(static_cast<uint>(key), seed);
}


#endif // THEME_H
