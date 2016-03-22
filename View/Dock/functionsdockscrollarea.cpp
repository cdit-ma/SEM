#include "functionsdockscrollarea.h"
#include "docktogglebutton.h"
#include "docknodeitem.h"

#include <QDebug>

/**
 * @brief FunctionsDockScrollArea::FunctionsDockScrollArea
 * @param type
 * @param view
 * @param parent
 */
FunctionsDockScrollArea::FunctionsDockScrollArea(DOCK_TYPE type, NodeView *view, DockToggleButton *parent) :
    DockScrollArea(type, view, parent, "No worker definition has been imported.")
{
    // setup definitions-dock specific layout
    mainLayout = new QVBoxLayout();
    itemsLayout = new QVBoxLayout();
    mainLayout->addLayout(itemsLayout);
    mainLayout->addStretch();
    getLayout()->addLayout(mainLayout);

    setDockOpen(false);
    connectToView();

    connect(this, SIGNAL(dock_opened(bool)), this, SLOT(dockToggled(bool)));
    connect(this, SIGNAL(dock_closed(bool)), this, SLOT(dockToggled(bool)));
}


/**
 * @brief FunctionsDockScrollArea::addDockNodeItems
 * @param functionKinds
 */
void FunctionsDockScrollArea::addDockNodeItems(QList<QPair<QString, QString> > functionKinds)
{
    QPair<QString, QString> function;
    foreach (function, functionKinds) {

        QString className = function.first;
        QString functionName = function.second;
        DockNodeItem* dockItem = new DockNodeItem(functionName, 0, this, false, className);
        dockItem->setID(className + "_" + functionName);

        // check if there is already a layout and label for the parent File
        if (!classLayoutItems.contains(className)){
            // create a new Class label and add it to the Class's layout
            DockNodeItem* classDockItem = new DockNodeItem(className, 0, this, true);
            QVBoxLayout* classLayout = new QVBoxLayout();

            classLayoutItems[className] = classLayout;
            classLayout->addWidget(classDockItem);
            addDockNodeItem(classDockItem, -1, false);
            insertDockNodeItem(classDockItem);
        }

        // connect the new dock item to its parent item
        DockNodeItem* parentItem = getDockNodeItem(className);
        if (parentItem) {
            dockItem->setParentDockNodeItem(parentItem);
            parentItem->addChildDockItem(dockItem);
        }

        if (classLayoutItems.contains(className)) {
            QVBoxLayout* classLayout = classLayoutItems[className];
            classLayout->addWidget(dockItem);
            addDockNodeItem(dockItem, -1, false);
            insertDockNodeItem(dockItem);
        }
    }
}


/**
 * @brief FunctionsDockScrollArea::connectToView
 */
void FunctionsDockScrollArea::connectToView()
{
    NodeView* view = getNodeView();
    if (view) {
        connect(view, SIGNAL(view_nodeSelected()), this, SLOT(updateCurrentNodeItem()));
    }
}


/**
 * @brief FunctionsDockScrollArea::dockNodeItemClicked
 */
void FunctionsDockScrollArea::dockNodeItemClicked()
{
    DockNodeItem* dockItem = qobject_cast<DockNodeItem*>(QObject::sender());
    DockNodeItem* parentItem = dockItem->getParentDockNodeItem();

    if (dockItem && parentItem) {
        getNodeView()->constructWorkerProcessNode(parentItem->getKind(), dockItem->getKind(), 0);
    }

    // this closes this dock and then re-opens the parts dock
    emit dock_forceOpenDock();
}


/**
 * @brief FunctionsDockScrollArea::updateDock
 */
void FunctionsDockScrollArea::updateDock()
{
    if (isDockOpen()) {
        emit dock_forceOpenDock();
    }
}


/**
 * @brief FunctionsDockScrollArea::clear
 */
void FunctionsDockScrollArea::clear()
{
    DockScrollArea::clear();
    classLayoutItems.clear();
}


/**
 * @brief FunctionsDockScrollArea::insertDockNodeItem
 * @param dockItem
 */
void FunctionsDockScrollArea::insertDockNodeItem(DockNodeItem *dockItem)
{
    if (!dockItem) {
        return;
    }

    DockNodeItem* parentDockItem = dockItem->getParentDockNodeItem();
    QVBoxLayout* layoutToSort = 0;

    QString ID = dockItem->getID();


    QString labelToSort = dockItem->getLabel();
    bool isClassLabel = dockItem->isDockItemLabel();

    if (isClassLabel) {
        // remove  classLayout from itemsLayout
        layoutToSort = itemsLayout;
        QVBoxLayout* classLayout = classLayoutItems[ID];
        if (classLayout) {
            layoutToSort->removeItem(classLayout);
        }
    } else {
        // remove the function from its parent class' layout
        if (parentDockItem) {
            QString parentID = parentDockItem->getID();
            layoutToSort = classLayoutItems[parentID];
            if (layoutToSort) {
                layoutToSort->removeWidget(dockItem);
            }
        }
    }

    if (!layoutToSort) {
        qWarning() << "DefinitionsDockScrollArea::insertDockNodeItem - Layout to sort is null.";
        return;
    }

    // iterate through the items in this dock's layout
    for (int i = 0; i < layoutToSort->count(); i++) {

        QLayoutItem* layoutItem = layoutToSort->itemAt(i);
        QString dockItemLabel;

        if (isClassLabel) {
            // get the Class name
            QVBoxLayout* classLayout = dynamic_cast<QVBoxLayout*>(layoutItem);
            if (classLayout) {
                QString fileID = classLayoutItems.key(classLayout, "");
                DockNodeItem* dockItem = getDockNodeItem(fileID);
                if (dockItem) {
                    dockItemLabel = dockItem->getLabel();
                }
            }
        } else {
            // get the function name
            DockNodeItem* dockNodeItem = dynamic_cast<DockNodeItem*>(layoutItem->widget());
            if (dockNodeItem && !dockNodeItem->isDockItemLabel()) {
                dockItemLabel = dockNodeItem->getLabel();
            }
        }

        // if for some reason the label is empty, skip to the next item
        if (dockItemLabel.isEmpty()) {
            continue;
        }

        // compare existing file names to the new file name
        // insert the new File into the correct alphabetical spot in the layout
        if (labelToSort.compare(dockItemLabel, Qt::CaseInsensitive) <= 0) {
            if (isClassLabel) {
                QVBoxLayout* classLayout = classLayoutItems[ID];
                if (classLayout) {
                    layoutToSort->insertLayout(i, classLayout);
                }
            } else {
                layoutToSort->insertWidget(i, dockItem);
            }
            return;
        }
    }

    // if there was no spot to insert the new File layout, add it to the end of the layout
    if (isClassLabel) {
        QVBoxLayout* classLayout = classLayoutItems[ID];
        if (classLayout) {
            layoutToSort->addLayout(classLayout);
        }
    } else {
        layoutToSort->addWidget(dockItem);
    }
}


/**
 * @brief FunctionsDockScrollArea::dockToggled
 * @param opened
 */
void FunctionsDockScrollArea::dockToggled(bool opened)
{
    QString action = "";
    if (opened) {
        //action = "Select to construct a Process";
        action = "Process";
    }
    emit dock_toggled(opened, action);
}


/**
 * @brief FunctionsDockScrollArea::forceOpenDock
 */
void FunctionsDockScrollArea::forceOpenDock()
{
    if (!isDockOpen()) {
        // close the sender dock then open this dock
        DockScrollArea* dock = qobject_cast<DockScrollArea*>(QObject::sender());
        if (dock) {
            dock->setDockOpen(false);
        }
        setDockOpen();
    }
}
