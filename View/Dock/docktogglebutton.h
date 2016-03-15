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

    void setEnabled(bool enable);
    bool isEnabled();

    DockScrollArea* getDock();
    void setDock(DockScrollArea* dock);
    void hideDock();

    int getWidth();
    int getHeight();

    DOCK_TYPE getDockType();
    MedeaWindow* getParentWindow();

public slots:
    void dockButtonPressed(DOCK_TYPE type);
    void themeChanged();

private:
    void updateStyleSheet(int state = -1);

    DockScrollArea* dock;
    MedeaWindow* parentWindow;

    DOCK_TYPE dockType;
    QString kind;

    bool selected;
    bool enabled;

    QString fixedStyleSheet;
    QString currentStyleSheet;

};

#endif // DOCKTOGGLEBUTTON_H
