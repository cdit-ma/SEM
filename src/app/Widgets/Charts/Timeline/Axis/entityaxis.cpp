#include "entityaxis.h"
#include "entityset.h"
#include "../../../../theme.h"

#include <QDebug>

/**
 * @brief EntityAxis::EntityAxis
 * @param parent
 */
EntityAxis::EntityAxis(QWidget *parent)
    : QWidget(parent)
{
    mainLayout_ = new QVBoxLayout(this);
    mainLayout_->setSpacing(0);
    mainLayout_->setMargin(0);
    mainLayout_->setContentsMargins(5, 0, 0, 0);
    mainLayout_->setAlignment(Qt::AlignCenter);

    connect(Theme::theme(), &Theme::theme_Changed, this, &EntityAxis::themeChanged);
    themeChanged();

    setMinimumWidth(140);
}


/**
 * @brief EntityAxis::appendEntity
 * @param entitySet
 * @return
 */
int EntityAxis::appendEntity(EntitySet* entitySet)
{
    int index = -1;
    if (entitySet) {
        entitySet->setAxisLineVisible(axisLineVisible_);
        entitySet->setTickVisible(tickVisible_);
        mainLayout_->addWidget(entitySet);
        index = mainLayout_->indexOf(entitySet);
    }
    return index;
}


/**
 * @brief EntityAxis::insertEntity
 * @param prevEntitySet
 * @param entitySet
 * @return
 */
int EntityAxis::insertEntity(EntitySet* prevEntitySet, EntitySet* entitySet)
{
    int index = -1;
    if (prevEntitySet && entitySet) {
        entitySet->setAxisLineVisible(axisLineVisible_);
        entitySet->setTickVisible(tickVisible_);
        int prevIndex = mainLayout_->indexOf(prevEntitySet);
        if (prevIndex != -1) {
            index = prevIndex + prevEntitySet->getAllDepthChildrenCount();
            mainLayout_->insertWidget(index, entitySet);
        }
    }
    return index;
}


/**
 * @brief EntityAxis::removeEntity
 * @param entitySet
 * @return
 */
int EntityAxis::removeEntity(EntitySet* entitySet)
{
    int index = -1;
    if (entitySet) {
        index = mainLayout_->indexOf(entitySet);
        mainLayout_->removeWidget(entitySet);
    }
    return index;
}


/**
 * @brief EntityAxis::getEntityAxisIndex
 * @param entitySet
 * @return
 */
int EntityAxis::getEntityAxisIndex(EntitySet* entitySet) const
{
    return mainLayout_->indexOf(entitySet);
}


/**
 * @brief EntityAxis::setAxisLineVisible
 * @param visible
 */
void EntityAxis::setAxisLineVisible(bool visible)
{
    QList<EntitySet*> childrenSets = findChildren<EntitySet*>();
    for (EntitySet* set : childrenSets) {
        set->setAxisLineVisible(visible);
    }
    axisLineVisible_ = visible;
}


/**
 * @brief EntityAxis::setTickVisible
 * @param visible
 */
void EntityAxis::setTickVisible(bool visible)
{
    QList<EntitySet*> childrenSets = findChildren<EntitySet*>();
    for (EntitySet* set : childrenSets) {
        set->setTickVisible(visible);
    }
    tickVisible_ = visible;
}


/**
 * @brief EntityAxis::themeChanged
 */
void EntityAxis::themeChanged()
{
    Theme* theme = Theme::theme();
    QList<EntitySet*> childrenSets = findChildren<EntitySet*>();
    for (EntitySet* set : childrenSets) {
        set->themeChanged(theme);
    }
}


/**
 * @brief EntityAxis::resizeEvent
 * @param event
 */
void EntityAxis::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    emit sizeChanged(size());
}
