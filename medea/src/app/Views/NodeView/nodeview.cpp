#include "nodeview.h"
#include "SceneItems/Node/defaultnodeitem.h"
#include "SceneItems/Node/stacknodeitem.h"
#include "SceneItems/Node/compactnodeitem.h"
#include "SceneItems/Node/hardwarenodeitem.h"
#include "SceneItems/Node/membernodeitem.h"
#include "SceneItems/Node/deploymentcontainernodeitem.h"
#include "../ContextMenu/contextmenu.h"
#include "../../Controllers/WindowManager/windowmanager.h"
#include "../../Widgets/DockWidgets/viewdockwidget.h"

#include <QtMath>
#include <QTimer>
#include <QGraphicsItem>
#include <QKeyEvent>
#include <QDateTime>
#include <QOpenGLWidget>

const qreal zoom_incrementor = 1.05;
const qreal zoom_min_ratio = 0.25;
const qreal zoom_max_ratio = 10.0;

const qreal centered_rect_scale = 1.1;

const int invalid_node_id = -1;

const QPointF NodeView::null_point_ = QPointF();

/**
 * @brief NodeView::NodeView
 * @param view_controller
 * @param parent
 * @throws std::runtime_error
 */
NodeView::NodeView(ViewController& view_controller, QWidget* parent)
	: QGraphicsView(parent),
	  view_controller_(view_controller)
{
    connectViewController();

    // Setup the scene/scene rect
    QRectF sceneRect;
    sceneRect.setSize(QSize(50000,50000));
    sceneRect.moveCenter(QPointF(0,0));
    setSceneRect(sceneRect);
    setScene(new QGraphicsScene(this));
    scene()->setItemIndexMethod(QGraphicsScene::NoIndex);

    setMinimumSize(200, 200);
    setupStateMachine();

    // Set QT options for this QGraphicsView
    setDragMode(NoDrag);
    setAcceptDrops(true);
    setFocusPolicy(Qt::StrongFocus);
    setCacheMode(QGraphicsView::CacheBackground);
    setTransformationAnchor(QGraphicsView::NoAnchor);
    setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);
    setRenderHint(QPainter::Antialiasing, true);
    setRenderHint(QPainter::SmoothPixmapTransform, true);
    setRenderHint(QPainter::HighQualityAntialiasing, true);

    // Turn off the scroll bars
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Set the background font
    background_font_.setPixelSize(50);
    setFont(background_font_);

    rubberband_ = new QRubberBand(QRubberBand::Rectangle, this);
    constructed_node_id_ = invalid_node_id;

    connect(WindowManager::manager(), &WindowManager::activeViewDockWidgetChanged, this, &NodeView::activeViewDockChanged);
    connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));
    themeChanged();

    // Connect view zoom anchor setting
    connect(SettingsController::settings(), &SettingsController::settingChanged, [this](SETTINGS key, const QVariant& value) {
        if (key == SETTINGS::GENERAL_ZOOM_UNDER_MOUSE) {
            bool zoom_under_mouse = value.toBool();
            zoom_anchor_ = zoom_under_mouse ? ViewportAnchor::AnchorUnderMouse : ViewportAnchor::AnchorViewCenter;
        }
    });
}

/**
 * @brief NodeView::~NodeView
 */
NodeView::~NodeView()
{
    if (contained_node_view_item_) {
        QList<ViewItem*> items = contained_node_view_item_->getNestedChildren();
        items.insert(0, contained_node_view_item_);

        QListIterator<ViewItem*> it(items);
        it.toBack();

        while (it.hasPrevious()) {
            ViewItem* item = it.previous();
            viewItem_Destructed(item->getID(), item);
        }
    }
}

/**
 * @brief NodeView::connectViewController
 * This needs to be setup in the constructor
 * @throws std::runtime_error
 */
void NodeView::connectViewController()
{
    // Add the actions
    addActions(view_controller_.getActionController()->getNodeViewActions());

    selection_handler_ = view_controller_.getSelectionController()->constructSelectionHandler(this);
    if (selection_handler_) {
        connect(selection_handler_, &SelectionHandler::itemSelectionChanged, this, &NodeView::selectionHandler_ItemSelectionChanged);
        connect(selection_handler_, &SelectionHandler::itemActiveSelectionChanged, this, &NodeView::selectionHandler_ItemActiveSelectionChanged);
    } else {
        throw std::runtime_error("NodeView::setViewController - Selection handler is null.");
    }

    connect(&view_controller_, &ViewController::vc_viewItemConstructed, this, &NodeView::viewItem_Constructed);
    connect(&view_controller_, &ViewController::vc_viewItemDestructing, this, &NodeView::viewItem_Destructed);

    connect(&view_controller_, &ViewController::vc_centerItem, this, &NodeView::centerOnItem);
    connect(&view_controller_, &ViewController::vc_fitToScreen, this, &NodeView::fitViewToScreen);
    connect(&view_controller_, &ViewController::vc_centerOnItems, this, &NodeView::centerOnItemIDs);
    connect(&view_controller_, &ViewController::vc_selectItems, this, &NodeView::selectItemIDs);
    connect(&view_controller_, &ViewController::vc_highlightItem, this, &NodeView::highlightItem);
    connect(&view_controller_, &ViewController::vc_selectAndCenterConnectedEntities, this, &NodeView::selectAndCenterConnections);

    connect(this, &NodeView::toolbarRequested, &view_controller_, &ViewController::vc_showToolbar);
    connect(this, &NodeView::triggerAction, &view_controller_, &ViewController::TriggerAction);
    connect(this, &NodeView::setData, &view_controller_, &ViewController::SetData);
    connect(this, &NodeView::removeData, &view_controller_, &ViewController::RemoveData);
    connect(this, &NodeView::editData, &view_controller_, &ViewController::vc_editTableCell);
}

/**
 * @brief NodeView::fitViewToScreen
 * This slot is triggered by the fit to screen actions
 * @param only_if_active
 */
void NodeView::fitViewToScreen(bool only_if_active)
{
    if (!only_if_active || is_active_) {
        fitToScreen();
    }
}


/**
 * @brief NodeView::fitToScreen
 * This scales the view such that all of its items fits within the viewport
 */
void NodeView::fitToScreen()
{
    centerOnItems(getTopLevelEntityItems());
}

/**
 * @brief NodeView::translate
 * @param point
 */
void NodeView::translate(QPointF point)
{
    // NOTE: View transformations move the scene (not the view)
    //  Hence, some stored scene positions may need to be remapped after any transform functions
    QGraphicsView::translate(point.x(), point.y());
}

/**
 * @brief NodeView::setContainedViewAspect
 * @param aspect
 */
void NodeView::setContainedViewAspect(VIEW_ASPECT aspect)
{
    contained_aspect_ = aspect;
    is_aspect_view_ = true;
    themeChanged();

    // Set default values
    auto settings = SettingsController::settings();
    center_on_construct_ = settings->getSetting(SETTINGS::GENERAL_ON_CONSTRUCTION_CENTER).toBool();
    select_on_construct_ = settings->getSetting(SETTINGS::GENERAL_ON_CONSTRUCTION_SELECT).toBool();

    // Connect the select and center on construction settings - we only want these for aspect views
    connect(settings, &SettingsController::settingChanged, [this](SETTINGS key, const QVariant& value) {
        if (key == SETTINGS::GENERAL_ON_CONSTRUCTION_CENTER) {
            center_on_construct_ = value.toBool();
        } else if (key == SETTINGS::GENERAL_ON_CONSTRUCTION_SELECT) {
            select_on_construct_ = value.toBool();
        }
    });

    // Catch the view controller's signals to get the required data for the above to work
    connect(&view_controller_, &ViewController::ConstructNodeAtPos, this, &NodeView::addNodeTriggered);
    connect(&view_controller_, &ViewController::ConstructNodeAtIndex, this, &NodeView::addNodeTriggered);
    connect(&view_controller_, &ViewController::ConstructConnectedNodeAtPos, this, &NodeView::addNodeTriggered);
    connect(&view_controller_, &ViewController::ConstructConnectedNodeAtIndex, this, &NodeView::addNodeTriggered);
    connect(&view_controller_, &ViewController::ActionFinished, this, &NodeView::actionFinished);
}

/**
 * @brief NodeView::setContainedNodeViewItem
 * @param item
 */
void NodeView::setContainedNodeViewItem(NodeViewItem *item)
{
    // Unregister the previous item
    if (contained_node_view_item_) {
        disconnect(contained_node_view_item_, &NodeViewItem::labelChanged, this, &NodeView::viewItem_LabelChanged);
        contained_node_view_item_->unregisterObject(this);
        if (!is_aspect_view_) {
            deleteLater();
        }
    }

    contained_node_view_item_ = item;

    // If we have a new item, register it
    if (contained_node_view_item_) {

        contained_node_view_item_->registerObject(this);
        connect(contained_node_view_item_, &NodeViewItem::labelChanged, this, &NodeView::viewItem_LabelChanged);

        // This sets the view's background text
        viewItem_LabelChanged(contained_node_view_item_->getData(KeyName::Label).toString());

        // NOTE: This NEEDS to be called before the code below or the NodeViewItem won't be setup correctly!
        contained_aspect_ = contained_node_view_item_->getViewAspect();

        // Only add the item to the scene if it's not an aspect item
        if (!is_aspect_view_) {
            viewItem_Constructed(item);
            for (auto child_item : item->getNestedChildren()) {
                viewItem_Constructed(child_item);
            }
        }
    }

    clearSelection();
}

/**
 * @brief NodeView::getContainedViewItem
 * @return
 */
ViewItem* NodeView::getContainedViewItem()
{
    return contained_node_view_item_;
}

/**
 * @brief NodeView::getBackgroundColor
 * @return
 */
QColor NodeView::getBackgroundColor() const
{
    return background_color_;
}

/**
 * @brief NodeView::viewItem_Constructed
 * @param view_item
 */
