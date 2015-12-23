#include "searchitem.h"
#include "../medeawindow.h"

#include <QDebug>

#define DIALOG_PADDING 200
#define MIN_HEIGHT 50.0

#define BUTTON_SIZE 28
#define BUTTON_RADIUS (BUTTON_SIZE / 6)
#define KEY_LABEL_WIDTH 100

#define LAYOUT_MARGIN 2
#define LAYOUT_SPACING 5
#define MARGIN_OFFSET (LAYOUT_MARGIN + LAYOUT_SPACING)

//#define LABEL_RATIO (2.0 / 5.0)
#define LABEL_RATIO 0.5
#define ICON_RATIO 0.8
#define ICON_SIZE (MIN_HEIGHT * ICON_RATIO - MARGIN_OFFSET)


/**
 * @brief SearchItem::SearchItem
 * @param item
 * @param parent
 */
SearchItem::SearchItem(GraphMLItem *item, QWidget *parent) : QLabel(parent)
{
    if (parent) {
        MIN_WIDTH = parent->width() - DIALOG_PADDING;
    } else {
        MIN_WIDTH = 100;
    }

    if (item) {
        graphMLItem = item;
        graphMLItemID = graphMLItem->getID();
    } else {
        qWarning() << "SearchItem:: Cannot create a search item widget with a NULL graphMLItem.";
        return;
    }

    selected = false;
    expanded = true;
    valuesSet = false;

    setupLayout();
    updateColor();
    expandItem();

    setClickToCenter(true);
    setDoubleClickToExpand(false);

    connect(centerOnButton, SIGNAL(clicked()), this, SIGNAL(searchItem_clicked()));
    connect(expandButton, SIGNAL(clicked()), this, SLOT(expandItem()));
    connect(centerOnButton, SIGNAL(clicked()), this, SLOT(centerOnItem()));
}


/**
 * @brief SearchItemButton::connectToWindow
 * This connects the signals and slots to the MEDEA window.
 */
void SearchItem::connectToWindow(QMainWindow* window)
{
    MedeaWindow* medea = dynamic_cast<MedeaWindow*>(window);
    if (medea) {
        connect(this, SIGNAL(searchItem_clicked()), medea, SLOT(searchItemClicked()));
        connect(medea, SIGNAL(window_searchItemClicked(SearchItem*)), this, SLOT(itemClicked(SearchItem*)));
        connect(this, SIGNAL(searchItem_centerOnItem(int)), medea, SLOT(on_searchResultItem_clicked(int)));
    }
}


/**
 * @brief SearchItem::getKeyValue
 * @param key
 * @return
 */
QString SearchItem::getKeyValue(QString key)
{
    if (key.isEmpty() ) {
        return "";
    }
    if (!dataKeys.contains(key)) {
        if (key == "label" && entityLabel) {
            return entityLabel->text();
        } else {
            qWarning() << "There is no value stored for data key: " << key;
            return "";
        }
    }
    if (graphMLItem && graphMLItem->getEntityAdapter()) {
        return graphMLItem->getEntityAdapter()->getDataValue("kind").toString();
    }
    return "";
}


/**
 * @brief SearchItem::getItemID
 * @return
 */
int SearchItem::getItemID()
{
    return graphMLItemID;
}


/**
 * @brief SearchItem::getItemWidth
 * @return
 */
int SearchItem::getItemWidth()
{
    return ITEM_WIDTH;
}


/**
 * @brief SearchItem::itemClicked
 * @param item
 */
void SearchItem::itemClicked(SearchItem *item)
{
    bool itemSelected = item == this;
    if (itemSelected != selected) {
        selected = itemSelected;
        updateColor();
    }
    if (selected) {
        centerOnItem();
    }
}


/**
 * @brief SearchItem::expandItem
 */
void SearchItem::expandItem()
{
    expanded = !expanded;
    if (expanded && !valuesSet) {
        getDataValues();
        valuesSet = true;
    }
    dataBox->setVisible(expanded);
    if (expanded) {
        setFixedHeight(MIN_HEIGHT + dataBox->sizeHint().height());
        expandButton->setIcon(QIcon(contractPixmap));
    } else {
        setFixedHeight(MIN_HEIGHT);
        expandButton->setIcon(QIcon(expandPixmap));
    }
}


