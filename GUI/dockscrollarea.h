#ifndef DOCKSCROLLAREA_H
#define DOCKSCROLLAREA_H

#include <QScrollArea>
#include <QPushButton>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QLabel>

#include "nodeitem.h"

class DockToggleButton;

class DockScrollArea : public QScrollArea
{
    Q_OBJECT
public:
    explicit DockScrollArea(QString label, DockToggleButton *parent);
    ~DockScrollArea();

    DockToggleButton* getParentButton();
    QGroupBox* getGroupBox();
    void addDockNode(NodeItem* item);
    void addAdoptableDockNodes(Node* parentNode, QStringList nodes);

protected:
    void paintEvent(QPaintEvent *e);

private:
    NodeItem* nodeItem;
    Node* parentNode;
    DockToggleButton *parentButton;
    QGroupBox *groupBox;
    QVBoxLayout *layout;
    QString label;
    bool activated;


    QVector<QWidget*> dockNodes;

signals:
    void constructDockNode(Node* node, QString kind);
    void trigger_addComponentInstance(NodeItem* itm);

public slots:
    void activate();
    void clear();

    void buttonPressed(QString kind);
    void dock_addComponentInstance(NodeItem *itm);

    void checkScrollBar();
};

#endif // DOCKSCROLLAREA_H
