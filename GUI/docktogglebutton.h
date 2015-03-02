#ifndef DOCKTOGGLEBUTTON_H
#define DOCKTOGGLEBUTTON_H

#include <QPushButton>
#include <QGroupBox>

#include "../Model/node.h"


class NewMedeaWindow;
class DockScrollArea;

class DockToggleButton : public QPushButton
{
    Q_OBJECT
public:
    explicit DockToggleButton(QString label, NewMedeaWindow* window, QWidget* parent = 0);
    ~DockToggleButton();

    bool getSelected();
    void setSelected(bool b);

    DockScrollArea* getContainer();
    void setContainer(DockScrollArea* area);
    void hideContainer();

    int getWidth();

    QString getKind();



    void checkEnabled();

protected:
    void paintEvent(QPaintEvent* e);

signals:
    void dockButtonPressed(QString buttonName);

public slots:
    void on_buttonPressed();

private:
    DockScrollArea* scrollArea;
    bool selected;

    QString kind;
    QColor color;
    QColor fillColor;
    QColor selectedColor;
    int width;
    int height;

};

#endif // DOCKTOGGLEBUTTON_H
