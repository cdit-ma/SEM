#include "searchdialog.h"

#include <QDebug>

#define DIALOG_PADDING 200

#define ICON_RATIO 0.8

#define LAYOUT_MARGIN 2
#define LAYOUT_SPACING 5

#define DEFAULT_SORT_KEY "label"


/**
 * @brief SearchDialog::SearchDialog
 * @param minimumSize
 * @param parent
 */
SearchDialog::SearchDialog(QSize minimumSize, MedeaWindow* window) : QDialog(window)
{
    if (!window) {
        qWarning() << "SearchDialog::SearchDialog - Parent window is null.";
        return;
    }

    parentWindow = window;
    currentSortKey = DEFAULT_SORT_KEY;

    setMinimumSize(minimumSize);
    setWindowTitle("Search Results");
    setVisible(false);
    setupLayout();
}


/**
 * @brief SearchDialog::addSearchItems
 * @param searchResult
 */
bool SearchDialog::addSearchItems(QList<GraphMLItem*> searchResult)
{
    if (searchResult == resultGraphmlItems) {
        return false;
    }

    clear();

    // for each item to display, create a button for it and add it to the results layout
    foreach (GraphMLItem* gmlItem, searchResult) {
        SearchItem* searchItem = new SearchItem(gmlItem, this);
        searchItem->connectToWindow(parentWindow);
        addSearchItem(searchItem);
    }

    resultGraphmlItems = searchResult;
    return true;
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

    // clear the stored lists
    searchItems.clear();
    resultGraphmlItems.clear();

    // make sure that the menus are close
    sortMenu->close();
    settingsMenu->close();
}


/**
 * @brief SearchDialog::updateHedearLabels
 * @param search
 * @param aspects
 * @param kinds
 */
