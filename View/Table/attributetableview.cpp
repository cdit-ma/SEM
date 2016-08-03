#include "attributetableview.h"
#include "multilinedelegate.h"
#include <QHeaderView>
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
    resizeColumnsToContents();
}

