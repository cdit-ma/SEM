#include "attributetableview.h"
#include "multilinedelegate.h"
#include <QHeaderView>
#include "attributetablemodel.h"
AttributeTableView::AttributeTableView(QWidget* parent):QTableView(parent)
{

    //Setup a new Attribute Table Delegate
    setItemDelegate(new AttributeTableDelegate(this));

    //Setup the Header on the Top
    horizontalHeader()->setVisible(false);
    horizontalHeader()->setHighlightSections(false);
    horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    //Setup the Header on the Left.
    verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    verticalHeader()->setHighlightSections(false);
}

void AttributeTableView::setModel(QAbstractItemModel *model)
{
    QTableView::setModel(model);
}

void AttributeTableView::editDataValue(int ID, QString keyName)
{
    QAbstractItemModel* m = model();
    QModelIndex index;
    if(m){
        if(m->data(index, AttributeTableModel::ID_ROLE).toInt() == ID){
            for(int i = 0; i < m->rowCount(); i++){
                QString rowName = m->headerData(i, Qt::Vertical, Qt::DisplayRole).toString();
                if(rowName == keyName){
                    index = m->index(i,0);
                    break;
                }
            }

            if(m->flags(index) & Qt::ItemIsEditable){
                edit(index);
            }
        }
    }
}