void SearchDialog::updateHedearLabels(QString search, QStringList aspects, QStringList kinds, QStringList keys)
{
    searchLabel->setText("\"" + search + "\"");

    QString aspectsText;
    if (aspects.isEmpty()) {
        aspectsText = "Entire Model";
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

    QString keysText;
    if (keys.isEmpty()) {
        keysText = "All Available Data Keys";
    } else {
        foreach (QString key, keys) {
            keysText += key + ", ";
        }
        keysText.truncate(keysText.length() - 2);
    }
    dataKeysLabel->setText(keysText);
}


/**
 * @brief SearchDialog::show
 */
void SearchDialog::show()
{
    if (searchItems.isEmpty()) {
        notFoundLabel->show();
        //resize(minimumSize());
    } else {
        notFoundLabel->hide();
        //resize(MAX_ITEM_WIDTH, height());
    }
    sortItems();
    QDialog::show();
    QDialog::raise();
}


/**
 * @brief SearchDialog::returnFocus
 */
void SearchDialog::returnFocus()
{

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
    sortItems(key);
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

    double fontSize = 8.5;
    defaultFont = QFont("Verdana", fontSize);
    int keyLabelWidth = fontMetrics().width("WWWWWWWW");

    QGroupBox* headerBox = new QGroupBox(this);
    searchLabel = constructHeadearLabel("Search", headerLabelsLayout, keyLabelWidth);
    aspectsLabel = constructHeadearLabel("Aspect(s)", headerLabelsLayout, keyLabelWidth);
    kindsLabel = constructHeadearLabel("Kind(s)", headerLabelsLayout, keyLabelWidth);
    dataKeysLabel = constructHeadearLabel("Data Key(s)", headerLabelsLayout, keyLabelWidth);

    searchLabel->setStyleSheet("color: darkBlue;");

    searchFont = defaultFont;
    searchFont.setPointSize(10);
    searchFont.setItalic(true);

    notFoundLabel = new QLabel("Search Not Found", this);
    notFoundLabel->setStyleSheet("color: darkRed; padding: 10px 8px;");
    notFoundLabel->setFont(searchFont);
    resultsLayout->addWidget(notFoundLabel);

    searchFont.setItalic(false);
    searchLabel->setFont(searchFont);

    headerBox->setLayout(headerLabelsLayout);
    headerBox->setMinimumWidth(minimumWidth() * LABEL_RATIO / 2);
    headerLayout->addWidget(headerBox, 0, Qt::AlignTop);

    resultsMainLayout->addLayout(headerLayout);
    resultsMainLayout->addLayout(resultsLayout);
    resultsMainLayout->addStretch(1);

    scrollableWidget->setLayout(resultsMainLayout);
    scrollableSearchResults->setWidget(scrollableWidget);
    scrollableSearchResults->setWidgetResizable(true);

    layout->addWidget(scrollableSearchResults);
    layout->setMargin(0);

    setLayout(layout);
    setupMenus(headerLayout);

    setStyleSheet("QGroupBox{ border: none; padding: 0px; }"
                  "QLabel{ color: rgb(30,30,30); }"
                  "QPushButton{ border: 1px solid darkGray; border-radius: 4px; }"
                  "QPushButton:hover{ border: 2px solid rgb(150,150,150); background: white; }"
                  "QPushButton::menu-indicator{ width: 0px; image: none; }"
                  "QMenu{ padding: 3px; }");
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

    // set the default sort key - check the corresponding radio button
    if (currentSortKey == "kind") {
        rbk->setChecked(true);
    } else if (currentSortKey == "label") {
        rbl->setChecked(true);
    }

    sortMenu = new QMenu(this);
    sortMenu->addAction(kindAction);
    sortMenu->addAction(labelAction);

    QPushButton* sortButton = new QPushButton("Sort Results", this);
    sortButton->setToolTip("Sort results by label or entity kind");
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
    settingsButton->setIconSize(QSize(buttonSize, buttonSize) * 0.7);
    settingsButton->setToolTip("Click function settings");
    settingsButton->setMenu(settingsMenu);

    // setup the refresh button
    QPushButton* refreshButton = new QPushButton(QIcon(":/Actions/Refresh"), "", this);
    refreshButton->setToolTip("Refresh search results");
    refreshButton->setIconSize(QSize(buttonSize, buttonSize) * 0.7);

    sortButton->setFixedSize(sortButton->fontMetrics().width("Sort Results") + 15, buttonSize);
    settingsButton->setFixedSize(buttonSize, buttonSize);
    refreshButton->setFixedSize(buttonSize, buttonSize);

    QHBoxLayout* hLayout = new QHBoxLayout();
    hLayout->addWidget(sortButton);
    hLayout->addSpacing(2);
    hLayout->addWidget(settingsButton);
    hLayout->addSpacing(2);
    hLayout->addWidget(refreshButton);

    QVBoxLayout* vLayout = new QVBoxLayout();
    vLayout->addLayout(hLayout);
    vLayout->addStretch();

    layout->addLayout(vLayout);

    connect(rbk, SIGNAL(toggled(bool)), this, SLOT(sortItems(bool)));
    connect(rbl, SIGNAL(toggled(bool)), this, SLOT(sortItems(bool)));
    connect(rbk, SIGNAL(toggled(bool)), sortMenu, SLOT(hide()));
    connect(rbl, SIGNAL(toggled(bool)), sortMenu, SLOT(hide()));

    connect(cbc, SIGNAL(toggled(bool)), this, SIGNAL(searchDialog_clickToCenter(bool)));
    connect(cbd, SIGNAL(toggled(bool)), this, SIGNAL(searchDialog_doubleClickToExpand(bool)));
    connect(cbc, SIGNAL(toggled(bool)), settingsMenu, SLOT(hide()));
    connect(cbd, SIGNAL(toggled(bool)), settingsMenu, SLOT(hide()));

    connect(refreshButton, SIGNAL(clicked()), this, SIGNAL(searchDialog_refresh()));

    connect(sortMenu, SIGNAL(aboutToHide()), this, SLOT(returnFocus()));
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
 * @brief SearchDialog::sortItems
 * @param sortKey
 */
void SearchDialog::sortItems(QString sortKey)
{
    if (sortKey == currentSortKey) {
        return;
    }

    if (sortKey.isEmpty()) {
        sortKey = currentSortKey;
    } else if (sortKey != "kind" && sortKey != "label") {
        return;
    }

    QHash<SearchItem*, QString> itemValueHash;
    QStringList keyValues;

    foreach (SearchItem* item, searchItems) {
        QString val = item->getKeyValue(sortKey);
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

    currentSortKey = sortKey;
}

