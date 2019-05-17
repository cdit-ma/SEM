#include "entityset.h"

#include <QPainter>

#define CHILD_TAB_WIDTH 15

/**
 * @brief EntitySet::EntitySet
 * @param label
 * @param parent
 */
EntitySet::EntitySet(QString label, QWidget* parent)
    : QWidget(parent),
      depth_(1),
      ID_(-1)
{
    iconLabel_ = new QLabel(this);
    iconLabel_->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    textLabel_ = new QLabel(label, this);
    textLabel_->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    toolbar_ = new QToolBar(this);
    toolbar_->setToolButtonStyle(Qt::ToolButtonIconOnly);
    closeAction_ = toolbar_->addAction("");
    closeAction_->setToolTip("Close " + label + "'s chart");

    connect(closeAction_, &QAction::triggered, this, &EntitySet::closeEntity);

    tickLength_ = fontMetrics().height() / 4;
    axisLineVisible_ = false;

    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    mainLayout->setSpacing(3); //CHILD_TAB_WIDTH / 3);
    mainLayout->setMargin(0);
    mainLayout->setContentsMargins(0, 0, CHILD_TAB_WIDTH, 0);
    mainLayout->addWidget(iconLabel_);
    mainLayout->addWidget(textLabel_, 1);
    mainLayout->addWidget(toolbar_);

    setLabel(label);
    setContentsMargins(0, 0, 0, 0);
    setLayoutDirection(Qt::LeftToRight);
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    setObjectName("#MAIN_FRAME");
}


/**
 * @brief EntitySet::~EntitySet
 */
EntitySet::~EntitySet()
{
    emit childRemoved(this);
}


/**
 * @brief EntitySet::getAllDepthChildrenCount
 * This returns this set's total number of children all the way down to the lowest depth.
 * @return
 */
int EntitySet::getAllDepthChildrenCount() const
{
    return allDepthChildrenCount_;
}


/**
 * @brief EntitySet::isExpanded
 * @return
 */
bool EntitySet::isExpanded()
{
    return isExpanded_;
}


/**
 * @brief EntitySet::setDepth
 * @param depth
 */
void EntitySet::setDepth(int depth)
{
    depth_ = depth;
}


/**
 * @brief EntitySet::getDepth
 * @return
 */
int EntitySet::getDepth() const
{
    return depth_;
}


/**
 * @brief EntitySet::getLabel
 * @return
 */
QString EntitySet::getLabel() const
{
    return label_;
}


/**
 * @brief EntitySet::setLabel
 * @param label
 */
void EntitySet::setLabel(QString label)
{
    label_ = label;
    textLabel_->setText(label);
    //setToolTip(label);
}


/**
 * @brief EntitySet::addChildEntitySet
 * This sets up the child's layout and visibility based on this parent.
 * It also connects this parent's change of visibility to the child.
 * @param child
 */
void EntitySet::addChildEntitySet(EntitySet* child)
{
    if (child) {

        child->setParentEntitySet(this);
        child->setDepth(depth_ + 1);
        child->setContentsMargins(CHILD_TAB_WIDTH * child->getDepth(), 0, 0, 0);
        child->setVisible(isExpanded_);
        childrenSets_.append(child);

        connect(child, &EntitySet::childAdded, this, &EntitySet::childEntityAdded);
        connect(child, &EntitySet::childRemoved, this, &EntitySet::childEntityRemoved);
        connect(this, &EntitySet::setChildVisible, child, &EntitySet::setVisible);

        // update allDepthChildrenCount and propagate signal to the ancestor
        childEntityAdded();
    }
}


/**
 * @brief EntitySet::setParentEntitySet
 * @param parent
 */
void EntitySet::setParentEntitySet(EntitySet* parent)
{
    if (parent)
        parentEntitySet_ = parent;
}


/**
 * @brief EntitySet::getChildrenEntitySets
 * @return
 */
const QList<EntitySet *> &EntitySet::getChildrenEntitySets() const
{
    return childrenSets_;
}


/**
 * @brief EntitySet::setHovered
 * @param hovered
 */
void EntitySet::setHovered(bool hovered)
{
    if (hovered) {
        textLabel_->setStyleSheet("color: " + highlighColorStr_ + ";");
        closeAction_->setIcon(closeIcon_);
        /*auto mappedCursor = mapFromGlobal(cursor().pos());
        if (geometry().contains(mappedCursor)) {
            closeAction->setVisible(true);
        }*/
    } else {
        textLabel_->setStyleSheet("color: " + textColorStr_ + ";");
        closeAction_->setIcon(QIcon());
    }
}


/**
 * @brief EntitySet::themeChanged
 * @param theme
 */
