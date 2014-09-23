#include <qabstractitemmodel.h>

#ifndef ATTRIBUTETABLEMODEL_H
#define ATTRIBUTETABLEMODEL_H

#include <QAbstractTableModel>


#include "../Model/node.h"
#include "../Model/graphmldata.h"
#include <QVector>

class NodeItem;

class AttributeTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    AttributeTableModel(NodeItem* item, QObject* parent = 0);

public slots:
    void updatedData(GraphMLData* data);
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
    NodeItem* gui;
    Node* attachedNode;
    QVector<GraphMLData*> attachedData;
};

#endif // ATTRIBUTETABLEMODEL_H
