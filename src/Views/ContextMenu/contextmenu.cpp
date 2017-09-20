#include "contextmenu.h"
#include "../../theme.h"

#include "../../ModelController/entityfactory.h"

#include <QDebug>
#include <QStyleFactory>
#include <QLabel>
#include <QWidgetAction>

#define MENU_ICON_SIZE 32

ContextMenu::ContextMenu(ViewController *vc){
    view_controller = vc;
    Theme::theme()->setIconAlias("EntityIcons", EntityFactory::getNodeKindString(NODE_KIND::NONE) , "Icons", "tiles");

    //Setup the complex relationship nodes
    connect_node_edge_kinds[NODE_KIND::BLACKBOX_INSTANCE] = EDGE_KIND::DEFINITION;
    connect_node_edge_kinds[NODE_KIND::COMPONENT_INSTANCE] = EDGE_KIND::DEFINITION;
    connect_node_edge_kinds[NODE_KIND::VECTOR_INSTANCE] = EDGE_KIND::DEFINITION;
    connect_node_edge_kinds[NODE_KIND::COMPONENT_IMPL] = EDGE_KIND::DEFINITION;
    connect_node_edge_kinds[NODE_KIND::AGGREGATE_INSTANCE] = EDGE_KIND::DEFINITION;
    connect_node_edge_kinds[NODE_KIND::OUTEVENTPORT_IMPL] = EDGE_KIND::DEFINITION;
    connect_node_edge_kinds[NODE_KIND::WORKER_PROCESS] = EDGE_KIND::DEFINITION;

    connect_node_edge_kinds[NODE_KIND::INEVENTPORT] = EDGE_KIND::AGGREGATE;
    connect_node_edge_kinds[NODE_KIND::OUTEVENTPORT] = EDGE_KIND::AGGREGATE;
    connect_node_edge_kinds[NODE_KIND::INEVENTPORT_DELEGATE] = EDGE_KIND::AGGREGATE;
    connect_node_edge_kinds[NODE_KIND::OUTEVENTPORT_DELEGATE] = EDGE_KIND::AGGREGATE;

    action_controller = view_controller->getActionController();

    setupMenus();
    themeChanged();

    
    connect(main_menu, &QMenu::aboutToShow, this, &ContextMenu::update_main_menu);
    connect(add_node_menu, &QMenu::aboutToShow, this, &ContextMenu::update_add_node_menu);
    connect(add_edge_menu, &QMenu::aboutToShow, this, &ContextMenu::update_add_edge_menu);
    connect(remove_edge_menu, &QMenu::aboutToShow, this, &ContextMenu::update_remove_edge_menu);

    
    connect(deploy_menu, &QMenu::aboutToShow, this, &ContextMenu::update_deploy_menu);
    
    connect(dock_deploy_menu, &QMenu::aboutToShow, this, &ContextMenu::update_deploy_menu);
    connect(dock_add_node_menu, &QMenu::aboutToShow, this, &ContextMenu::update_add_node_menu);

    connect(main_menu, &QMenu::aboutToHide, this, &ContextMenu::clear_hover);
    connect(dock_deploy_menu, &QMenu::aboutToHide, this, &ContextMenu::clear_hover);
    connect(dock_add_node_menu, &QMenu::aboutToHide, this, &ContextMenu::clear_hover);


    connect(Theme::theme(), &Theme::theme_Changed, this, &ContextMenu::themeChanged);
    

    connect(vc, &ViewController::vc_ActionFinished, this, &ContextMenu::invalidate_menus);
    connect(vc->getSelectionController(), &SelectionController::selectionChanged, this, &ContextMenu::invalidate_menus);

    connect(vc->getSelectionController(), &SelectionController::selectionChanged, this, &ContextMenu::update_add_node_menu);
    connect(vc->getSelectionController(), &SelectionController::selectionChanged, this, &ContextMenu::update_deploy_menu);

    connect(vc, &ViewController::vc_ActionFinished, this, &ContextMenu::update_add_node_menu);
    connect(vc, &ViewController::vc_ActionFinished, this, &ContextMenu::update_deploy_menu);
    

    update_deploy_menu();
    update_add_node_menu();
}


