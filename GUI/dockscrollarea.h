#ifndef DOCKSCROLLAREA_H
#define DOCKSCROLLAREA_H

#include <QScrollArea>
#include <QPushButton>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QLabel>

#include "nodeitem.h"

class DockToggleButton;
class NodeView;

class DockScrollArea : public QScrollArea
{
    Q_OBJECT
public:
    explicit DockScrollArea(QString label, DockToggleButton *parent);
    ~DockScrollArea();

    void setNodeView(NodeView* v);

    void setCurrentNodeItem(NodeItem* currentNode);
    DockToggleButton* getParentButton();
    void addDockNode(NodeItem* item);
    void addAdoptableDockNodes(Node* parentNode, QStringList nodes);

    void checkDockNodesList();
    QStringList getAdoptableNodesList();
    QList<Node*> getComponentDefinitions();

protected:
    void paintEvent(QPaintEvent *e);

signals:
    void trigger_addChildNode(QString kind, int sender);

    void getSelectedNode();
    void selectedNode(Node* node);

    void trigger_addComponentInstance(Node* assm, Node* defn, int sender);
    void trigger_connectComponentInstance(Node* inst, Node* defn);
    void trigger_connectHardwareNode(Node* src, Node* hardwareNode);

public slots:
    void activate();
    void clear();
    void checkScrollBar();

    void buttonPressed(QString kind);

    void dock_getSelectedNode();

    void dock_addComponentInstance(Node* assm, Node* defn);
    void dock_connectComponentInstance(Node* inst, Node* defn);
    void dock_connectHardwareNode(Node* src, Node* hardwareNode);

    void removeFromDockNodeList(QWidget* widget);

public:
    void updatePartsDock();

    //virtual void updateDock() = 0;


private:
    NodeItem* nodeItem;
    QStringList utilisedKinds;

    DockToggleButton *parentButton;

    QGroupBox *groupBox;
    QVBoxLayout *layout;
    QString label;
    bool activated;

    NodeView* nodeView;
    QVector<QWidget*> dockNodes;

    QStringList adoptableNodesList;
    QList<Node*> componentDefinitions;

};

#endif // DOCKSCROLLAREA_H
