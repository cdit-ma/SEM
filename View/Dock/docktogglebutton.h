#ifndef DOCKTOGGLEBUTTON_H
#define DOCKTOGGLEBUTTON_H

#include <QPushButton>
#include <QGroupBox>

#include "../../Model/node.h"
#include "enumerations.h"


class MedeaWindow;
class DockScrollArea;

class DockToggleButton : public QPushButton
{
    Q_OBJECT

public:
    explicit DockToggleButton(DOCK_TYPE type, MedeaWindow *window, QWidget* parent = 0);
    ~DockToggleButton();

    void setSelected(bool b);
    bool isSelected();

    void setEnabled(bool enable, bool repaint = false);
    bool isEnabled();

    DockScrollArea* getDock();
    void setDock(DockScrollArea* dock);
    void hideDock();

    int getWidth();
    DOCK_TYPE getDockType();

    MedeaWindow* getParentWindow();

protected:
    void paintEvent(QPaintEvent* e);

public slots:
    void dockButtonPressed(DOCK_TYPE type);

private:
    void setColor(int state, bool needRepaint = false);

    DockScrollArea* dock;
    MedeaWindow* parentWindow;

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

    DOCK_TYPE dockType;
    QString kind;
    int width;
    int height;
    bool selected;
    bool enabled;

};

#endif // DOCKTOGGLEBUTTON_H
