#ifndef DOCKTOGGLEBUTTON_H
#define DOCKTOGGLEBUTTON_H

#include <QPushButton>
#include <QGroupBox>

#include "../../Model/node.h"


class MedeaWindow;
class DockScrollArea;

class DockToggleButton : public QPushButton
{
    Q_OBJECT
public:
    explicit DockToggleButton(QString label, MedeaWindow *window, QWidget* parent = 0);
    ~DockToggleButton();

    bool getSelected();
    void setSelected(bool b);

    DockScrollArea* getContainer();
    void setContainer(DockScrollArea* area);
    void hideContainer();

    int getWidth();
    QString getKind();

    void setEnabled(bool enable);
    bool isEnabled();

protected:
    void paintEvent(QPaintEvent* e);

signals:
    void dockButton_pressed(QString buttonName);

public slots:
    void on_buttonPressed();
    void enableDock(bool enable);

private:
    void setColor(int state);

    DockScrollArea* scrollArea;

    QColor brushColor;
    QColor defaultBrushColor;
    QColor disabledBrushColor;
    QColor selectedBrushColor;

    QColor penColor;
    QColor defaultPenColor;
    QColor disabledPenColor;
    QColor selectedPenColor;

    int penWidth;
    int defaultPenWidth;
    int selectedPenWidth;

    QString kind;
    int width;
    int height;
    bool selected;
    bool enabled;

};

#endif // DOCKTOGGLEBUTTON_H