void NodeView::viewItem_Constructed(ViewItem* view_item)
{
    if (view_item) {
        if (view_item->isNode()) {
            nodeViewItem_Constructed(qobject_cast<NodeViewItem*>(view_item));
        } else if (view_item->isEdge()) {
            edgeViewItem_Constructed(qobject_cast<EdgeViewItem*>(view_item));
        }
    }
}

/**
 * @brief NodeView::viewItem_Destructed
 * @param ID
 * @param view_item
 */
void NodeView::viewItem_Destructed(int ID, ViewItem* view_item)
{
    auto item = getEntityItem(ID);
    if (item) {
        top_level_gui_item_ids_.removeAll(ID);
        gui_items_.remove(ID);
        if (item->scene()) {
            scene()->removeItem(item);
        }
        delete item;
    }

    if (view_item && contained_node_view_item_ == view_item) {
        setContainedNodeViewItem(nullptr);
    }
}

/**
 * @brief NodeView::selectionHandler_ItemSelectionChanged
 * @param item
 * @param selected
 */
void NodeView::selectionHandler_ItemSelectionChanged(ViewItem* item, bool selected)
{
    if (item) {
        auto entity_item = getEntityItem(item->getID());
        if (entity_item) {
            entity_item->setSelected(selected);
            emit itemSelectionChanged(item, selected);
        }
    }
}

/**
 * @brief NodeView::selectionHandler_ItemActiveSelectionChanged
 * @param item
 * @param is_active
 */
void NodeView::selectionHandler_ItemActiveSelectionChanged(ViewItem* item, bool is_active)
{
    if (item) {
        auto entity_item = getEntityItem(item->getID());
        if (entity_item) {
            entity_item->setActiveSelected(is_active);
        }
    }
}

/**
 * @brief NodeView::selectAll
 */
void NodeView::selectAll()
{
    // This is triggered by the OS select all key sequence (CTRL+A)
    // NOTE: This slot is triggered by the edit_selectAll action's triggered signal
    //  edit_selectAll has a QKeySequence::SelectAll shortcut attached to it which is connected
    //  to the selectAll signal from the SelectionController that's connected to this NodeView
    auto items_to_select = getTopLevelViewItems();
    if (!items_to_select.isEmpty()) {
        getSelectionHandler().toggleItemsSelection(items_to_select, false);
    }
}

/**
 * @brief NodeView::selectAllChildren
 */
void NodeView::selectAllChildren()
{
    // This is triggered by the key sequence (CTRL+Alt+A)
    // Deselect the current selection and select all their direct children instead
    QSet<NodeViewItem*> items_to_select;

    const auto& current_selection = getSelectionHandler().getSelection();
    for (auto item : current_selection) {
        if (item != nullptr && item->isNode()) {
            auto node_view_item = qobject_cast<NodeViewItem*>(item);
            items_to_select.unite(node_view_item->getChildrenNodeViewItems());
        }
    }

    if (!items_to_select.isEmpty()) {
        auto&& to_select_list = QList<ViewItem*>(items_to_select.begin(), items_to_select.end());
        getSelectionHandler().toggleItemsSelection(to_select_list, false);
    }
}

/**
 * @brief NodeView::alignHorizontal
 */
void NodeView::alignHorizontal()
{
    emit triggerAction("Aligning Selection Horizontally");

    QList<EntityItem*> selected_items = getSelectedItems();
    QRectF sceneRect = getSceneBoundingRectOfItems(selected_items);

    for (auto item : selected_items) {

        item->setMoveStarted();

        auto parent = item->getParent();
        if (!parent) {
            parent = item;
        }

        QPointF pos = item->getPos();
        pos.setY(parent->mapFromScene(sceneRect.topLeft()).y());
        pos.ry() += item->getTopLeftOffset().y();
        item->setPos(pos);

        if (item->setMoveFinished()) {
            pos = item->getNearestGridPoint(null_point_);
            emit setData(item->getID(), KeyName::X, pos.x());
            emit setData(item->getID(), KeyName::Y, pos.y());
        }
    }
}

/**
 * @brief NodeView::alignVertical
 */
void NodeView::alignVertical()
{
    emit triggerAction("Aligning Selection Vertically");

    QList<EntityItem*> selected_items = getSelectedItems();
    QRectF sceneRect = getSceneBoundingRectOfItems(selected_items);

    for (auto item : selected_items) {

        item->setMoveStarted();

        auto parent = item->getParent();
        if (!parent) {
            parent = item;
        }

        QPointF pos = item->getPos();
        pos.setX(parent->mapFromScene(sceneRect.topLeft()).x());
        pos.rx() += item->getTopLeftOffset().x();
        item->setPos(pos);

        if (item->setMoveFinished()) {
            pos = item->getNearestGridPoint(null_point_);
            emit setData(item->getID(), KeyName::X, pos.x());
            emit setData(item->getID(), KeyName::Y, pos.y());
        }
    }
}

/**
 * @brief NodeView::clearSelection
 */
void NodeView::clearSelection()
{
    // Depending on the type of NodeView we are.
    if (contained_node_view_item_) {
        if (!(getSelectionHandler().getSelectionCount() == 1 && getSelectionHandler().getActiveSelectedItem() == contained_node_view_item_)) {
            // If we are the aspect select the aspect.
            getSelectionHandler().toggleItemsSelection(contained_node_view_item_);
        }
    } else {
        // TODO: When is this ever the case???
        // If we aren't an aspect clear the selection.
        getSelectionHandler().clearSelection();
    }
}

/**
 * @brief NodeView::themeItem
 * @param entity
 */
void NodeView::themeItem(EntityItem* entity)
{
    if (entity) {
        entity->setBaseBodyColor(body_color_);
        entity->setAltBodyColor(alt_body_color_);
        entity->setTextColor(text_color_);
        entity->setAltTextColor(alt_text_color_);
        entity->setHeaderColor(header_color_);
        entity->setHighlightColor(highlight_color_);
        entity->setHighlightTextColor(highlight_text_color_);
        entity->setDefaultPen(default_pen_);
    }
}

/**
 * @brief NodeView::themeChanged
 */
void NodeView::themeChanged()
{
    auto theme = Theme::theme();

    if (is_aspect_view_) {
        background_color_ = theme->getAspectBackgroundColor(contained_aspect_);
    } else {
        background_color_ = theme->getAltBackgroundColor();
    }

    background_text_color_ = background_color_.darker(110);
    setBackgroundBrush(background_color_);

    body_color_ = theme->getAltBackgroundColor();
    text_color_ = theme->getTextColor();
    alt_text_color_ = theme->getAltTextColor();
    header_color_ = theme->getBackgroundColor();
    highlight_color_ = theme->getHighlightColor();
    highlight_text_color_ = theme->getTextColor(ColorRole::SELECTED);
    alt_body_color_ = theme->getDisabledBackgroundColor();
    default_pen_ = QPen(theme->getTextColor(ColorRole::DISABLED));
    default_pen_.setCosmetic(true);

    for (auto entity : gui_items_) {
        themeItem(entity);
    }
}

/**
 * @brief NodeView::node_ConnectEdgeMenu
 * This is called when a NodeItem's edge knob (button) has been clicked
 * It shows a menu listing the edges with the provided kind that the clicked item can connect to
 * @param scene_pos
 * @param kind
 * @param direction
 */
void NodeView::node_ConnectEdgeMenu(QPointF scene_pos, EDGE_KIND kind, EDGE_DIRECTION direction)
{
    auto edge_direction = direction == EDGE_DIRECTION::SOURCE ? EDGE_DIRECTION::TARGET : EDGE_DIRECTION::SOURCE;
    auto global_pos = mapToGlobal(mapFromScene(scene_pos));
    view_controller_.getContextMenu()->popup_edge_menu(global_pos, kind, edge_direction);
}

/**
 * @brief NodeView::node_AddMenu
 * This is called when a NodeItem's plus/add button has been clicked
 * It shows a menu listing all the node items that the clicked item can adopt
 * @param scene_pos
 * @param index
 */
void NodeView::node_AddMenu(QPointF scene_pos, int index)
{
    auto global_pos = mapToGlobal(mapFromScene(scene_pos));
    view_controller_.getContextMenu()->popup_add_menu(global_pos, index);
}

/**
 * @brief NodeView::item_EditData
 * @param item
 * @param key_name
 */
void NodeView::item_EditData(ViewItem* item, const QString& key_name)
{
    getSelectionHandler().setActiveSelectedItem(item);
	emit editData(item->getID(), key_name);
}

/**
 * @brief NodeView::item_RemoveData
 * @param item
 * @param key_name
 */
void NodeView::item_RemoveData(ViewItem* item, const QString& key_name)
{
    if (item) {
        emit removeData(item->getID(), key_name);
    }
}

/**
 * @brief NodeView::centerSelection
 */
void NodeView::centerSelection()
{
    centerOnItems(getSelectedItems());
}

/**
 * @brief NodeView::selectAndCenterConnections
 * Select and center on the provided view items
 * @param items
 */
void NodeView::selectAndCenterConnections(const QVector<ViewItem*>& items)
{
    QSet<ViewItem*> to_select;
    QSet<EntityItem*> to_center;

    for (const auto& item : items) {
        if (item) {
            QSet<EdgeViewItem*> edges;
            if (item->isNode()) {
                edges = qobject_cast<NodeViewItem*>(item)->getEdges();
            } else if (item->isEdge()) {
                edges += qobject_cast<EdgeViewItem*>(item);
            }
            for (auto e : edges) {
                if (!e) {
                    continue;
                }
                auto s = e->getSource();
                auto d = e->getDestination();
                auto src = getEntityItem(s);
                auto dst = getEntityItem(d);
                auto edge = getEntityItem(e);
                if (src && s) {
                    to_select += s;
                    to_center += src;
                }
                if (dst && d) {
                    to_select += d;
                    to_center += dst;
                }
                if (edge) {
                    to_select += e;
                    to_center += edge;
                }
            }
        }
    }

    if (!to_select.isEmpty()) {
        // select the items
        getSelectionHandler().toggleItemsSelection(to_select.toList());
        // center on the items
        centerOnItems(to_center.toList());
    } else {
        clearSelection();
    }
}

