#include "docknodeitem.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QDebug>


/**
 * @brief DockNodeItem::DockNodeItem
 * @param _kind
 * @param parent
 */
DockNodeItem::DockNodeItem(QString kind, NodeItem *item, QWidget *parent) :
    QPushButton(parent)
{
    nodeItem = item;
    parentDockItem = 0;
    fileLabel = false;
    expanded = true;

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
    textLabel->setText(label);
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
 * @brief DockNodeItem::isFileLabel
 * @return
 */
bool DockNodeItem::isFileLabel()
{
    return fileLabel;
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

    // make the font size smaller to fit the whole text inside textLabel
    textLabel = new QLabel(label, this);
    QFont font = textLabel->font();
    if (kind.length() > 18) {
        QFontMetrics fm(font);
        font.setPointSizeF(fm.boundingRect(kind).width()/kind.size()*1.3);
    } else if (fileLabel) {
        font.setPointSizeF(10);
    } else {
        font.setPointSizeF(7.75);
    }

    setFlat(true);
    setStyleSheet("margin: 0px; padding: 0px;");

    if (fileLabel) {
        setFixedSize(100, 28);
        textLabel->setAlignment(Qt::AlignHCenter);
    } else {
        setFixedSize(100, 100);
        textLabel->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    }

    textLabel->setFont(font);
    textLabel->setFixedSize(width()-2, 21);

    if (!fileLabel) {
        QImage* image = new QImage(":/Resources/Icons/" + kind + ".png");
        QImage scaledImage = image->scaled(width(),
                                           height()-textLabel->height(),
                                           Qt::KeepAspectRatio,
                                           Qt::SmoothTransformation);

        QLabel* imageLabel = new QLabel(this);
        imageLabel->setBackgroundRole(QPalette::Base);
        imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
        imageLabel->setPixmap(QPixmap::fromImage(scaledImage));
        layout->addWidget(imageLabel);
        layout->setAlignment(imageLabel, Qt::AlignHCenter | Qt::AlignBottom);
    }

    layout->addWidget(textLabel);
    layout->setAlignment(textLabel, Qt::AlignHCenter);

    setLayout(layout);
}


/**
 * @brief DockNodeItem::connectToNodeItem
 */
void DockNodeItem::connectToNodeItem()
{
    connect(nodeItem, SIGNAL(updateDockNodeItem()), this, SLOT(updateData()));
    connect(nodeItem, SIGNAL(updateOpacity(qreal)), this, SLOT(setOpacity(qreal)));
    connect(nodeItem, SIGNAL(destroyed()), this, SLOT(deleteLater()));
}


/**
 * @brief DockNodeItem::updateTextLabel
 */
void DockNodeItem::updateTextLabel()
{
    if (fileLabel) {
        if (expanded) {
            textLabel->setStyleSheet("margin: 1px 0px 0px;"
                                     "padding: 2px 0px 2px;"
                                     "border-radius: 5px;"
                                     "background-color: rgba(208,197,134,0.85);");
        } else {
            textLabel->setStyleSheet("margin: 1px 0px 0px;"
                                     "padding: 2px 0px 2px;"
                                     "border-radius: 5px;"
                                     "background-color: rgba(138,127,64,0.75);");
        }
    }
}


/**
 * @brief DockNodeItem::paintEvent
 * Change the node image (button icon) depending on the node type.
 * @param e
 */
void DockNodeItem::paintEvent(QPaintEvent *e)
{
    setStyleSheet("QPushButton:hover{"
                  "background-color: rgba(0,0,0,0);"
                  "border: 1px solid black;"
                  "border-radius: 5px;"
                  "}");

    QPushButton::paintEvent(e);
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
        updateTextLabel();
        emit dockItem_fileClicked(expanded);
    }
}


/**
 * @brief DockNodeItem::parentDockItemClicked
 * Show/hide this dock item when its parent dock item is clicked.
 */
void DockNodeItem::parentDockItemClicked(bool show)
{
    setVisible(show);
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
    repaint();
}


/**
 * @brief DockNodeItem::setOpacity
 * This disables this button when SHIFT is held down and the currently
 * selected node can't be connected to this button.
 * @param opacity
 */
void DockNodeItem::setOpacity(double opacity)
{
    if (opacity < 1) {
        setEnabled(false);
    } else {
        setEnabled(true);
    }
    repaint();
}

