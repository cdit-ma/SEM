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
    nodeItem = 0;
    actionButton = 0;
    kind = nodeKind;
    label = nodeKind;

    widgetMenu = 0;
    willHaveMenu = false;
    deletable = true;

    if (nodeKind == "ComponentInstance" || nodeKind == "ComponentImpl" || nodeKind == "BlackBoxInstance" ||
            nodeKind == "InEventPortDelegate" || nodeKind == "OutEventPortDelegate") {
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
ToolbarWidgetAction::ToolbarWidgetAction(NodeItem* nodeItem, QWidget *parent, bool willHaveMenu) :
    QWidgetAction(parent)
{
    this->nodeItem = nodeItem;
    if(nodeItem){
        kind = nodeItem->getNodeKind();
        label =  nodeItem->getNodeLabel();
    }

    widgetMenu = 0;
    deletable = true;
    this->willHaveMenu = willHaveMenu;
}


/**
 * @brief ToolbarWidgetAction::setMenu
 * @param menu
 */
void ToolbarWidgetAction::setMenu(ToolbarWidgetMenu *menu)
{
    prevWidgetMenu = widgetMenu;
    if (prevWidgetMenu) {
        disconnect(this, SIGNAL(triggered()), prevWidgetMenu, SLOT(execMenu()));
    }

    widgetMenu = menu;
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
 * @brief ToolbarWidgetAction::getNodeItem
 * @return
 */
NodeItem *ToolbarWidgetAction::getNodeItem()
{
    return nodeItem;
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

    actionButton = new ToolbarWidgetButton(parent);
    actionButton->setMouseTracking(true);

    actionButton->setMinimumHeight(33);
    actionButton->setStyleSheet("QPushButton{"
                                "border: 0px;"
                                "margin: 0px;"
                                "padding: 0px;"
                                "background-color: rgba(0,0,0,0);"
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
    QFont font = textLabel->font();
    font.setPointSizeF(8.5);
    textLabel->setFixedHeight(25);
    textLabel->setFont(font);
    textLabel->setStyleSheet("background-color: rgba(0,0,0,0);");

    layout->addSpacerItem(new QSpacerItem(5,0));
    layout->addWidget(imageLabel);
    layout->addWidget(textLabel);
    actionButton->setLayout(layout);

    int minWidth = textLabel->fontMetrics().width(label) + imageLabel->width();

    // if this action is going to have a menu, add menu_arrow icon to the widget
    if (willHaveMenu) {
        QImage* menuImage = new QImage(":/Resources/Icons/right_arrow.png");
        QImage scaledMenuImage = menuImage->scaled(actionButton->height()/2.5,
                                                   actionButton->height()/2.5,
                                                   Qt::KeepAspectRatio,
                                                   Qt::SmoothTransformation);
        QLabel* menuImageLabel = new QLabel(actionButton);
        menuImageLabel->setPixmap(QPixmap::fromImage(scaledMenuImage));
        menuImageLabel->setFixedSize(scaledMenuImage.size());
        menuImageLabel->setStyleSheet("background-color: rgba(0,0,0,0);");
        layout->addWidget(menuImageLabel);
        minWidth += menuImageLabel->width();
    }

    actionButton->setMinimumWidth(minWidth + 30);

    if (isEnabled()) {
        connect(this, SIGNAL(hovered()), this, SLOT(hover()));
        connect(actionButton, SIGNAL(pressed()), this, SLOT(actionButtonPressed()));
        connect(actionButton, SIGNAL(clicked()), this, SLOT(actionButtonClicked()));
        connect(actionButton, SIGNAL(mouseEntered()), this, SLOT(hover()));
        connect(actionButton, SIGNAL(mouseExited()), this, SLOT(endHover()));
    }

    return actionButton;
}


/**
 * @brief ToolbarWidgetAction::hover
 * This method passes the hover on this item to the actionButton.
 */
void ToolbarWidgetAction::hover()
{
    QPalette pal = actionButton->palette();
    pal.setColor(QPalette::Button, QColor(10,10,10,50));
    actionButton->setAutoFillBackground(true);
    actionButton->setPalette(pal);
    actionButton->update();

    actionButton->grabMouse();
    actionButton->releaseMouse();
}

/**
 * @brief ToolbarWidgetAction::endHover
 * This slot is called when the mouse leaves a hover
 */
void ToolbarWidgetAction::endHover()
{
    if(!actionButton->getCheck()) {
        QPalette pal = actionButton->palette();
        pal.setColor(QPalette::Button, QColor(0,0,0,0));
        actionButton->setAutoFillBackground(true);
        actionButton->setPalette(pal);
        actionButton->update();
    }
}


/**
 * @brief ToolbarWidgetAction::actionButtonPressed
 */
void ToolbarWidgetAction::actionButtonPressed()
{
    emit toolbarAction_pressed();
}


/**
 * @brief ToolbarWidgetAction::actionButtonClicked
 * When the actionButton is clicked, this action is triggered.
 */
void ToolbarWidgetAction::actionButtonClicked()
{
    QPalette pal = actionButton->palette();
    pal.setColor(QPalette::Button, QColor(10,10,10,50));
    actionButton->setAutoFillBackground(true);
    actionButton->setPalette(pal);
    actionButton->update();

    actionButton->setCheck(true);
    emit trigger();
}


/**
 * @brief ToolbarWidgetAction::menuOpened
 */
void ToolbarWidgetAction::menuOpened()
{
   actionButton->setCheck(true);
}


/**
 * @brief ToolbarWidgetAction::menuClosed
 */
void ToolbarWidgetAction::menuClosed()
{
    actionButton->setCheck(false);
}
