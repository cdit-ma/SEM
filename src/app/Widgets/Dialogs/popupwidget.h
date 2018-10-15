#ifndef POPUPWIDGET_H
#define POPUPWIDGET_H

#include <QDialog>
#include <QFrame>

class PopupWidget : public QDialog
{
    Q_OBJECT

public:
    enum class TYPE{DIALOG, POPUP, TOOL, SPLASH};
    explicit PopupWidget(PopupWidget::TYPE type, QWidget* parent = 0);

    void setWidget(QWidget* widget);
    void setBackgroundOpacity(qreal opactiy);

protected:
    Q_INVOKABLE void adjustSize();
    void paintEvent(QPaintEvent* event);

private:
    void themeChanged();

    QColor background_color;
    QColor border_color;
    qreal opacity = 0.85;
};

#endif // POPUPWIDGET_H
