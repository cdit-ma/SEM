#include "toolbarwidgetaction.h"

#include <QHBoxLayout>
#include <QLabel>


ToolbarWidgetAction::ToolbarWidgetAction(QString nodeKind, QWidget *parent) :
    QWidgetAction(parent)
{
    kind = nodeKind;
}


/**
 * @brief ToolbarWidgetAction::getKind
 * @return
 */
QString ToolbarWidgetAction::getKind()
{
   return kind;
}


/**
 * @brief ToolbarWidgetAction::createWidget
 * @param parent
 * @return
 */
QWidget* ToolbarWidgetAction::createWidget(QWidget *parent)
{
    actionButton = new QPushButton(parent);
    //actionButton->setFlat(true);
    actionButton->setFixedSize(150, 30);
    actionButton->setStyleSheet("QPushButton{ margin: 0px; padding: 0px; }");


    QHBoxLayout* layout = new QHBoxLayout();
    layout->setMargin(0);
    //layout->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);

    QLabel* textLabel = new QLabel(kind, actionButton);
    textLabel->setFixedHeight(25);

    QLabel* imageLabel = new QLabel(actionButton);
    QImage* image = new QImage(":/Resources/Icons/" + kind + ".png");
    QImage scaledImage = image->scaled(actionButton->height(),
                                       textLabel->height(),
                                       Qt::KeepAspectRatio,
                                       Qt::SmoothTransformation);
    imageLabel->setPixmap(QPixmap::fromImage(scaledImage));
    imageLabel->setFixedSize(scaledImage.size());

    layout->addSpacerItem(new QSpacerItem(5,0));
    layout->addWidget(imageLabel);
    layout->addWidget(textLabel);
    actionButton->setLayout(layout);

    connect(actionButton, SIGNAL(pressed()), this, SLOT(actionButtonPressed()));

    return actionButton;
}


/**
 * @brief ToolbarWidgetAction::actionButtonPressed
 */
void ToolbarWidgetAction::actionButtonPressed()
{
    emit trigger();
}