void ClearMenu(QMenu* menu){
    if(menu){
        for(auto action : menu->actions()){
            auto action_menu = action->menu();
    
            if(action_menu && action_menu->parent() == menu){
                //does the action represent a Menu, owned by this menu
                delete action_menu;
            }else if(action->parent() == menu){
                //Is the action attached to the menu
                delete action;
            }else{
                //otherwise just remove the action
                menu->removeAction(action);
            }
        }
    }
}

void ContextMenu::invalidate_menus(){
    valid_menus.clear();
}

QMenu* ContextMenu::getAddMenu(){
    return dock_add_node_menu;
}
QMenu* ContextMenu::getDeployMenu(){
    return dock_deploy_menu;
}

void ContextMenu::themeChanged(){
    auto theme = Theme::theme();

    main_menu->setStyleSheet(theme->getMenuStyleSheet(MENU_ICON_SIZE) + " QLabel{color:" + theme->getTextColorHex(Theme::CR_NORMAL) + ";}");// QMenu::item{padding: 4px 8px 4px " + QString::number(MENU_ICON_SIZE + 8)  + "px; }"

    //Set Icons for top level icons
    add_node_menu->setIcon(theme->getIcon("Icons", "plus"));
    add_edge_menu->setIcon(theme->getIcon("Icons", "connect"));
    remove_edge_menu->setIcon(theme->getIcon("Icons", "connectStriked"));
    deploy_menu->setIcon(theme->getIcon("Icons", "screen"));

    for(auto node_menu : add_node_action_hash.values()){
        auto node_kind = node_menu->property("node_kind").value<NODE_KIND>();
        auto node_kind_str = EntityFactory::getNodeKindString(node_kind);
        node_menu->setIcon(theme->getIcon("EntityIcons", node_kind_str));
    }

    for(auto edge_menu : add_edge_menu_hash.values()){
        auto edge_kind = edge_menu->property("edge_kind").value<EDGE_KIND>();
        auto edge_kind_str = EntityFactory::getEdgeKindString(edge_kind);
        edge_menu->setIcon(theme->getIcon("EntityIcons", edge_kind_str));
    }

    for(auto edge_menu : remove_edge_menu_hash.values()){
        auto edge_kind = edge_menu->property("edge_kind").value<EDGE_KIND>();
        auto edge_kind_str = EntityFactory::getEdgeKindString(edge_kind);
        edge_menu->setIcon(theme->getIcon("EntityIcons", edge_kind_str));
    }

    for(auto edge_menu : add_edge_menu_direct_hash.values()){
        auto edge_kind = edge_menu->property("edge_direction").value<EDGE_DIRECTION>();
        if(edge_kind == EDGE_DIRECTION::SOURCE){
            edge_menu->setIcon(theme->getIcon("Icons", "arrowLeft"));
        }else{
            edge_menu->setIcon(theme->getIcon("Icons", "arrowRight"));
        }
    }

    
}

void popup_menu(QMenu* menu, QPoint pos){
    if(menu){
        emit menu->aboutToShow();
        menu->popup(pos);
    }
}

void ContextMenu::popup_edge_menu(QPoint global_pos, EDGE_KIND edge_kind, EDGE_DIRECTION edge_direction){
    popup_menu(add_edge_menu_direct_hash.value({edge_direction, edge_kind}, 0), global_pos);
}

void ContextMenu::popup(QPoint global_pos, QPointF item_pos){
    popup_menu(main_menu, global_pos);
}

QMenu* ContextMenu::construct_menu(QString label, QMenu* parent_menu, int icon_size){
    if(icon_size <=0 ){
        icon_size = MENU_ICON_SIZE;
    }
    auto menu = parent_menu ? parent_menu->addMenu(label) : new QMenu(label);
    menu->setStyle(new CustomMenuStyle(icon_size));
    return menu;
}

void ContextMenu::clear_hover(){
    set_hovered_id(0);
}

void ContextMenu::set_hovered_id(int id){
    if(id != current_hovered_id){
        if(current_hovered_id > 0){
            emit view_controller->vc_highlightItem(current_hovered_id, false);
        }
        current_hovered_id = id;
        if(current_hovered_id > 0){
            emit view_controller->vc_highlightItem(current_hovered_id, true);
        }
    }
}

void ContextMenu::action_hovered(QAction* action){
    set_hovered_id(action->property("id").value<int>());
}

