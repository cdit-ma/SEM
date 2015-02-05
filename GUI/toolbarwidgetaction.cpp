#include "toolbarwidgetaction.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QDebug>


/**
 * @brief ToolbarWidgetAction::ToolbarWidgetAction
 * @param nodeKind
 * @param parent
 */
ToolbarWidgetAction::ToolbarWidgetAction(QString nodeKind, ToolbarWidget *parent) :
    QWidgetAction(parent)
{
    node = 0;
    kind = nodeKind;
    label = nodeKind;
}


/**
 * @brief ToolbarWidgetAction::ToolbarWidgetAction
 * @param node
 * @param parent
 */
ToolbarWidgetAction::ToolbarWidgetAction(Node* node, QWidget *parent, bool instance) :
    QWidgetAction(parent)
{
    this->node = node;
    if (instance) {
        kind = "ComponentInstance";
    } else {
        kind = node->getDataValue("kind");
    }
    label = node->getDataValue("label");
}


/**
 * @brief ToolbarWidgetAction::getNode
 * @return
 */
Node *ToolbarWidgetAction::getNode()
{
   if (node) {
       return node;
   }
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

    actionButton->setMouseTracking(true);
    actionButton->setFixedSize(150, 30);
    actionButton->setStyleSheet("QPushButton{"
                                "border: 0px;"
                                "margin: 0px;"
                                "padding: 0px;"
                                "background-color: rgba(0,0,0,0);"
                                "}"

                                "QPushButton:hover{"
                                "background-color: rgba(10,10,10,50);"
                                "}");

    QHBoxLayout* layout = new QHBoxLayout();
    layout->setMargin(0);

    textLabel = new QLabel(label, actionButton);
    textLabel->setFixedHeight(25);
    textLabel->setStyleSheet("background-color: rgba(0,0,0,0);");

    QLabel* imageLabel = new QLabel(actionButton);
    QImage* image = new QImage(":/Resources/Icons/" + kind + ".png");
    QImage scaledImage = image->scaled(actionButton->height(),
                                       textLabel->height(),
                                       Qt::KeepAspectRatio,
                                       Qt::SmoothTransformation);
    imageLabel->setPixmap(QPixmap::fromImage(scaledImage));
    imageLabel->setFixedSize(scaledImage.size());
    imageLabel->setStyleSheet("background-color: rgba(0,0,0,0);");

    layout->addSpacerItem(new QSpacerItem(5,0));
    layout->addWidget(imageLabel);
    layout->addWidget(textLabel);
    actionButton->setLayout(layout);

    connect(actionButton, SIGNAL(clicked()), this, SLOT(actionButtonClicked()));
    connect(this, SIGNAL(hovered()), this, SLOT(hover()));

    return actionButton;
}


/**
 * @brief ToolbarWidgetAction::hover
 */
void ToolbarWidgetAction::hover()
{
    actionButton->grabMouse();
    actionButton->releaseMouse();
}


/**
 * @brief ToolbarWidgetAction::actionButtonClicked
 */
void ToolbarWidgetAction::actionButtonClicked()
{
    emit trigger();

    if (node) {
        // if a new edge has been constructed using the toolbar, update tool buttons
        ToolbarWidget* toolbar = qobject_cast<ToolbarWidget*>(parent());
        toolbar->checkDefinition(node, false);
        toolbar->checkImplementation(node, false);
        toolbar->updateMenuList("connect", node);
    }
}
