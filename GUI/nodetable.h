#ifndef NODETABLE_H
#define NODETABLE_H
#include <QHash>
#include "../Model/graphml.h"
#include <QModelIndex>
#include <QAbstractTableModel>
#include "nodetableitem.h"

class NodeTable: public QAbstractTableModel
{
    Q_OBJECT
public:
    NodeTable();
    ~NodeTable();
    // GraphMLView interface
    void addItem(Node* item);
    void removeItem(QString ID);
        Node* getNodeFromIndex(QModelIndex index );
    // QAbstractItemModel interface
signals:
    void triggerAction(QString actionName);
    void updateGraphMLData(GraphML* element, QString keyName, QString value);
public slots:
    void updatedData(GraphMLData* data);
public:
    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);

    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    QModelIndex parent(const QModelIndex &child) const;

    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;


private:
    void storeNodeInHash(NodeTableItem* item);
    bool removeNodeFromHash(QString ID);

    int count;
    NodeTableItem* rootItem;

    QHash<QModelIndex, QString> indexLookup;
    QHash<QString, QModelIndex> IDLookup;


    QHash<QString, NodeTableItem*> modelLookup;


    // QAbstractItemModel interface
public:

    // QAbstractItemModel interface
public:

    // QAbstractItemModel interface
public:

};

#endif // NODETABLEVIEW_H
