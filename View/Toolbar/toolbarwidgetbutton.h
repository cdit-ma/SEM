#ifndef TOOLBARWIDGETBUTTON_H
#define TOOLBARWIDGETBUTTON_H

#include <QPushButton>

class ToolbarWidgetButton : public QPushButton
{
    Q_OBJECT

public:
    explicit ToolbarWidgetButton(QWidget *parent = 0);

    ~ToolbarWidgetButton();

    void setCheck(bool checked);
    bool getCheck();

signals:
    void mouseEntered();
    void mouseExited();
public slots:

protected:
    void enterEvent(QEvent *e);
    void leaveEvent(QEvent *e);
    void mouseDoubleClickEvent(QMouseEvent* e);

private:
    bool checked;

};

#endif // TOOLBARWIDGETBUTTON_H
