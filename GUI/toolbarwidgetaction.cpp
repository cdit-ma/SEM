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
 * @brief ToolbarWidgetAction::setMenu
 * @param menu
 */
void ToolbarWidgetAction::setMenu(QMenu *menu)
{
    this->menu = menu;
    connect(this->menu, SIGNAL(aboutToHide()), this, SLOT(actionButtonUnclicked()));
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
 * @brief ToolbarWidgetAction::getButtonPos
 * @return
 */
QPoint ToolbarWidgetAction::getButtonPos()
{
    QPoint point = actionButton->mapToGlobal(actionButton->pos());
    point.setX(point.x() + actionButton->width() - 10);
    return point;
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
                                "}"

                                "QPushButton:checked{"
                                "background-color: rgba(10,10,10,50);"
                                "}");

    QHBoxLayout* layout = new QHBoxLayout();
    layout->setMargin(0);

    QImage* image = new QImage(":/Resources/Icons/" + kind + ".png");
    QImage scaledImage = image->scaled(actionButton->height(),
                                       actionButton->height(),
                                       Qt::KeepAspectRatio,
                                       Qt::SmoothTransformation);

    QLabel* imageLabel = new QLabel(actionButton);
    imageLabel->setPixmap(QPixmap::fromImage(scaledImage));
    imageLabel->setFixedSize(scaledImage.size());
    imageLabel->setStyleSheet("background-color: rgba(0,0,0,0);");

    QLabel* textLabel = new QLabel(label, actionButton);
    textLabel->setFixedHeight(25);
    textLabel->setStyleSheet("background-color: rgba(0,0,0,0);");

    layout->addSpacerItem(new QSpacerItem(5,0));
    layout->addWidget(imageLabel);
    layout->addWidget(textLabel);
    actionButton->setLayout(layout);

    // if this action's kind is ComponentInstance, setCheckable to true
    // to help highlight this action when its menu is visible
    if (node == 0 && kind == "ComponentInstance") {
        actionButton->setCheckable(true);
    }

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


/**
 * @brief ToolbarWidgetAction::actionButtonUnclicked
 * This method corrects the checked state of actionButton when the menu is hidden.
 */
void ToolbarWidgetAction::actionButtonUnclicked()
{
    actionButton->setChecked(false);
    actionButton->repaint();
}
