#include "searchitembutton.h"
#include "../medeawindow.h"

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
    triggeredWithin = false;

    setupLayout();
    updateColor(selected);

    connect(this, SIGNAL(clicked()), this, SLOT(itemClicked()));
}


/**
 * @brief SearchItemButton::connectToWindow
 * This connects the signals and slots to the MEDEA window.
 */
void SearchItemButton::connectToWindow(QMainWindow* window)
{
    MedeaWindow* medea = dynamic_cast<MedeaWindow*>(window);
    if (medea) {
        connect(this, SIGNAL(clicked()), medea, SLOT(searchItemClicked()));
        connect(this, SIGNAL(searchItem_centerOnItem(GraphMLItem*)), medea, SLOT(on_searchResultItem_clicked(GraphMLItem*)));
        connect(medea, SIGNAL(window_searchItemClicked(SearchItemButton*)), this, SLOT(itemClicked(SearchItemButton*)));
    }
}


/**
 * @brief SearchItemButton::itemClicked
 * This is called every time this item is clicked.
 * It updates the selected state and the color of this button accordingly.
 * If this item is selected, it sends a signal to MEDEA to center on its graphMLItem.
 * @param changeColor
 */
void SearchItemButton::itemClicked()
{
    // if this button is already selected and the user clicks on it again,
    // keep it selected - don't unselect it
    if (!triggeredWithin && selected) {
        return;
    }

    // otherwise, update the selected state
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
 * This is called every time a SearchItemButton is clicked.
 * A signal is sent by MEDEA to say which item was clicked.
 * If this item is currently selected and the clicked item was not this item,
 * animate a click to deselect this item and hence update its color.
 * @param item
 */
void SearchItemButton::itemClicked(SearchItemButton *item)
{
    if (item != this && selected) {
        triggeredWithin = true;
        itemClicked();
        triggeredWithin = false;
    }
}


/**
 * @brief SearchItemButton::setupLayout
 * This sets up the layout and widgets within this button.
 */
void SearchItemButton::setupLayout()
{
    QHBoxLayout* layout = new QHBoxLayout();
    iconLabel = new QLabel(this);
    objectLabel = new QLabel(this);
    parentLabel = new QLabel(this);
    int marginOffset = 8;

    setFlat(true);
    setMinimumSize(300, 50);

    // setup icon label
    QString graphMLKind = graphMLItem->getGraphML()->getDataValue("kind");
    QImage* image = new QImage(":/Resources/Icons/" + graphMLKind + ".png");
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
 * This updates this button's color depending on its selected state.
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
