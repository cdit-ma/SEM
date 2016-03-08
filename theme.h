#ifndef THEME_H
#define THEME_H

#include <QHash>
#include <QPixmap>
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
    QColor getAltBackgroundColor();
    QColor getHighlightColor();

    void setBackgroundColor(QColor color);
    void setAltBackgroundColor(QColor color);
    void setHighlightColor(QColor color);

    QColor getTextColor(COLOR_ROLE role = CR_NORMAL);
    void setTextColor(COLOR_ROLE role, QColor color);

    QColor getMenuIconColor(COLOR_ROLE role = CR_NORMAL);
    void setMenuIconColor(COLOR_ROLE role, QColor color);


    void setIconToggledImage(QString prefix, QString alias, QString toggledAlias, QString toggledImageName);


    QColor getDefaultImageTintColor();
    void setDefaultImageTintColor(QColor color);
    void setDefaultImageTintColor(QString prefix, QString alias, QColor color);

    void applyTheme();
    bool isValid();

    QIcon getIcon(QString prefix, QString alias);
    QPixmap getImage(QString prefix, QString alias, QColor tintColor = QColor());
signals:
    void theme_Changed();
private:
    void updateValid();
    QHash<QString, QPixmap> pixmapLookup;
    QHash<QString, QIcon> iconLookup;
    QHash<QString, QColor> pixmapTintLookup;
    QHash<QString, QString> iconToggledLookup;

    QHash<COLOR_ROLE, QColor> textColor;
    QHash<COLOR_ROLE, QColor> menuIconColor;


    QColor highlightColor;
    QColor backgroundColor;
    QColor altBackgroundColor;

    QColor iconColor;

    bool themeChanged;

    bool valid;

public:
    static QString QColorToHex(const QColor color);
    static Theme* theme();

private:
    static Theme* themeSingleton;
};

#endif // THEME_H