/**
 * @brief SearchItem::centerOnItem
 */
void SearchItem::centerOnItem()
{
    emit searchItem_centerOnItem(graphMLItemID);
}


/**
 * @brief SearchItem::setClickToCenter
 * @param b
 */
void SearchItem::setClickToCenter(bool b)
{
    CLICK_TO_CENTER = b;
    centerOnButton->setVisible(!CLICK_TO_CENTER);
}


/**
 * @brief SearchItem::setDoubleClickToExpand
 * @param b
 */
void SearchItem::setDoubleClickToExpand(bool b)
{
    DOUBLE_CLICK_TO_EXPAND = b;
    if (DOUBLE_CLICK_TO_EXPAND) {
        connect(expandButton, SIGNAL(clicked()), this, SIGNAL(searchItem_clicked()));
    } else {
        disconnect(expandButton, SIGNAL(clicked()), this, SIGNAL(searchItem_clicked()));
    }
}


/**
 * @brief SearchItem::mouseReleaseEvent
 * @param event
 */
void SearchItem::mouseReleaseEvent(QMouseEvent* event)
{
    if (CLICK_TO_CENTER) {
        if (event->button() == Qt::LeftButton) {
            emit searchItem_clicked();
        }
    } else {
        QLabel::mouseReleaseEvent(event);
    }
}


/**
 * @brief SearchItem::mouseDoubleClickEvent
 * @param event
 */
void SearchItem::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (DOUBLE_CLICK_TO_EXPAND) {
        if (event->button() == Qt::LeftButton) {
            expandItem();
        }
    } else {
        QLabel::mouseDoubleClickEvent(event);
    }
}


/**
 * @brief SearchItem::setupLayout
 * This sets up the layout and widgets within this button.
 */
