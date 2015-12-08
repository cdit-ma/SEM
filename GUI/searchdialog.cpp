#include "searchdialog.h"

#include <QDebug>

#define DIALOG_PADDING 200

#define LABEL_RATIO (2.0 / 5.0)
#define ICON_RATIO 0.8

#define LAYOUT_MARGIN 2
#define LAYOUT_SPACING 5


/**
 * @brief SearchDialog::SearchDialog
 * @param minimumSize
 * @param parent
 */
SearchDialog::SearchDialog(QSize minimumSize, QWidget* parent) : QDialog(parent)
{
    sortedByKind = false;
    sortedByLabel = false;

    setMinimumSize(minimumSize);
    setWindowTitle("Search Results");
    setVisible(false);
    setupLayout();
}


/**
 * @brief SearchDialog::addSearchItem
 * @param searchItem
 */
void SearchDialog::addSearchItem(SearchItem* searchItem)
{
    if (!searchItem) {
        return;
    }

    MAX_ITEM_WIDTH = qMax(MAX_ITEM_WIDTH, searchItem->getItemWidth());
    connect(this, SIGNAL(searchDialog_clickToCenter(bool)), searchItem, SLOT(setClickToCenter(bool)));
    connect(this, SIGNAL(searchDialog_doubleClickToExpand(bool)), searchItem, SLOT(setDoubleClickToExpand(bool)));

    resultsLayout->addWidget(searchItem);
    searchItems.append(searchItem);
}


/**
 * @brief SearchDialog::insertSearchItem
 * @param item
 */
void SearchDialog::insertSearchItem(SearchItem* searchItem)
{
    if (!searchItem) {
        return;
    }

    MAX_ITEM_WIDTH = qMax(MAX_ITEM_WIDTH, searchItem->getItemWidth());
    connect(this, SIGNAL(searchDialog_clickToCenter(bool)), searchItem, SLOT(setClickToCenter(bool)));
    connect(this, SIGNAL(searchDialog_doubleClickToExpand(bool)), searchItem, SLOT(setDoubleClickToExpand(bool)));

    if (!searchItems.isEmpty()) {
        QString searchItemLabel = searchItem->getKeyValue("label");
        for (int i = 0; i < searchItems.count(); i++) {
            SearchItem* item = searchItems.at(i);
            if (!item) {
                continue;
            }
            QString itemLabel = item->getKeyValue("label");
            int compare = searchItemLabel.compare(itemLabel, Qt::CaseInsensitive);
            if (compare <= 0) {
                resultsLayout->insertWidget(i, searchItem);
                searchItems.insert(i, searchItem);
                return;
            }
        }
    }

    resultsLayout->addWidget(searchItem);
    searchItems.append(searchItem);
}


/**
 * @brief SearchDialog::clear
 */
void SearchDialog::clear()
{
    // remove the layout item's widget, then remove the layout item
    for (int i = resultsLayout->count()-1; i >= 0; i--) {
        QWidget* widget = resultsLayout->itemAt(i)->widget();
        if (widget && widget != notFoundLabel) {
            delete resultsLayout->itemAt(i)->widget();
        }
    }

    // clear the stored list
    searchItems.clear();

    sortedByKind = false;
    sortedByLabel = false;
}


/**
 * @brief SearchDialog::updateHedearLabels
 * @param search
 * @param aspects
 * @param kinds
 */
void SearchDialog::updateHedearLabels(QString search, QStringList aspects, QStringList kinds)
{
    searchLabel->setText("\"" + search + "\"");

    QString aspectsText;
    if (aspects.isEmpty()) {
        aspectsText = "All Aspects";
    } else {
        foreach (QString aspect, aspects) {
            aspectsText += aspect + ", ";
        }
        aspectsText.truncate(aspectsText.length() - 2);
    }
    aspectsLabel->setText(aspectsText);

    QString kindsText;
    if (kinds.isEmpty()) {
        kindsText = "All Kinds";
    } else {
        foreach (QString kind, kinds) {
            kindsText += kind + ", ";
        }
        kindsText.truncate(kindsText.length() - 2);
    }
    kindsLabel->setText(kindsText);
}


