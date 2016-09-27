#include "shortcutdialog.h"
#include <QHeaderView>
#include <QAction>
#include <QPalette>
#include <QBrush>
#include <QDebug>
#include <QStringBuilder>
#define WIDTH 450

#define IMAGE_PREFIX QTableWidgetItem::UserType + 1
#define IMAGE_NAME QTableWidgetItem::UserType + 2

#define HEIGHT 650

#include "../../theme.h"

ShortcutDialog::ShortcutDialog(QWidget *parent) :
    QDialog(parent)
{
    setWindowTitle("MEDEA - Shortcuts");
    setWindowFlags(windowFlags() & (~Qt::WindowContextHelpButtonHint));
    setWindowIcon(Theme::theme()->getImage("Actions", "Keyboard"));
    setModal(false);

    resize(WIDTH, HEIGHT);

    setupLayout();

    connect(Theme::theme(), &Theme::theme_Changed, this, &ShortcutDialog::themeChanged);
}

void ShortcutDialog::addShortcut(QString shortcut, QString description, QString alias, QString image)
{
    QFont boldFont;
    QFont italicFont;
    boldFont.setBold(true);
    italicFont.setItalic(true);
    boldFont.setPointSize(boldFont.pointSize() - 2);
    italicFont.setPointSize(italicFont.pointSize() - 1);

    #ifdef Q_OS_DARWIN
        //Replace shortcut with Command.
        shortcut.replace("CTRL", "COMMAND");
    #endif

    QTableWidgetItem * shortcutR = new QTableWidgetItem(shortcut);
    QTableWidgetItem * descriptionR = new QTableWidgetItem(description);

    descriptionR->setData(IMAGE_PREFIX, alias);
    descriptionR->setData(IMAGE_NAME, image);

    descriptionR->setFlags(shortcutR->flags() ^ Qt::ItemIsEditable);
    descriptionR->setTextAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    descriptionR->setFont(italicFont);

    shortcutR->setFlags(shortcutR->flags() ^ Qt::ItemIsEditable);
    shortcutR->setTextAlignment(Qt::AlignCenter);

    if(tableWidget){
        int insertLocation = tableWidget->rowCount();
        tableWidget->insertRow(insertLocation);
        tableWidget->setItem(insertLocation, 0, descriptionR);
        tableWidget->setVerticalHeaderItem(insertLocation, shortcutR);
        tableWidget->resizeColumnsToContents();
    }
}

void ShortcutDialog::addTitle(QString label, QString alias, QString image)
{
    if(tableWidget){
        QFont boldFont;
        boldFont.setBold(true);
        boldFont.setPointSize(boldFont.pointSize() + 1);

        QTableWidgetItem * titleRow = new QTableWidgetItem();
        titleRow->setFlags(titleRow->flags() ^ Qt::ItemIsEditable);
        titleRow->setFont(boldFont);

        QTableWidgetItem* textItem = new QTableWidgetItem(label);
        textItem->setFlags(textItem->flags() ^ Qt::ItemIsEditable);
        textItem->setTextAlignment(Qt::AlignCenter);
        textItem->setFont(boldFont);

        int insertLocation = tableWidget->rowCount();
        tableWidget->insertRow(insertLocation);
        tableWidget->setItem(insertLocation, 0, textItem);
        tableWidget->setVerticalHeaderItem(insertLocation, titleRow);
    }
}

void ShortcutDialog::resizeTable()
{
    themeChanged();
}

void ShortcutDialog::themeChanged()
{

    Theme* theme = Theme::theme();
    this->setStyleSheet(theme->getWindowStyleSheet() % theme->getScrollBarStyleSheet());
    tableWidget->setStyleSheet(theme->getAbstractItemViewStyleSheet() +  "QAbstractItemView::item {"
                                                                         "border: 1px solid " + theme->getDisabledBackgroundColorHex() + ";"
                                                                         "}");
    this->tableWidget->verticalHeader()->setStyleSheet(theme->getAbstractItemViewStyleSheet());

    for (int row = 0 ; row < tableWidget->rowCount() ; ++row) {
        //QTableWidgetItem* header = tableWidget->verticalHeaderItem(row);
        QTableWidgetItem* header = tableWidget->item(row, 0);
        if(header){
            QString prefix = header->data(IMAGE_PREFIX).toString();
            QString image = header->data(IMAGE_NAME).toString();
            if(!prefix.isEmpty() && !image.isEmpty()){
                header->setIcon(theme->getIcon(prefix, image));
            }
        }
    }
}

void ShortcutDialog::setupLayout()
{
    layout = new QVBoxLayout();
    this->setLayout(layout);
    tableWidget = new QTableWidget(0, 1, this);
    tableWidget->verticalHeader()->setVisible(true);
    tableWidget->verticalHeader()->setHighlightSections(false);
    tableWidget->horizontalHeader()->setVisible(false);
    tableWidget->horizontalHeader()->setStretchLastSection(true);
    tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    tableWidget->setShowGrid(false);
    tableWidget->setEnabled(true);
    tableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    tableWidget->setFocusPolicy(Qt::NoFocus);
    layout->addWidget(tableWidget);
}