/**
 * @brief NodeView::centerOnItemIDs
 * Center the view on the items with the provided ids
 * @param ids
 */
void NodeView::centerOnItemIDs(const QList<int>& ids)
{
    QList<EntityItem*> items;
    for (const auto& id : ids) {
        auto e = getEntityItem(id);
        if (e) {
            items += e;
        }
    }
    if (!items.isEmpty()) {
        centerOnItems(items);
    }
}

/**
 * @brief NodeView::selectItemIDs
 * Select the items with the provided ids
 * @param ids
 */
void NodeView::selectItemIDs(const QList<int>& ids)
{
    auto view_items = view_controller_.getViewItems(ids);
    getSelectionHandler().toggleItemsSelection(view_items);
}

/**
 * @brief NodeView::item_Selected
 * @param item
 * @param append
 */
void NodeView::item_Selected(ViewItem* item, bool append)
{
    getSelectionHandler().toggleItemsSelection(item, append);
}

/**
 * @brief NodeView::item_ActiveSelected
 * @param item
 */
void NodeView::item_ActiveSelected(ViewItem* item)
{
    getSelectionHandler().setActiveSelectedItem(item);
}

/**
 * @brief NodeView::item_SetExpanded
 * @param item
 * @param expand
 */
void NodeView::item_SetExpanded(EntityItem* item, bool expand)
{
    if (item) {
        int ID = item->getID();
        emit triggerAction("Expanding Selection");
        emit setData(ID, KeyName::IsExpanded, expand);
    }
}

/**
 * @brief NodeView::minimap_Pan
 * @param delta
 */
void NodeView::minimap_Pan(QPointF delta)
{
    translate(delta);
}

/**
 * @brief NodeView::minimap_Zoom
 * @param delta
 */
void NodeView::minimap_Zoom(int delta)
{
    zoom(delta);
}

/**
 * @brief NodeView::receiveMouseMove
 * This function is used to pass mouse move events from the overlay splitter to this view.
 * This is needed for the hover on graphics items to work.
 * @param event
 */
void NodeView::receiveMouseMove(QMouseEvent *event)
{
    switch (event->type()) {
    case QEvent::MouseMove:
        mouseMoveEvent(event);
        break;
    case QEvent::MouseButtonDblClick:
        mouseDoubleClickEvent(event);
        break;
    default:
        break;
    }
}

/**
 * @brief NodeView::centerOnItem
 * This centers the view on the item with the provided ID
 * It makes sure that the item's parent widget is visible
 * @param ID
 */
void NodeView::centerOnItem(int ID)
{
    auto item = getEntityItem(ID);
    if (item) {
        if (parentWidget()) {
            parentWidget()->show();
        }
        centerOnItemInternal(item);
    }
}

/**
 * @brief NodeView::highlightItem
 * @param ID
 * @param highlighted
 */
void NodeView::highlightItem(int ID, bool highlighted)
{
    auto item = getEntityItem(ID);
    if (item) {
        item->setHighlighted(highlighted);
    }
}

/**
 * @brief NodeView::setupItemConnections
 * @param item
 */
void NodeView::setupItemConnections(EntityItem* item)
{
    themeItem(item);
    
    connect(item, &EntityItem::req_activeSelected, this, &NodeView::item_ActiveSelected);
    connect(item, &EntityItem::req_selected, this, &NodeView::item_Selected);
    connect(item, &EntityItem::req_expanded, this, &NodeView::item_SetExpanded);

    connect(item, &EntityItem::req_StartMove, this, &NodeView::itemMoveTriggered);
    connect(item, &EntityItem::req_Move, this, &NodeView::moveSelection);

    connect(item, &EntityItem::req_triggerAction, this, &NodeView::triggerAction);
    connect(item, &EntityItem::req_removeData, this, &NodeView::item_RemoveData);
    connect(item, &EntityItem::req_editData, this, &NodeView::item_EditData);

    if (item->isNodeItem()) {
        auto node = qobject_cast<NodeItem*>(item);
        connect(node, &NodeItem::req_connectEdgeMode, this, &NodeView::setConnectingModeOn);
        connect(node, &NodeItem::req_connectEdgeMenu, this, &NodeView::node_ConnectEdgeMenu);
        connect(node, &NodeItem::req_addNodeMenu, this, &NodeView::node_AddMenu);
    }
}

/**
 * @brief NodeView::showItem
 * @param item
 */
void NodeView::showItem(EntityItem* item)
{
    auto parent = item->getParent();
    emit triggerAction("Expanding Selection");

    while (parent) {
        if (parent->isNodeItem()) {
            auto node_item = qobject_cast<NodeItem*>(parent);
            if (!node_item->isExpanded()) {
                node_item->setExpanded(true);
                int ID = parent->getID();
                emit setData(ID, KeyName::IsExpanded, true);
            }
        }
        parent = parent->getParent();
    }
}

/**
 * @brief NodeView::centerOnItems
 * @param items
 */
void NodeView::centerOnItems(const QList<EntityItem*>& items)
{
    for (auto item: items) {
        if (!item->isVisibleTo(nullptr)) {
            showItem(item);
        }
    }
    centerRect(getSceneBoundingRectOfItems(items));
}

/**
 * @brief NodeView::centerOnItemInternal
 * This centers the view on the provided item
 * It makes sure that the item is visible
 * @param item
 */
void NodeView::centerOnItemInternal(EntityItem* item)
{
    if (!item->isVisibleTo(nullptr)) {
        showItem(item);
    }
    centerRect(item->sceneBoundingRect());
}

/**
 * @brief NodeView::getSceneBoundingRectOfItems
 * @param items
 * @return
 */
QRectF NodeView::getSceneBoundingRectOfItems(const QList<EntityItem*>& items)
{
    QRectF itemsRect;
    for (auto item : items) {
        if (item->isVisible()) {
            itemsRect = itemsRect.united(item->sceneBoundingRect());
        }
    }
    return itemsRect;
}

/**
 * @brief NodeView::centerRect
 * @param rect_scene
 */
void NodeView::centerRect(QRectF rect_scene)
{
    if (rect_scene.isValid()) {
        // CENTER_RECT_SCALE adds a margin to the rect that we're trying to center and fill the view with
        QRectF visibleRect = viewportSceneRect();
        qreal widthRatio = visibleRect.width() / (rect_scene.width() * centered_rect_scale);
        qreal heightRatio = visibleRect.height() / (rect_scene.height() * centered_rect_scale);
        qreal scaleRatio = qMin(widthRatio, heightRatio);

        cappedScale(scaleRatio);
        centerViewOn(rect_scene.center());

    } else {
        resetMatrix();
    }
}

/**
 * @brief NodeView::centerViewOn
 * Center the view on the given scene position
 * @param scenePos
 */
void NodeView::centerViewOn(QPointF scene_pos)
{
    QPointF delta = viewportSceneRect().center() - scene_pos;
    translate(delta);
}

/**
 * @brief NodeView::getSelectionHandler
 * @throws std::runtime_error
 * @return
 */
SelectionHandler& NodeView::getSelectionHandler() const
{
    if (selection_handler_) {
        return *selection_handler_;
    } else {
        throw std::runtime_error("NodeView::getSelectionHandler - Selection handler is null.");
    }
}

/**
 * @brief NodeView::topLevelItemMoved
 */
void NodeView::topLevelItemMoved()
{
    auto new_scene_rect = getSceneBoundingRectOfItems(getTopLevelEntityItems());
    if (new_scene_rect != current_scene_rect_) {
        current_scene_rect_ = new_scene_rect;
        emit scenerect_changed(current_scene_rect_);
    }
}

/**
 * @brief NodeView::update_minimap
 */
void NodeView::update_minimap()
{
    emit viewport_changed(viewportSceneRect(), transform().m11());
    emit scenerect_changed(current_scene_rect_);
}

/**
 * @brief NodeView::paintEvent
 * @param event
 */
void NodeView::paintEvent(QPaintEvent* event)
{
    QGraphicsView::paintEvent(event);

    auto new_transform = transform();
    if (old_transform_ != new_transform) {
        old_transform_ = new_transform;
        update_minimap();
    }
}

/**
 * @brief NodeView::event
 * Need to override this function in order to properly pass through events from the OverlaySplitter/Panel
 * @param event
 * @return
 */
bool NodeView::event(QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress) {
        mousePressEvent(dynamic_cast<QMouseEvent*>(event));
    } else if (event->type() == QEvent::MouseMove) {
        mouseMoveEvent(dynamic_cast<QMouseEvent*>(event));
    } else if (event->type() == QEvent::MouseButtonRelease) {
        mouseReleaseEvent(dynamic_cast<QMouseEvent*>(event));
    } else if (event->type() == QEvent::Wheel) {
        wheelEvent(dynamic_cast<QWheelEvent*>(event));
    }
    return QGraphicsView::event(event);
}

/**
 * @brief NodeView::viewItem_LabelChanged
 * @param label
 */
void NodeView::viewItem_LabelChanged(const QString& label)
{
    auto text = label.toUpper();
    background_text_.setText(text);

    // Calculate the rectangle which contains the background text
    auto fm = QFontMetrics(background_font_);
    background_text_rect_ = fm.boundingRect(text);

    // Since the cached mode is set to CacheBackground, resetting the cache is necessary to redraw the background text
    resetCachedContent();
}

/**
 * @brief NodeView::activeViewDockChanged
 * @param dw
 */
void NodeView::activeViewDockChanged(ViewDockWidget* dw)
{

    bool active = dw && dw->widget() == this;
    if (active != is_active_) {
        is_active_ = active;
        update();
    }
}