/**
 * @brief SearchDialog::show
 */
void SearchDialog::show()
{
    if (searchItems.isEmpty()) {
        notFoundLabel->show();
        resize(minimumSize());
    } else {
        notFoundLabel->hide();
        resize(MAX_ITEM_WIDTH, height());
    }
    QDialog::show();
}


/**
 * @brief SearchDialog::sortItems
 * @param rb
 */
void SearchDialog::sortItems(bool checked)
{
    if (!checked || searchItems.isEmpty()) {
        return;
    }

    QRadioButton* rb = qobject_cast<QRadioButton*>(QObject::sender());
    QString key = rb->text();

    QHash<SearchItem*, QString> itemValueHash;
    QStringList keyValues;

    foreach (SearchItem* item, searchItems) {
        QString val = item->getKeyValue(key);
        keyValues.append(val);
        itemValueHash[item] = val;
        resultsLayout->removeWidget(item);
    }

    keyValues.sort(Qt::CaseInsensitive);

    foreach (QString value, keyValues) {
        SearchItem* item = itemValueHash.key(value);
        itemValueHash.remove(item);
        resultsLayout->addWidget(item);
    }

    sortedByKind = key == "kind";
    sortedByLabel = key == "label";
}


/**
 * @brief SearchDialog::constructHeadearLabel
 * @param labelText
 * @param vLayout
 * @param fixedWidth
 * @return
 */
QLabel* SearchDialog::constructHeadearLabel(QString labelText, QVBoxLayout* vLayout, int fixedWidth)
{
    QHBoxLayout* hLayout = new QHBoxLayout();
    QLabel* keyLabel = new QLabel(labelText + ":", this);
    QLabel* valueLabel = new QLabel(this);
    valueLabel->setTextFormat(Qt::RichText);
    keyLabel->setFixedWidth(fixedWidth);
    keyLabel->setStyleSheet("color: rgb(80,80,80);");
    hLayout->addWidget(keyLabel);
    hLayout->addWidget(valueLabel);
    vLayout->addLayout(hLayout);
    return valueLabel;
}


/**
 * @brief SearchDialog::setupLayout
 */
void SearchDialog::setupLayout()
{
    QScrollArea* scrollableSearchResults = new QScrollArea(this);
    QWidget* scrollableWidget = new QWidget(this);

    resultsLayout = new QVBoxLayout();
    QVBoxLayout* layout = new QVBoxLayout();
    QVBoxLayout* resultsMainLayout = new QVBoxLayout();
    QHBoxLayout* headerLayout = new QHBoxLayout();
    QVBoxLayout* headerLabelsLayout = new QVBoxLayout();

    defaultFont = QFont("Verdana", 8.5);
    int keyLabelWidth = fontMetrics().width("WWWWWWWW");

    QGroupBox* headerBox = new QGroupBox(this);
    searchLabel = constructHeadearLabel("Search", headerLabelsLayout, keyLabelWidth);
    aspectsLabel = constructHeadearLabel("Aspects", headerLabelsLayout, keyLabelWidth);
    kindsLabel = constructHeadearLabel("Kinds", headerLabelsLayout, keyLabelWidth);

    setStyleSheet("QLabel{ color: rgb(30,30,30); }");
    searchLabel->setStyleSheet("color: darkBlue;");
    //searchLabel->setStyleSheet("color: rgb(30,60,110);");

    searchFont = defaultFont;
    searchFont.setPointSize(10);
    searchFont.setItalic(true);

    notFoundLabel = new QLabel("Search Not Found", this);
    notFoundLabel->setStyleSheet("color: darkRed; padding: 10px 8px;");
    notFoundLabel->setFont(searchFont);
    resultsLayout->addWidget(notFoundLabel);

    searchFont.setItalic(false);
    searchLabel->setFont(searchFont);

    headerBox->setMinimumWidth(minimumWidth() * LABEL_RATIO / 2);
    headerBox->setLayout(headerLabelsLayout);

    headerLayout->addWidget(headerBox);
    headerLayout->addStretch();

    resultsMainLayout->addLayout(headerLayout);
    resultsMainLayout->addLayout(resultsLayout);
    resultsMainLayout->addStretch();

    scrollableWidget->setLayout(resultsMainLayout);
    scrollableSearchResults->setWidget(scrollableWidget);
    scrollableSearchResults->setWidgetResizable(true);

    layout->addWidget(scrollableSearchResults);
    layout->setMargin(0);

    setLayout(layout);
    setupMenus(headerLayout);
}


