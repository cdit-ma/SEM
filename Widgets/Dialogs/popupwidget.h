#ifndef POPUPWIDGET_H
#define POPUPWIDGET_H

#include <QDialog>
#include <QFrame>

class PopupWidget : public QDialog
{
    Q_OBJECT
public:
    enum POPUPWIDGET_TYPE{DIALOG, POPUP, TOOL};
    enum SIZE_TYPE{FIXED, MIN, MAX};
    explicit PopupWidget(POPUPWIDGET_TYPE type, QWidget* parent = 0);

    void setWidget(QWidget* widget);
    void setWidth(int width, SIZE_TYPE type = FIXED);
    void setHeight(int height, SIZE_TYPE type = FIXED);
    void setSize(int width, int height, SIZE_TYPE type = FIXED);

private:
    QFrame* mainFrame;
};

#endif // POPUPWIDGET_H
