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
    void addDockNode(NodeItem* item);
    void addAdoptableDockNodes(Node* parentNode, QStringList nodes);

    void checkDockNodesList();

protected:
    void paintEvent(QPaintEvent *e);

signals:
    void constructDockNode(Node* node, QString kind);
    void trigger_addComponentInstance(NodeItem* itm);

public slots:
    void activate();
    void clear();

    void buttonPressed(QString kind);
    void dock_addComponentInstance(NodeItem *itm);

    void removeFromDockNodeList(QWidget* widget);

    void checkScrollBar();

private:
    NodeItem* nodeItem;
    Node* parentNode;
    DockToggleButton *parentButton;
    QGroupBox *groupBox;
    QVBoxLayout *layout;
    QString label;
    bool activated;

    QVector<QWidget*> dockNodes;

};

#endif // DOCKSCROLLAREA_H
