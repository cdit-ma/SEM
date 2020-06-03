#ifndef POPUPWIDGET_H
#define POPUPWIDGET_H

#include <QDialog>

class PopupWidget : public QDialog
{
    Q_OBJECT

public:
    enum class TYPE{ DIALOG, POPUP, TOOL, SPLASH };

    explicit PopupWidget(PopupWidget::TYPE type, QWidget* parent = nullptr);

    QWidget* getWidget();
    void setWidget(QWidget* widget);

    void setBackgroundOpacity(qreal opacity);

protected:
    // NOTE: Q_INVOKABLE causes adjustSize to be registered with the meta-object system
    //  It enables it to be invoked using QMetaObject::invokeMethod()
    Q_INVOKABLE void adjustSize();

    void paintEvent(QPaintEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private:
    void themeChanged();

    QWidget* widget_ = nullptr;

    QColor background_color;
    QColor border_color;

    qreal opacity = 0.85;
};

#endif // POPUPWIDGET_H