/**
 * @brief SearchDialog::setupMenus
 */
void SearchDialog::setupMenus(QHBoxLayout* layout)
{
    int buttonSize = 30;

    // setup the sort menu and button
    QWidgetAction* kindAction = new QWidgetAction(this);
    QRadioButton* rbk = new QRadioButton("kind", this);
    kindAction->setDefaultWidget(rbk);

    QWidgetAction* labelAction = new QWidgetAction(this);
    QRadioButton* rbl = new QRadioButton("label", this);
    labelAction->setDefaultWidget(rbl);

    sortMenu = new QMenu(this);
    sortMenu->addAction(kindAction);
    sortMenu->addAction(labelAction);

    QPushButton* sortButton = new QPushButton("Sort Results", this);
    sortButton->setMenu(sortMenu);

    // setup the settings menu and button
    QWidgetAction* clickAction = new QWidgetAction(this);
    QCheckBox* cbc = new QCheckBox("Click to center", this);
    cbc->setChecked(true);
    clickAction->setDefaultWidget(cbc);

    QWidgetAction* doubleClickAction = new QWidgetAction(this);
    QCheckBox* cbd = new QCheckBox("Double-click to expand", this);
    doubleClickAction->setDefaultWidget(cbd);

    settingsMenu = new QMenu(this);
    settingsMenu->addAction(clickAction);
    settingsMenu->addAction(doubleClickAction);

    QPushButton* settingsButton = new QPushButton(QIcon(":/Actions/Settings"), "", this);
    settingsButton->setIconSize(QSize(buttonSize, buttonSize) * ICON_RATIO);
    settingsButton->setMenu(settingsMenu);

    // setup the refresh button
    QPushButton* refreshButton = new QPushButton(QIcon(":/Actions/Refresh"), "", this);
    refreshButton->setIconSize(QSize(buttonSize, buttonSize) * ICON_RATIO);

    sortButton->setFixedSize(buttonSize * 2, buttonSize);
    settingsButton->setFixedSize(buttonSize, buttonSize);
    refreshButton->setFixedSize(buttonSize, buttonSize);

    layout->addWidget(sortButton);
    layout->addSpacing(2);
    layout->addWidget(settingsButton);
    layout->addSpacing(2);
    layout->addWidget(refreshButton);

    setStyleSheet("QPushButton{ border: 1px solid darkGray; border-radius: 5px; }"
                  "QPushButton:hover{ border: 2px solid rgb(150,150,150); background: white; }"
                  "QPushButton::menu-indicator{ width: 0px; image: none; }"
                  "QMenu{ padding: 3px; }");

    connect(rbk, SIGNAL(toggled(bool)), this, SLOT(sortItems(bool)));
    connect(rbl, SIGNAL(toggled(bool)), this, SLOT(sortItems(bool)));
    connect(rbk, SIGNAL(toggled(bool)), sortMenu, SLOT(hide()));
    connect(rbl, SIGNAL(toggled(bool)), sortMenu, SLOT(hide()));

    connect(cbc, SIGNAL(toggled(bool)), this, SIGNAL(searchDialog_clickToCenter(bool)));
    connect(cbd, SIGNAL(toggled(bool)), this, SIGNAL(searchDialog_doubleClickToExpand(bool)));
    connect(cbc, SIGNAL(toggled(bool)), settingsMenu, SLOT(hide()));
    connect(cbd, SIGNAL(toggled(bool)), settingsMenu, SLOT(hide()));

    connect(refreshButton, SIGNAL(clicked()), this, SIGNAL(searchDialog_refresh()));
}

