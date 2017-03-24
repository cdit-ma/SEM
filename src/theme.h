#ifndef THEME_H
#define THEME_H

#include <QHash>
#include <QPixmap>
#include <QThread>
#include <QColor>
#include <QIcon>

#include <QObject>
#include <QFuture>

#include "enumerations.h"
#include "Controllers/SettingsController/settingscontroller.h"


#define THEME_STYLE_QMENU "THEME_STYLE_QMENU"
#define THEME_STYLE_QPUSHBUTTON "THEME_STYLE_QPUSHBUTTON"
#define THEME_STYLE_QPUSHBUTTON_JENKINS "THEME_STYLE_QPUSHBUTTON_JENKINS"
#define THEME_STYLE_GROUPBOX "THEME_STYLE_GROUPBOX"
#define THEME_STYLE_HIDDEN_TOOLBAR "HIDDEN_TOOLBAR"


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

    QColor getDeployColor();
    QString getDeployColorHex();

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
    QColor getMainImageColor(QPair<QString, QString> path);

    QIcon getIcon(QPair<QString, QString> icon);
    QIcon getIcon(QString prefix, QString alias);

private:
    QPixmap _getImage(QString resourceName, QSize size = QSize(), QColor tintColor = QColor());
public:
    QPixmap getImage(QString prefix, QString alias, QSize size = QSize(), QColor tintColor = QColor());

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
    bool isValid();

    bool gotImage(QPair<QString, QString> icon) const;
    bool gotImage(QString, QString) const;


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
    QString getLineEditStyleSheet();
    QString getRadioButtonStyleSheet();
    QString getMessageBoxStyleSheet();
    QString getPopupWidgetStyleSheet();
    QString getProgressBarStyleSheet();
    QString getLabelStyleSheet();
    QString getAspectButtonStyleSheet(VIEW_ASPECT aspect);

signals:
    void theme_Changed();
    void changeSetting(SETTING_KEY setting, QVariant value);
    void preloadFinished();
    void _preload();
public slots:
    void preloadImages();
    void settingChanged(SETTING_KEY setting, QVariant value);

private:
    void calculateImageColor(QString resourceName);
    QString getResourceName(QString prefix, QString alias) const;
    QString getResourceName(QPair<QString, QString> icon) const;
    void resetTheme(VIEW_THEME themePreset);
    void resetAspectTheme(bool colorBlind);
    void setupToggledIcons();
    void updateValid();

    QPair<QString, QString> splitImagePath(QString path);

    bool tintIcon(QString prefix, QString alias);
    bool tintIcon(QSize size);
    QHash<QString, bool> imageExistsHash;
    QHash<QString, QPixmap> pixmapLookup;
    QHash<QString, QSize> pixmapSizeLookup;
    QHash<QString, QIcon> iconLookup;
    QHash<QString, QColor> pixmapTintLookup;



    QHash<QString, QPair<QString, QString> > iconToggledLookup;
    QHash<QString, QImage> imageLookup;
    QHash<QString, QColor> pixmapMainColorLookup;

    QHash<VIEW_ASPECT, QColor> aspectColor;

    QHash<COLOR_ROLE, QColor> textColor;
    QHash<COLOR_ROLE, QColor> menuIconColor;


    QColor deployColor;
    QColor highlightColor;
    QColor backgroundColor;
    QColor altBackgroundColor;
    QColor disabledBackgroundColor;
    QColor iconColor;
    QColor selectedItemBorderColor;
    QColor selectedWidgetBorderColor;
    QColor altTextColor;

    QString slash;

    bool themeChanged;

    int readCount;
    bool valid;

    bool preloadedImages;
    bool terminating;

    QFuture<void> preloadThread;

public:
    static QColor blendColors(const QColor color1, const QColor color2, qreal blendRatio=0.5);
    static QString QColorToHex(const QColor color);
    static Theme* theme();
    static void teardownTheme();
    static QSize roundQSize(QSize size);
    static QPair<QString, QString> getIconPair(QString prefix, QString alias);
private:
    static Theme* themeSingleton;
};

#endif // THEME_H
