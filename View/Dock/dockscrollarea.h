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

    void addDockNodeItem(DockNodeItem* item, int insertIndex = -1, bool addToLayout = true);
    //DockNodeItem* getDockNodeItem(NodeItem* item);
    //DockNodeItem* getDockNodeItem(Node* node);
    DockNodeItem* getDockNodeItem(QString nodeID);
    QList<DockNodeItem*> getDockNodeItems();

    bool isDockOpen();
    void setDockEnabled(bool enabled);
    DockToggleButton* getParentButton();
    NodeItem* getCurrentNodeItem();
    QString getCurrentNodeID();

    QString getLabel();
    QVBoxLayout* getLayout();

    NodeView* getNodeView();
    QStringList getAdoptableNodeListFromView();

    virtual void onNodeDeleted(QString ID);
    virtual void onEdgeDeleted();

protected:
    void paintEvent(QPaintEvent *e);

public slots:
    virtual void dockNodeItemClicked() = 0;
    virtual void updateDock();
    virtual void clear();

    virtual void nodeDeleted(QString nodeID, QString parentID);
    void edgeDeleted(QString srcID, QString dstID);

    void updateCurrentNodeItem();

    void removeDockNodeItemFromList(DockNodeItem* item);

    void activate();

    void parentHeightChanged(double height);

private:
    void setupLayout();
    void setParentButton(DockToggleButton* parent);

    NodeView* nodeView;
    QString currentNodeItemID;
    NodeItem* currentNodeItem;
    DockToggleButton *parentButton;

    QVBoxLayout* mainLayout;
    QVBoxLayout* layout;
    QString label;
    bool activated;

    QHash<QString, DockNodeItem*> dockNodeItems;
    QStringList dockNodeIDs;
    QStringList notAllowedKinds;

};

#endif // DOCKSCROLLAREA_H
