#ifndef TOOLBARABSTRACTBUTTON_H
#define TOOLBARABSTRACTBUTTON_H

#include <QAbstractButton>

class ToolbarAbstractButton : public QAbstractButton
{
    Q_OBJECT

public:
    enum BUTTON_STATE{BS_DEFAULT, BS_HOVERED, BS_CHECKED};

    ToolbarAbstractButton(QWidget* parent, QColor defaultColor = QColor(), QColor checkedColor = QColor(), float borderWidth = 0, int borderRadius = 0);

    void enableHoveredColor(bool enable, bool darker = true);
    void setAcceptMouseEvents(bool accept);

    void setButtonHovered(bool hover);
    void setButtonChecked();
    bool isButtonChecked();

    void stealMouse();

protected:
    void paintEvent(QPaintEvent* event);
    void enterEvent(QEvent* event);
    void leaveEvent(QEvent* event);
    void mousePressEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent*);

private:
    void updateButtonStyle();

    BUTTON_STATE state;
    bool acceptMouseEvents;

    int borderRadius;
    QColor checkedColor;
    QColor hoveredColor;

    QColor defaultColor;
    QColor currentColor;

    float defaultBorderWidth;
    float currentBorderWidth;

};

#endif // TOOLBARABSTRACTBUTTON_H
