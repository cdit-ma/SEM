#include "qosprofilemodel.h"
#include "../../theme.h"

#include <keynames.h>

/**
 * @brief QOSProfileModel::QOSProfileModel
 * @param parent
 */
QOSProfileModel::QOSProfileModel(QObject* parent)
	: QStandardItemModel(parent) {}

void QOSProfileModel::viewItem_Constructed(ViewItem* viewItem)
{
    //We only care about nodes
    if (viewItem && viewItem->isNode()) {
        auto item = qobject_cast<NodeViewItem*>(viewItem);
        if (item->getViewAspect() == VIEW_ASPECT::ASSEMBLIES) {

            QString nodeKind = viewItem->getData(KeyName::Kind).toString();
            QStandardItem* parentItem = nullptr;

            if (nodeKind.endsWith("QOSProfile")) {
                parentItem = invisibleRootItem();
            } else {
                parentItem = modelItems.value(item->getParentID(), nullptr);
            }

            if (parentItem) {
                auto modelItem = new QOSModelItem(item);
                parentItem->setChild(parentItem->rowCount(), modelItem);
                modelItems.insert(item->getID(), modelItem);
            }
        }
    }
}

void QOSProfileModel::viewItem_Destructed(int ID, ViewItem *)
{
    if (modelItems.contains(ID)) {

        QOSModelItem* modelItem = modelItems.take(ID);
        QStandardItem* parentItem = ((QStandardItem*)modelItem)->parent();

        //Remove the row
        if (parentItem) {
            removeRow(modelItem->row(), parentItem->index());
        } else {
            removeRow(modelItem->row());
        }
    }
}

QAbstractTableModel* QOSProfileModel::getTableModel(const QModelIndex &index) const
{
    if (index.isValid()) {
        QStandardItem* item = itemFromIndex(index);
        if (item) {
            auto table_model_data = item->data(QOSProfileModel::DATATABLE_ROLE);
            if (!table_model_data.isNull() && table_model_data.isValid()) {
                return (QAbstractTableModel*) table_model_data.value<void *>();
            }
        }
    }
    return nullptr;
}

QVariant QOSProfileModel::data(const QModelIndex &index, int role) const
{
    if(index.isValid()){
        QStandardItem* item = itemFromIndex(index);
        if(item){
            return item->data(role);
        }
    }
    return QStandardItemModel::data(index, role);
}

bool QOSProfileModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(index.isValid()){
        QStandardItem* item = itemFromIndex(index);
        if(item){
            item->setData(value, role);
            return true;
        }
    }
    return false;
}

Qt::ItemFlags QOSProfileModel::flags(const QModelIndex &index) const
{
    if(index.isValid()){
        QStandardItem* item = itemFromIndex(index);
        if(item){
            if(item->data(EDITABLELABEL_ROLE).toBool()){
                return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
            }
        }
    }
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

/**
 * @brief QOSModelItem::QOSModelItem
 * @param item
 */
QOSModelItem::QOSModelItem(NodeViewItem* item)
	: QObject(),
	  QStandardItem(),
	  nodeViewItem_(item)
{
    if (item) {
        ID = item->getID();
        item->registerObject(this);
        connect(item, SIGNAL(iconChanged()), this, SLOT(itemChanged()));
        connect(item, SIGNAL(labelChanged(QString)), this, SLOT(itemChanged()));
    } else {
        ID = -1;
    }
}

QOSModelItem::~QOSModelItem()
{
    if (nodeViewItem_) {
        nodeViewItem_->unregisterObject(this);
    }
}

void QOSModelItem::itemChanged()
{
    emitDataChanged();
}

QVariant QOSModelItem::data(int role) const
{
    switch (role) {
        case Qt::DisplayRole:
        case Qt::EditRole:
        case Qt::ToolTipRole:
            if (nodeViewItem_) {
                return nodeViewItem_->getData(KeyName::Label);
            }
            break;
        case QOSProfileModel::ID_ROLE:
            return ID;
        case QOSProfileModel::DATATABLE_ROLE:
            if (nodeViewItem_) {
                return QVariant::fromValue((void *)nodeViewItem_->getTableModel());
            } else {
                return QVariant();
            }
        case QOSProfileModel::EDITABLELABEL_ROLE:
            if (nodeViewItem_) {
                return !nodeViewItem_->isDataProtected(KeyName::Label);
            } else {
                return false;
            }
        default:
            break;
    }
    return QStandardItem::data(role);
}

void QOSModelItem::setData(const QVariant &value, int role)
{
    // NOTE: According to Qt's documentation, this function should call emitDataChanged() if the base implementation
    //  of setData() is not being called. This will ensure that views using the model are notified of the changes.
    //  At the moment, we call emitDataChanged when the nodeViewItem (associated with this model item)'s data has changed
    if (role == Qt::EditRole) {
        if(item && !item->isDataProtected(KeyName::Label)){
            emit item->getTableModel()->req_dataChanged(ID, KeyName::Label, value);
        }
    }
}

QVariant QOSProfileModel::headerData(int, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        return "Policies";
    }
    if (role == Qt::DecorationRole) {
        return Theme::theme()->getImage("Icons", "buildingColumns", QSize(16,16), Theme::theme()->getMenuIconColorHex());
    }
    return QVariant();
}