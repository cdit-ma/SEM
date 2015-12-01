#include "docknodeitem.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QDebug>

#define MAX_LABEL_LENGTH 15
#define BUTTON_WIDTH 141
#define ICON_RATIO 0.9

#define DEFAULT 0
#define HIGHLIGHTED 1
#define READONLY 2


/**
 * @brief DockNodeItem::DockNodeItem
 * @param kind
 * @param item
 * @param parent
 * @param isLabel
 * @param imageName
 */
DockNodeItem::DockNodeItem(QString kind, EntityItem* item, QWidget *parent, bool isLabel, QString imageName) :
    QPushButton(parent)
{
    parentDock = dynamic_cast<DockScrollArea*>(parent);
    nodeItem = item;
    parentDockItem = 0;
    expanded = true;
    hidden = false;
    forceHidden = false;
    dockItemVisible = true;
    dockItemLabel = isLabel;

    state = DEFAULT;

    if (nodeItem) {

        this->kind = nodeItem->getNodeKind();
        label = nodeItem->getGraphMLDataValue("label");
        strID = QString::number(nodeItem->getID());
        highlightColor = "rgba(90,150,200,210)";

        if (nodeItem->getNode()) {
            GraphMLData* label = nodeItem->getNode()->getData("label");
            if (label) {
                connect(label, SIGNAL(valueChanged(QString)), this, SLOT(labelChanged(QString)));
            }
        }

        if (nodeItem->isEntityItem()) {
            connect((EntityItem*)nodeItem, SIGNAL(entityItem_iconChanged()), this, SLOT(iconChanged()));
        }

        // if kind == DockItemLabel, don't create an icon
        if (kind == "DockItemLabel") {
            this->kind = kind;
        }

    } else {
        // if there is no node item, it means that this item belongs
        // to the parts dock and it uses its kind as its label and ID
        this->kind = kind;
        label = kind;
        strID = kind;
    }

    if (imageName.isEmpty()) {
        this->imageName = kind;
    } else {
        this->imageName = imageName;
    }

    setupLayout();
    updateStyleSheet();
    updateTextLabel();

    connect(this, SIGNAL(clicked()), this , SLOT(clicked()));

    // initially contract labels
    if (isDockItemLabel()) {
        setDockItemExpanded();
    }
}


/**
 * @brief DockNodeItem::getNodeItem
 * Returns the NodeItem this dock item is conneted to.
 * @return nodeItem
 */
EntityItem* DockNodeItem::getNodeItem()
{
    return nodeItem;
}


/**
 * @brief DockNodeItem::getKind
 * Returns the kind of this dock item.
 * @return
 */
QString DockNodeItem::getKind()
{
    return kind;
}


/**
 * @brief DockNodeItem::setID
 * @param strID
 */
void DockNodeItem::setID(QString strID)
{
    this->strID = strID;
}


/**
 * @brief DockNodeItem::setLabel
 * This updates the label and textLabel of this dock item.
 * @param newLabel
 */
void DockNodeItem::setLabel(QString newLabel)
{
    label = newLabel;
    updateTextLabel();
}


/**
 * @brief DockNodeItem::getLabel
 * Returns the label of this dock item.
 * @return
 */
QString DockNodeItem::getLabel()
{
    return label;
}


/**
 * @brief DockNodeItem::setParentDockNodeItem
 * @param parentItem
 */
void DockNodeItem::setParentDockNodeItem(DockNodeItem* parentItem)
{
    parentDockItem = parentItem;
    if (parentDockItem) {
        setDockItemVisible(parentDockItem->isDockItemVisible());
        connect(parentDockItem, SIGNAL(dockItem_fileClicked(bool)), this, SLOT(parentDockItemClicked(bool)));
        connect(this, SIGNAL(dockItem_visibilityChanged()), parentDockItem, SLOT(childVisibilityChanged()));
    }
}


/**
 * @brief DockNodeItem::getParentDockNodeItem
 * @return
 */
