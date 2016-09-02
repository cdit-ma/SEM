#include "validatedialog.h"
#include <QXmlQuery>

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


    spinning = 0;
    buttonLayout = new QHBoxLayout();
    titleLayout = new QHBoxLayout();
    statusIcon = new QLabel();
    label = new QLabel("Validation Report");
    label->setStyleSheet("font-family: Helvetica, Arial, sans-serif; font-size: 18px;  font-weight: bold;");
    titleLayout->addWidget(statusIcon);
    titleLayout->addWidget(label,1);


    QPushButton* closeButton = new QPushButton("Close");
    revalidateButton = new QPushButton(QIcon(":/Actions/Validate.png"),"Re-Validate");



    connect(closeButton, SIGNAL(clicked()), this, SLOT(accept()));
    connect(revalidateButton, SIGNAL(clicked()), this, SLOT(revalidate()));
    buttonLayout->addStretch();
    buttonLayout->addWidget(revalidateButton);
    buttonLayout->addWidget(closeButton);

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->setSizeConstraint(QLayout::SetNoConstraint);
    mainLayout->addLayout(titleLayout,0,0);
    mainLayout->addWidget(itemsTable, 1, 0);
    mainLayout->addLayout(buttonLayout, 2, 0);
    mainLayout->setRowStretch(1, 1);

    setLayout(mainLayout);

    Qt::WindowFlags flags = 0;
    flags |= Qt::WindowMaximizeButtonHint;
    flags |= Qt::WindowCloseButtonHint;
    flags |= Qt::WindowMinimizeButtonHint;

    setWindowModality(Qt::NonModal);
    setWindowFlags(flags);
    setWindowTitle(tr("MEDEA - Validation Report"));

    connect( itemsTable, SIGNAL( cellClicked (int, int) ), this, SLOT( cellSelected( int, int ) ) );

}


void ValidateDialog::gotResults(QString reportPath)
{
    QFile xmlFile(reportPath);

    if (!xmlFile.exists() || !xmlFile.open(QIODevice::ReadOnly)){
        //displayNotification("XSL validation failed to produce a report.");
        return;
    }

    QXmlQuery query;
    query.bindVariable("graphmlFile", &xmlFile);
    const QString queryMessages = QString("declare namespace svrl = \"http://purl.oclc.org/dsdl/svrl\"; doc('file:///%1')//svrl:schematron-output/svrl:failed-assert/string()").arg(xmlFile.fileName());
    query.setQuery(queryMessages);

    QStringList messagesResult;
    bool result = query.evaluateTo(&messagesResult);
    xmlFile.close();
    setupItemsTable(messagesResult);
}

void ValidateDialog::setupItemsTable(QStringList items)
{
    /* clear table and remove excess rows */
    itemsTable->clear();
    itemsID.clear();
    for (int row = itemsTable->rowCount() - 1; row >= 0; --row) {
        itemsTable->removeRow(row);
    }

    /* populate table with validation messages */
    for (int row = 0; row < items.count(); ++row) {
        itemsTable->insertRow(row);
        QString reportMessage = items[row];
        QString id = reportMessage.split('[').last().split(']').first();
        QString message = reportMessage.split(']').last();
        itemsID.append(id);
        QTableWidgetItem *name = new QTableWidgetItem(message);
        name->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        itemsTable->setSelectionBehavior( QAbstractItemView::SelectRows );
        itemsTable->setSelectionMode( QAbstractItemView::SingleSelection );
        itemsTable->setItem(row, 0, name);
    }

    if(items.count() == 0){
        label->setText("Validation Succeeded!");
        QImage successImage(":/Actions/Job_Built.png");
        statusIcon->setPixmap(QPixmap::fromImage(successImage));
    }else{
        label->setText("Validation Failed! " + QString::number(items.count()) + " Issues!");
        QImage failImage(":/Actions/Job_Failed.png");
        statusIcon->setPixmap(QPixmap::fromImage(failImage));
    }
    revalidateButton->setEnabled(true);

    itemsTable->resizeColumnsToContents();
    setMinimumWidth(itemsTable->width());
}

void ValidateDialog::cellSelected(int nRow, int nCol)
{
    Q_UNUSED(nCol);
    //qDebug() << "Cell at row "+QString::number(nRow)+"was clicked.";
    QString ID = itemsID[nRow];
    bool okay;
    int numberID = ID.toInt(&okay);
    if(okay){
        searchItem_centerOnItem(numberID);
    }
}

void ValidateDialog::revalidate()
{
    if(!spinning){
        spinning = new QMovie(this);
        spinning->setFileName(":/Actions/Waiting.gif");
        spinning->start();
    }
    revalidateButton->setEnabled(false);
    statusIcon->setPixmap(QPixmap());
    statusIcon->setMovie(spinning);
    label->setText("Re-Validating Model");
    emit revalidate_Model();
}
