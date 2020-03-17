#ifndef HOVERPOPUP_H
#define HOVERPOPUP_H

#include "../../../Dialogs/popupwidget.h"

class HoverPopup : public PopupWidget
{
    Q_OBJECT

public:
    explicit HoverPopup(QWidget* parent = nullptr);

    void adjustChildrenSize(const QString& widgetName = "", Qt::FindChildOptions options = Qt::FindChildrenRecursively);

    void setBorderColor(const QColor& color);

public slots:
    void themeChanged();

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QColor backgroundColor_;
    QColor borderColor_;
    bool useDefaultBorder;
};

#endif // HOVERPOPUP_H