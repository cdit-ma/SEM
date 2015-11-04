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
    explicit DockScrollArea(QString label, NodeView* view, DockToggleButton* parent);

    void setNotAllowedKinds(QStringList kinds);
    QStringList getNotAllowedKinds();

    void addDockNodeItem(DockNodeItem* dockItem, int insertIndex = -1, bool addToLayout = true);
    void removeDockNodeItem(DockNodeItem* dockItem, bool deleteItem = false);

    DockNodeItem* getDockNodeItem(QString nodeID);
    virtual QList<DockNodeItem*> getDockNodeItems();

    bool isDockOpen();
    bool isDockEnabled();
    void setDockEnabled(bool enabled);

    DockToggleButton* getParentButton();
    NodeItem* getCurrentNodeItem();
    int getCurrentNodeID();

    QString getLabel();
    QVBoxLayout* getLayout();

    NodeView* getNodeView();
    QStringList getAdoptableNodeListFromView();

    virtual void nodeDeleted(QString nodeID);
    virtual void edgeDeleted();

public slots:
    virtual void dockNodeItemClicked() = 0;
    virtual void updateDock();
    virtual void clear();

    void on_parentButtonPressed();

    void onNodeDeleted(int nodeID, int parentID);
    void onEdgeDeleted(int srcID = -1, int dstID = -1);

    void clearSelected();
    void updateCurrentNodeItem();

    void parentHeightChanged(double height);

private:
    void setupLayout();
    void setParentButton(DockToggleButton* parent);

    NodeView* nodeView;
    DockToggleButton *parentButton;

    NodeItem* currentNodeItem;
    int currentNodeItemID;

    QVBoxLayout* mainLayout;
    QVBoxLayout* layout;
    QString label;
    bool dockOpen;

    QHash<QString, DockNodeItem*> dockNodeItems;
    QList<int> dockNodeIDs;
    QStringList notAllowedKinds;

};

#endif // DOCKSCROLLAREA_H
