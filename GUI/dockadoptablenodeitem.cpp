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

    setParent(parent);
    setFlat(true);
    setFixedSize(100, 100);
    setStyleSheet("padding: 0px;");

    QLabel* textLabel = new QLabel(kind, this);
    QLabel* imageLabel = new QLabel(this);
    QVBoxLayout* vLayout = new QVBoxLayout();
    QImage* image = new QImage(":/Resources/Icons/" + kind + ".png");
    QImage scaledImage = image->scaled(width(),
                                       height()-textLabel->height(),
                                       Qt::KeepAspectRatio,
                                       Qt::SmoothTransformation);

    double offSet = 2.5;

    vLayout->setMargin(0);
    imageLabel->setAlignment(Qt::AlignBottom | Qt::AlignHCenter);
    textLabel->setAlignment(Qt::AlignCenter);
    textLabel->setFixedSize(width()+offSet, 40);
    imageLabel->setBackgroundRole(QPalette::Base);
    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageLabel->setPixmap(QPixmap::fromImage(scaledImage));
    imageLabel->setFixedSize(width()-offSet, height()-30);

    vLayout->addWidget(imageLabel);
    vLayout->addWidget(textLabel);
    setLayout(vLayout);

    connect(this, SIGNAL(clicked()), this , SLOT(buttonPressed()));
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


/**
 * @brief DockAdoptableNodeItem::destructGraphML
 */
/**
void DockAdoptableNodeItem::destructGraphML()
{
    delete this;
}
*/