void ContextMenu::action_triggered(QAction* action){
    auto node_kind = action->property("node_kind").value<NODE_KIND>();
    auto edge_kind = action->property("edge_kind").value<EDGE_KIND>();
    auto action_kind = action->property("action_kind").value<ACTION_KIND>();
    auto id = action->property("id").value<int>();

    switch(action_kind){
        case ACTION_KIND::ADD_NODE:{
            auto item = view_controller->getSelectionController()->getActiveSelectedItem();
            if(item){
                auto parent_id = item->getID();
                //Expand!
                emit view_controller->vc_setData(parent_id, "isExpanded", true);
                emit view_controller->vc_constructNode(parent_id, node_kind);
            }
            break;
        }
        case ACTION_KIND::ADD_NODE_WITH_EDGE:{
            auto item = view_controller->getSelectionController()->getActiveSelectedItem();
            if(item){
                auto parent_id = item->getID();
                emit view_controller->vc_setData(parent_id, "isExpanded", true);
                emit view_controller->vc_constructConnectedNode(parent_id, node_kind, id, edge_kind);
            }
            break;
        }
        case ACTION_KIND::ADD_EDGE:{
            auto edge_direction = action->property("edge_direction").value<EDGE_DIRECTION>();

            auto selection = view_controller->getSelectionController()->getSelectionIDs();

            if(!selection.isEmpty()){
                auto id_list = {id};
                auto src_ids = edge_direction == EDGE_DIRECTION::SOURCE ? id_list : selection;
                auto dst_ids = edge_direction == EDGE_DIRECTION::TARGET ? id_list : selection;
                emit view_controller->vc_constructEdges(src_ids, dst_ids, edge_kind);
            }
            break;
        }
        case ACTION_KIND::REMOVE_EDGE:{
            auto remove_all = action->property("remove_all").value<bool>();
            auto selection = view_controller->getSelectionController()->getSelectionIDs();
            if(selection.size()){
                if(remove_all){
                    emit view_controller->vc_destructAllEdges(selection, edge_kind);
                }else{
                    emit view_controller->vc_destructEdges(selection, id, edge_kind);
                }
            }
            break;
        }
        default:
        break;
    }
}

void ContextMenu::populate_dynamic_add_edge_menu(QMenu* menu){
    if(menu && menu_requires_update(menu)){
        auto edge_kind = menu->property("edge_kind").value<EDGE_KIND>();
        //Use the clear function so we don't remove the sub menus, but so we remove

        menu->clear();

        auto src_menu = add_edge_menu_direct_hash.value({EDGE_DIRECTION::SOURCE, edge_kind}, 0);
        auto dst_menu = add_edge_menu_direct_hash.value({EDGE_DIRECTION::TARGET, edge_kind}, 0);

        auto item_map = view_controller->getValidEdges2(edge_kind);
        
        if(src_menu){
            ClearMenu(src_menu);
            auto valid_sources = item_map.values(EDGE_DIRECTION::SOURCE);
            construct_view_item_menus(src_menu, valid_sources);
            src_menu->menuAction()->setVisible(!valid_sources.isEmpty());
            menu->addAction(src_menu->menuAction());
        }

        if(dst_menu){
            ClearMenu(dst_menu);
            auto valid_targets = item_map.values(EDGE_DIRECTION::TARGET);
            construct_view_item_menus(dst_menu, valid_targets);
            dst_menu->menuAction()->setVisible(!valid_targets.isEmpty());
            menu->addAction(dst_menu->menuAction());
        }

        if(item_map.isEmpty()){
            get_no_valid_items_action(menu);
        }
        

        //Add menu into the set of validated menus to avoid recalculating unnecessarily
        menu_updated(menu);
    }
}

