#include "toolbarwidgetaction.h"
#include "toolbarwidgetmenu.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QDebug>


/**
 * @brief ToolbarWidgetAction::ToolbarWidgetAction
 * @param nodeKind
 * @param parent
 */
ToolbarWidgetAction::ToolbarWidgetAction(QString nodeKind, QString textLabel, QWidget *parent) :
    QWidgetAction(parent)
{
    widgetMenu = 0;
    node = 0;
    kind = nodeKind;

    if (nodeKind == "info") {
        setEnabled(false);
        label = textLabel;
    } else {
        label = nodeKind;
    }
}


/**
 * @brief ToolbarWidgetAction::ToolbarWidgetAction
 * @param node
 * @param parent
 */
ToolbarWidgetAction::ToolbarWidgetAction(Node* node, QWidget *parent, QString actionKind) :
    QWidgetAction(parent)
{
    widgetMenu = 0;
    this->node = node;
    label = node->getDataValue("label");

    if (actionKind == "instance") {
        kind = "ComponentInstance";
    } else {
        kind = node->getDataValue("kind");
    }
}


/**
 * @brief ToolbarWidgetAction::setMenu
 * @param menu
 */
void ToolbarWidgetAction::setMenu(ToolbarWidgetMenu *menu)
{
    widgetMenu = menu;
    connect(widgetMenu, SIGNAL(aboutToHide()), this, SLOT(actionButtonUnclicked()));
    connect(this, SIGNAL(triggered()), widgetMenu, SLOT(execMenu()));
}


/**
 * @brief ToolbarWidgetAction::getMenu
 * @return
 */
ToolbarWidgetMenu* ToolbarWidgetAction::getMenu()
{
    return widgetMenu;
}


/**
 * @brief ToolbarWidgetAction::getNode
 * @return
 */
Node *ToolbarWidgetAction::getNode()
{
    return node;
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
 * @brief ToolbarWidgetAction::getButton
 * @return
 */
QPushButton *ToolbarWidgetAction::getButton()
{
    return actionButton;
}


/**
 * @brief ToolbarWidgetAction::getButtonPos
 * This is used to determine where to draw the menu for this action.
 * @return
 */
QPoint ToolbarWidgetAction::getButtonPos()
{
    QPoint point = actionButton->mapToGlobal(actionButton->rect().topRight());
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

    if (isEnabled()) {
        actionButton->setFixedSize(155, 33);
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
    } else {
        actionButton->setFixedSize(label.size()*7.25, 33);
        actionButton->setStyleSheet("QPushButton{"
                                    "border: 0px;"
                                    "margin: 0px;"
                                    "padding: 0px;"
                                    "background-color: rgba(0,0,0,0);"
                                    "}");
    }

    QHBoxLayout* layout = new QHBoxLayout();
    layout->setMargin(0);

    QImage* image = new QImage(":/Resources/Icons/" + kind + ".png");
    QImage scaledImage = image->scaled(actionButton->height(),
                                       actionButton->height(),
                                       Qt::KeepAspectRatio,
                                       Qt::SmoothTransformation);

    //image
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
    /*
    if (node == 0 && kind == "ComponentInstance") {
        actionButton->setCheckable(true);
        actionButton->setChecked(false);
    }
    */

    connect(actionButton, SIGNAL(clicked()), this, SLOT(actionButtonClicked()));
    connect(this, SIGNAL(hovered()), this, SLOT(hover()));

    return actionButton;
}


/**
 * @brief ToolbarWidgetAction::hover
 * This method passes the hover on this item to the actionButton.
 */
void ToolbarWidgetAction::hover()
{
    actionButton->grabMouse();
    actionButton->releaseMouse();
}


/**
 * @brief ToolbarWidgetAction::actionButtonClicked
 * When the actionButton is clicked, this action is triggered.
 */
void ToolbarWidgetAction::actionButtonClicked()
{
    emit trigger();
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
