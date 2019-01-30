#ifndef HOVERPOPUP_H
#define HOVERPOPUP_H

#include "../../../Dialogs/popupwidget.h"

class HoverPopup : public PopupWidget
{
    Q_OBJECT

public:
    explicit HoverPopup(QWidget* parent = 0);

    void setBorderColor(QColor color);

public slots:
    void themeChanged();

protected:
    void paintEvent(QPaintEvent* event);
    void adjustChildrenSize(QString widgetName = "", Qt::FindChildOptions options = Qt::FindChildrenRecursively);

private:
    QColor _backgroundColor;
    QColor _borderColor;
    bool useDefaultBorder;

};

#endif // HOVERPOPUP_H