/**
 * @brief NodeView::viewportSceneRect
 * This returns the viewport's rectangle in scene coordinates
 * @return
 */
QRectF NodeView::viewportSceneRect()
{
    return mapToScene(viewport()->rect()).boundingRect();
}

/**
 * @brief NodeView::nodeViewItem_Constructed
 * @param item
 */
void NodeView::nodeViewItem_Constructed(NodeViewItem* item)
{
    // If the item is null, or we're constructing in the wrong aspect, do nothing
    if (item == nullptr || item->getViewAspect() != contained_aspect_) {
        return;
    }

    if (contained_node_view_item_) {

        if (contained_node_view_item_->isAncestorOf(item)) {

            NodeItem* parent_node_item = getParentNodeItem(item);

            auto ID = item->getID();
            auto node_kind = item->getNodeKind();
            auto parent_node_kind = item->getParentNodeKind();
            
            // Ignore QOS Elements
            if (item->isNodeOfType(NODE_TYPE::QOS)) {
                return;
            }
            
            // Ignore Functions contained within Class Instances
            if (parent_node_kind == NODE_KIND::CLASS_INST) {
                if (node_kind == NODE_KIND::FUNCTION || node_kind == NODE_KIND::CALLBACK_FNC) {
                    return;
                }
            }

            NodeItem* node_item =  nullptr;

            switch (node_kind) {
            case NODE_KIND::HARDWARE_NODE:
                node_item = new HardwareNodeItem(item, parent_node_item);
                node_item->setSecondaryTextKey(KeyName::IpAddress);
                node_item->setExpandEnabled(true);
                break;
            case NODE_KIND::FPGA_FFT_DEVICE:
                node_item = new StackNodeItem(item, parent_node_item);
                node_item->setSecondaryTextKey("ip_address");
                node_item->setIconVisible(EntityItem::EntityRect::SECONDARY_ICON, {"Icons", "ethernet"}, true);
                break;
            case NODE_KIND::LOGGINGSERVER:
                node_item = new DefaultNodeItem(item, parent_node_item);
                node_item->setExpandEnabled(false);
                node_item->setSecondaryTextKey(KeyName::Database);
                node_item->setIconVisible(NodeItem::EntityRect::SECONDARY_ICON, {"Icons", "servers"}, true);
                break;
            case NODE_KIND::DEPLOYMENT_CONTAINER:
                node_item = new DeploymentContainerNodeItem(item, parent_node_item);
                break;
            case NODE_KIND::LOGGINGPROFILE:
                node_item = new DefaultNodeItem(item, parent_node_item);
                node_item->setExpandEnabled(false);
                node_item->setSecondaryTextKey(KeyName::Mode);
                node_item->setIconVisible(EntityItem::EntityRect::SECONDARY_ICON, {"Icons", "gear"}, true);
                break;
            case NODE_KIND::IDL:
                node_item = new DefaultNodeItem(item, parent_node_item);
                break;
            case NODE_KIND::NAMESPACE: {
                {
                    auto stack_item = new StackNodeItem(item, parent_node_item, Qt::Horizontal);
                    stack_item->SetUseColumnCount(0, 0, true);
                    stack_item->SetRenderCellSuffixIcon(0, 0, true, "Icons", "plus");
                    node_item = stack_item;
                }
                node_item->setSecondaryTextKey(KeyName::Namespace);
                node_item->setIconVisible(EntityItem::EntityRect::SECONDARY_ICON, {"Icons", "letterA"}, true);
                break;
            }
            case NODE_KIND::SHARED_DATATYPES:
                {
                    auto stack_item = new StackNodeItem(item, parent_node_item, Qt::Horizontal);
                    stack_item->SetUseColumnCount(0, 0, true);
                    stack_item->SetRenderCellSuffixIcon(0, 0, true, "Icons", "plus");
                    node_item = stack_item;
                }
                node_item->setSecondaryTextKey(KeyName::Version);
                node_item->setIconVisible(EntityItem::EntityRect::SECONDARY_ICON, {"Icons", "tag"}, true);
                break;
            case NODE_KIND::COMPONENT:
            {
                auto stack_item = new StackNodeItem(item, parent_node_item, Qt::Vertical);
                stack_item->SetRenderCellSuffixIcon(3, 0, true, "Icons", "plus");
                node_item = stack_item;
                break;
            }
            case NODE_KIND::COMPONENT_IMPL:
            {
                auto stack_item = new StackNodeItem(item, parent_node_item, Qt::Horizontal);
                node_item = stack_item;
                break;
            }
            case NODE_KIND::CLASS:
            {
                auto stack_item = new StackNodeItem(item, parent_node_item, Qt::Horizontal);
                node_item = stack_item;
                node_item->setSecondaryTextKey(KeyName::Version);
                node_item->setIconVisible(EntityItem::EntityRect::SECONDARY_ICON, {"Icons", "tag"}, true);
                break;
            }
            case NODE_KIND::COMPONENT_INST:
                node_item = new StackNodeItem(item, parent_node_item);
                node_item->setSecondaryTextKey(KeyName::Type);
                node_item->setIconVisible(EntityItem::EntityRect::SECONDARY_ICON, {"EntityIcons", "Component"}, true);
                break;
            case NODE_KIND::COMPONENT_ASSEMBLY:
                node_item = new DefaultNodeItem(item, parent_node_item);
                node_item->setSecondaryTextKey(KeyName::ReplicateValue);
                node_item->setIconVisible(EntityItem::EntityRect::SECONDARY_ICON, {"Icons", "copyX"}, true);
                break;
            case NODE_KIND::HARDWARE_CLUSTER:
                node_item = new StackNodeItem(item, parent_node_item);
                break;
            case NODE_KIND::PORT_REQUEST_DELEGATE:
                node_item = new DefaultNodeItem(item, parent_node_item);
                node_item->setExpandEnabled(false);
                node_item->setSecondaryTextKey(KeyName::Type);
                node_item->setIconVisible(EntityItem::EntityRect::SECONDARY_ICON, {"EntityIcons", "ServerInterface"}, true);
                break;
            case NODE_KIND::PORT_SUBSCRIBER_DELEGATE:
            case NODE_KIND::PORT_PUBLISHER_DELEGATE:
            case NODE_KIND::PORT_PUBSUB_DELEGATE:
                node_item = new DefaultNodeItem(item, parent_node_item);
                node_item->setExpandEnabled(false);
                node_item->setSecondaryTextKey(KeyName::Type);
                node_item->setIconVisible(EntityItem::EntityRect::SECONDARY_ICON, {"EntityIcons", "Aggregate"}, true);
                break;
            case NODE_KIND::PORT_REPLIER_INST:
            case NODE_KIND::PORT_REQUESTER_INST:
            case NODE_KIND::PORT_SUBSCRIBER_INST:
            case NODE_KIND::PORT_PUBLISHER_INST:
                node_item = new CompactNodeItem(item, parent_node_item);
                node_item->setSecondaryTextKey(KeyName::Type);
                node_item->setIconVisible(EntityItem::EntityRect::SECONDARY_ICON, {"EntityIcons", "Aggregate"}, true);
                node_item->setTertiaryTextKey(KeyName::Middleware);
                node_item->setIconVisible(EntityItem::EntityRect::TERTIARY_ICON, {"Icons", "sliders"}, true);
                if (node_kind == NODE_KIND::PORT_PUBLISHER_INST || node_kind == NODE_KIND::PORT_REQUESTER_INST) {
                    node_item->setRightJustified(true);
                }
                break;
            case NODE_KIND::EXTERNAL_SERVER_DELEGATE: {
                node_item = new StackNodeItem(item, parent_node_item, Qt::Horizontal);
                node_item->setSecondaryTextKey(KeyName::Type);
                node_item->setIconVisible(EntityItem::EntityRect::SECONDARY_ICON, {"EntityIcons", "ServerInterface"}, true);
                break;
            }
            case NODE_KIND::EXTERNAL_PUBSUB_DELEGATE: {
                node_item = new StackNodeItem(item, parent_node_item, Qt::Horizontal);
                node_item->setSecondaryTextKey(KeyName::Type);
                node_item->setIconVisible(EntityItem::EntityRect::SECONDARY_ICON, {"EntityIcons", "Aggregate"}, true);
                break;
            }
            case NODE_KIND::DEPLOYMENT_ATTRIBUTE:
                node_item = new CompactNodeItem(item, parent_node_item);
                node_item->setMoveEnabled(true);
                node_item->setSecondaryTextKey(KeyName::Type);
                node_item->setIconVisible(EntityItem::EntityRect::SECONDARY_ICON, {"Icons", "category"}, true);
                node_item->setTertiaryTextKey(KeyName::Value);
                node_item->setIconVisible(EntityItem::EntityRect::TERTIARY_ICON, {"Icons", "pencil"}, true);
                break;
            case NODE_KIND::ATTRIBUTE_INST:
                node_item = new StackNodeItem(item, parent_node_item);
                node_item->setExpandEnabled(false);
                switch (item->getViewAspect()) {
                    case VIEW_ASPECT::BEHAVIOUR: {
                        node_item->setSecondaryTextKey(KeyName::Type);
                        node_item->setIconVisible(EntityItem::EntityRect::SECONDARY_ICON, {"Icons", "category"}, true);
                        break;
                    }
                    default: {
                        node_item->setSecondaryTextKey(KeyName::Value);
                        node_item->setIconVisible(EntityItem::EntityRect::SECONDARY_ICON, {"Icons", "pencil"}, true);
                        break;
                    }
                }
                break;
            case NODE_KIND::SERVER_INTERFACE:
                node_item = new StackNodeItem(item, parent_node_item, Qt::Horizontal);
                node_item->setSecondaryTextKey(KeyName::Namespace);
                node_item->setIconVisible(EntityItem::EntityRect::SECONDARY_ICON, {"Icons", "letterA"}, true);
                break;
            case NODE_KIND::AGGREGATE:
                {
                    auto stack_item = new StackNodeItem(item, parent_node_item, Qt::Horizontal);
                    stack_item->SetUseColumnCount(0, 0, true);
                    node_item = stack_item;
                }
                node_item->setSecondaryTextKey(KeyName::Namespace);
                node_item->setIconVisible(EntityItem::EntityRect::SECONDARY_ICON, {"Icons", "letterA"}, true);
                break;
            case NODE_KIND::AGGREGATE_INST:
                {
                    auto stack_item = new StackNodeItem(item, parent_node_item, Qt::Horizontal);
                    stack_item->SetUseColumnCount(0, 0, true);
                    node_item = stack_item;
                }
                node_item->setSecondaryTextKey(KeyName::Type);
                node_item->setIconVisible(EntityItem::EntityRect::SECONDARY_ICON, {"Icons", "tiles"}, true);
                break;
            case NODE_KIND::FUNCTION_CALL:
            case NODE_KIND::CALLBACK_FNC_INST:
                node_item = new StackNodeItem(item, parent_node_item, Qt::Horizontal);
                node_item->setSecondaryTextKey(KeyName::Class);
                node_item->setIconVisible(EntityItem::EntityRect::SECONDARY_ICON, {"Icons", "spanner"}, true);
                break;
            case NODE_KIND::MEMBER:
            case NODE_KIND::MEMBER_INST:
                node_item = new MemberNodeItem(item, parent_node_item);
                node_item->setExpandEnabled(false);
                if (item->hasData(KeyName::Value)) {
                    node_item->setSecondaryTextKey(KeyName::Value);
                    node_item->setIconVisible(EntityItem::EntityRect::SECONDARY_ICON, {"Icons", "pencil"}, true);
                } else {
                    node_item->setSecondaryTextKey(KeyName::Type);
                    node_item->setIconVisible(EntityItem::EntityRect::SECONDARY_ICON, {"Icons", "category"}, true);
                }
                break;
            case NODE_KIND::VARIABLE:
                node_item = new StackNodeItem(item, parent_node_item);
                switch (item->getViewAspect()) {
                    case VIEW_ASPECT::ASSEMBLIES: {
                        node_item->setSecondaryTextKey(KeyName::Value);
                        node_item->setIconVisible(EntityItem::EntityRect::SECONDARY_ICON, {"Icons", "pencil"}, true);
                        break;
                    }
                    default: {
                        node_item->setSecondaryTextKey(KeyName::Type);
                        node_item->setIconVisible(EntityItem::EntityRect::SECONDARY_ICON, {"Icons", "category"}, true);
                        break;
                    }
                }
                break;
            case NODE_KIND::ATTRIBUTE_IMPL:
                node_item = new StackNodeItem(item, parent_node_item, Qt::Vertical);
                node_item->setExpandEnabled(false);
                node_item->setSecondaryTextKey(KeyName::Type);
                node_item->setIconVisible(EntityItem::EntityRect::SECONDARY_ICON, {"Icons", "tiles"}, true);
                break;
            case NODE_KIND::ENUM: {
                {
                    auto stack_item = new StackNodeItem(item, parent_node_item, Qt::Horizontal);
                    stack_item->SetUseColumnCount(0, 0, true);
                    stack_item->SetRenderCellSuffixIcon(0, 0, true, "Icons", "plus");
                    node_item = stack_item;
                }
                break;
            }
            case NODE_KIND::ENUM_INST:
                node_item = new StackNodeItem(item, parent_node_item);
                node_item->setExpandEnabled(false);
                if (item->hasData(KeyName::Value)) {
                    node_item->setSecondaryTextKey(KeyName::Value);
                    node_item->setIconVisible(EntityItem::EntityRect::SECONDARY_ICON, {"Icons", "pencil"}, true);
                } else {
                    node_item->setSecondaryTextKey(KeyName::Type);
                    node_item->setIconVisible(EntityItem::EntityRect::SECONDARY_ICON, {"Icons", "tiles"}, true);
                }
                break;
            case NODE_KIND::PORT_PUBLISHER_IMPL:
                node_item = new StackNodeItem(item, parent_node_item);
                node_item->setSecondaryTextKey(KeyName::Type);
                node_item->setIconVisible(EntityItem::EntityRect::SECONDARY_ICON, {"Icons", "tiles"}, true);
                break;
            case NODE_KIND::ATTRIBUTE:
                node_item = new StackNodeItem(item, parent_node_item);
                node_item->setExpandEnabled(false);
                node_item->setSecondaryTextKey(KeyName::Type);
                node_item->setIconVisible(EntityItem::EntityRect::SECONDARY_ICON, {"Icons", "category"}, true);
                break;
            case NODE_KIND::VARIABLE_PARAMETER:
                node_item = new StackNodeItem(item, parent_node_item);
                node_item->setExpandEnabled(false);
                node_item->setIconVisible(EntityItem::EntityRect::SECONDARY_ICON, {"Icons", KeyName::Value}, true);
                break;
            case NODE_KIND::INPUT_PARAMETER: {
                node_item = new StackNodeItem(item, parent_node_item);
                node_item->setExpandEnabled(false);
                if (item->hasData(KeyName::EditableKey)) {
                    node_item->setPrimaryTextKey("");
                    node_item->setSecondaryTextKey(item->getData(KeyName::EditableKey).toString());
                } else {
                    node_item->setPrimaryTextKey(KeyName::Label);
                    node_item->setSecondaryTextKey(KeyName::Value);
                }
                node_item->setIconVisible(EntityItem::EntityRect::SECONDARY_ICON, {"Icons", "pencil"}, true);
                break;
            }
            case NODE_KIND::VARIADIC_PARAMETER:
                node_item = new StackNodeItem(item, parent_node_item);
                node_item->setExpandEnabled(false);
                node_item->setPrimaryTextKey("");
                node_item->setSecondaryTextKey(KeyName::Value);
                node_item->setIconVisible(EntityItem::EntityRect::SECONDARY_ICON, {"Icons", "pencil"}, true);
                break;
            case NODE_KIND::RETURN_PARAMETER:
                node_item = new StackNodeItem(item, parent_node_item);
                node_item->setExpandEnabled(false);
                node_item->setSecondaryTextKey(KeyName::Type);
                node_item->setIconVisible(EntityItem::EntityRect::SECONDARY_ICON, {"Icons", "category"}, true);
                break;
            case NODE_KIND::PORT_REPLIER:
            case NODE_KIND::PORT_REQUESTER: {
                node_item = new StackNodeItem(item, parent_node_item, Qt::Horizontal);
                node_item->setSecondaryTextKey(KeyName::Type);
                node_item->setIconVisible(EntityItem::EntityRect::SECONDARY_ICON, {"Icons", "tiles"}, true);
                break;
            }
            case NODE_KIND::PORT_REQUESTER_IMPL: {
                auto stack_item = new StackNodeItem(item, parent_node_item, Qt::Horizontal);
                node_item = stack_item;
                node_item->setSecondaryTextKey(KeyName::Type);
                node_item->setIconVisible(EntityItem::EntityRect::SECONDARY_ICON, {"Icons", "tiles"}, true);

                stack_item->SetRenderCellArea(0, -1, true, true);
                stack_item->SetCellOrientation(0, -1, Qt::Vertical);
                stack_item->SetRenderCellArea(0, 1, true, true);
                stack_item->SetCellOrientation(0, 1, Qt::Vertical);
                break;
            }
            case NODE_KIND::VOID_TYPE: {
                node_item = new BasicNodeItem(item, parent_node_item);
                node_item->setExpandEnabled(false);
                node_item->setContractedHeight(node_item->getContractedHeight() / 2);
                node_item->setContractedWidth(40);
                break;
            }
            case NODE_KIND::CLASS_INST: {
                node_item = new StackNodeItem(item, parent_node_item);
                if (item->getData(KeyName::IsWorker).toBool()) {
                    node_item->setIconVisible(EntityItem::EntityRect::MAIN_ICON_OVERLAY, {"Icons", "spanner"}, true);
                }
                if (parent_node_kind == NODE_KIND::COMPONENT_INST) {
                    // Move ClassInstances to the bottom-right-most cell in the Assemblies aspect
                    emit setData(ID, KeyName::Row, 2);
                    emit setData(ID, KeyName::Column, 1);
                }
                node_item->setSecondaryTextKey(KeyName::Version);
                node_item->setIconVisible(EntityItem::EntityRect::SECONDARY_ICON, {"Icons", "tag"}, true);
                break;
            }
            case NODE_KIND::INPUT_PARAMETER_GROUP:
            case NODE_KIND::INPUT_PARAMETER_GROUP_INST:
            case NODE_KIND::RETURN_PARAMETER_GROUP:
            case NODE_KIND::RETURN_PARAMETER_GROUP_INST:
            case NODE_KIND::PORT_PERIODIC_INST: {
                node_item = new StackNodeItem(item, parent_node_item);
                node_item->setContractedHeight(node_item->getContractedHeight() / 2);
                node_item->setContractedWidth(40);
                break;
            }
            case NODE_KIND::PORT_PUBLISHER:
            case NODE_KIND::PORT_SUBSCRIBER:
                node_item = new StackNodeItem(item, parent_node_item);
                node_item->setSecondaryTextKey(KeyName::Type);
                node_item->setIconVisible(EntityItem::EntityRect::SECONDARY_ICON, {"Icons", "tiles"}, true);
                break;
            case NODE_KIND::PORT_SUBSCRIBER_IMPL:
                node_item = new StackNodeItem(item, parent_node_item, Qt::Horizontal);
                node_item->setSecondaryTextKey(KeyName::Type);
                node_item->setIconVisible(EntityItem::EntityRect::SECONDARY_ICON, {"Icons", "tiles"}, true);
                break;
            case NODE_KIND::PORT_PERIODIC:
                node_item = new StackNodeItem(item, parent_node_item, Qt::Horizontal);
                break;
            case NODE_KIND::CODE:
                node_item = new StackNodeItem(item, parent_node_item);
                node_item->setExpandEnabled(false);
                break;
            case NODE_KIND::VECTOR:
            case NODE_KIND::VECTOR_INST:
                node_item = new StackNodeItem(item, parent_node_item);
                node_item->setSecondaryTextKey(KeyName::Type);
                node_item->setIconVisible(EntityItem::EntityRect::SECONDARY_ICON, {"Icons", "category"}, true);
                break;
            case NODE_KIND::FUNCTION:
                node_item = new StackNodeItem(item, parent_node_item, Qt::Horizontal);
                if (!item->isDataProtected(KeyName::Operation)) {
                    node_item->setPrimaryTextKey(KeyName::Operation);
                } else {
                    node_item->setPrimaryTextKey(KeyName::Label);
                }
                break;
            case NODE_KIND::TRIGGER_INST:
                node_item = new StackNodeItem(item, parent_node_item);
                node_item->setSecondaryTextKey("type");
                node_item->setIconVisible(EntityItem::EntityRect::SECONDARY_ICON, {"Icons", "tiles"}, true);
                break;
            default:
                node_item = new StackNodeItem(item, parent_node_item);
                break;
            }

            // Ignore the position for the contained node view item
            if (contained_node_view_item_ == item) {
                node_item->setIgnorePosition(true);
            }

            // Remove data-fields that don't need to be shown in the data table for a Trigger/StrategyInst
            if (node_kind == NODE_KIND::TRIGGER_INST || node_kind == NODE_KIND::STRATEGY_INST) {
                auto data_table = node_item->getViewItem()->getTableModel();
                data_table->removedData("column");
                data_table->removedData("row");
                data_table->removedData("index");
            }

            auto stack_item = qobject_cast<StackNodeItem*>(node_item);

            if (stack_item) {
                stack_item->setDefaultCellSpacing(stack_item->getGridSize());

                if (item->isNodeOfType(NODE_TYPE::BEHAVIOUR_CONTAINER) || item->isNodeOfType(NODE_TYPE::TOP_BEHAVIOUR_CONTAINER)) {
                    stack_item->setAlignment(Qt::Horizontal);

                    if (node_kind == NODE_KIND::COMPONENT_IMPL || node_kind == NODE_KIND::CLASS) {
                        stack_item->SetRenderCellText(0, 0, true, "Functions");
                        stack_item->SetCellOrientation(0, 0, Qt::Vertical);
                        stack_item->SetRenderCellSuffixIcon(0, 0, true, "Icons", "plus");
                    } else {
                        stack_item->SetRenderCellArea(0, -1, true, true);
                        stack_item->SetCellOrientation(0, -1, Qt::Vertical);

                        stack_item->SetRenderCellArea(0, 1, true, true);
                        stack_item->SetCellOrientation(0, 1, Qt::Vertical);

                        stack_item->SetRenderCellArea(0, 0, true);
                        stack_item->SetRenderCellText(0, 0, true, "Workflow");

                        stack_item->SetRenderCellPrefixIcon(0, 0, true, "Icons", "arrowDownRightLong");
                        stack_item->SetRenderCellGapIcons(0, 0, true, "Icons", "arrowRightLong");
                        stack_item->SetRenderCellSuffixIcon(0, 0, true, "Icons", "plus");
                        stack_item->SetRenderCellHoverIcons(0, 0, true, "Icons", "plus");
                        stack_item->SetCellSpacing(0, 0, 20);
                    }

                    stack_item->SetRenderCellArea(1, 0, true, true);
                    stack_item->SetRenderCellText(1, 0, true, "Attributes");
                    stack_item->SetUseColumnCount(1, 0, true);
                    stack_item->SetCellSpacing(1, 0, 10);

                    stack_item->SetRenderCellArea(1, 1, true, true);
                    stack_item->SetRenderCellText(1, 1, true, "Variables");
                    stack_item->SetUseColumnCount(1, 1, true);
                    stack_item->SetCellSpacing(1, 1, 10);

                    stack_item->SetRenderCellArea(1, -1, true, true);
                    stack_item->SetRenderCellText(1, -1, true, "Headers");
                    stack_item->SetCellOrientation(1, -1, Qt::Vertical);
                    stack_item->SetUseColumnCount(1, -1, true);

                    stack_item->SetRenderCellArea(1, 2, true, true);
                    stack_item->SetRenderCellText(1, 2, true, "Workers");
                    stack_item->SetUseColumnCount(1, 2, true);
                    stack_item->SetCellSpacing(1, 2, 10);

                } else {
                    if (node_kind == NODE_KIND::AGGREGATE || node_kind == NODE_KIND::INPUT_PARAMETER_GROUP || node_kind == NODE_KIND::RETURN_PARAMETER_GROUP || node_kind == NODE_KIND::AGGREGATE_INST) {
                        stack_item->SetRenderCellSuffixIcon(0, 0, true, "Icons", "plus");
                    } else if (node_kind == NODE_KIND::COMPONENT_INST) {
                        // Add TriggerInstances vertically to the bottom-middle cell labelled "Triggers"
                        int row = 1, col = 3;
                        stack_item->SetRenderCellArea(row, col, true, true);
                        stack_item->SetRenderCellText(row, col, true, "Triggers");
                        stack_item->SetCellOrientation(row, col, Qt::Vertical);
                    }
                }
            }

            // Insert into the GUI hash
            gui_items_[ID] = node_item;
            setupItemConnections(node_item);

            // If the newly constructed item has no parent, add it directly to the scene
            if (parent_node_item == nullptr) {
                scene()->addItem(node_item);
                top_level_gui_item_ids_.append(ID);
                connect(node_item, &NodeItem::positionChanged, this, &NodeView::topLevelItemMoved);
                connect(node_item, &NodeItem::sizeChanged, this, &NodeView::topLevelItemMoved);
            }

            if (stack_item) {
                stack_item->RecalculateCells();
            }

            if (is_active_) {
                // This is used for centering/selecting the newly constructed item
                if (node_kind == clicked_node_kind_) {
                    constructed_node_id_ = ID;
                    clicked_node_kind_ = NODE_KIND::NONE;
                }
            }
        }

    } else {
        // Set the contained node view item (The top most item in the view)
        // This case is called when the item that was constructed iss an aspect
        // If it's not an aspect, setContainedNodeViewItem calls back into this function
        setContainedNodeViewItem(item);
    }
}

