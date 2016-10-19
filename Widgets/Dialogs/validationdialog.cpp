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
    setMinimumSize(DIALOG_MIN_WIDTH, DIALOG_MIN_HEIGHT);

    Qt::WindowFlags flags = 0;
    flags |= Qt::WindowMaximizeButtonHint;
    flags |= Qt::WindowCloseButtonHint;
    flags |= Qt::WindowMinimizeButtonHint;

    setWindowModality(Qt::NonModal);
    setWindowFlags(flags);
    setWindowTitle("Validation Report");

    connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));
    themeChanged();

    connect(itemsTable, &QTableWidget::cellDoubleClicked, this, &ValidationDialog::cellSelected);
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
 * @brief ValidationDialog::showDialog
 */
void ValidationDialog::showDialog()
{
    show();
    raise();
}


/**
 * @brief ValidationDialog::themeChanged
 */
void ValidationDialog::themeChanged()
{
    Theme* theme = Theme::theme();
    setStyleSheet("QLabel{ background: rgba(0,0,0,0); color:" + theme->getTextColorHex() + ";}"
                  + theme->getDialogStyleSheet()
                  + theme->getScrollBarStyleSheet()
                  + theme->getAltAbstractItemViewStyleSheet()
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
    itemsTable->verticalHeader()->setDefaultAlignment(Qt::AlignCenter);
    itemsTable->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    itemsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    itemsTable->horizontalHeader()->setVisible(false);
    itemsTable->setSelectionMode(QAbstractItemView::SingleSelection);
    itemsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    itemsTable->setFocusPolicy(Qt::NoFocus);
    itemsTable->setShowGrid(false);

    loadingMovie = new QMovie(this);
    loadingMovie->setScaledSize(QSize(32, 32));
    loadingMovie->setFileName(":/Actions/Waiting");
    //loadingMovie->setFileName(":/Actions/Job_Building");
    //loadingMovie->setFileName(":/Gifs/Loading");

    statusIcon = new QLabel(this);

    label = new QLabel("Validation Report", this);
    label->setFont(QFont(font().family(), 13, 1));
    label->setWordWrap(true);

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
    mainLayout->setSpacing(DIALOG_SPACING);
    mainLayout->setSizeConstraint(QLayout::SetNoConstraint);
    mainLayout->addLayout(titleLayout, 0, 0);
    mainLayout->addWidget(itemsTable, 1, 0);
    mainLayout->addLayout(buttonLayout, 2, 0);
    mainLayout->setColumnStretch(0, 1);
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

    bool succeeded = items.isEmpty();

    if (succeeded) {

        label->setText("Validation Succeeded");
        QPixmap successPixmap(Theme::theme()->getImage("Actions", "Job_Built"));
        statusIcon->setPixmap(successPixmap);

    } else {

        /* populate table with validation messages */
        for (int row = 0; row < items.count(); ++row) {
            QString reportMessage = items[row];
            QString message = reportMessage.split(']').last();
            QString id = reportMessage.split('[').last().split(']').first();
            itemsID.append(id);

            QTableWidgetItem* name = new QTableWidgetItem(message);
            name->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
            itemsTable->insertRow(row);
            itemsTable->setItem(row, 0, name);
        }

        label->setText("Validation Failed: " + QString::number(items.count()) + " Issue(s)");
        QPixmap failPixmap(Theme::theme()->getImage("Actions", "Job_Failed"));
        statusIcon->setPixmap(failPixmap);
    }

    revalidateButton->setEnabled(true);
}

