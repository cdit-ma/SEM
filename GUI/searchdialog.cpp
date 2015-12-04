#include "searchdialog.h"

#define LABEL_RATIO (2.0 / 5.0)
#define ICON_RATIO 0.8

#define LAYOUT_MARGIN 2
#define LAYOUT_SPACING 5

/**
 * @brief SearchDialog::SearchDialog
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

    QLabel* objectLabel = new QLabel("Entity Label:", this);
    //QLabel* objectLabel = new QLabel("ENTITY LABEL:", this);
    QLabel* iconHolder = new QLabel(this);

    iconHolder->setFixedWidth(minimumSize.height() * ICON_RATIO);
    objectLabel->setFixedWidth(minimumSize.width() * LABEL_RATIO);
    headerLayout->addWidget(objectLabel);
    headerLayout->addWidget(iconHolder);
    headerLayout->addStretch();

    QFont font = objectLabel->font();
    font.setPointSize(10);
    objectLabel->setFont(font);
    objectLabel->setFixedHeight(35);

    /*
    QLabel* objectLabel = new QLabel("Entity Label:", this);
    QLabel* parentLabel = new QLabel("Location:", this);
    QLabel* iconHolder = new QLabel(this);

    iconHolder->setFixedWidth(minimumSize.height() * ICON_RATIO);
    objectLabel->setFixedWidth(minimumSize.width() * LABEL_RATIO);
    parentLabel->setMinimumWidth(minimumSize.width() - objectLabel->width());

    headerLayout->setMargin(LAYOUT_MARGIN);
    headerLayout->setSpacing(LAYOUT_SPACING);
    headerLayout->addWidget(objectLabel);
    headerLayout->addWidget(iconHolder);
    headerLayout->addWidget(parentLabel);
    */

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
            if (compare < 0) {
                resultsLayout->insertWidget(i, searchItem);
                searchItems.append(searchItem);
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
        if (resultsLayout->itemAt(i)->widget()) {
            delete resultsLayout->itemAt(i)->widget();
        }
    }
    // clear the stored list
    searchItems.clear();
}


/**
 * @brief SearchDialog::sortItems
 * @param rb
 */
void SearchDialog::sortItems()
{
    QRadioButton* rb = qobject_cast<QRadioButton*>(QObject::sender());
    QString key = rb->text();

    foreach (SearchItem* searchItem, searchItems) {
        QString searchItemLabel = searchItem->getKeyValue(key);
        for (int i = 0; i < searchItems.count(); i++) {
            SearchItem* item = searchItems.at(i);
            if (!item) {
                continue;
            }
            QString itemLabel = item->getKeyValue(key);
            int compare = searchItemLabel.compare(itemLabel, Qt::CaseInsensitive);
            if (compare < 0) {
                resultsLayout->insertWidget(i, searchItem);
                searchItems.append(searchItem);
                return;
            }
        }
    }

    sortMenu->hide();
}


/**
 * @brief SearchDialog::setupMenus
 */
void SearchDialog::setupMenus(QLayout* layout)
{
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
    layout->addWidget(sortButton);

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
    settingsButton->setMenu(settingsMenu);
    layout->addWidget(settingsButton);

    sortButton->setFixedSize(80, 30);
    settingsButton->setFixedSize(50, 35);

    sortButton->hide();

    setStyleSheet("QPushButton::menu-indicator{ width: 0px; image: none; }"
                  "QMenu{ padding: 3px; }");

    connect(rbk, SIGNAL(toggled(bool)), this, SLOT(sortItems()));
    connect(rbl, SIGNAL(toggled(bool)), this, SLOT(sortItems()));
    connect(rbk, SIGNAL(toggled(bool)), sortMenu, SLOT(hide()));
    connect(rbl, SIGNAL(toggled(bool)), sortMenu, SLOT(hide()));

    connect(cbc, SIGNAL(toggled(bool)), this, SIGNAL(searchDialog_clickToCenter(bool)));
    connect(cbd, SIGNAL(toggled(bool)), this, SIGNAL(searchDialog_doubleClickToExpand(bool)));
    connect(cbc, SIGNAL(toggled(bool)), settingsMenu, SLOT(hide()));
    connect(cbd, SIGNAL(toggled(bool)), settingsMenu, SLOT(hide()));
}