DockNodeItem* DockNodeItem::getParentDockNodeItem()
{
    return parentDockItem;
}


/**
 * @brief DockNodeItem::appendChildDockItem
 * @param dockItem
 */
void DockNodeItem::addChildDockItem(DockNodeItem* dockItem)
{
    childrenDockItems.append(dockItem);
}


/**
 * @brief DockNodeItem::removeChildDockItem
 * @param dockItem
 */
void DockNodeItem::removeChildDockItem(DockNodeItem* dockItem)
{
    childrenDockItems.removeAll(dockItem);
}


/**
 * @brief DockNodeItem::getChildrenDockItems
 * @return
 */
QList<DockNodeItem*> DockNodeItem::getChildrenDockItems()
{
    return childrenDockItems;
}

/**
 * @brief DockNodeItem::setImage Replaces the image used by the DockNodeItem.
 * @param prefix The Image Prefix
 * @param image The Image
 */
void DockNodeItem::setImage(QString prefix, QString image)
{
    if(parentDock){
        NodeView* nodeView = parentDock->getNodeView();
        if(imageLabel && textLabel  && nodeView){
            QPixmap pixMap = nodeView->getImage(prefix, image);
            QPixmap scaledPixmap =  pixMap.scaled(width()*ICON_RATIO,
                                                  (height()-textLabel->height())*ICON_RATIO,
                                                  Qt::KeepAspectRatio,
                                                  Qt::SmoothTransformation);
            imageLabel->setPixmap(scaledPixmap);
        }
    }
}


/**
 * @brief DockNodeItem::setForceHidden
 * @param hide
 */
void DockNodeItem::setForceHidden(bool hide)
{
    forceHidden = hide;
    setDockItemVisible(!hide);
}


/**
 * @brief DockNodeItem::isForceHidden
 * @return
 */
bool DockNodeItem::isForceHidden()
{
    return forceHidden;
}


/**
 * @brief DockNodeItem::getID
 * @return
 */
QString DockNodeItem::getID()
{
    return strID;
}


/**
 * @brief DockNodeItem::setHidden
 * @param hide
 */
void DockNodeItem::setHidden(bool hide)
{
    hidden = hide;
    setDockItemVisible(!hide);
    emit dockItem_visibilityChanged();
}


/**
 * @brief DockNodeItem::isHidden
 * @return
 */
bool DockNodeItem::isHidden()
{
    return hidden;
}


/**
 * @brief DockNodeItem::isDockItemLabel
 * @return
 */
bool DockNodeItem::isDockItemLabel()
{
    return dockItemLabel;
}


/**
 * @brief DockNodeItem::isExpanded
 * @return
 */
bool DockNodeItem::isExpanded()
{
    return expanded;
}


/**
 * @brief DockNodeItem::setReadOnlyState
 * @param on
 */
void DockNodeItem::setReadOnlyState(bool on)
{
    if (on) {
        state = READONLY;
    } else {
        switch (state) {
        case READONLY:
            state = DEFAULT;
            break;
        default:
            return;
        }
    }
    updateStyleSheet();
}


/**
 * @brief DockNodeItem::setDockItemVisible
 * @param visible
 */
void DockNodeItem::setDockItemVisible(bool visible)
{
    if (!isDockItemLabel()) {
        if (hidden || forceHidden) {
            visible = false;
        } else if (parentDockItem) {
            if (visible) {
                parentDockItem->setDockItemVisible(true);
            }
            visible = visible && parentDockItem->isExpanded();
        }
    }

    if (visible == dockItemVisible) {
        return;
    }

    setVisible(visible);
    dockItemVisible = visible;
}


/**
 * @brief DockNodeItem::isDockItemVisible
 * @return
 */
bool DockNodeItem::isDockItemVisible()
{
    return dockItemVisible;
}


/**
 * @brief DockNodeItem::labelChanged
 * This gets called when this dock item's attached nodeItem label has been changed.
 * @param label
 */
