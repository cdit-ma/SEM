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

    void addDockNodeItem(DockNodeItem* item, int insertIndex = -1, bool addToLayout = true);
    DockNodeItem* getDockNodeItem(QString nodeID);
    virtual QList<DockNodeItem*> getDockNodeItems();

    void removeDockNodeItemFromList(DockNodeItem* dockNodeItem);

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

    virtual void onNodeDeleted(QString nodeID);
    virtual void onEdgeDeleted();

public slots:
    virtual void dockNodeItemClicked() = 0;
    virtual void dockClosed();
    virtual void updateDock();
    virtual void clear();

    void clearSelected();

    virtual void nodeDeleted(int nodeID, int parentID);
    void edgeDeleted(int srcID, int dstID);

    void updateCurrentNodeItem();

    void removeDockNodeItem(DockNodeItem* item);

    void parentHeightChanged(double height);

    void on_parentButtonPressed();

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