void ContextMenu::construct_view_item_menus(QMenu* menu, QList<ViewItem*> view_items, bool flatten_menu, QString empty_label){
    if(menu){
        //Get sorted lists of valid items and parents
        std::sort(view_items.begin(), view_items.end(), ViewItem::SortByKind);

        auto parent_items = view_controller->getViewItemParents(view_items);
        
        bool use_parent_menus = flatten_menu ? false : parent_items.size() > 1;
        
        //Keep a hash to keep track of the parent_menues
        QHash<int, QMenu*> parent_menus;
    
        //Construct the parent menus first
        for(auto view_item : parent_items){
            if(use_parent_menus){
                //By adding this menu to the parent_menu, we can clean up memory on clear
                auto parent_menu = construct_viewitem_menu(view_item, menu);
                parent_menus.insert(view_item->getID(), parent_menu);
            }
        }
    
        for(auto view_item : view_items){
            auto parent_view_item = view_item->getParentItem();
            auto parent_menu = menu;
            
            if(use_parent_menus && parent_view_item){
                parent_menu = parent_menus.value(parent_view_item->getID(), parent_menu);
            }
    
            //By adding the Action to the parent_menu, we can clean up memory on clear
            auto action = construct_viewitem_action(view_item, parent_menu);
    
            //Set the properties on the action
            action->setProperty("node_kind", menu->property("node_kind"));
            action->setProperty("edge_kind", menu->property("edge_kind"));
            action->setProperty("action_kind", menu->property("action_kind"));
            action->setProperty("edge_direction", menu->property("edge_direction"));
        }
    
        //Add an invalid item
        if(view_items.empty() && !empty_label.isEmpty()){
            get_no_valid_items_action(menu, empty_label);
        }
    }
}

void ContextMenu::populate_dynamic_remove_edge_menu(QMenu* menu){
    //Only update if we need to
    if(menu && menu_requires_update(menu)){
        auto edge_kind = menu->property("edge_kind").value<EDGE_KIND>();

        //Clear all the old items, This should delete the old menus
        ClearMenu(menu);

        //Get the list of existing edge points
        auto item_list = view_controller->getExistingEdgeEndPointsForSelection(edge_kind);

        //Update the menu
        construct_view_item_menus(menu, item_list);

        auto connected_count = item_list.count();
        
        if (connected_count > 1){
            //Construct a remove all buutton
            construct_remove_all_action(menu, connected_count);
        }

        //Add menu into the set of validated menus to avoid recalculating unnecessarily
        menu_updated(menu);
    }
}

QAction* ContextMenu::construct_remove_all_action(QMenu* menu, int number){
    auto remove_all = construct_base_action(menu, "Disconnect All (" + QString::number(number) + ")");
    remove_all->setIcon(Theme::theme()->getIcon("Icons", "bin"));
    remove_all->setProperty("node_kind", menu->property("node_kind"));
    remove_all->setProperty("edge_kind", menu->property("edge_kind"));
    remove_all->setProperty("action_kind", menu->property("action_kind"));
    remove_all->setProperty("remove_all", true);
    return remove_all;
}


void ContextMenu::populate_dynamic_add_node_menu(QMenu* menu){
    //Only update if we need to
    if(menu && menu_requires_update(menu)){
        //Get the node and edge kind of the menu
        auto node_kind = menu->property("node_kind").value<NODE_KIND>();
        auto edge_kind = menu->property("edge_kind").value<EDGE_KIND>();
    
        //Clear all the old items out of the menu (This should delete)
        ClearMenu(menu);
        
        //Construct the view_item menu hiararchy
        construct_view_item_menus(menu, view_controller->getConstructableNodeDefinitions(node_kind, edge_kind));

        //Add menu into the set of validated menus to avoid recalculating unnecessarily
        menu_updated(menu);
    }
}

QAction* ContextMenu::get_no_valid_items_action(QMenu* menu, QString label){
    auto action = construct_base_action(menu, label);
    action->setEnabled(false);
    action->setText(label);
    action->setIcon(Theme::theme()->getIcon("Icons", "circleInfoDark"));
    return action;
}

void ContextMenu::update_add_edge_menu(){
    if(menu_requires_update(add_edge_menu)){
        auto visible_edge_kinds = view_controller->getValidEdgeKindsForSelection().toSet();
        
        for(auto edge_menu : add_edge_menu_hash.values()){
            auto edge_kind = edge_menu->property("edge_kind").value<EDGE_KIND>();
            edge_menu->menuAction()->setVisible(visible_edge_kinds.contains(edge_kind));
        }

        //Add menu into the set of validated menus to avoid recalculating unnecessarily
        menu_updated(add_edge_menu);
    }
}

