#include "docknodeitem.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QDebug>

#define MAX_LABEL_LENGTH 13

/**
 * @brief DockNodeItem::DockNodeItem
 * @param _kind
 * @param parent
 */
DockNodeItem::DockNodeItem(QString kind, NodeItem *item, QWidget *parent) :
    QPushButton(parent)
{
    parentDock = dynamic_cast<DockScrollArea*>(parent);
    nodeItem = item;
    parentDockItem = 0;
    fileLabel = false;
    expanded = true;
    hidden = false;
    highlighted = false;

    if (nodeItem) {

        this->kind = nodeItem->getNodeKind();
        label = nodeItem->getNode()->getDataValue("label");
        connectToNodeItem();

        // if kind == FileLabel, don't create an icon
        if (kind == "FileLabel") {
            fileLabel = true;
            this->kind = kind;
        }

    } else {
        this->kind = kind;
        label = kind;
    }

    setupLayout();
    updateStyleSheet();
    updateTextLabel();

    connect(this, SIGNAL(clicked()), this , SLOT(clicked()));
}


/**
 * @brief DockNodeItem::getNodeItem
 * Returns the NodeItem this dock item is conneted to.
 * @return nodeItem
 */
NodeItem *DockNodeItem::getNodeItem()
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
void DockNodeItem::setParentDockNodeItem(DockNodeItem *parentItem)
{
    parentDockItem = parentItem;
    connect(parentDockItem, SIGNAL(dockItem_fileClicked(bool)), this, SLOT(parentDockItemClicked(bool)));
    connect(this, SIGNAL(dockItem_hidden()), parentDockItem, SLOT(childDockItemHidden()));
}


/**
 * @brief DockNodeItem::getParentDockNodeItem
 * @return
 */
DockNodeItem *DockNodeItem::getParentDockNodeItem()
{
    return parentDockItem;
}


/**
 * @brief DockNodeItem::appendChildDockItem
 * @param dockItem
 */
void DockNodeItem::addChildDockItem(DockNodeItem *dockItem)
{
    childrenDockItems.append(dockItem);
}


/**
 * @brief DockNodeItem::removeChildDockItem
 * @param dockItem
 */
void DockNodeItem::removeChildDockItem(DockNodeItem *dockItem)
{
    childrenDockItems.removeAll(dockItem);
}


/**
 * @brief DockNodeItem::getChildrenDockItems
 * @return
 */
QList<DockNodeItem *> DockNodeItem::getChildrenDockItems()
{
    return childrenDockItems;
}


/**
 * @brief DockNodeItem::setHidden
 * @param hide
 */
void DockNodeItem::setHidden(bool hideItem)
{
    hidden = hideItem;
    emit dockItem_hidden();
    if (hideItem) {
        hide();
    } else if (parentDockItem && parentDockItem->isExpanded()) {
        show();
    }
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
 * @brief DockNodeItem::isFileLabel
 * @return
 */
bool DockNodeItem::isFileLabel()
{
    return fileLabel;
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
 * @brief DockNodeItem::setupLayout
 * Sets up the visual layout for dock items.
 * A groupbox is used to group the text and image labels.
 */
void DockNodeItem::setupLayout()
{
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(2);

    textLabel = new QLabel(label, this);

    QFont font = textLabel->font();
    font.setPointSizeF(8);

    textLabel->setFont(QFont(textLabel->font().family(), 8));
    textLabel->setFixedSize(width()-2, 21);

    if (fileLabel) {
        setFixedSize(100, 28);
        textLabel->setAlignment(Qt::AlignHCenter);
    } else {
        setFixedSize(100, 100);
        textLabel->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    }

    if (!fileLabel) {

        QImage* image = new QImage();
        if (parentDock && parentDock->getNodeView()) {
            *image = parentDock->getNodeView()->getImage(kind);
        }

        if (nodeItem && kind == "HardwareNode") {
            QString hardwareOS = (nodeItem->getNode()->getDataValue("os")).remove(QChar::Space);
            QString hardwareArch = nodeItem->getNode()->getDataValue("architecture");
            QString hardwareKind = hardwareOS + "_" + hardwareArch;
            if (parentDock && parentDock->getNodeView()) {
                *image = parentDock->getNodeView()->getImage(hardwareKind);
            }
            highlightColor = "rgba(90,150,200,210)";
        }

        if (!image) {
            qWarning() << "DockNodeItem::setupLayout - Image is null";
        }

        QPixmap scaledPixmap = QPixmap::fromImage(image->scaled(width(),
                                             height()-textLabel->height(),
                                             Qt::KeepAspectRatio,
                                             Qt::SmoothTransformation));

        imageLabel = new QLabel(this);
        imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
        imageLabel->setPixmap(scaledPixmap);
        layout->addWidget(imageLabel);
        layout->setAlignment(imageLabel, Qt::AlignHCenter | Qt::AlignBottom);

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
 * @brief DockNodeItem::connectToNodeItem
 */
void DockNodeItem::connectToNodeItem()
{
    connect(nodeItem, SIGNAL(updateDockNodeItem()), this, SLOT(updateData()));
    connect(nodeItem, SIGNAL(destroyed()), this, SLOT(deleteLater()));
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
    if (fileLabel) {
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
    if (fileLabel) {
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
    if (!fileLabel) {
        emit dockItem_clicked();
    } else {
        if (expanded) {
            expanded = false;
        } else {
            expanded = true;
        }
        updateStyleSheet();
        emit dockItem_fileClicked(expanded);
    }
}


/**
 * @brief DockNodeItem::parentDockItemClicked
 * Show/hide this dock item when its parent dock item is clicked.
 */
void DockNodeItem::parentDockItemClicked(bool show)
{
    if (!isHidden()) {
        setVisible(show);
    }
}


/**
 * @brief DockNodeItem::deleteLater
 * This is called when the nodeitem attached to this dock item is deleted.
 * This dock item is removed from its dock's list and then it's deleted.
 */
void DockNodeItem::deleteLater()
{
    emit dockItem_removeFromDock(this);
    QObject::deleteLater();
}


/**
 * @brief DockNodeItem::updateData
 * This gets called when the dataTable value for the node item has been changed.
 */
void DockNodeItem::updateData()
{
    setLabel(nodeItem->getNode()->getDataValue("label"));
    emit dockItem_relabelled(this);
    //repaint();
}


/**
 * @brief DockNodeItem::childHidden
 * This is called whenever a File's child dock item is hidden.
 * If all of the File's children dock items are hidden, hide the File label.
 */
void DockNodeItem::childDockItemHidden()
{
    bool hideFileLabel = true;
    foreach (DockNodeItem* dockItem, childrenDockItems) {
        if (!dockItem->isHidden()) {
            hideFileLabel = false;
            break;
        }
    }
    if (hideFileLabel) {
        hide();
    } else {
        show();
    }
}


/**
 * @brief DockNodeItem::highlightDockItem
 * This adds/removes highlight to this dock item.
 * @param node
 */
void DockNodeItem::highlightDockItem(Node *node)
{
    QString backgroundColor = "rgba(0,0,0,0);";
    QString hoverBorder = "1px solid black;";

    if (node && node == getNodeItem()->getNode()) {
        if (highlighted) {
            return;
        }
        backgroundColor = highlightColor;
        hoverBorder = "none;";
    }

    setStyleSheet("QPushButton{"
                  "border-radius: 5px;"
                  "background-color:" + backgroundColor +
                  "}"
                  "QPushButton:hover{"
                  "border:" + hoverBorder +
                  "}");
}

