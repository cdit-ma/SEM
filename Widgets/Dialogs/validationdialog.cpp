#include "validationdialog.h"
#include "../../theme.h"

#include <QHBoxLayout>
#include <QHeaderView>
#include <QXmlQuery>
#include <QFile>

/**
 * @brief ValidationDialog::ValidationDialog
 * @param parent
 */
ValidationDialog::ValidationDialog(QWidget *parent)
    : QDialog(parent)
{
    setupLayout();
    setMinimumSize(300, 200);

    Qt::WindowFlags flags = 0;
    flags |= Qt::WindowMaximizeButtonHint;
    flags |= Qt::WindowCloseButtonHint;
    flags |= Qt::WindowMinimizeButtonHint;

    setWindowModality(Qt::NonModal);
    setWindowFlags(flags);
    setWindowTitle("Validation Report");

    connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));
    themeChanged();
}


/**
 * @brief ValidationDialog::gotResults
 * @param filePath
 */
void ValidationDialog::gotResults(QString filePath)
{
    QFile xmlFile(filePath);

    if (!xmlFile.exists() || !xmlFile.open(QIODevice::ReadOnly)){
        return;
    }

    QXmlQuery query;
    query.bindVariable("graphmlFile", &xmlFile);
    const QString queryMessages = QString("declare namespace svrl = \"http://purl.oclc.org/dsdl/svrl\"; doc('file:///%1')//svrl:schematron-output/svrl:failed-assert/string()").arg(xmlFile.fileName());
    query.setQuery(queryMessages);

    QStringList messagesResult;
    query.evaluateTo(&messagesResult);
    xmlFile.close();

    setupItemsTable(messagesResult);
    loadingMovie->stop();
}


/**
 * @brief ValidationDialog::themeChanged
 */
void ValidationDialog::themeChanged()
{
    Theme* theme = Theme::theme();
    setStyleSheet("QDialog{ background:" + theme->getBackgroundColorHex() + ";}"
                  "QAbstractItemView {"
                  "border: 1px solid " + theme->getDisabledBackgroundColorHex() + ";"
                  "background:" + theme->getBackgroundColorHex() + ";"
                  "}"
                  "QAbstractItemView::item {"
                  "border: 1px solid " + theme->getDisabledBackgroundColorHex() + ";"
                  //"padding: 10px 0px 0px 0px;"
                  "padding: 3px 0px;"
                  "color:" + theme->getTextColorHex() + ";"
                  "}"
                  "QAbstractItemView::item:selected {"
                  "background:" + theme->getAltBackgroundColorHex() + ";"
                  "}"
                  "QAbstractItemView::item:hover {"
                  "background:" + theme->getDisabledBackgroundColorHex() + ";"
                  "}"
                  "QHeaderView {"
                  "background:" + theme->getBackgroundColorHex() + ";"
                  "border: 0px;"
                  "color:" + theme->getTextColorHex() + ";"
                  "}"
                  "QHeaderView::section {"
                  "border: 1px solid " + theme->getDisabledBackgroundColorHex() + ";"
                  "background:" + theme->getAltBackgroundColorHex() + ";"
                  "padding: 0px 5px;"
                  "}"
                  "QLabel{ background: rgba(0,0,0,0); color:" + theme->getTextColorHex() + ";}"
                  + theme->getScrollBarStyleSheet()
                  + theme->getPushButtonStyleSheet() + "QPushButton{ padding: 5px; }");
}


/**
 * @brief ValidationDialog::revalidate
 */
void ValidationDialog::revalidate()
{
    loadingMovie->start();
    statusIcon->setPixmap(QPixmap());
    statusIcon->setMovie(loadingMovie);

    label->setText("Re-Validating Model...");
    revalidateButton->setEnabled(false);

    emit revalidateModel();
}


/**
 * @brief ValidationDialog::cellSelected
 * @param nRow
 * @param nCol
 */
void ValidationDialog::cellSelected(int nRow, int nCol)
{
    Q_UNUSED(nCol);

    QString ID = itemsID[nRow];
    bool okay;
    int numberID = ID.toInt(&okay);
    if (okay) {
        centerOnItem(numberID);
    }
}


/**
 * @brief ValidationDialog::setupLayout
 */
void ValidationDialog::setupLayout()
{
    itemsTable = new QTableWidget(1, 1, this);
    itemsTable->horizontalHeader()->setVisible(false);
    itemsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    itemsTable->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    itemsTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    itemsTable->setSelectionMode(QAbstractItemView::SingleSelection);
    itemsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    itemsTable->setFocusPolicy(Qt::NoFocus);

    loadingMovie = new QMovie(this);
    loadingMovie->setScaledSize(QSize(32, 32));
    loadingMovie->setFileName(":/Actions/Waiting");
    //loadingMovie->setFileName(":/Actions/Job_Building");
    //loadingMovie->setFileName(":/Gifs/Loading");

    statusIcon = new QLabel(this);
    label = new QLabel("Validation Report", this);
    label->setFont(QFont(font().family(), 14, 1));

    QHBoxLayout* titleLayout = new QHBoxLayout();
    titleLayout->addWidget(statusIcon);
    titleLayout->addSpacerItem(new QSpacerItem(5,0));
    titleLayout->addWidget(label, 1);

    revalidateButton = new QPushButton("Re-Validate", this);
    revalidateButton->setToolTip("Re-validate Model");
    connect(revalidateButton, SIGNAL(clicked()), this, SLOT(revalidate()));

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(revalidateButton, 0, Qt::AlignRight);

    QGridLayout* mainLayout = new QGridLayout(this);
    mainLayout->setSizeConstraint(QLayout::SetNoConstraint);
    mainLayout->addLayout(titleLayout, 0, 0);
    mainLayout->addWidget(itemsTable, 1, 0);
    mainLayout->addLayout(buttonLayout, 2, 0);
    mainLayout->setRowStretch(1, 1);
}


/**
 * @brief ValidationDialog::setupItemsTable
 * @param items
 */
void ValidationDialog::setupItemsTable(QStringList items)
{
    /* clear table and remove excess rows */
    itemsTable->clear();
    itemsID.clear();
    for (int row = itemsTable->rowCount() - 1; row >= 0; --row) {
        itemsTable->removeRow(row);
    }

    /* populate table with validation messages */
    for (int row = 0; row < items.count(); ++row) {
        QString reportMessage = items[row];
        QString message = reportMessage.split(']').last();
        QString id = reportMessage.split('[').last().split(']').first();
        itemsID.append(id);

        QTableWidgetItem* name = new QTableWidgetItem(message);
        name->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        //name->setTextAlignment(Qt::AlignVCenter);
        name->setTextAlignment(Qt::AlignBottom);

        itemsTable->insertRow(row);
        itemsTable->setItem(row, 0, name);
    }

    if (items.count() == 0) {
        label->setText("Validation Succeeded");
        QPixmap successPixmap(Theme::theme()->getImage("Actions", "Job_Built"));
        statusIcon->setPixmap(successPixmap);
    } else {
        label->setText("Validation Failed: " + QString::number(items.count()) + " Issue(s)");
        QPixmap failPixmap(Theme::theme()->getImage("Actions", "Job_Failed"));
        statusIcon->setPixmap(failPixmap);
    }

    revalidateButton->setEnabled(true);
}

