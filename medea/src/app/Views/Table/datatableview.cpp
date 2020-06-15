#include "datatablemodel.h"
#include "datatableview.h"
#include "datatabledelegate.h"

#include <QHeaderView>

DataTableView::DataTableView(QWidget* parent)
	: QTableView(parent)
{
    //Setup a new Attribute Table Delegate
    setItemDelegate(new DataTableDelegate(this));

    //Setup the Header on the Top
    horizontalHeader()->setVisible(false);
    horizontalHeader()->setHighlightSections(false);
    horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    //Setup the Header on the Left.
    verticalHeader()->setHighlightSections(false);

    setFocusPolicy(Qt::TabFocus);
    setShowGrid(false);
}

void DataTableView::setModel(QAbstractItemModel *model)
{
    QTableView::setModel(model);
}

void DataTableView::editDataValue(int ID, const QString& keyName)
{
    QAbstractItemModel* m = model();
    if (m == nullptr) {
    	return;
    }
	QModelIndex index;
	if (m->data(index, DataTableModel::ID_ROLE).toInt() == ID) {
		for (int i = 0; i < m->rowCount(); i++) {
			auto rowName = m->headerData(i, Qt::Vertical, Qt::DisplayRole).toString();
			if (rowName == keyName) {
				index = m->index(i,0);
				break;
			}
		}
		if (m->flags(index) & Qt::ItemIsEditable) {
			edit(index);
		}
	}
}