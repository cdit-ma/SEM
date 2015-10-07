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

    bool hideDisabledDock();

    DockToggleButton* getParentButton();
    NodeItem* getCurrentNodeItem();
    QString getCurrentNodeID();

    QString getLabel();
    QVBoxLayout* getLayout();

    NodeView* getNodeView();
    QStringList getAdoptableNodeListFromView();

    virtual void onNodeDeleted(QString ID);
    virtual void onEdgeDeleted();

public slots:
    virtual void dockNodeItemClicked() = 0;
    virtual void dockClosed();
    virtual void updateDock();
    virtual void clear();

    void clearSelected();

    virtual void nodeDeleted(QString nodeID, QString parentID);
    void edgeDeleted(QString srcID, QString dstID);

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
    QString currentNodeItemID;

    QVBoxLayout* mainLayout;
    QVBoxLayout* layout;
    QString label;
    bool dockOpen;

    QHash<QString, DockNodeItem*> dockNodeItems;
    QStringList dockNodeIDs;
    QStringList notAllowedKinds;

    bool dockEnabled;

};

#endif // DOCKSCROLLAREA_H