void ContextMenu::update_remove_edge_menu(){
    if(menu_requires_update(remove_edge_menu)){
        auto visible_edge_kinds = view_controller->getExistingEdgeKindsForSelection().toSet();
        
        for(auto edge_menu : remove_edge_menu_hash.values()){
            auto edge_kind = edge_menu->property("edge_kind").value<EDGE_KIND>();
            edge_menu->menuAction()->setVisible(visible_edge_kinds.contains(edge_kind));
        }

        //Add menu into the set of validated menus to avoid recalculating unnecessarily
        menu_updated(remove_edge_menu);
    }
}

void ContextMenu::update_main_menu(){
    if(menu_requires_update(main_menu)){
        bool can_add_node = view_controller->getAdoptableNodeKinds2().size();
        auto valid_edge_kinds = view_controller->getValidEdgeKindsForSelection();
        auto existant_edge_kinds = view_controller->getExistingEdgeKindsForSelection();
        bool can_add_edge = false;
        bool can_remove_edge = false;
        
        //Check if we have an Add Edge item for any of the valid edge kinds
        for(auto edge_kind : existant_edge_kinds){
            if(remove_edge_menu_hash.contains(edge_kind)){
                can_remove_edge = true;
                break;
            }
        }

        //Check if we have an Add Edge item for any of the valid edge kinds
        for(auto edge_kind : valid_edge_kinds){
            if(add_edge_menu_hash.contains(edge_kind)){
                can_add_edge = true;
                break;
            }
        }

        //Show and hide the menu actions for the sub menus
        add_node_menu->menuAction()->setVisible(can_add_node);
        add_edge_menu->menuAction()->setVisible(can_add_edge);
        remove_edge_menu->menuAction()->setVisible(can_remove_edge);
        deploy_menu->menuAction()->setVisible(valid_edge_kinds.contains(EDGE_KIND::DEPLOYMENT));
        
        //Add menu into the set of validated menus to avoid recalculating unnecessarily
        menu_updated(main_menu);
    }
}
bool ContextMenu::menu_requires_update(QMenu* menu){
    return !valid_menus.contains(menu);
}
void ContextMenu::menu_updated(QMenu* menu){
    valid_menus.insert(menu);
}

void ContextMenu::update_add_node_menu(){
    if(menu_requires_update(add_node_menu)){
        auto visible_node_kinds = view_controller->getAdoptableNodeKinds2().toSet();

        //Show the NONE status if we don't have anything to adopt
        if(visible_node_kinds.empty()){
            visible_node_kinds.insert(NODE_KIND::NONE);
        }

        //Get all the actions for the node kinds. Some of these actions are menu actions themselves
        for(auto node_action : add_node_action_hash.values()){
            auto node_kind = node_action->property("node_kind").value<NODE_KIND>();
            node_action->setVisible(visible_node_kinds.contains(node_kind));
        }

        //Place menu into the set of valid menus to avoid recalculating
        menu_updated(add_node_menu);
    }
}

void updateAction(QAction* action, ViewItem* item){
    if(action && item){
        action->setText(item->getData("label").toString());
        action->setIcon(Theme::theme()->getIcon(item->getIcon()));
    }
}

QAction* ContextMenu::construct_base_action(QMenu* menu, QString label){
    auto action = menu ? menu->addAction(label) : new QAction(label, this);
    connect(action, &QAction::hovered, this, [=](){action_hovered(action);});
    connect(action, &QAction::triggered, [=](){action_triggered(action);});
    return action;
}

QAction* ContextMenu::construct_viewitem_action(ViewItem* item, QMenu* menu){
    if(item){
        auto action = construct_base_action(menu, 0);

        action->setProperty("id", item->getID());
        if(item->isNode()){
            auto node_item = (NodeViewItem*)item;
            action->setProperty("node_kind", QVariant::fromValue(node_item->getNodeKind()));
        }else if(item->isEdge()){
            auto edge_item = (EdgeViewItem*)item;
            action->setProperty("edge_kind", QVariant::fromValue(edge_item->getEdgeKind()));
        }

        //connect(action, &QAction::hovered, [=](){action_hovered(action);});
        
        updateAction(action, item);
        return action;
    }
    return 0;
}

