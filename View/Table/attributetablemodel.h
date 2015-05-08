#ifndef ATTRIBUTETABLEMODEL_H
#define ATTRIBUTETABLEMODEL_H

#include <QAbstractTableModel>


#include "../../Model/graphmldata.h"
#include "../../Model/graphml.h"
#include "../GraphicsItems/graphmlitem.h"
#include <QVector>

class NodeItem;
class EdgeItem;
class GraphMLItem;

class AttributeTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    AttributeTableModel(GraphMLItem* guiItem, QObject* parent = 0);
    ~AttributeTableModel();

signals:
    void editMultilineData(GraphMLData* data);
public slots:
    void updatedData(GraphMLData* data);
    void removedData(GraphMLData* data);
    void addData(GraphMLData* data);

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
    void setupDataBinding();
    bool popupMultiLine(const QModelIndex &index) const;
    GraphMLItem* guiItem;

    bool isNode;
    GraphML* attachedGraphML;
    QList<GraphMLData*> attachedData;
    QStringList permanentlyLockedKeyNames;
    QStringList hiddenKeyNames;
    QStringList multiLineKeyNames;
};

#endif // ATTRIBUTETABLEMODEL_H
