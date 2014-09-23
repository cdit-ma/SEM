#ifndef NODEVIEWTREEMODEL_H
#define NODEVIEWTREEMODEL_H

#include <QAbstractItemModel>
#include "nodeitemtreeitem.h"
#include "nodeitem.h"

class NodeView;

class NodeViewTreeModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    NodeViewTreeModel(QObject* parent);
    ~NodeViewTreeModel();

    void addToParentModel(NodeItemTreeItem* item);
    NodeItemTreeItem* rootItem;

private:
    NodeView* attachedView;

public slots:
    void updatedData(GraphMLData* data);

    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    QModelIndex parent(const QModelIndex &child) const;



    // QAbstractItemModel interface
    Qt::ItemFlags flags(const QModelIndex &index) const;
};

#endif // NODEVIEWTREEMODEL_H
