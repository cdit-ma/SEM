#ifndef ASPECTTOGGLEWIDGET_H
#define ASPECTTOGGLEWIDGET_H

#include <QWidget>
#include <QMouseEvent>
#include <QFrame>
#include <QLabel>

class MedeaWindow;

class AspectToggleWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AspectToggleWidget(QString text, double size,  MedeaWindow* parent);

    QString getText();

    bool isClicked();
    void setClicked(bool checked);

protected:
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseDoubleClickEvent(QMouseEvent* event);

signals:
    void aspectToggle_clicked(bool checked, int state);

public slots:
    void aspectDoubleClicked(AspectToggleWidget* aspect);
    void highlightToggleButton(QString aspect);

private:
    void setupColor();
    void setupLayout(double widgetSize);
    void updateStyleSheet();

    void click(bool checked, int state);

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

};

#endif // ASPECTTOGGLEWIDGET_H
