#ifndef ASPECTTOGGLEWIDGET_H
#define ASPECTTOGGLEWIDGET_H

#include "../enumerations.h"

#include <QWidget>
#include <QMouseEvent>
#include <QFrame>
#include <QLabel>

class MedeaWindow;

class AspectToggleWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AspectToggleWidget(VIEW_ASPECT aspect, double size,  MedeaWindow* parent);

    QString getText();
    VIEW_ASPECT getAspect();

    bool isClicked();
    void setClicked(bool checked);

protected:
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseDoubleClickEvent(QMouseEvent* event);

signals:
    void aspectToggle_clicked(bool checked, int state);
    void aspectToggle_doubleClicked(VIEW_ASPECT aspect);
    void aspectToggle_middleClicked(VIEW_ASPECT aspect);

    void aspectToggled(VIEW_ASPECT aspect, bool on);

public slots:
    void aspectDoubleClicked(VIEW_ASPECT aspect);
    void highlightToggleButton(QString aspect);

private:
    QString toColorStr(QColor color, int alpha);
    void setupColor();
    void setupLayout(double widgetSize);
    void stateChanged();
    void updateStyleSheet();

    void click(bool checked, int state);


    VIEW_ASPECT aspect;

    QString aspectText;
    QLabel* aspectLabel;

    QFrame* shadowFrame;
    QFrame* mainFrame;

    QString defaultColor;
    QString p1_Color;
    QString p2_Color;
    QString p3_Color;
    QString p4_Color;

    bool CHECKED;
    int STATE;
    int THEME;

};

#endif // ASPECTTOGGLEWIDGET_H