/**
 * @brief NodeView::edgeViewItem_Constructed
 * @param item
 */
void NodeView::edgeViewItem_Constructed(EdgeViewItem* item)
{
    switch (item->getEdgeKind()) {
        case EDGE_KIND::ASSEMBLY:
        case EDGE_KIND::DATA:
        case EDGE_KIND::DEPLOYMENT:
            break;
        default:
            return;
    }

    if (!contained_node_view_item_ || !contained_node_view_item_->isAncestorOf(item->getParentItem())) {
        return;
    }

    NodeItem* parent = getParentNodeItem(item->getParentItem());
    NodeItem* source = getParentNodeItem(item->getSource());
    NodeItem* destination = getParentNodeItem(item->getDestination());

	if (source && destination) {
		auto edgeItem = new EdgeItem(item, parent, source, destination);
		auto theme = Theme::theme();

		edgeItem->setBaseBodyColor(theme->getAltBackgroundColor());
		edgeItem->setHeaderColor(theme->getBackgroundColor());
		edgeItem->setTextColor(theme->getTextColor());

		QPen defaultPen(theme->getTextColor(ColorRole::DISABLED));
		defaultPen.setCosmetic(true);
		edgeItem->setDefaultPen(defaultPen);

		gui_items_[item->getID()] = edgeItem;
		setupItemConnections(edgeItem);

		if (!scene()->items().contains(edgeItem)) {
			scene()->addItem(edgeItem);
		}
	}
}

