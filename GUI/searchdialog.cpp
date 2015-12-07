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
    QScrollArea* scrollableSearchResults = new QScrollArea(this);
    QWidget* scrollableWidget = new QWidget(this);

    resultsLayout = new QVBoxLayout();
    QVBoxLayout* layout = new QVBoxLayout();
    QVBoxLayout* resultsMainLayout = new QVBoxLayout();
    QHBoxLayout* headerLayout = new QHBoxLayout();
    QVBoxLayout* headerLabelsLayout = new QVBoxLayout();

    QFont guiFont = QFont("Verdana", 8.5);
    int keyLabelWidth = fontMetrics().width("WWWWWWWW");

    QGroupBox* headerBox = new QGroupBox(this);
    searchLabel = constructHeadearLabel("Search", headerLabelsLayout, keyLabelWidth);
    aspectsLabel = constructHeadearLabel("Aspects", headerLabelsLayout, keyLabelWidth);
    kindsLabel = constructHeadearLabel("Kinds", headerLabelsLayout, keyLabelWidth);

    guiFont.setPointSize(10);
    searchLabel->setFont(guiFont);

    notFoundLabel = new QLabel("Search Not Found", this);
    guiFont.setItalic(true);
    notFoundLabel->setStyleSheet("padding: 10px 8px;");
    notFoundLabel->setFont(guiFont);
    resultsLayout->addWidget(notFoundLabel);

    headerBox->setMinimumWidth(minimumSize.width() * LABEL_RATIO / 2);
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
    setMinimumSize(minimumSize);
    setWindowTitle("Search Results");
    setVisible(false);

    setupMenus(headerLayout);
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
}


/**
 * @brief SearchDialog::updateHedearLabels
 * @param search
 * @param aspects
 * @param kinds
 */
void SearchDialog::updateHedearLabels(QString search, QStringList aspects, QStringList kinds)
{
    searchLabel->setText("\"<i>" + search + "</i>\"");

    QString aspectsText;
    if (aspects.isEmpty()) {
        aspectsText = "All Aspects";
    } else {
        foreach (QString aspect, aspects) {
            aspectsText += aspect + ", ";
        }
        aspectsText.truncate(aspectsText.length() - 2);
    }
    aspectsLabel->setText("<i>" + aspectsText + "</i>");

    QString kindsText;
    if (kinds.isEmpty()) {
        kindsText = "All Kinds";
    } else {
        foreach (QString kind, kinds) {
            kindsText += kind + ", ";
        }
        kindsText.truncate(kindsText.length() - 2);
    }
    kindsLabel->setText("<i>" + kindsText + "</i>");
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
void SearchDialog::sortItems()
{
    QRadioButton* rb = qobject_cast<QRadioButton*>(QObject::sender());
    if (!rb->isChecked()) {
        return;
    }

    QList<SearchItem*> searchItemsCopy;
    searchItemsCopy.append(searchItems);

    QString key = rb->text();

    for (int x = 0; x < searchItemsCopy.count(); x++) {

        SearchItem* searchItem = searchItemsCopy.at(x);
        QString searchItemLabel = searchItem->getKeyValue(key);

        for (int i = 0; i < searchItemsCopy.count(); i++) {
            SearchItem* item = searchItemsCopy.at(i);
            if (!item || item == searchItem) {
                continue;
            }
            QString itemLabel = item->getKeyValue(key);
            int compare = searchItemLabel.compare(itemLabel, Qt::CaseInsensitive);
            if (compare <= 0) {
                // remove item
                resultsLayout->removeWidget(searchItem);
                searchItems.removeAll(searchItem);
                // re-insert item
                resultsLayout->insertWidget(i, searchItem);
                searchItems.insert(i, searchItem);
                return;
            }
        }
    }
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
    hLayout->addWidget(keyLabel);
    hLayout->addWidget(valueLabel);
    vLayout->addLayout(hLayout);
    return valueLabel;
}


/**
 * @brief SearchDialog::setupMenus
 */
void SearchDialog::setupMenus(QLayout* layout)
{
    int buttonSize = 30;

    // setup the sort menu and button
    QWidgetAction* kindAction = new QWidgetAction(this);
    QRadioButton* rbk = new QRadioButton("kind", this);
    kindAction->setDefaultWidget(rbk);

    QWidgetAction* labelAction = new QWidgetAction(this);
    QRadioButton* rbl = new QRadioButton("label", this);
    rbl->setChecked(true);
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

    sortButton->hide();

    layout->addWidget(refreshButton);
    layout->addWidget(settingsButton);
    layout->addWidget(sortButton);

    setStyleSheet("QPushButton{ border: 1px solid darkGray; border-radius: 5px; }"
                  "QPushButton:hover{ border: 2px solid rgb(150,150,150); background: white; }"
                  "QPushButton::menu-indicator{ width: 0px; image: none; }"
                  "QMenu{ padding: 3px; }");

    connect(rbk, SIGNAL(toggled(bool)), this, SLOT(sortItems()));
    connect(rbl, SIGNAL(toggled(bool)), this, SLOT(sortItems()));
    connect(rbk, SIGNAL(toggled(bool)), sortMenu, SLOT(hide()));
    connect(rbl, SIGNAL(toggled(bool)), sortMenu, SLOT(hide()));

    connect(cbc, SIGNAL(toggled(bool)), this, SIGNAL(searchDialog_clickToCenter(bool)));
    connect(cbd, SIGNAL(toggled(bool)), this, SIGNAL(searchDialog_doubleClickToExpand(bool)));
    connect(cbc, SIGNAL(toggled(bool)), settingsMenu, SLOT(hide()));
    connect(cbd, SIGNAL(toggled(bool)), settingsMenu, SLOT(hide()));

    connect(refreshButton, SIGNAL(clicked()), this, SIGNAL(searchDialog_refresh()));
}

