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
    explicit AspectToggleWidget(VIEW_ASPECT viewAspect, double size,  MedeaWindow* parent);

    QString getText();
    QPoint getToggleGridPos();
    VIEW_ASPECT getAspect();

    bool isClicked();
    void setClicked(bool checked);

protected:
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseDoubleClickEvent(QMouseEvent* event);

signals:
    void aspectToggled(VIEW_ASPECT viewAspect, bool on);
    void aspectToggle_doubleClicked(VIEW_ASPECT viewAspect);
    void aspectToggle_middleClicked(VIEW_ASPECT viewAspect);

public slots:
    void aspectDoubleClicked(VIEW_ASPECT viewAspect);
    void highlightToggleButton(VIEW_ASPECT viewAspect);

private:
    QString colorToString(QColor color, int alpha = 255);
    QColor adjustColorRGB(QColor color, int delta);

    void setupColor();
    void setupLayout(double widgetSize);

    void click(bool checked, int state);
    void stateChanged();
    void updateStyleSheet();

    VIEW_ASPECT viewAspect;

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
