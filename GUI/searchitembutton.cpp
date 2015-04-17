#include "searchitembutton.h"
#include <QDebug>

/**
 * @brief SearchItemButton::SearchItemButton
 * @param item
 * @param parent
 */
SearchItemButton::SearchItemButton(GraphMLItem* item, QWidget *parent) :
    QPushButton(parent)
{
    if (item) {
        graphMLItem = item;
    } else {
        qWarning() << "SearchItemButton:: Cannot create a search item button with a NULL graphMLItem.";
        return;
    }

    selected = false;

    setupLayout();
    updateColor(selected);

    connect(this, SIGNAL(clicked()), this, SLOT(itemClicked()));
}


/**
 * @brief SearchItemButton::getGraphMLItem
 * @return
 */
GraphMLItem *SearchItemButton::getGraphMLItem()
{
    return graphMLItem;
}


/**
 * @brief SearchItemButton::getNodeKind
 * @return
 */
QString SearchItemButton::getNodeKind()
{
    return graphMLItem->getGraphML()->getDataValue("kind");
}


/**
 * @brief SearchItemButton::itemClicked
 */
void SearchItemButton::itemClicked()
{
    // update selected state
    if (selected) {
        selected = false;
    } else {
        selected = true;
        searchItem_centerOnItem(graphMLItem);
    }
    updateColor(selected);
}


/**
 * @brief SearchItemButton::itemClicked
 * @param item
 */
void SearchItemButton::itemClicked(SearchItemButton *item)
{
    if (item != this && selected) {
        itemClicked();
    }
}


/**
 * @brief SearchItemButton::setupLayout
 */
void SearchItemButton::setupLayout()
{
    QHBoxLayout* layout = new QHBoxLayout();
    iconLabel = new QLabel(this);
    objectLabel = new QLabel(this);
    parentLabel = new QLabel(this);
    int marginOffset = 8;

    setFlat(true);
    setMinimumSize(250, 50);

    // setup icon label
    QImage* image = new QImage(":/Resources/Icons/" + getNodeKind() + ".png");
    QImage scaledImage = image->scaled((minimumWidth() / 4) - marginOffset,
                                       minimumHeight() - marginOffset,
                                       Qt::KeepAspectRatio,
                                       Qt::SmoothTransformation);
    iconLabel->setFixedSize(scaledImage.size());
    iconLabel->setAlignment(Qt::AlignCenter);
    iconLabel->setPixmap(QPixmap::fromImage(scaledImage));

    // setup object label
    QString graphMLLabel = graphMLItem->getGraphML()->getDataValue("label");
    objectLabel->setMinimumWidth((minimumWidth() / 2) - marginOffset);
    objectLabel->setFixedHeight(minimumHeight() - marginOffset);
    objectLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    objectLabel->setText(graphMLLabel);

    // setup parent label
    NodeItem* nodeItem = qobject_cast<NodeItem*>(graphMLItem);
    QString parentGraphMLLabel;
    if (nodeItem && nodeItem->getParentNodeItem()) {
        parentGraphMLLabel = nodeItem->getParentNodeItem()->getNode()->getDataValue("label");
    }
    parentLabel->setMinimumWidth((minimumWidth() / 2) - marginOffset);
    parentLabel->setFixedHeight(minimumHeight() - marginOffset);
    parentLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    parentLabel->setText(parentGraphMLLabel);

    // make labels transparent so that when this button change's color, it can be seen
    iconLabel->setStyleSheet("background-color: rgba(0,0,0,0);");
    objectLabel->setStyleSheet("background-color: rgba(0,0,0,0);");
    parentLabel->setStyleSheet("background-color: rgba(0,0,0,0);");

    // add labels to layout
    layout->setMargin(2);
    layout->setSpacing(5);
    layout->addWidget(iconLabel);
    layout->addWidget(objectLabel);
    layout->addWidget(parentLabel);
    setLayout(layout);
}


/**
 * @brief SearchItemButton::updateColor
 * @param selected
 */
void SearchItemButton::updateColor(bool selected)
{
    if (selected) {
        setStyleSheet("QPushButton{"
                      "border: 2px solid gray;"
                      "border-radius: 5px;"
                      "margin: 2px 0px;"
                      "padding-left: 4px;"
                      "background: rgb(200,200,200);"
                      "}");
    } else {
        setStyleSheet("QPushButton{"
                      "border: 2px solid gray;"
                      "border-radius: 5px;"
                      "margin: 2px 0px;"
                      "padding-left: 4px;"
                      "background: rgb(240,240,240);"
                      "}");
    }
    repaint();
}
