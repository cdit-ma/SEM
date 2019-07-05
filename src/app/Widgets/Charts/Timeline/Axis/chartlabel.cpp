#include "chartlabel.h"

#include <QPainter>

#define CHILD_TAB_WIDTH 15

using namespace MEDEA;

/**
 * @brief ChartLabel::ChartLabel
 * @param label
 * @param parent
 */
ChartLabel::ChartLabel(QString label, QWidget* parent)
    : QWidget(parent),
      depth_(1),
      ID_(-1)
{
    iconLabel_ = new QLabel(this);
    iconLabel_->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    // TODO - hide icon label while we're not using the ChartLabel parenting/grouping functionality
    //iconLabel_->setVisible(false);

    textLabel_ = new QLabel(label, this);
    textLabel_->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    toolbar_ = new QToolBar(this);
    toolbar_->setToolButtonStyle(Qt::ToolButtonIconOnly);
    closeAction_ = toolbar_->addAction("");
    closeAction_->setToolTip("Close " + label + "'s chart");

    connect(closeAction_, &QAction::triggered, this, &ChartLabel::closeChart);

    tickLength_ = fontMetrics().height() / 4;
    axisLineVisible_ = false;

    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    mainLayout->setSpacing(3); //CHILD_TAB_WIDTH / 3);
    mainLayout->setMargin(0);
    mainLayout->setContentsMargins(0, 0, CHILD_TAB_WIDTH, 0);
    mainLayout->addWidget(iconLabel_);
    //mainLayout->addSpacerItem(new QSpacerItem(CHILD_TAB_WIDTH, 5));
    mainLayout->addWidget(textLabel_, 1);
    mainLayout->addWidget(toolbar_);

    setLabel(label);
    setContentsMargins(0, 0, 0, 0);
    setLayoutDirection(Qt::LeftToRight);
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    setObjectName("#MAIN_FRAME");
}


/**
 * @brief ChartLabel::~ChartLabel
 */
ChartLabel::~ChartLabel()
{
    emit childRemoved(this);
}


/**
 * @brief ChartLabel::getAllDepthChildrenCount
 * This returns this set's total number of children all the way down to the lowest depth.
 * @return
 */
int ChartLabel::getAllDepthChildrenCount() const
{
    return allDepthChildrenCount_;
}


/**
 * @brief ChartLabel::isExpanded
 * @return
 */
bool ChartLabel::isExpanded()
{
    return isExpanded_;
}


/**
 * @brief ChartLabel::setDepth
 * @param depth
 */
void ChartLabel::setDepth(int depth)
{
    depth_ = depth;
}


/**
 * @brief ChartLabel::getDepth
 * @return
 */
int ChartLabel::getDepth() const
{
    return depth_;
}


/**
 * @brief ChartLabel::getLabel
 * @return
 */
QString ChartLabel::getLabel() const
{
    return label_;
}


/**
 * @brief ChartLabel::setLabel
 * @param label
 */
void ChartLabel::setLabel(QString label)
{
    label_ = label;
    textLabel_->setText(label);
    //setToolTip(label);
}


/**
 * @brief ChartLabel::addChildChartLabel
 * This sets up the child's layout and visibility based on this parent.
 * It also connects this parent's change of visibility to the child.
 * @param child
 */
void ChartLabel::addChildChartLabel(ChartLabel* child)
{
    if (child) {

        child->setParentChartLabel(this);
        child->setDepth(depth_ + 1);
        child->setContentsMargins(CHILD_TAB_WIDTH * child->getDepth(), 0, 0, 0);
        child->setVisible(isExpanded_);
        childrenChartLabels_.append(child);

        connect(child, &ChartLabel::childAdded, this, &ChartLabel::childLabelAdded);
        connect(child, &ChartLabel::childRemoved, this, &ChartLabel::childLabelRemoved);
        connect(this, &ChartLabel::setChildVisible, child, &ChartLabel::setVisible);

        // update allDepthChildrenCount and propagate signal to the ancestor
        childLabelAdded();
    }
}


/**
 * @brief ChartLabel::setParentChartLabel
 * @param parent
 */
void ChartLabel::setParentChartLabel(ChartLabel* parent)
{
    if (parent)
        parentChartLabel_ = parent;
}


/**
 * @brief ChartLabel::getChildrenChartLabels
 * @return
 */
const QList<ChartLabel *> &ChartLabel::getChildrenChartLabels() const
{
    return childrenChartLabels_;
}


/**
 * @brief ChartLabel::setHovered
 * @param hovered
 */
void ChartLabel::setHovered(bool hovered)
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
 * @brief ChartLabel::themeChanged
 * @param theme
 */
void ChartLabel::themeChanged(Theme* theme)
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
 * @brief ChartLabel::toggleExpanded
 */
void ChartLabel::toggleExpanded()
{
    isExpanded_ = !isExpanded_;
    isChildrenVisible_ = isExpanded_;
    emit setChildVisible(isExpanded_);

    QPixmap pixmap = isExpanded_ ? expandedPixmap_ : contractedPixmap_;
    iconLabel_->setPixmap(pixmap);
}


/**
 * @brief ChartLabel::setVisible
 * @param visible
 */
void ChartLabel::setVisible(bool visible)
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
 * @brief ChartLabel::childLabelAdded
 * This is called when a new child has been added to a child's tree.
 * A child added to this child's tree means that a child has been added to this label's tree.
 */
void ChartLabel::childLabelAdded()
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
 * @brief ChartLabel::childLabelRemoved
 * This is called when a child has been destructed.
 * @param child
 */
void ChartLabel::childLabelRemoved(ChartLabel* child)
{
    if (!child)
        return;

    int totalChildrenToRemove = 1 + child->getAllDepthChildrenCount();
    allDepthChildrenCount_ -= totalChildrenToRemove;
    childrenChartLabels_.removeAll(child);

    // update the icon to show that it can't be expanded
    if (allDepthChildrenCount_ == 0) {
        iconLabel_->setPixmap(unExpandablePixmap_);
        isExpanded_ = false;
        removeEventFilter(this);
    }

    emit childRemoved(child);
}


/**
 * @brief ChartLabel::eventFilter
 * @param object
 * @param event
 * @return
 */
bool ChartLabel::eventFilter(QObject* object, QEvent* event)
{
    // TODO - Why use eventFilter instead of mouseDoubleClickEvent?
    if (event->type() == QEvent::MouseButtonDblClick) {
        ChartLabel* set = dynamic_cast<ChartLabel*>(object);
        set->toggleExpanded();
        return true;
    }
    return QWidget::eventFilter(object, event);
}


/**
 * @brief ChartLabel::enterEvent
 * @param event
 */
void ChartLabel::enterEvent(QEvent* event)
{
    emit hovered(true);
}


/**
 * @brief ChartLabel::leaveEvent
 * @param event
 */
void ChartLabel::leaveEvent(QEvent* event)
{
    emit hovered(false);
}


/**
 * @brief ChartLabel::paintEvent
 * @param event
 */
void ChartLabel::paintEvent(QPaintEvent* event)
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
 * @brief ChartLabel::setAxisLineVisible
 * @param visible
 */
void ChartLabel::setAxisLineVisible(bool visible)
{
    axisLineVisible_ = visible;
    update();
}


/**
 * @brief ChartLabel::setTickVisible
 * @param visible
 */
void ChartLabel::setTickVisible(bool visible)
{
    tickVisible_ = visible;
    update();
}