void SearchItem::setupLayout()
{
    QString borderRadius = "border-radius:" + QString::number(BUTTON_RADIUS) + "px;";
    fixedStyleSheet = "QPushButton:hover{"
                      "background-color: white;"
                      "border: 2px solid rgb(150,150,150);"
                      "}"
                      "QPushButton{"
                      "background-color: rgba(250,250,250,250);"
                      "border: 1px solid darkGray;" + borderRadius + "}";

    QVBoxLayout* mainLayout = new QVBoxLayout();
    QHBoxLayout* layout = new QHBoxLayout();

    // setup icon label
    QString graphMLKind = graphMLItem->getEntityAdapter()->getDataValue("kind").toString();
    QPixmap pixmap(":/Items/" + graphMLKind + ".png");
    pixmap = pixmap.scaled(ICON_SIZE, ICON_SIZE, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    iconLabel = new QLabel(this);
    iconLabel->setFixedSize(MIN_HEIGHT - MARGIN_OFFSET, MIN_HEIGHT - MARGIN_OFFSET);
    iconLabel->setAlignment(Qt::AlignCenter);
    iconLabel->setPixmap(pixmap);

    // setup entity label
    QString graphMLLabel = graphMLItem->getEntityAdapter()->getDataValue("label").toString();
    entityLabel = new QLabel(this);
    entityLabel->setFixedSize(MIN_WIDTH * LABEL_RATIO, iconLabel->height());
    entityLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    entityLabel->setText(graphMLLabel);

    // setup location label
    locationLabel = new QLabel(this);
    locationLabel->setMinimumWidth(MIN_WIDTH - entityLabel->width());
    locationLabel->setFixedHeight(iconLabel->height());
    locationLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    locationLabel->setText(getItemLocation());

    expandPixmap = QPixmap(":/Actions/Arrow_Down");
    contractPixmap = QPixmap(":/Actions/Arrow_Up");
    expandButton = new QPushButton(QIcon(expandPixmap), "", this);
    centerOnButton = new QPushButton(QIcon(":/Actions/Crosshair"), "", this);

    QSize buttonSize(BUTTON_SIZE, BUTTON_SIZE);
    expandButton->setFixedSize(buttonSize);
    centerOnButton->setFixedSize(buttonSize);
    expandButton->setIconSize(buttonSize*0.75);
    centerOnButton->setIconSize(buttonSize*0.75);

    dataBox = new QGroupBox(this);
    QVBoxLayout* boxLayout = new QVBoxLayout();

    //locationLabel = setupDataValueBox("Location", boxLayout, false);
    kindLabel = setupDataValueBox("kind", boxLayout);
    typeLabel = setupDataValueBox("type", boxLayout);
    topicLabel = setupDataValueBox("topicName", boxLayout);
    workerLabel = setupDataValueBox("worker", boxLayout);
    descriptionLabel = setupDataValueBox("description", boxLayout);

    dataBox->setLayout(boxLayout);
    dataBox->setStyleSheet("QGroupBox{ background: rgba(0,0,0,0); }");

    // add labels to layout
    layout->setMargin(LAYOUT_MARGIN);
    layout->setSpacing(LAYOUT_SPACING);
    layout->addWidget(iconLabel);
    layout->addWidget(entityLabel);
    layout->addSpacing(LAYOUT_SPACING * 2);
    layout->addWidget(locationLabel);
    layout->addStretch();
    layout->addWidget(expandButton);
    layout->addWidget(centerOnButton);
    layout->addSpacing(MARGIN_OFFSET);

    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->addLayout(layout);
    mainLayout->addWidget(dataBox);

    setMinimumSize(MIN_WIDTH, MIN_HEIGHT);
    setLayout(mainLayout);

    ITEM_WIDTH = mainLayout->sizeHint().width();
}


/**
 * @brief SearchItem::setupDataValueBox
 * @param key
 * @param layout
 * @return
 */
QLabel* SearchItem::setupDataValueBox(QString key, QLayout *layout, bool storeInHash)
{
    QGroupBox* dataValBox = new QGroupBox(this);
    QLabel* keyLabel = new QLabel(key + ":", this);
    QLabel* valueLabel = new QLabel(this);
    QHBoxLayout* subLayout = new QHBoxLayout();
    keyLabel->setFixedSize(KEY_LABEL_WIDTH, font().pointSize() + MARGIN_OFFSET);
    subLayout->addWidget(keyLabel);
    subLayout->addWidget(valueLabel);
    dataValBox->setLayout(subLayout);
    if (layout) {
        layout->addWidget(dataValBox);
    }
    if (storeInHash) {
        dataKeys.append(key);
        dataValueLabels[key] = valueLabel;
        dataValueBoxes[key] = dataValBox;
    }
    return valueLabel;
}


/**
 * @brief SearchItem::updateColor
 * This updates this button's color depending on its selected state.
 */
void SearchItem::updateColor()
{
    if (selected) {
        setStyleSheet("QLabel{ background: rgb(204,229,250); }"
                      "SearchItem{ border: 2px solid rgb(100,130,180); }"
                      + fixedStyleSheet);
    } else {
        setStyleSheet("QLabel{ background: rgb(240,240,240); }"
                      "SearchItem{ border: 1px solid rgb(180,180,180); }"
                      + fixedStyleSheet);
    }
}


/**
 * @brief SearchItem::getDataValues
 */
void SearchItem::getDataValues()
{
    if (!graphMLItem) {
        return;
    }
    // retrieve the values for the stored data keys
    EntityAdapter* gml= graphMLItem->getEntityAdapter();
    if (gml) {
        foreach (QString key, dataValueBoxes.keys()) {
            QString value = gml->getDataValue(key).toString();
            dataValueLabels[key]->setText(value);
            dataValueBoxes[key]->setVisible(!value.isEmpty());
        }
    }
}


/**
 * @brief SearchItem::getItemLocation
 */
QString SearchItem::getItemLocation()
{
    if (!graphMLItem || !graphMLItem->isNodeItem() || !entityLabel) {
        return "";
    }

    // get NodeItem's location in the model
    NodeItem* nodeItem = (NodeItem*) graphMLItem;
    NodeItem* parentItem = nodeItem->getParentNodeItem();
    QString objectLocation = entityLabel->text();

    while (parentItem && parentItem->getEntityAdapter()) {
        QString parentLabel = parentItem->getEntityAdapter()->getDataValue("label").toString();
        if (parentLabel.isEmpty()) {
            parentLabel = parentItem->getNodeKind();
        }
        objectLocation = parentLabel + " / " + objectLocation;
        parentItem = parentItem->getParentNodeItem();
    }

    return objectLocation;
}
