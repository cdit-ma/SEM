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
    mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setMargin(0);
    mainLayout->setContentsMargins(5, 0, 0, 0);
    mainLayout->setAlignment(Qt::AlignCenter);

    connect(Theme::theme(), &Theme::theme_Changed, this, &EntityAxis::themeChanged);
    themeChanged();
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
        entitySet->setAxisLineVisible(axisLineVisible);
        entitySet->setTickVisible(tickVisible);
        mainLayout->addWidget(entitySet);
        index = mainLayout->indexOf(entitySet);
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
        entitySet->setAxisLineVisible(axisLineVisible);
        entitySet->setTickVisible(tickVisible);
        int prevIndex = mainLayout->indexOf(prevEntitySet);
        if (prevIndex != -1) {
            index = prevIndex + prevEntitySet->getAllDepthChildrenCount();
            mainLayout->insertWidget(index, entitySet);
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
        index = mainLayout->indexOf(entitySet);
        mainLayout->removeWidget(entitySet);
    }
    return index;
}


/**
 * @brief EntityAxis::getEntityAxisIndex
 * @param entitySet
 * @return
 */
int EntityAxis::getEntityAxisIndex(EntitySet* entitySet)
{
    return mainLayout->indexOf(entitySet);
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
    axisLineVisible = visible;
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
    tickVisible = visible;
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

void EntityAxis::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    emit sizeChanged(size());
}
