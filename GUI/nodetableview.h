#ifndef NODETABLEVIEW_H
#define NODETABLEVIEW_H
#include "graphmlview.h"
#include <QAbstractTableModel>

class NodeTableView:public QObject, public QAbstractTableModel, public GraphMLView
{
    Q_OBJECT
public:
    NodeTableView();
    // GraphMLView interface
public:
    void connectController(NewController *controller);
    void disconnectController();

public slots:
    void view_ConstructItem(GraphML *item);
    void view_DestructItem(QString ID);
    void view_SelectItem(GraphML *item, bool selected);
    void view_RefreshView();
    void view_SetViewDrawDepth(int depth);
    void view_PrintErrorText(GraphML *item, QString text);
    void view_FocusItem(GraphML *item);
    void view_SetItemOpacity(GraphML *item, qreal opacity);

    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    bool insertRows(int row, int count, const QModelIndex &parent);

    bool removeRows(int row, int count, const QModelIndex &parent);
    Qt::ItemFlags flags(const QModelIndex &index) const;

private:
    int count;
};

#endif // NODETABLEVIEW_H
