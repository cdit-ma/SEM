#ifndef DOCKSCROLLAREA_H
#define DOCKSCROLLAREA_H

#include <QScrollArea>
#include <QPushButton>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QScrollBar>
#include <QLabel>

#include "../nodeview.h"
#include "../GraphicsItems/nodeitem.h"
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

    void addDockNodeItem(DockNodeItem* item, bool addToLayout = true);
    DockNodeItem* getDockNodeItem(NodeItem* item);
    DockNodeItem* getDockNodeItem(Node* node);
    QList<DockNodeItem*> getDockNodeItems();

    NodeItem* getCurrentNodeItem();
    DockToggleButton* getParentButton();
    QString getLabel();

    NodeView* getNodeView();
    QStringList getAdoptableNodeListFromView();

    QVBoxLayout* getLayout();

protected:
    void paintEvent(QPaintEvent *e);

public slots:
    virtual void dockNodeItemClicked() = 0;
    virtual void updateDock();

    void updateCurrentNodeItem(Node* selectedNode);

    void activate();
    void clear();
    void checkScrollBar();

    void removeDockNodeItemFromList(DockNodeItem* item);

private:
    void setupLayout();
    void setParentButton(DockToggleButton* parent);

    NodeView* nodeView;
    NodeItem* currentNodeItem;
    DockToggleButton *parentButton;

    QVBoxLayout* layout;
    QString label;
    bool activated;

    QList<DockNodeItem*> dockNodeItems;
    QStringList notAllowedKinds;

};

#endif // DOCKSCROLLAREA_H