/**
 * @brief NodeView::getTopLevelViewItems
 * @return
 */
QList<ViewItem*> NodeView::getTopLevelViewItems() const
{
    QList<ViewItem*> items;
    for (auto item : getTopLevelEntityItems()) {
        items.append(item->getViewItem());
    }
    return items;
}

/**
 * @brief NodeView::getTopLevelEntityItems
 * @return
 */
QList<EntityItem*> NodeView::getTopLevelEntityItems() const
{
    QList<EntityItem*> items;
    for (int ID : top_level_gui_item_ids_) {
        auto item = gui_items_.value(ID, nullptr);
        if (item) {
            items.append(item);
        }
    }
    return items;
}

/**
 * @brief NodeView::getSelectedItems
 * @return
 */
QList<EntityItem*> NodeView::getSelectedItems() const
{
    QList<EntityItem*> items;
    for (auto item : getSelectionHandler().getSelection()) {
        auto eItem = getEntityItem(item);
        if (eItem) {
            items.append(eItem);
        }
    }
    return items;
}

/**
 * @brief NodeView::getParentNodeItem
 * @param item
 * @return
 */
NodeItem* NodeView::getParentNodeItem(NodeViewItem* item)
{
     while (item) {
        int ID = item->getID();
        if (gui_items_.contains(ID)) {
            return qobject_cast<NodeItem*>(gui_items_[ID]);
        } else {
            item = item->getParentNodeViewItem();
        }
     }
     return nullptr;
}

/**
 * @brief NodeView::getEntityItem
 * @param ID
 * @return
 */
EntityItem* NodeView::getEntityItem(int ID) const
{
    if (gui_items_.contains(ID)) {
        return gui_items_[ID];
    }
    return nullptr;
}

/**
 * @brief NodeView::getEntityItem
 * @param item
 * @return
 */
EntityItem* NodeView::getEntityItem(ViewItem *item) const
{
    if (item) {
        return getEntityItem(item->getID());
    }
    return nullptr;
}

/**
 * @brief NodeView::zoom
 * This zooms the view in/out and centers it on the set anchor point
 * @param delta
 * @param anchor_screen_pos
 */
