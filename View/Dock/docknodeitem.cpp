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

    if (nodeItem) {
        this->kind = nodeItem->getNodeKind();
        label = nodeItem->getNode()->getDataValue("label");
        connectToNodeItem();
    } else {
        this->kind = kind;
        label = kind;
    }

    setupLayout();
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
 * @brief DockNodeItem::setupLayout
 * Sets up the visual layout for dock items.
 * A groupbox is used to group the text and image labels.
 */
void DockNodeItem::setupLayout()
{
    QVBoxLayout *layout = new QVBoxLayout();
    QLabel* imageLabel = new QLabel(this);
    textLabel = new QLabel(label, this);

    setFlat(true);
    setFixedSize(100, 100);
    setStyleSheet("margin: 0px; padding: 0px;");

    // make the font size smaller to fit the whole text inside textLabel
    QFont font = textLabel->font();
    if (kind.length() > 18) {
        QFontMetrics fm(font);
        font.setPointSizeF(fm.boundingRect(kind).width()/kind.size()*1.3);
    } else {
        font.setPointSizeF(7.75);
    }

    textLabel->setFont(font);
    textLabel->setFixedSize(width(), 21);
    textLabel->setAlignment(Qt::AlignHCenter | Qt::AlignTop);

    QImage* image = new QImage(":/Resources/Icons/" + kind + ".png");
    QImage scaledImage = image->scaled(width(),
                                       height()-textLabel->height(),
                                       Qt::KeepAspectRatio,
                                       Qt::SmoothTransformation);

    imageLabel->setBackgroundRole(QPalette::Base);
    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageLabel->setPixmap(QPixmap::fromImage(scaledImage));

    layout->addWidget(imageLabel);
    layout->addWidget(textLabel);

    layout->setMargin(0);
    layout->setSpacing(2);
    layout->setAlignment(imageLabel, Qt::AlignHCenter | Qt::AlignBottom);
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
 */
void DockNodeItem::clicked()
{
    emit dockItem_clicked();
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