QAction* ContextMenu::get_deploy_viewitem_action(ACTION_KIND kind, ViewItem* item){
    if(item){
        auto id = item->getID();
        auto action = deploy_view_item_hash.value({id, kind}, 0);
        if(!action){
            action = construct_viewitem_action(item);
            action->setProperty("action_kind", QVariant::fromValue(kind));
            //Insert into the action
            deploy_view_item_hash[{id, kind}] = action;
        }
        //Run an update
        updateAction(action, item);
        return action;
    }
    return 0;
}

QMenu* ContextMenu::construct_viewitem_menu(ViewItem* item, QMenu* parent_menu){
    if(item){
        auto id = item->getID();
        auto menu = construct_menu("", parent_menu);
        
        auto menu_action = menu->menuAction();
        menu->setProperty("id", id);
        menu_action->setProperty("id", id);
        if(item->isNode()){
            auto node_item = (NodeViewItem*)item;
            menu->setProperty("node_kind", QVariant::fromValue(node_item->getNodeKind()));
            menu_action->setProperty("node_kind", menu->property("node_kind"));
        }else if(item->isEdge()){
            auto edge_item = (EdgeViewItem*)item;
            menu->setProperty("edge_kind", QVariant::fromValue(edge_item->getEdgeKind()));
            menu_action->setProperty("edge_kind", menu->property("edge_kind"));
        }


        updateAction(menu_action, item);
        return menu;
    }
    return 0;
}

void ContextMenu::update_deploy_menu(){
    if(deploy_menu && dock_deploy_menu && menu_requires_update(deploy_menu)){
        auto edge_kind = EDGE_KIND::DEPLOYMENT;
        
        auto menu_labels = deploy_labels[deploy_menu];
        auto dock_labels = deploy_labels[dock_deploy_menu];
        
        deploy_menu->clear();
        dock_deploy_menu->clear();

         //Get the list of existing edge points
        auto item_list = view_controller->getExistingEdgeEndPointsForSelection(edge_kind);


        auto connected_count = item_list.count();

        //Show the current deployed label if we have any
        if(connected_count >= 0){
            deploy_menu->addAction(menu_labels->deployed_nodes_action);
            dock_deploy_menu->addAction(dock_labels->deployed_nodes_action);
        }

        //Set these properties temporarily so that the sub actions will have the correct properties
        deploy_menu->setProperty("action_kind", QVariant::fromValue(ACTION_KIND::REMOVE_EDGE));
        deploy_menu->setProperty("edge_direction", QVariant::fromValue(EDGE_DIRECTION::TARGET));
        deploy_menu->setProperty("edge_kind", QVariant::fromValue(edge_kind));
        dock_deploy_menu->setProperty("action_kind", deploy_menu->property("action_kind"));
        dock_deploy_menu->setProperty("edge_kind", deploy_menu->property("edge_kind"));
        dock_deploy_menu->setProperty("edge_direction", deploy_menu->property("edge_direction"));

        //Update the menus, using flattened menus
        construct_view_item_menus(deploy_menu, item_list, true, "Not currently deployed");
        construct_view_item_menus(dock_deploy_menu, item_list, true, "Not currently deployed");

        if (connected_count > 1){
            //Construct a remove all button in each menu
            construct_remove_all_action(deploy_menu, connected_count);
            construct_remove_all_action(dock_deploy_menu, connected_count);
        }
        
        
        //Get the deployable nodes
        auto item_map = view_controller->getValidEdges2(edge_kind);

        //Set these properties temporarily so that the sub actions will have the correct properties
        deploy_menu->setProperty("action_kind", QVariant::fromValue(ACTION_KIND::ADD_EDGE));
        dock_deploy_menu->setProperty("action_kind", deploy_menu->property("action_kind"));
        

        QHash<int, QMenu*> sub_menus;
        
        //Show the current deployed label if we have any
        //Get only the Hardware end of the deployment edges
        auto deployable_items = item_map.values(EDGE_DIRECTION::TARGET);
        auto deployable_count = deployable_items.count();
        
        if(deployable_count >= 0){
            deploy_menu->addAction(menu_labels->available_nodes_action);
            dock_deploy_menu->addAction(dock_labels->available_nodes_action);
        }

        //Update the menus, using flattened menus
        construct_view_item_menus(deploy_menu, deployable_items, false, "No available nodes");
        construct_view_item_menus(dock_deploy_menu, deployable_items, false, "No available nodes");

        //Place menu into the set of valid menus to avoid recalculating
        menu_updated(deploy_menu);
    }
}

