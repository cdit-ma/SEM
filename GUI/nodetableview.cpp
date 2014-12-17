#include "nodetableview.h"
#include "graphmlview.h"
#include <QDebug>
#include <QVariant>
NodeTableView::NodeTableView():QObject(),QAbstractTableModel(), GraphMLView()
{
    qCritical() << "Constructed NodeTableView";
    count=0;
}

void NodeTableView::connectController(NewController *controller)
{

    QObject::connect(controller, SIGNAL(view_ConstructGraphMLGUI(GraphML*)), this, SLOT(view_ConstructItem(GraphML*)));
    QObject::connect(controller, SIGNAL(view_DestructGraphMLGUIFromID(QString)), this, SLOT(view_DestructItem(QString)));
    QObject::connect(controller, SIGNAL(view_SetGraphMLSelected(GraphML*,bool)), this, SLOT(view_SelectItem(GraphML*, bool)));
    qCritical() << "DONE STUFF";
}

void NodeTableView::disconnectController()
{

}

void NodeTableView::view_ConstructItem(GraphML *item)
{
    if(item){
        qCritical() << "NodeTableView: view_ConstructItem()";// << item->toString();

        beginInsertRows(QModelIndex(), count, count);
        endInsertRows();
        count++;

    }
}

void NodeTableView::view_DestructItem(QString ID)
{
    qCritical() << "NodeTableView: view_DestructItem() " << ID;
    count--;
}

void NodeTableView::view_SelectItem(GraphML *item, bool selected)
{
    if(item){
        qCritical() << "NodeTableView: view_SelectItem(" << selected << ") ";// << item->toString();
    }
}

void NodeTableView::view_RefreshView()
{

}

void NodeTableView::view_SetViewDrawDepth(int depth)
{

}

void NodeTableView::view_PrintErrorText(GraphML *item, QString text)
{

}

void NodeTableView::view_FocusItem(GraphML *item)
{

}

void NodeTableView::view_SetItemOpacity(GraphML *item, qreal opacity)
{

}

int NodeTableView::rowCount(const QModelIndex &parent) const
{
    return count;
}

int NodeTableView::columnCount(const QModelIndex &parent) const
{

    return 1;
}

QVariant NodeTableView::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= count || index.row() < 0)
        return QVariant();

    if (role == Qt::DisplayRole) {
        return "DATA";
    }
    return QVariant();

    return QVariant();
}

QVariant NodeTableView::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        switch (section) {
        case 0:
            return tr("Data Name");
        case 1:
            return tr("Data Value");

        default:
            return QVariant();
        }
    }
    return QVariant();
}

bool NodeTableView::setData(const QModelIndex &index, const QVariant &value, int role)
{
    return true;
}

bool NodeTableView::insertRows(int row, int count, const QModelIndex &parent)
{
    return true;
}

bool NodeTableView::removeRows(int row, int count, const QModelIndex &parent)
{
    return true;
}

Qt::ItemFlags NodeTableView::flags(const QModelIndex &index) const
{
    return Qt::ItemIsEnabled;
}
