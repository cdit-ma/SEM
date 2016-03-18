#ifndef THEME_H
#define THEME_H

#include <QHash>
#include <QPixmap>
#include <QThread>
#include <QColor>
#include <QIcon>

#include <QObject>
#include "enumerations.h"

class Theme: public QObject
{
Q_OBJECT
public:
    enum COLOR_ROLE{CR_NONE, CR_NORMAL, CR_DISABLED, CR_SELECTED};
    Theme();

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

    QIcon getIcon(QString prefix, QString alias);
    QPixmap getImage(QString prefix, QString alias, QSize size = QSize(), QColor tintColor = QColor());

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

    QHash<VIEW_ASPECT, QColor> aspectColor;

    QHash<COLOR_ROLE, QColor> textColor;
    QHash<COLOR_ROLE, QColor> menuIconColor;


    QColor highlightColor;
    QColor backgroundColor;
    QColor altBackgroundColor;
    QColor disabledBackgroundColor;
    QColor iconColor;

    QString slash;

    bool themeChanged;

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
