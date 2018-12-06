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
      _depth(1),
      _ID(-1)
{
    iconLabel = new QLabel(this);
    iconLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    textLabel = new QLabel(label, this);
    textLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    _tickLength = fontMetrics().height() / 4;
    _axisLineVisible = false;

    QHBoxLayout* labelLayout = new QHBoxLayout(this);
    labelLayout->setSpacing(CHILD_TAB_WIDTH / 3);
    labelLayout->setMargin(0);
    labelLayout->setContentsMargins(0, 0, CHILD_TAB_WIDTH, 0);
    labelLayout->addWidget(iconLabel);
    labelLayout->addWidget(textLabel, 1);

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
 * @brief EntitySet::getID
 * @return
 */
int EntitySet::getID()
{
    return _ID;
}


/**
 * @brief EntitySet::getParentEntityID
 * @return
 */
int EntitySet::getParentEntityID()
{
    if (parentEntitySet) {
        return parentEntitySet->getID();
    }
    return -1;
}


/**
 * @brief EntitySet::getLastChildID
 * @return
 */
int EntitySet::getLastChildID()
{
    if (!childrenSets.isEmpty()) {
        return childrenSets.last()->getID();
    }
    return _ID;
}


/**
 * @brief EntitySet::getAllDepthChildrenCount
 * This returns this set's total number of children all the way down to the lowest depth.
 * @return
 */
int EntitySet::getAllDepthChildrenCount()
{
    return allDepthChildrenCount;
}


/**
 * @brief EntitySet::getChildEntitySet
 * @param ID
 * @return
 */
EntitySet* EntitySet::getChildEntitySet(int ID)
{
    return childrenHash.value(ID, 0);
}


/**
 * @brief EntitySet::getChildrenEntitySets
 * @return
 */
QList<EntitySet*> EntitySet::getChildrenEntitySets()
{
    return childrenSets;
}


/**
 * @brief EntitySet::isExpanded
 * @return
 */
bool EntitySet::isExpanded()
{
    return _isExpanded;
}


/**
 * @brief EntitySet::setHovered
 * @param hovered
 */
void EntitySet::setHovered(bool hovered)
{
    if (hovered) {
        textLabel->setStyleSheet("color: " + highlighColorStr + ";");
    } else {
        textLabel->setStyleSheet("color: " + textColorStr + ";");
    }
}


/**
 * @brief EntitySet::setPreviousID
 * @param ID
 */
void EntitySet::setPreviousID(int ID)
{
    _previousID = ID;
}


/**
 * @brief EntitySet::getPreviousID
 * @return
 */
int EntitySet::getPreviousID()
{
    return _previousID;
}


/**
 * @brief EntitySet::setNextID
 * @param ID
 */
void EntitySet::setNextID(int ID)
{
    _nextID = ID;
}


/**
 * @brief EntitySet::getNextID
 * @return
 */
int EntitySet::getNextID()
{
    return _nextID;
}


/**
 * @brief EntitySet::setDepth
 * @param depth
 */
void EntitySet::setDepth(int depth)
{
    _depth = depth;
}


/**
 * @brief EntitySet::getDepth
 * @return
 */
int EntitySet::getDepth()
{
    return _depth;
}


/**
 * @brief EntitySet::getLabel
 * @return
 */
QString EntitySet::getLabel()
{
    return _label;
}


/**
 * @brief EntitySet::setID
 * @param ID
 */
void EntitySet::setID(int ID)
{
    _ID = ID;
}


/**
 * @brief EntitySet::setLabel
 * @param label
 */
void EntitySet::setLabel(QString label)
{
    _label = label;
    textLabel->setText(label);
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

        int childID = child->getID();
        child->setParentEntitySet(this);
        child->setDepth(_depth + 1);
        child->setContentsMargins(CHILD_TAB_WIDTH * child->getDepth(), 0, 0, 0);
        child->setVisible(_isExpanded);
        childrenSets.append(child);
        childrenHash[childID] = child;

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
        parentEntitySet = parent;
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

    unExpandablePixmap = theme->getImage("Icons", "circleDark", theme->getIconSize(), theme->getMenuIconColor());
    expandedPixmap = theme->getImage("Icons", "triangleDown", theme->getIconSize(), theme->getMenuIconColor());
    contractedPixmap = theme->getImage("Icons", "triangleRight", theme->getIconSize(), theme->getMenuIconColor());

    textLabel->setFont(theme->getFont());
    _tickPen = QPen(theme->getAltTextColor(), 2);

    textColorStr = theme->getTextColorHex();
    highlighColorStr = theme->getHighlightColorHex();
    setHovered(false);

    if (allDepthChildrenCount <= 0) {
        iconLabel->setPixmap(unExpandablePixmap);
    } else {
        QPixmap pixmap = _isExpanded ? expandedPixmap : contractedPixmap;
        iconLabel->setPixmap(pixmap);
    }
}


/**
 * @brief EntitySet::toggleExpanded
 */
void EntitySet::toggleExpanded()
{
    _isExpanded = !_isExpanded;
    _isChildrenVisible = _isExpanded;
    emit setChildVisible(_isExpanded);

    QPixmap pixmap = _isExpanded ? expandedPixmap : contractedPixmap;
    iconLabel->setPixmap(pixmap);
}


/**
 * @brief EntitySet::setVisible
 * @param visible
 */
void EntitySet::setVisible(bool visible)
{
    QWidget::setVisible(visible);
    if (visible != _isVisible) {
        _isVisible = visible;
        emit visibilityChanged(_isVisible);
        bool showChildren = _isExpanded && visible;
        if (showChildren != _isChildrenVisible) {
            _isChildrenVisible = showChildren;
            emit setChildVisible(_isChildrenVisible);
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
    if (allDepthChildrenCount == 0) {
        iconLabel->setPixmap(contractedPixmap);
        installEventFilter(this);
    }

    allDepthChildrenCount++;
    emit childAdded();
}


/**
 * @brief EntitySet::childEntityRemoved
 * This is called when a child has been destructed.
 * @param child
 */
void EntitySet::childEntityRemoved(EntitySet* child)
{
    int totalChildrenToRemove = 1 + child->getAllDepthChildrenCount();
    allDepthChildrenCount -= totalChildrenToRemove;
    childrenSets.removeAll(child);
    childrenHash.remove(child->getID());

    // update the icon to show that it can't be expanded
    if (allDepthChildrenCount == 0) {
        iconLabel->setPixmap(unExpandablePixmap);
        _isExpanded = false;
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
    painter.setPen(_tickPen);

    if (_axisLineVisible)
        painter.drawLine(rect().topRight(), rect().bottomRight());

    if (_tickVisible) {
        double centerY = rect().center().y() + _tickPen.widthF() / 2.0;
        painter.drawLine(rect().right() - _tickLength, centerY, rect().right(), centerY);
    }
}


/**
 * @brief EntitySet::setAxisLineVisible
 * @param visible
 */
void EntitySet::setAxisLineVisible(bool visible)
{
    _axisLineVisible = visible;
    update();
}


/**
 * @brief EntitySet::setTickVisible
 * @param visible
 */
void EntitySet::setTickVisible(bool visible)
{
    _tickVisible = visible;
    update();
}
