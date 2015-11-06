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

    DockScrollArea* getContainer();
    void setContainer(DockScrollArea* area);
    void hideContainer();

    int getWidth();
    DOCK_TYPE getDockType();

protected:
    void paintEvent(QPaintEvent* e);

signals:
    void dockButton_pressed(DOCK_TYPE type);
    void dockButton_dockOpen(bool open);

public slots:
    void on_buttonPressed();

private:
    void setColor(int state, bool needRepaint = false);

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

    DOCK_TYPE dockType;
    QString kind;
    int width;
    int height;
    bool selected;
    bool enabled;

};

#endif // DOCKTOGGLEBUTTON_H