QWidgetAction* construct_menu_label(QString label){
    auto action = new QWidgetAction(0); 
    auto label_widget = new QLabel(label);
    label_widget->setAlignment(Qt::AlignLeft);
    label_widget->setContentsMargins(4,8,4,8);
    label_widget->setAlignment(Qt::AlignLeft);
    label_widget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    action->setDefaultWidget(label_widget);
    return action;
}


void ContextMenu::setupMenus(){
    main_menu = construct_menu("", 0);

    deploy_menu = construct_menu("Deploy", main_menu);
    
    add_edge_menu = construct_menu("Connect", main_menu);
    remove_edge_menu = construct_menu("Disconnect", main_menu);
    
    //Add a Label to thte Add Node Menu
    add_node_menu = construct_menu("Add Node", main_menu);
    add_node_menu->addAction(construct_menu_label("Available Entities"));
    
    //Setup the menus that will be going in the dock
    dock_add_node_menu = construct_menu("Parts", 0, 32);
    dock_deploy_menu = construct_menu("Deploy", 0, 32);
    dock_add_node_menu->addAction(construct_menu_label("Available Entities"));

    {
        auto dock_deploy_labels = new DeployLabels;
        dock_deploy_labels->deployed_nodes_action = construct_menu_label("Deployed Entities");
        dock_deploy_labels->available_nodes_action = construct_menu_label("Available Nodes");
        
        deploy_labels[dock_deploy_menu] = dock_deploy_labels;
    }

    {
        auto dock_deploy_labels = new DeployLabels;
        dock_deploy_labels->deployed_nodes_action = construct_menu_label("Deployed Entities");
        dock_deploy_labels->available_nodes_action = construct_menu_label("Available Nodes");
        deploy_labels[deploy_menu] = dock_deploy_labels;
    }
    
    

    /*auto action = get_no_valid_items_action(0);
    action->setEnabled(false);
    add_node_action_hash[node_kind] = action;*/
    
    //Get the list of adoptable node kind items
    for(auto node_view_item : view_controller->getNodeKindItems()){
        auto node_kind = node_view_item->getNodeKind();
        auto edge_kind = connect_node_edge_kinds.value(node_kind, EDGE_KIND::NONE);

        if(edge_kind == EDGE_KIND::NONE){
            //Make an action for the Items which don't need a submenu
            auto action = construct_viewitem_action(node_view_item);
            action->setProperty("action_kind", QVariant::fromValue(ACTION_KIND::ADD_NODE));
            add_node_action_hash[node_kind] = action;

            //Add to both the Dock and the add_node_menus
            add_node_menu->addAction(action);
            dock_add_node_menu->addAction(action);
        }else{
            //Make a menu for the Nodes which need to be constructed with an edge
            auto menu = construct_viewitem_menu(node_view_item, add_node_menu);
            menu->setProperty("edge_kind", QVariant::fromValue(edge_kind));
            menu->setProperty("action_kind", QVariant::fromValue(ACTION_KIND::ADD_NODE_WITH_EDGE));
            
            add_node_menu_hash[node_kind] = menu;
            auto menu_action = menu->menuAction();

            //Add the action to open the menu to both the dock and the add_node menus
            //add_node_menu->addAction(menu_action);
            dock_add_node_menu->addAction(menu_action);

            add_node_action_hash[node_kind] = menu_action;

            //We need to populate the menu dynamically when its about to show
            connect(menu, &QMenu::aboutToShow, [=](){populate_dynamic_add_node_menu(menu);});
        }
    }
    auto empty_action = get_no_valid_items_action(0, "No Adoptable Entities");
    empty_action->setProperty("node_kind", QVariant::fromValue(NODE_KIND::NONE));
    add_node_action_hash[NODE_KIND::NONE] = empty_action;
    add_node_menu->addAction(empty_action);
    dock_add_node_menu->addAction(empty_action);

    //Get the list of constructable edge kind items
    auto edge_kinds = view_controller->getEdgeKindItems();
    
    for(auto edge_view_item : edge_kinds){
        auto edge_kind = edge_view_item->getEdgeKind();
        
        //Ignore Deployment Edges, they get dealt with lower
        if(edge_kind == EDGE_KIND::DEPLOYMENT){
            continue;
        }
        
        //Construct an Add Edge Kind Menu
        auto add_edge_kind_menu = construct_viewitem_menu(edge_view_item, add_edge_menu);
        add_edge_kind_menu->setProperty("action_kind", QVariant::fromValue(ACTION_KIND::ADD_EDGE));

        //Construct an Remove Edge Kind Menu
        auto remove_edge_kind_menu = construct_viewitem_menu(edge_view_item, remove_edge_menu);
        remove_edge_kind_menu->setProperty("action_kind", QVariant::fromValue(ACTION_KIND::REMOVE_EDGE));
        
        //Construct an Add Edge Kind From Menu
        auto add_edge_kind_src_menu = construct_menu("From", add_edge_kind_menu);
        add_edge_kind_src_menu->setProperty("edge_kind", add_edge_kind_menu->property("edge_kind"));
        add_edge_kind_src_menu->setProperty("action_kind", add_edge_kind_menu->property("action_kind"));
        add_edge_kind_src_menu->setProperty("edge_direction", QVariant::fromValue(EDGE_DIRECTION::SOURCE));

        //Construct an Add Edge Kind To Menu
        auto add_edge_kind_dst_menu = construct_menu("To", add_edge_kind_menu);
        add_edge_kind_dst_menu->setProperty("edge_kind", add_edge_kind_menu->property("edge_kind"));
        add_edge_kind_dst_menu->setProperty("action_kind", add_edge_kind_menu->property("action_kind"));
        add_edge_kind_dst_menu->setProperty("edge_direction", QVariant::fromValue(EDGE_DIRECTION::TARGET));

        //Insert these menus into the hash
        add_edge_menu_hash[edge_kind] = add_edge_kind_menu;
        remove_edge_menu_hash[edge_kind] = remove_edge_kind_menu;
        add_edge_menu_direct_hash[{EDGE_DIRECTION::SOURCE, edge_kind}] = add_edge_kind_src_menu;
        add_edge_menu_direct_hash[{EDGE_DIRECTION::TARGET, edge_kind}] = add_edge_kind_dst_menu;

        //Setup the edge menu
        add_edge_kind_menu->addMenu(add_edge_kind_src_menu);
        add_edge_kind_menu->addMenu(add_edge_kind_dst_menu);
        add_edge_menu->addMenu(add_edge_kind_menu);
        remove_edge_menu->addMenu(remove_edge_kind_menu);

        //Setup the set
        connect(add_edge_kind_src_menu, &QMenu::aboutToShow, [=](){populate_dynamic_add_edge_menu(add_edge_kind_menu);});
        connect(add_edge_kind_dst_menu, &QMenu::aboutToShow, [=](){populate_dynamic_add_edge_menu(add_edge_kind_menu);});

        connect(add_edge_kind_menu, &QMenu::aboutToShow, [=](){populate_dynamic_add_edge_menu(add_edge_kind_menu);});
        connect(remove_edge_kind_menu, &QMenu::aboutToShow, [=](){populate_dynamic_remove_edge_menu(remove_edge_kind_menu);});
    }

    //Put our custom deploy menu into the edge hash
    add_edge_menu_direct_hash[{EDGE_DIRECTION::SOURCE, EDGE_KIND::DEPLOYMENT}] = deploy_menu;

    //Setup the main_menu
    main_menu->addMenu(add_node_menu);
    main_menu->addAction(action_controller->edit_delete->constructSubAction(true));
    main_menu->addSeparator();

    main_menu->addMenu(deploy_menu);
    main_menu->addMenu(add_edge_menu);
    main_menu->addMenu(remove_edge_menu);
    main_menu->addSeparator();
    main_menu->addAction(action_controller->view_viewConnections->constructSubAction(true));
    main_menu->addAction(action_controller->model_getCodeForComponent->constructSubAction(true));
    main_menu->addSeparator();
    main_menu->addAction(action_controller->toolbar_replicateCount->constructSubAction(true));
    main_menu->addAction(action_controller->view_viewInNewWindow->constructSubAction(true));
    main_menu->addAction(action_controller->toolbar_wiki->constructSubAction(true));
}
