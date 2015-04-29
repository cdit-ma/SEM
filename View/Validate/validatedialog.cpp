#include "validatedialog.h"
#include "../../medeawindow.h"

#include <QDebug>
/**
 * @brief ValidateDialog::ValidateDialog
 * @param parent
 */
ValidateDialog::ValidateDialog(QWidget *parent)
    : QDialog(parent)
{
    itemsTable = new QTableWidget(1, 1);
    itemsTable->horizontalHeader()->setVisible(false);

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->setSizeConstraint(QLayout::SetNoConstraint);
    mainLayout->addWidget(itemsTable, 0, 0);
    mainLayout->addWidget(buttonBox, 1, 0);
    mainLayout->setRowStretch(2, 1);

    setLayout(mainLayout);

    Qt::WindowFlags flags = 0;
    flags |= Qt::WindowMaximizeButtonHint;
    flags |= Qt::WindowCloseButtonHint;
    flags |= Qt::WindowMinimizeButtonHint;

    setWindowModality(Qt::NonModal);
    setWindowFlags(flags);
    setWindowTitle(tr("Validation Report"));

    connect( itemsTable, SIGNAL( cellClicked (int, int) ), this, SLOT( cellSelected( int, int ) ) );

}

/**
 * @brief ValidateDialog::connectToWindow
 * This connects the signals and slots to the MEDEA window.
 */
void ValidateDialog::connectToWindow(QMainWindow* window)
{
    MedeaWindow* medea = dynamic_cast<MedeaWindow*>(window);
    if (medea) {
        connect(this, SIGNAL(searchItem_centerOnItem(QString)), medea, SLOT(on_validationItem_clicked(QString)));
    }
}

void ValidateDialog::setupItemsTable(QStringList *items)
{
    /* clear table and remove excess rows */
    itemsTable->clear();
    itemsID.clear();
    for (int row = itemsTable->rowCount() - 1; row >= 0; --row) {
        itemsTable->removeRow(row);
    }

    /* populate table with validation messages */
    for (int row = 0; row < items->count(); ++row) {
        itemsTable->insertRow(row);
        QString reportMessage = (*items)[row];
        QString id = reportMessage.split('[').last().split(']').first();
        QString message = reportMessage.split(']').last();
        itemsID.append(id);
        QTableWidgetItem *name = new QTableWidgetItem(message);
        name->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        itemsTable->setSelectionBehavior( QAbstractItemView::SelectRows );
        itemsTable->setSelectionMode( QAbstractItemView::SingleSelection );
        itemsTable->setItem(row, 0, name);

    }

    itemsTable->resizeColumnsToContents();
    setMinimumWidth(itemsTable->width());
}

void ValidateDialog::cellSelected(int nRow, int nCol)
{
    //qDebug() << "Cell at row "+QString::number(nRow)+"was clicked.";
    searchItem_centerOnItem(itemsID[nRow]);
    Q_UNUSED(nCol);
}
