#ifndef THEME_H
#define THEME_H

#include <QHash>
#include <QPixmap>
#include <QThread>
#include <QColor>
#include <QIcon>

#include <QObject>
#include "enumerations.h"

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

    QSize roundQSize(QSize size);
    void setBackgroundColor(QColor color);
    void setDisabledBackgroundColor(QColor color);
    void setAltBackgroundColor(QColor color);
    void setHighlightColor(QColor color);

    QColor getTextColor(COLOR_ROLE role = CR_NORMAL);
    QString getTextColorHex(COLOR_ROLE role = CR_NORMAL);

    void setTextColor(COLOR_ROLE role, QColor color);

    QColor getMenuIconColor(COLOR_ROLE role = CR_NORMAL);
    QString getMenuIconColorHex(COLOR_ROLE role = CR_NORMAL);
    void setMenuIconColor(COLOR_ROLE role, QColor color);

    void setAspectBackgroundColor(VIEW_ASPECT aspect, QColor color);
    QColor getAspectBackgroundColor(VIEW_ASPECT aspect);
    QString getAspectBackgroundColorHex(VIEW_ASPECT aspect);

    void setIconToggledImage(QString prefix, QString alias, QString toggledAlias, QString toggledImageName);

    QColor getDefaultImageTintColor();
    QString getDefaultImageTintColorHex();
    void setDefaultImageTintColor(QColor color);
    void setDefaultImageTintColor(QString prefix, QString alias, QColor color);

    void applyTheme();
    bool isValid();

    QIcon getIcon(QPair<QString, QString> icon);
    QIcon getIcon(QString prefix, QString alias);
    QPixmap getImage(QString prefix, QString alias, QSize size = QSize(), QColor tintColor = QColor());

    // Default StyleSheets
    QString getWindowStyleSheet();
    QString getViewStyleSheet();
    QString getDockWidgetStyleSheet();
    QString getMenuBarStyleSheet();
    QString getMenuStyleSheet();
    QString getToolBarStyleSheet();
    QString getTableViewStyleSheet();
    QString getGroupBoxStyleSheet();
    QString getPushButtonStyleSheet();
    QString getLineEditStyleSheet();
    QString getRadioButtonStyleSheet();
    QString getMessageBoxStyleSheet();

signals:
    void theme_Changed();

private slots:
    void preloadImages();

private:
    void updateValid();

    bool tintIcon(QString prefix, QString alias);
    bool tintIcon(QSize size);
    QHash<QString, QPixmap> pixmapLookup;
    QHash<QString, QSize> pixmapSizeLookup;
    QHash<QString, QIcon> iconLookup;
    QHash<QString, QColor> pixmapTintLookup;
    QHash<QString, QString> iconToggledLookup;
    QHash<QString, QImage> imageLookup;

    QHash<VIEW_ASPECT, QColor> aspectColor;

    QHash<COLOR_ROLE, QColor> textColor;
    QHash<COLOR_ROLE, QColor> menuIconColor;

    QColor Theme_white;
    QColor Theme_black;

    QColor deployColor;
    QColor highlightColor;
    QColor backgroundColor;
    QColor altBackgroundColor;
    QColor disabledBackgroundColor;
    QColor iconColor;
    QColor selectedItemBorderColor;

    QString slash;

    bool themeChanged;

    int readCount;
    bool valid;

public:
    static QString QColorToHex(const QColor color);
    static Theme* theme();
    static void teardownTheme();

signals:
    void initPreloadImages();
private:
    static Theme* themeSingleton;
    static QThread* themeThread;
};

#endif // THEME_H
