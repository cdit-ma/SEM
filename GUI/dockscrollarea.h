#ifndef DOCKSCROLLAREA_H
#define DOCKSCROLLAREA_H

#include <QScrollArea>
#include <QPushButton>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QScrollBar>
#include <QLabel>

#include "nodeview.h"
#include "nodeitem.h"
#include "docknodeitem.h"


class DockToggleButton;
class DockNodeItem;
class NodeView;

class DockScrollArea : public QScrollArea
{
    Q_OBJECT

public:
    explicit DockScrollArea(QString label, NodeView *view, DockToggleButton *parent);

    void setNotAllowedKinds(QStringList kinds);

    void addDockNodeItem(DockNodeItem* item);
    QList<DockNodeItem*> getDockNodeItems();

    NodeItem* getCurrentNodeItem();
    DockToggleButton* getParentButton();
    QString getLabel();

    NodeView* getNodeView();
    QStringList getAdoptableNodeListFromView();

protected:
    void paintEvent(QPaintEvent *e);

public slots:
    void updateCurrentNodeItem(Node* selectedNode);

    void activate();
    void clear();
    void checkScrollBar();

    void removeDockNodeItemFromList(DockNodeItem* item);

    virtual void dockNodeItemClicked() = 0;
    virtual void updateDock();

private:
    void setupLayout();
    void setParentButton(DockToggleButton* parent);

    NodeView* nodeView;
    NodeItem* currentNodeItem;
    DockToggleButton *parentButton;

    QString label;
    bool activated;

    QVBoxLayout* layout;

    QList<DockNodeItem*> dockNodeItems;
    QStringList notAllowedKinds;

};

#endif // DOCKSCROLLAREA_H
