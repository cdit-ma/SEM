#ifndef THEME_H
#define THEME_H

#include <QHash>
#include <QPixmap>
#include <QThread>
#include <QColor>
#include <QIcon>
#include <QReadWriteLock>

#include <QObject>
#include <QFuture>

#include "ModelController/nodekinds.h"
#include "enumerations.h"
#include "Controllers/SettingsController/settingscontroller.h"


#define THEME_STYLE_QMENU "THEME_STYLE_QMENU"
#define THEME_STYLE_QPUSHBUTTON "THEME_STYLE_QPUSHBUTTON"
#define THEME_STYLE_QPUSHBUTTON_JENKINS "THEME_STYLE_QPUSHBUTTON_JENKINS"
#define THEME_STYLE_GROUPBOX "THEME_STYLE_GROUPBOX"
#define THEME_STYLE_HIDDEN_TOOLBAR "HIDDEN_TOOLBAR"


typedef QPair<QString, QString> IconPair;
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
    void setIconToggledImage(QString prefix, QString alias, QString toggledOnPrefix, QString toggledOnAlias, QString toggledOffPrefix, QString toggleOffAlias);
    void setDefaultImageTintColor(QColor color);
    void setDefaultImageTintColor(QString prefix, QString alias, QColor color);

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
    QString getMenuStyleSheet();
    QString getToolBarStyleSheet();
    QString getAbstractItemViewStyleSheet();
    QString getAltAbstractItemViewStyleSheet();
    QString getComboBoxStyleSheet();
    QString getGroupBoxStyleSheet();
    QString getPushButtonStyleSheet();
    QString getToolButtonStyleSheet();
    QString getLineEditStyleSheet();
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
    void changeSetting(SETTING_KEY setting, QVariant value);
public slots:
    void settingChanged(SETTING_KEY setting, QVariant value);
private:
    void clearIconMap();
    void preloadImages();
    QColor calculateImageColor(QImage image);
    QString getResourceName(QString prefix, QString alias) const;
    QString getResourceName(IconPair icon) const;
    void resetTheme(VIEW_THEME themePreset);
    void resetAspectTheme(bool colorBlind);
    void setupToggledIcons();
    void updateValid();

    QImage getImage(QString resource_name);
    QColor getTintColor(QString resource_name);
    QSize getOriginalSize(QString resource_name);

    IconPair splitImagePath(QString path);

    bool tintIcon(QString prefix, QString alias);
    bool tintIcon(QSize size);


    QHash<QString, QImage> imageLookup;
    QHash<QString, QPixmap> pixmapLookup;
    QHash<QString, QIcon> iconLookup;

    QHash<QString, QSize> pixmapSizeLookup;
    QHash<QString, QColor> pixmapTintLookup;

    QHash<QString, QPair<IconPair, IconPair> > iconToggledLookup;
    QHash<QString, QColor> pixmapMainColorLookup;

    QHash<VIEW_ASPECT, QColor> aspectColor;

    QHash<COLOR_ROLE, QColor> textColor;
    QHash<COLOR_ROLE, QColor> menuIconColor;

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
private:
    static Theme* themeSingleton;
};

#endif // THEME_H
