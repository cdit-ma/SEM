#ifndef NODETABLEVIEWMANAGER_H
#define NODETABLEVIEWMANAGER_H

#include "graphmlview.h"
#include "../Model/graphml.h"

class NodeTable;

class NodeTableView: public QObject, public GraphMLView
{
    Q_OBJECT
public:
    NodeTableView();

    QAbstractTableModel* getModel();
    // GraphMLView interface
public:
    void connectController(NewController *controller);
    void disconnectController();

    void view_TreeViewItemSelected(QModelIndex index);
    void view_TreeViewItemCentered(QModelIndex index);


public slots:
    void view_ConstructItem(GraphML *item);
    void view_DestructItem(QString ID);
    void view_RefreshView();
    void view_SetViewDrawDepth(int depth);
    void view_PrintErrorText(GraphML *item, QString text);
    void view_FocusItem(GraphML *item);
    void view_SelectItem(GraphML *item, bool selected);
    void view_SetItemOpacity(GraphML *item, qreal opacity);


private:
    NodeTable *tableView;



    // GraphMLView interface
signals:
    void view_TriggerSelected(GraphML *, bool setSelected);
    void view_CenterItem(GraphML*);
};

#endif // NODETABLEVIEWMANAGER_H