void NodeView::zoom(int delta, QPoint anchor_screen_pos)
{
    // Store anchor point
    auto anchor_scene_pos = viewportSceneRect().center();
    switch (zoom_anchor_) {
    case ViewportAnchor::AnchorUnderMouse:
        anchor_scene_pos = mapToScene(anchor_screen_pos);
        break;
    default:
        break;
    }

    // Scale the view
    if (delta < 0) {
        cappedScale(1 / zoom_incrementor);
    } else {
        cappedScale(zoom_incrementor);
    }

    // Center on the anchor point
    switch (zoom_anchor_) {
    case ViewportAnchor::AnchorUnderMouse: {
        auto delta_pos = mapToScene(anchor_screen_pos) - anchor_scene_pos;
        translate(delta_pos);
        break;
    }
    default:
        centerViewOn(anchor_scene_pos);
        break;
    }
}

/**
 * @brief NodeView::cappedScale
 * This scales the view capped to the defined min/max zoom ratio
 * @param scale
 */
void NodeView::cappedScale(qreal scale)
{
    auto current_scale = transform().m11();
    auto new_scale = current_scale * scale;

    if (new_scale < zoom_min_ratio) {
        resetMatrix();
        scale = zoom_min_ratio;
    } else if (new_scale > zoom_max_ratio) {
        resetMatrix();
        scale = zoom_max_ratio;
    }

    QGraphicsView::scale(scale, scale);
}

/**
 * @brief NodeView::selectItemsInRubberband
 * This is called when the state_rubberband_mode_ is active, and the mouseReleased signal was sent
 * It selects the entities that are intersected by the rubberband
 */
void NodeView::selectItemsInRubberband()
{
    QPolygonF rubberband_rect = mapToScene(rubberband_->geometry());
    QList<ViewItem*> items_to_select;

    // I think this was needed to deselect the aspect/contained item if it was selected
    // TODO: Find out if this is still needed since I changed the toggleItemsSelection's append parameter to false
    /*
    // Check for aspect selection.
    if (selection_handler_->getSelection().contains(contained_node_view_item_)) {
        items_to_select.append(contained_node_view_item_);
    }
    */

    for (auto item : scene()->items(rubberband_rect, Qt::IntersectsItemShape)) {
        auto entityItem = dynamic_cast<EntityItem*>(item);
        if (entityItem) {
            items_to_select.append(entityItem->getViewItem());
        }
    }

    getSelectionHandler().toggleItemsSelection(items_to_select, false);
}

/**
 * @brief NodeView::setConnectingModeOn
 * This is called when a NodeItem's edge knob (button) has been dragged
 * It sets up the required data for the edge that is to be constructed if the connect line was dragged to a valid NodeItem
 * @param scene_pos
 * @param edge_kind
 * @param direction
 */
void NodeView::setConnectingModeOn(QPointF scene_pos, EDGE_KIND edge_kind, EDGE_DIRECTION direction)
{
    // Attach the data required to construct the edge to the connect mode state
    auto edge_direction = direction == EDGE_DIRECTION::SOURCE ? EDGE_DIRECTION::TARGET : EDGE_DIRECTION::SOURCE;
    state_connecting_mode_->setProperty("edge_kind", QVariant::fromValue(edge_kind));
    state_connecting_mode_->setProperty("edge_direction", QVariant::fromValue(edge_direction));

    // Highlight the entities that can be connected to
    auto item_map = view_controller_.getValidEdges(edge_kind);
    for (auto item : item_map.values(edge_direction)) {
        emit view_controller_.vc_highlightItem(item->getID(), true);
    }

    if (!connect_line_) {
        connect_line_ = new ArrowLine();
        connect_line_->setPen(Qt::DashLine);
        connect_line_->setZValue(100);
        scene()->addItem(connect_line_);
    }

    connect_line_->set_begin_point(scene_pos);
    connect_line_->set_end_point(scene_pos);
    connect_line_->setVisible(true);

    // Send a signal to enter the connect mode
    emit connectModeTriggered();
}

/**
 * @brief NodeView::connectUsingConnectLine
 * This is triggered when the state_connecting_mode_ is exited, which is caused by a mouseReleased signal
 */
void NodeView::connectUsingConnectLine()
{
    if (connect_line_ == nullptr)
        return;

    auto edge_kind = state_connecting_mode_->property("edge_kind").value<EDGE_KIND>();
    auto edge_direction = state_connecting_mode_->property("edge_direction").value<EDGE_DIRECTION>();

    // Remove item highlight
    auto item_map = view_controller_.getValidEdges(edge_kind);
    for (auto item : item_map.values(edge_direction)) {
        emit view_controller_.vc_highlightItem(item->getID(), false);
    }

    // Check if the the connect arrow pointed to an entity
    // If so, attemp to construct an edge to/from that entity
    QPointF scene_pos = edge_direction == EDGE_DIRECTION::SOURCE ? connect_line_->get_end_point() : connect_line_->get_begin_point();
    auto pointed_to_item = getEntityAtPos(scene_pos);
    if (pointed_to_item) {
        view_controller_.constructEdges(pointed_to_item->getID(), edge_kind, edge_direction);
    }

    connect_line_->setVisible(false);
}

/**
 * @brief NodeView::distance
 * @param p1
 * @param p2
 * @return
 */
qreal NodeView::distance(QPoint p1, QPoint p2)
{
    return qSqrt(qPow(p2.x() - p1.x(), 2) + qPow(p2.y() - p1.y(), 2));
}

/**
 * @brief NodeView::setMovingModeOn
 * This is called when the selection is about to be moved
 */
void NodeView::setMovingModeOn()
{
    for (auto view_item : getSelectionHandler().getSelection()) {
        auto item = getEntityItem(view_item);
        if (item && !item->isIgnoringPosition()) {
            item->setMoveStarted();
        }
    }
}

/**
 * @brief NodeView::setMovingModeOff
 * This is called when the selection has finished moving
 * Upon exiting the state_moving_mode_, signals are sent to the model controller to update the grapmhl's data
 */
void NodeView::setMovingModeOff()
{
    bool any_moved = false;
    for (auto view_item : getSelectionHandler().getSelection()) {
        auto item = getEntityItem(view_item);
        if (!item || item->isIgnoringPosition()) {
            continue;
        }
        // This sets the item's moving state to false, as well as returns whether it was moved or not
        if (item->setMoveFinished()) {
            any_moved = true;
        }
        if (any_moved) {
            // Send a signal to update the model (graphml) data
            QPointF pos = item->getNearestGridPoint(null_point_);
            emit setData(item->getID(), KeyName::X, pos.x());
            emit setData(item->getID(), KeyName::Y, pos.y());
        }
    }
}

/**
 * @brief NodeView::moveSelection
 * This is called when the state_moving_mode_ is active and the mouse is being moved
 * It caps the movement within the bounds of the entity that is being moved and then moves the selection
 * @param delta
 */
void NodeView::moveSelection(QPointF delta)
{
    if (state_moving_mode_->active()) {

        auto selected_items = getSelectionHandler().getSelection();

        // Validate the move for the entire selection
        for (auto view_item : selected_items) {
            auto item = getEntityItem(view_item);
            if (item) {
                delta = item->validateMove(delta);
                // If there is no valid adjustment that can be done, do nothing
                if (delta.isNull()){
                    return;
                }
            }
        }

        for (auto view_item : selected_items) {
            EntityItem* item = getEntityItem(view_item);
            if (item) {
                item->adjustPos(delta);
            }
        }
    }
}

/**
 * @brief NodeView::setupStateMachine
 */
void NodeView::setupStateMachine()
{
    state_machine_ = new QStateMachine(this);

    state_idle_ = new QState;
    state_panning_mode_ = new QState;
    state_moving_mode_ = new QState;
    state_connecting_mode_ = new QState;
    state_rubberband_keys_pressed_ = new QState;
    state_rubberband_mode_ = new QState;

    // Add the states to the state machine
    state_machine_->addState(state_idle_);
    state_machine_->addState(state_panning_mode_);
    state_machine_->addState(state_moving_mode_);
    state_machine_->addState(state_connecting_mode_);
    state_machine_->addState(state_rubberband_keys_pressed_);
    state_machine_->addState(state_rubberband_mode_);
    state_machine_->setInitialState(state_idle_);

    // Add the transitions from the initial state (state_idle_)
    state_idle_->addTransition(this, &NodeView::rightMousePressed, state_panning_mode_);
    state_idle_->addTransition(this, &NodeView::ctrlShiftPressed, state_rubberband_keys_pressed_);
    state_idle_->addTransition(this, &NodeView::itemMoveTriggered, state_moving_mode_);
    state_idle_->addTransition(this, &NodeView::connectModeTriggered, state_connecting_mode_);

    // When the mouse is released, return to the initial state from all the other mouse triggered states
    state_panning_mode_->addTransition(this, &NodeView::mouseReleased, state_idle_);
    state_connecting_mode_->addTransition(this, &NodeView::mouseReleased, state_idle_);
    state_moving_mode_->addTransition(this, &NodeView::mouseReleased, state_idle_);
    state_rubberband_keys_pressed_->addTransition(this, &NodeView::mouseReleased, state_idle_);
    state_rubberband_mode_->addTransition(this, &NodeView::mouseReleased, state_idle_);

    state_rubberband_keys_pressed_->addTransition(this, &NodeView::leftMousePressed, state_rubberband_mode_);

    // NOTE: Items only send a req_StartMove signal if they are move-enabled
    connect(state_moving_mode_, &QState::entered, this, &NodeView::setMovingModeOn);
    connect(state_moving_mode_, &QState::exited, this, &NodeView::setMovingModeOff);

    connect(state_rubberband_mode_, &QState::entered, this, &NodeView::setRubberbandModeOn);
    connect(state_rubberband_mode_, &QState::exited, this, &NodeView::setRubberbandModeOff);

    // There are data required from the clicked node item prior to entering connect mode
    // This is why we can't directly go into the connect mode state upon receiving a req_connectEdgeMode signal
    connect(state_connecting_mode_, &QState::exited, this, &NodeView::connectUsingConnectLine);

    state_machine_->start();
}

