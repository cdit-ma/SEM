#include "toolbarwidgetaction.h"
#include "toolbarwidgetmenu.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QDebug>


/**
 * @brief ToolbarWidgetAction::ToolbarWidgetAction
 * @param nodeKind
 * @param textLabel
 * @param parent
 */
ToolbarWidgetAction::ToolbarWidgetAction(QString nodeKind, QString textLabel, QWidget *parent) :
    QWidgetAction(parent)
{           
    node = 0;
    kind = nodeKind;
    label = nodeKind;

    widgetMenu = 0;
    willHaveMenu = false;
    deletable = true;

    if (nodeKind == "ComponentInstance" || nodeKind == "InEventPortDelegate" || nodeKind == "OutEventPortDelegate") {
        willHaveMenu = true;
        deletable = false;
    } else if (nodeKind == "info") {
        label = textLabel;
        deletable = false;
        setEnabled(false);
    }
}


/**
 * @brief ToolbarWidgetAction::ToolbarWidgetAction
 * @param node
 * @param parent
 * @param actionKind
 */
ToolbarWidgetAction::ToolbarWidgetAction(Node* node, QWidget *parent, QString actionKind) :
    QWidgetAction(parent)
{
    this->node = node;
    kind = node->getDataValue("kind");
    label = node->getDataValue("label");

    widgetMenu = 0;
    willHaveMenu = false;
    deletable = true;

    if (actionKind == "file" || actionKind == "eventPort") {
        willHaveMenu = true;
    } else if (actionKind == "instance") {
        kind = "ComponentInstance";
    }
}


/**
 * @brief ToolbarWidgetAction::setMenu
 * @param menu
 */
void ToolbarWidgetAction::setMenu(ToolbarWidgetMenu *menu)
{
    widgetMenu = menu;

    //connect(widgetMenu, SIGNAL(aboutToHide()), this, SLOT(actionButtonUnclicked()));
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
 * @brief ToolbarWidgetAction::isDeletable
 * @return
 */
bool ToolbarWidgetAction::isDeletable()
{
    return deletable;
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
    return actionButton->mapToGlobal(actionButton->rect().topRight());
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
        actionButton->setMinimumSize(160, 33);
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
        actionButton->setMinimumSize(label.size()*7.25, 33);
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

    // if this action is going to have a menu, add menu_arrow icon to the widget
    if (willHaveMenu) {
        QImage* menuImage = new QImage(":/Resources/Icons/menu_arrow.png");
        QImage scaledMenuImage = menuImage->scaled(actionButton->height()/2.5,
                                           actionButton->height()/2.5,
                                           Qt::KeepAspectRatio,
                                           Qt::SmoothTransformation);
        QLabel* menuImageLabel = new QLabel(actionButton);
        menuImageLabel->setPixmap(QPixmap::fromImage(scaledMenuImage));
        menuImageLabel->setFixedSize(scaledMenuImage.size());
        menuImageLabel->setStyleSheet("background-color: rgba(0,0,0,0);");
        layout->addWidget(menuImageLabel);
    }

    connect(this, SIGNAL(hovered()), this, SLOT(hover()));
    connect(actionButton, SIGNAL(pressed()), this, SLOT(actionButtonPressed()));
    connect(actionButton, SIGNAL(clicked()), this, SLOT(actionButtonClicked()));

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
 * @brief ToolbarWidgetAction::actionButtonPressed
 */
void ToolbarWidgetAction::actionButtonPressed()
{
    emit pressed();
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
    //qDebug() << "ToolbarWidgetAction::actionButtonUnclicked()";
    actionButton->setChecked(false);
    actionButton->repaint();
}