void EntitySet::themeChanged(Theme* theme)
{
    setStyleSheet(theme->getLabelStyleSheet() +
                  theme->getToolTipStyleSheet() +
                  "QFrame#MAIN_FRAME {"
                  "border: 1px solid " + theme->getAltTextColorHex() + ";"
                  "border-left: 0px;"
                  "border-right: 0px;"
                  "}");

    unExpandablePixmap_ = theme->getImage("Icons", "circleDark", theme->getIconSize(), theme->getMenuIconColor());
    expandedPixmap_ = theme->getImage("Icons", "triangleDown", theme->getIconSize(), theme->getMenuIconColor());
    contractedPixmap_ = theme->getImage("Icons", "triangleRight", theme->getIconSize(), theme->getMenuIconColor());

    textLabel_->setFont(theme->getFont());
    tickPen_ = QPen(theme->getAltTextColor(), 2.0);

    textColorStr_ = theme->getTextColorHex();
    highlighColorStr_ = theme->getHighlightColorHex();

    toolbar_->setIconSize(theme->getIconSize());
    toolbar_->setStyleSheet(theme->getToolBarStyleSheet() + "QToolBar{ margin: 0px; padding: 0px; }"
                           "QToolButton:!hover{ border: none; background: rgba(0,0,0,0); }");

    closeIcon_ = theme->getIcon("Icons", "cross");
    closeAction_->setIcon(closeIcon_);

    if (allDepthChildrenCount_ <= 0) {
        iconLabel_->setPixmap(unExpandablePixmap_);
    } else {
        QPixmap pixmap = isExpanded_ ? expandedPixmap_ : contractedPixmap_;
        iconLabel_->setPixmap(pixmap);
    }

    setHovered(false);
}


/**
 * @brief EntitySet::toggleExpanded
 */
void EntitySet::toggleExpanded()
{
    isExpanded_ = !isExpanded_;
    isChildrenVisible_ = isExpanded_;
    emit setChildVisible(isExpanded_);

    QPixmap pixmap = isExpanded_ ? expandedPixmap_ : contractedPixmap_;
    iconLabel_->setPixmap(pixmap);
}


/**
 * @brief EntitySet::setVisible
 * @param visible
 */
void EntitySet::setVisible(bool visible)
{
    QWidget::setVisible(visible);
    if (visible != isVisible_) {
        isVisible_ = visible;
        emit visibilityChanged(isVisible_);
        bool showChildren = isExpanded_ && visible;
        if (showChildren != isChildrenVisible_) {
            isChildrenVisible_ = showChildren;
            emit setChildVisible(isChildrenVisible_);
        }
    }
}


/**
 * @brief EntitySet::childEntityAdded
 * This is called when a new child has been added to a child's tree.
 * A child added to this child's tree means that a child has been added to this set's tree.
 */
void EntitySet::childEntityAdded()
{
    // if this is the first child, update the icon to show that it can be expanded
    if (allDepthChildrenCount_ == 0) {
        iconLabel_->setPixmap(contractedPixmap_);
        installEventFilter(this);
    }

    allDepthChildrenCount_++;
    emit childAdded();
}


/**
 * @brief EntitySet::childEntityRemoved
 * This is called when a child has been destructed.
 * @param child
 */
void EntitySet::childEntityRemoved(EntitySet* child)
{
    if (!child)
        return;

    int totalChildrenToRemove = 1 + child->getAllDepthChildrenCount();
    allDepthChildrenCount_ -= totalChildrenToRemove;
    childrenSets_.removeAll(child);

    // update the icon to show that it can't be expanded
    if (allDepthChildrenCount_ == 0) {
        iconLabel_->setPixmap(unExpandablePixmap_);
        isExpanded_ = false;
        removeEventFilter(this);
    }

    emit childRemoved(child);
}


/**
 * @brief EntitySet::eventFilter
 * @param object
 * @param event
 * @return
 */
bool EntitySet::eventFilter(QObject* object, QEvent* event)
{
    if (event->type() == QEvent::MouseButtonDblClick) {
        EntitySet* set = dynamic_cast<EntitySet*>(object);
        set->toggleExpanded();
        return true;
    }
    return QWidget::eventFilter(object, event);
}


/**
 * @brief EntitySet::enterEvent
 * @param event
 */
void EntitySet::enterEvent(QEvent* event)
{
    setHovered(true);
    emit hovered(true);
}


/**
 * @brief EntitySet::leaveEvent
 * @param event
 */
void EntitySet::leaveEvent(QEvent* event)
{
    setHovered(false);
    emit hovered(false);
}


/**
 * @brief EntitySet::paintEvent
 * @param event
 */
void EntitySet::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setPen(tickPen_);

    if (axisLineVisible_)
        painter.drawLine(rect().topRight(), rect().bottomRight());

    if (tickVisible_) {
        double centerY = rect().center().y() + tickPen_.widthF() / 2.0;
        painter.drawLine(rect().right() - tickLength_, centerY, rect().right(), centerY);
    }
}


/**
 * @brief EntitySet::setAxisLineVisible
 * @param visible
 */
void EntitySet::setAxisLineVisible(bool visible)
{
    axisLineVisible_ = visible;
    update();
}


/**
 * @brief EntitySet::setTickVisible
 * @param visible
 */
void EntitySet::setTickVisible(bool visible)
{
    tickVisible_ = visible;
    update();
}