/**
 * @brief NodeView::getEntityAtPos
 * @param scenePos
 * @return
 */
EntityItem* NodeView::getEntityAtPos(QPointF scenePos) const
{
    for (auto item : scene()->items(scenePos)) {
        auto entity_item = dynamic_cast<EntityItem*>(item);
        if (entity_item) {
            return entity_item;
        }
    }
    return nullptr;
}

/**
 * @brief NodeView::setRubberbandModeOn
 * This is called when the state_rubberband_mode_ is entered
 * It is entered when the CTRL & SHIFT keys and the left mouse button has been pressed
 */
void NodeView::setRubberbandModeOn()
{
    viewport()->setCursor(Qt::CrossCursor);
    rubberband_->setVisible(true);
    update();
}

/**
 * @brief NodeView::setRubberbandModeOff
 * This is called when the state_rubberband_mode_ is exited
 * It is exited when a mouseReleased signal has been sent
 */
void NodeView::setRubberbandModeOff()
{
    viewport()->unsetCursor();
    rubberband_->setGeometry(QRect());
    rubberband_->setVisible(false);
    update();
}

/**
 * @brief NodeView::addNodeTriggered
 * This is called when the add/plus button/menu is triggered
 * It stores the node kind that is to be constructed, which is used for
 * selecting/centering on the newly constructed item if the setting is set
 * @param parent_id
 * @param kind
 */
void NodeView::addNodeTriggered(int parent_id, NODE_KIND kind)
{
    Q_UNUSED(parent_id);
    clicked_node_kind_ = kind;
}

/**
 * @brief NodeView::actionFinished
 * This is called whenever an undo/redoable action is finished
 */
void NodeView::actionFinished()
{
    if (constructed_node_id_ != invalid_node_id) {
        if (center_on_construct_) {
            centerOnItem(constructed_node_id_);
        }
        if (select_on_construct_) {
            selectItemIDs({constructed_node_id_});
        }
        // Reset stored ID
        constructed_node_id_ = invalid_node_id;
    }
}

/**
 * @brief NodeView::keyPressEvent
 * @param event
 */
void NodeView::keyPressEvent(QKeyEvent* event)
{
    bool ctrl_down = event->modifiers() & Qt::ControlModifier;
    bool shift_down = event->modifiers() & Qt::ShiftModifier;

    if (ctrl_down && shift_down) {
        emit ctrlShiftPressed();
    }
}

/**
 * @brief NodeView::keyReleaseEvent
 * @param event
 */
void NodeView::keyReleaseEvent(QKeyEvent* event)
{
    if (event->modifiers().testFlag(Qt::AltModifier)) {
        switch (event->key()) {
        case Qt::Key_Left:
        case Qt::Key_Right:
        case Qt::Key_Up:
        case Qt::Key_Down: {
            auto selectedItems = getSelectedItems();
            if (selectedItems.count() == 1) {
                auto item = selectedItems.first();
                if (item->isNodeItem()) {
                    shiftOrderInParent(qobject_cast<NodeItem*>(item), event->key());
                }
            }
            break;
        }
        case Qt::Key_A: {
            if (event->modifiers().testFlag(Qt::ControlModifier)) {
                selectAllChildren();
            }
            break;
        }
        default:
            break;
        }
    }
}

/**
 * @brief NodeView::shiftOrderInParent
 * @param item
 * @param key
 */
void NodeView::shiftOrderInParent(NodeItem* item, int key)
{
    auto node_item = qobject_cast<BasicNodeItem*>(item);
    if (!node_item) {
        return;
    }

    auto parent_stack_item = node_item->getParentStackContainer();
    if (parent_stack_item) {

        auto index = StackNodeItem::GetCellIndex(node_item);

        // Get the orientation of the index
        auto orientation = parent_stack_item->getCellOrientation(index);
        auto increment = key == Qt::Key_Right || key == Qt::Key_Down;
        auto move_index = false;

        switch (key) {
        case Qt::Key_Left:
        case Qt::Key_Right:{
            move_index = orientation == Qt::Horizontal;
            break;
        }
        case Qt::Key_Up:
        case Qt::Key_Down:{
            move_index = orientation == Qt::Vertical;
            break;
        }
        default:
            return;
        }

        auto action_controller = view_controller_.getActionController();
        QAction* action = nullptr;
        if (move_index) {
            if (increment) {
                action = action_controller->edit_incrementIndex;
            } else {
                action = action_controller->edit_decrementIndex;
            }
        } else {
            if (increment) {
                action = action_controller->edit_incrementRow;
            } else {
                action = action_controller->edit_decrementRow;
            }
        }
        if (action && action->isEnabled()) {
            action->trigger();
        }
    }
}

/**
 * @brief NodeView::wheelEvent
 * @param event
 */
void NodeView::wheelEvent(QWheelEvent* event)
{
    // TODO: Does this check have anything to do with zooming???
    //  Ask Jackson if this can be removed
    //if (view_controller_.isControllerReady())
    zoom(event->delta(), event->pos());
}

/**
 * @brief NodeView::mousePressEvent
 * @param event
 */
void NodeView::mousePressEvent(QMouseEvent* event)
{
    auto mouse_button = event->button();
    event_last_scene_pos_ = mapToScene(event->pos());

    if (mouse_button == Qt::MiddleButton) {
        // Either fit all the items or center the clicked item
        auto item = getEntityAtPos(event_last_scene_pos_);
        if (item) {
            centerOnItemInternal(item);
        } else {
            fitToScreen();
        }
    } else {
        event_last_pos_ = event->pos();
        if (mouse_button == Qt::RightButton) {
            emit rightMousePressed();
            pan_distance_ = 0;
        } else if (mouse_button == Qt::LeftButton) {
            emit leftMousePressed();
            if (state_idle_->active()) {
                auto item = getEntityAtPos(event_last_scene_pos_);
                if (!item) {
                    clearSelection();
                }
            }
        }
        QGraphicsView::mousePressEvent(event);
    }
}

/**
 * @brief NodeView::mouseMoveEvent
 * @param event
 */
void NodeView::mouseMoveEvent(QMouseEvent* event)
{
    QPoint event_pos = event->pos();
    QPointF event_scene_pos = mapToScene(event_pos);
    
    if (state_panning_mode_->active()) {
        // Calculate the distance in screen pixels travelled
        // This is used to differentiate between a pan and a right-click
        pan_distance_ += distance(event_pos, event_last_pos_);

        auto scene_delta = event_scene_pos - event_last_scene_pos_;
        translate(scene_delta);

        // Need to remap the event pos to the scene after the translation
        event_last_pos_ = event->pos();
        event_last_scene_pos_ = mapToScene(event->pos());

    } else if (state_rubberband_mode_->active()) {

        rubberband_->setGeometry(QRect(event_last_pos_, event_pos).normalized());

    }  else if (state_connecting_mode_->active()) {

        auto edge_direction = state_connecting_mode_->property("edge_direction").value<EDGE_DIRECTION>();
        auto item = getEntityAtPos(event_scene_pos);
        if (item) {
            item = item->isHighlighted() ? item : nullptr;
        }
        if (edge_direction == EDGE_DIRECTION::SOURCE) {
            connect_line_->set_end_point(event_scene_pos);
        } else {
            connect_line_->set_begin_point(event_scene_pos);
        }
        connect_line_->setHighlighted(item);
    }

    QGraphicsView::mouseMoveEvent(event);
}

/**
 * @brief NodeView::mouseReleaseEvent
 * @param event
 */
void NodeView::mouseReleaseEvent(QMouseEvent *event)
{
    if (state_panning_mode_->active()) {
        if (pan_distance_ < 10) {
            // If the mouse was released within an item, show the right-click menu
            auto item_pos = mapToScene(event->pos());
            auto item = getEntityAtPos(item_pos);
            if (item) {
                item_pos = item->mapFromScene(item_pos);
                if (!item->isSelected()) {
                    // Select the right-clicked item
                    bool ctrl_pressed = event->modifiers() & Qt::ControlModifier;
                    getSelectionHandler().toggleItemsSelection(item->getViewItem(), ctrl_pressed);
                }
            }
            // Show the context-menu at the right-clicked position
            emit toolbarRequested(event->globalPos(), item_pos);
        }
    } else if (state_rubberband_mode_->active()) {
        selectItemsInRubberband();
    }

    // Reset mouse event related variables
    pan_distance_ = 0;
    emit mouseReleased();

    QGraphicsView::mouseReleaseEvent(event);
}

/**
 * @brief NodeView::drawForeground
 * @param painter
 * @param r
 */
void NodeView::drawForeground(QPainter *painter, const QRectF &r)
{
    QGraphicsView::drawForeground(painter, r);
    if (!is_active_) {
        painter->setBrush(QColor(0, 0, 0, 60));
        painter->setPen(Qt::NoPen);
        painter->drawRect(r);
    }
}

/**
 * @brief NodeView::drawBackground
 * @param painter
 * @param r
 */
void NodeView::drawBackground(QPainter *painter, const QRectF & r)
{
    // Paint the background
    QGraphicsView::drawBackground(painter, r);
    {
        painter->save();
        // Reset the transform to ignore zoom/view
        painter->resetTransform();
        // Set the pen and font
        painter->setPen(background_text_color_);
        painter->setFont(background_font_);

        // Calculate the top_left corner of the text rect
        QPointF point;
        auto brect = rect();
        point.setX((brect.width() - background_text_rect_.width()) / 2);
        point.setY(brect.height() - background_text_rect_.height());
        painter->drawStaticText(point, background_text_);
        painter->restore();
    }
}

/**
 * @brief NodeView::resizeEvent
 * @param event
 */
void NodeView::resizeEvent(QResizeEvent* event)
{
    QGraphicsView::resizeEvent(event);
    update_minimap();
}