void DockNodeItem::labelChanged(QString label)
{
    setLabel(label);
    emit dockItem_relabelled(this);
}


/**
 * @brief DockNodeItem::iconChanged
 * This gets called when this dock item's attached nodeItem icon has been changed.
 * This is currently only being used for Vectors and VectorInstances.
 */
void DockNodeItem::iconChanged()
{
    setupImageLabel();
}


/**
 * @brief DockNodeItem::setupLayout
 * Sets up the visual layout for dock items.
 * A groupbox is used to group the text and image labels.
 */
void DockNodeItem::setupLayout()
{
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setMargin(0);

    textLabel = new QLabel(label, this);

    // setup dock item size and text alignment
    if (isDockItemLabel()) {
        setFixedSize(BUTTON_WIDTH, 28);
        textLabel->setAlignment(Qt::AlignHCenter);
    } else {
        setFixedSize(BUTTON_WIDTH, 100);
        textLabel->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    }

    QFont font = textLabel->font();
    font.setPointSizeF(8);
    textLabel->setFont(QFont(textLabel->font().family(), 8));
    textLabel->setFixedSize(width() - 2, 21);

    // setup icon
    if (!isDockItemLabel()) {
        imageLabel = new QLabel(this);
        imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
        layout->addWidget(imageLabel);
        layout->setAlignment(imageLabel, Qt::AlignHCenter | Qt::AlignBottom);
        setupImageLabel();
    }

    layout->addWidget(textLabel);
    layout->setAlignment(textLabel, Qt::AlignHCenter);

    setFlat(true);
    setLayout(layout);
    setStyleSheet("QPushButton{"
                  "border-radius: 5px;"
                  "background-color: rgba(0,0,0,0);"
                  "}"
                  "QPushButton:hover{"
                  "border: 1px solid black;"
                  "}");
}


/**
 * @brief DockNodeItem::setupImageLabel
 */
void DockNodeItem::setupImageLabel()
{
    if (isDockItemLabel() || !parentDock || !textLabel || !imageLabel) {
        return;
    }

    NodeView* nodeView = parentDock->getNodeView();
    QSize pixMapSize = QSize(width() * ICON_RATIO, (height() - textLabel->height()) * ICON_RATIO);
    QPixmap pixMap = QPixmap(pixMapSize);

    if (nodeView) {
        if (nodeItem) {
            if (nodeItem->isEntityItem()) {
                EntityItem* entityItem = (EntityItem*)nodeItem;
                pixMap = nodeView->getImage(entityItem->getIconPrefix(), entityItem->getIconURL());
            } else {
                pixMap = nodeView->getImage("Items", imageName);
            }
        } else {
            if (parentDock->getDockType() ==  PARTS_DOCK) {
                pixMap = nodeView->getImage("Items", imageName);
            } else if (parentDock->getDockType() == FUNCTIONS_DOCK) {
                pixMap = nodeView->getImage("Functions", imageName);
            }
        }
    }

    if (pixMap.isNull()) {
        qWarning() << "DockNodeItem::setupImageLabel - Image is null for " << kind;
        return;
    }

    pixMap = pixMap.scaled(pixMapSize,
                           Qt::KeepAspectRatio,
                           Qt::SmoothTransformation);

    imageLabel->setPixmap(pixMap);
}


/**
 * @brief DockNodeItem::updateTextLabel
 * This checks whether the displayed label needs truncating or not.
 * If label.length() is greater than MAX_LABEL_LENGTH, truncate it.
 */
void DockNodeItem::updateTextLabel()
{
    QString newLabel = label;
    int maxLength = MAX_LABEL_LENGTH;

    // file labels have a bigger font and can therefore fit less chars
    if (isDockItemLabel()) {
        maxLength -= 2;
    }

    if (label.length() >= maxLength) {

        QFontMetrics fm(textLabel->fontMetrics());
        int textWidth = fm.width(newLabel + "__");

        if (textWidth > this->width()) {
            newLabel.truncate(maxLength - 1);
            newLabel += "..";
        }
    }

    textLabel->setText(newLabel);
}


