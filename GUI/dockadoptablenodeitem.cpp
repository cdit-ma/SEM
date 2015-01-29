#include "dockadoptablenodeitem.h"

#include <QVBoxLayout>
#include <QDebug>


/**
 * @brief DockAdoptableNodeItem::DockAdoptableNodeItem
 * @param _kind
 * @param parent
 */
DockAdoptableNodeItem::DockAdoptableNodeItem(QString _kind, QWidget *parent) :
    QPushButton(parent)
{
    kind = _kind;
    textLabel = new QLabel(kind, this);
    imageLabel = new QLabel(this);

    setParent(parent);
    setFlat(true);
    setFixedSize(100, 100);
    setStyleSheet("margin: 0px; padding: 0px;");

    setupLayout();
    connect(this, SIGNAL(clicked()), this , SLOT(buttonPressed()));
}


/**
 * @brief DockAdoptableNodeItem::setupLayout
 * @param layout
 */
void DockAdoptableNodeItem::setupLayout()
{
    QVBoxLayout *layout = new QVBoxLayout();

    // make the font size smaller to fit the whole text inside textLabel
    QFont font = textLabel->font();
    if (kind.length() > 18) {
        QFontMetrics fm(font);
        font.setPointSizeF(fm.boundingRect(kind).width()/kind.size()*1.3);
    } else {
        font.setPointSizeF(7.5);
    }

    textLabel->setFont(font);
    textLabel->setFixedSize(width(), 21);
    textLabel->setAlignment(Qt::AlignHCenter | Qt::AlignTop);

    QImage* image = new QImage(":/Resources/Icons/" + kind + ".png");
    QImage scaledImage = image->scaled(width(),
                                       height()-textLabel->height(),
                                       Qt::KeepAspectRatio,
                                       Qt::SmoothTransformation);
    initialSize = scaledImage.size();

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
 * @brief DockAdoptableNodeItem::setPixmapSize
 * This changes the size of the pixmap in the imageLabel.
 * @param size
 */
void DockAdoptableNodeItem::setPixmapSize(QSize size)
{
    if (imageLabel != 0) {
        QPixmap pixmap = imageLabel->pixmap()->scaled(size,
                                                      Qt::KeepAspectRatio,
                                                      Qt::SmoothTransformation);
        imageLabel->setPixmap(pixmap);
    }
}


/**
 * @brief DockAdoptableNodeItem::paintEvent
 * Change the node image (button icon) depending on the node type.
 * NOTE: setIcon() calls this and the scroll area's paintEvent() infinitely.
 * @param e
 */
void DockAdoptableNodeItem::paintEvent(QPaintEvent *e)
{
    setStyleSheet("QPushButton:hover{"
                  "background-color: rgba(0,0,0,0);"
                  "border: 1px solid black;"
                  "border-radius: 5px;"
                  "}");

    QPushButton::paintEvent(e);
}


/**
 * @brief DockAdoptableNodeItem::buttonPressed
 */
void DockAdoptableNodeItem::buttonPressed()
{
    emit itemPressed(kind);
}