/**
 * @brief DockNodeItem::updateStyleSheet
 * This updates the stylesheet of a file label when it's expanded/contracted.
 */
void DockNodeItem::updateStyleSheet()
{
    if (isDockItemLabel()) {
        QString textLabelBackground;
        if (expanded) {
            textLabelBackground = "background-color: rgba(208,197,134,0.85);";
        } else {
            textLabelBackground = "background-color: rgba(138,127,64,0.75);";
        }
        textLabel->setStyleSheet("margin: 1px 0px 0px;"
                                 "padding: 2px 0px 2px;"
                                 "border-radius: 5px;"
                                 + textLabelBackground);
    } else {

        QString backgroundColor = "rgba(0,0,0,0);";
        QString hoverBorder = "1px solid black;";

        switch (state) {
        case HIGHLIGHTED:
            backgroundColor = highlightColor;
            hoverBorder = "none";
            break;
        case READONLY:
            //backgroundColor = "rgba(200,200,200,0.8)";
            hoverBorder = "none";
            break;
        default:
            break;
        }

        setStyleSheet("QPushButton{"
                      "border-radius: 5px;"
                      "background-color:" + backgroundColor +
                      "}"
                      "QPushButton:hover{"
                      "border:" + hoverBorder +
                      "}");
    }
}


/**
 * @brief DockNodeItem::buttonPressed
 * This is called whenever this dock item is clicked.
 * If it's a file label, show/hide its children and update the textLabel.
 * If it's not a file label, send
 */
void DockNodeItem::clicked()
{
    if (state == READONLY) {
        return;
    }
    if (isDockItemLabel()) {
        setDockItemExpanded();
    } else {
        emit dockItem_clicked();
    }
}


/**
 * @brief DockNodeItem::parentDockItemClicked
 * Show/hide this dock item when its parent dock item is clicked.
 */
void DockNodeItem::parentDockItemClicked(bool show)
{
    setDockItemVisible(show);
}


/**
 * @brief DockNodeItem::highlightDockItem
 * This adds/removes highlight to this dock item.
 * @param nodeItem
 */
void DockNodeItem::highlightDockItem(NodeItem* nodeItem)
{
    if (nodeItem == getNodeItem()) {
        switch (state) {
        case DEFAULT:
            state = HIGHLIGHTED;
            break;
        default:
            return;
        }
    } else {
        switch (state) {
        case HIGHLIGHTED:
            state = DEFAULT;
            break;
        default:
            return;
        }
    }
    updateStyleSheet();
}


/**
 * @brief DockNodeItem::childVisibilityChanged
 * This is called whenever the Definitions dock has been filtered.
 * It checks if all of the IDL's (label's node item) children dock items are hidden.
 * If they are, hide the IDL's label as well.
 */
void DockNodeItem::childVisibilityChanged()
{
    if (isDockItemLabel()) {
        foreach (DockNodeItem* dockItem, childrenDockItems) {
            if (!dockItem->isHidden() && !dockItem->isForceHidden()) {
                setDockItemVisible(true);
                return;
            }
        }
        setDockItemVisible(false);
    }
}


/**
 * @brief DockNodeItem::changeVectorHiddenState
 */
void DockNodeItem::changeVectorHiddenState()
{
    NodeItem* item = getNodeItem();
    if (item) {
        if (item->hasChildren()) {
            setForceHidden(false);
        } else {
            setForceHidden(true);
        }
    }
}


/**
 * @brief DockNodeItem::setDockItemExpanded
 */
void DockNodeItem::setDockItemExpanded()
{
    if (isDockItemLabel()) {
        if (expanded) {
            expanded = false;
        } else {
            expanded = true;
        }
        updateStyleSheet();
        emit dockItem_fileClicked(expanded);
    }
}

