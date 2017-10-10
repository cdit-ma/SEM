#include "contextmenu.h"
#include "../../theme.h"

#include "../../ModelController/entityfactory.h"
#include "../../Widgets/filterwidget.h"

#include <QDebug>
#include <QStyleFactory>
#include <QLabel>
#include <QLineEdit>
#include <QWidgetAction>

#define MENU_ICON_SIZE 32

ContextMenu::ContextMenu(ViewController *vc){
    view_controller = vc;
    Theme::theme()->setIconAlias("EntityIcons", EntityFactory::getNodeKindString(NODE_KIND::NONE) , "Icons", "circleInfoDark");

    //Setup the complex relationship nodes
    connect_node_edge_kinds[NODE_KIND::BLACKBOX_INSTANCE] = EDGE_KIND::DEFINITION;
    connect_node_edge_kinds[NODE_KIND::COMPONENT_INSTANCE] = EDGE_KIND::DEFINITION;
    connect_node_edge_kinds[NODE_KIND::VECTOR_INSTANCE] = EDGE_KIND::DEFINITION;
    connect_node_edge_kinds[NODE_KIND::COMPONENT_IMPL] = EDGE_KIND::DEFINITION;
    connect_node_edge_kinds[NODE_KIND::AGGREGATE_INSTANCE] = EDGE_KIND::DEFINITION;
    connect_node_edge_kinds[NODE_KIND::OUTEVENTPORT_IMPL] = EDGE_KIND::DEFINITION;
    connect_node_edge_kinds[NODE_KIND::WORKER_PROCESS] = EDGE_KIND::DEFINITION;
    connect_node_edge_kinds[NODE_KIND::ENUM_INSTANCE] = EDGE_KIND::DEFINITION;

    connect_node_edge_kinds[NODE_KIND::INEVENTPORT] = EDGE_KIND::AGGREGATE;
    connect_node_edge_kinds[NODE_KIND::OUTEVENTPORT] = EDGE_KIND::AGGREGATE;
    connect_node_edge_kinds[NODE_KIND::INEVENTPORT_DELEGATE] = EDGE_KIND::AGGREGATE;
    connect_node_edge_kinds[NODE_KIND::OUTEVENTPORT_DELEGATE] = EDGE_KIND::AGGREGATE;

    action_controller = view_controller->getActionController();

    setupMenus();
    themeChanged();

    connect(Theme::theme(), &Theme::theme_Changed, this, &ContextMenu::themeChanged);
    

    connect(vc, &ViewController::vc_ActionFinished, this, &ContextMenu::invalidate_menus);
    connect(vc, &ViewController::vc_ActionFinished, this, &ContextMenu::update_dock_menus);

    connect(vc->getSelectionController(), &SelectionController::selectionChanged, this, &ContextMenu::invalidate_menus);
    connect(vc->getSelectionController(), &SelectionController::selectionChanged, this, &ContextMenu::update_dock_menus);

    

    update_deploy_menu();
    update_add_node_menu();
}


void ClearMenu(QMenu* menu){
    if(menu){
        for(auto action : menu->actions()){
            auto action_menu = action->menu();

            //Don't remove SearchBox
            if(action->property("SearchBox").toBool()){
                continue;
            }
    
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
    for(auto menu : valid_menus){
        menu->setProperty("load_count", 10);
    }
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

    auto icon_size = theme->getLargeIconSize();
    auto menu_style = new CustomMenuStyle(icon_size.width());

    main_menu->setStyle(menu_style);
    main_menu->setStyleSheet(theme->getMenuStyleSheet(icon_size.width()) + " QLabel{color:" + theme->getTextColorHex(Theme::CR_DISABLED) + ";}");

    //Set Icons for top level icons
    add_node_menu->setIcon(theme->getIcon("Icons", "plus"));
    add_edge_menu->setIcon(theme->getIcon("Icons", "connect"));
    remove_edge_menu->setIcon(theme->getIcon("Icons", "connectStriked"));
    deploy_menu->setIcon(theme->getIcon("Icons", "screen"));

    for(auto node_action : add_node_action_hash.values()){
        Theme::UpdateActionIcon(node_action, theme);
        auto menu = node_action->menu();
        if(menu){
            menu->setStyle(menu_style);
            menu->setStyleSheet(main_menu->styleSheet());
        }
    }

    for(auto menu : add_edge_menu_hash.values()){
        auto action = menu->menuAction();
        Theme::UpdateActionIcon(action, theme);
        if(menu){
            menu->setStyle(menu_style);
            menu->setStyleSheet(main_menu->styleSheet());
        }
    }

    for(auto menu : remove_edge_menu_hash.values()){
        auto action = menu->menuAction();
        Theme::UpdateActionIcon(action, theme);
        if(menu){
            menu->setStyle(menu_style);
            menu->setStyleSheet(main_menu->styleSheet());
        }
    }

    for(auto edge_menu : add_edge_menu_direct_hash.values()){
        auto edge_kind = edge_menu->property("edge_direction").value<EDGE_DIRECTION>();
        if(edge_kind == EDGE_DIRECTION::SOURCE){
            edge_menu->setIcon(theme->getIcon("Icons", "arrowLeft"));
        }else{
            edge_menu->setIcon(theme->getIcon("Icons", "arrowRight"));
        }
        if(edge_menu){
            edge_menu->setStyle(menu_style);
            edge_menu->setStyleSheet(main_menu->styleSheet());
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
    auto menu = parent_menu ? parent_menu->addMenu(label) : new QMenu(label);
    

    connect(menu, &QMenu::aboutToShow, [=](){update_menu(menu);menu_focussed(menu);});
    connect(menu, &QMenu::aboutToHide, this, &ContextMenu::clear_hover);
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

void ContextMenu::update_dock_menus(){
    for(auto menu : {dock_add_node_menu, dock_deploy_menu}){
        update_menu(menu);
    }
}

void ContextMenu::update_menu(QMenu* menu){
    if(menu == main_menu){
        return update_main_menu();
    }else if(menu == add_edge_menu){
        return update_add_edge_menu();
    }else if(menu == remove_edge_menu){
        return update_remove_edge_menu();
    }else if(menu == add_node_menu || menu == dock_add_node_menu){
        return update_add_node_menu();
    }else if(menu == deploy_menu || menu == dock_deploy_menu){
        return update_deploy_menu();
    }else{
        if(menu){
            auto action_kind = menu->property("action_kind").value<ACTION_KIND>();
            auto edge_kind = menu->property("edge_kind").value<EDGE_KIND>();
            switch(action_kind){
                case ACTION_KIND::ADD_NODE_WITH_EDGE:
                    return populate_dynamic_add_node_menu(menu);
                case ACTION_KIND::ADD_EDGE:{
                    auto edge_menu = add_edge_menu_hash.value(edge_kind, 0);
                    if(edge_menu){
                        return populate_dynamic_add_edge_menu(edge_menu);
                    }
                    break;
                }
                case ACTION_KIND::REMOVE_EDGE:{
                    auto edge_menu = remove_edge_menu_hash.value(edge_kind, 0);
                    if(edge_menu){
                        return populate_dynamic_remove_edge_menu(edge_menu);
                    }
                    break;
                }
                default:
                    break;
            }
        }
    }
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

            view_controller->constructEdges(id, edge_kind, edge_direction);
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
            src_menu->setProperty("filter", menu->property("filter"));
            ClearMenu(src_menu);
            
            auto valid_sources = item_map.values(EDGE_DIRECTION::SOURCE);
            construct_view_item_menus(src_menu, valid_sources);
            src_menu->menuAction()->setVisible(!valid_sources.isEmpty());
            menu->addAction(src_menu->menuAction());
        }

        if(dst_menu){
            dst_menu->setProperty("filter", menu->property("filter"));
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
        menu->setUpdatesEnabled(false);
        auto filter_str = menu->property("filter").toString();
        auto load_all = menu->property("load_all").toBool();
        auto load_count = menu->property("load_count").toInt();
        if(load_count == 0){
            load_count = 10;
        }
        
        
        //Filter the list of the view items 
        auto filtered_view_items = view_controller->filterList(filter_str, view_items);

        //Sort the filtered list
        std::sort(filtered_view_items.begin(), filtered_view_items.end(), ViewItem::SortByKind);

        //Get the unique parent_items
        auto parent_items = view_controller->getViewItemParents(filtered_view_items);

        //We should use a parent menu hiarachy, if we have more than 1 parent, and the use doesn't want flattened menus
        bool use_parent_menus = flatten_menu ? false : parent_items.size() > 1;
        if(!use_parent_menus){
            parent_items.clear();
        }else{
            //If we are using parent_menus load all of the items
            load_count = view_items.count();
        }

        //Keep a hash to keep track of the parent_menus
        QHash<int, QMenu*> parent_menus;
        
        //Construct the parent menus first
        for(auto view_item : parent_items){
            //By adding this menu to the parent_menu, memory will be cleared when we clear
            auto parent_menu = construct_viewitem_menu(view_item, menu);
            parent_menus.insert(view_item->getID(), parent_menu);
        }

        auto visible_count = 0;
        for(auto view_item : filtered_view_items){
            if(visible_count ++ >= load_count){
                break;
            }
            auto id = view_item->getID();
            auto parent_item = view_item->getParentItem();
            auto parent_menu = menu;
            
            if(parent_item){
                //If we have a parent item, try check for a menu
                parent_menu = parent_menus.value(parent_item->getID(), parent_menu);
            }
            
            auto action = construct_viewitem_action(view_item, parent_menu);
            action->setProperty("node_kind", menu->property("node_kind"));
            action->setProperty("edge_kind", menu->property("edge_kind"));
            action->setProperty("action_kind", menu->property("action_kind"));
            action->setProperty("edge_direction", menu->property("edge_direction"));
        }
        auto all_loaded = filtered_view_items.size() == visible_count;
        auto hidden_count = view_items.size() - filtered_view_items.size();

        if(!all_loaded){
            auto action = construct_base_action(menu, "Load more");
            //Disconnect the usual connect
            disconnect(action, &QAction::triggered, 0 , 0);
            action->setIcon(Theme::theme()->getIcon("Icons", "refresh"));
            //When the loadmore action is triggered, increment the load_count, 
            connect(action, &QAction::triggered, this, [=](){load_more_actions(menu);});
        }
        
        if(hidden_count > 0){
            get_no_valid_items_action(menu, QString::number(hidden_count) + " Entities filtered");
        }

        //Add an invalid item
        if(view_items.empty() && !empty_label.isEmpty()){
            get_no_valid_items_action(menu, empty_label);
        }
        menu->setUpdatesEnabled(true);
    }
}

void ContextMenu::load_more_actions(QMenu* menu){
    if(menu){
        auto load_count = menu->property("load_count").toInt();
        if(load_count == 0){
            load_count += 10;
        }
        load_count += 10;
        menu->setProperty("load_count", load_count);
        //Remove the 
        valid_menus.remove(menu);
        //Update the menu
        emit menu->aboutToShow();
        //Show the menu
        menu->show();
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
        //Focus the search
        auto items = view_controller->getConstructableNodeDefinitions(node_kind, edge_kind);
        construct_view_item_menus(menu, items);
        
        //Add menu into the set of validated menus to avoid recalculating unnecessarily
        menu_updated(menu);
    }
}

QAction* ContextMenu::get_no_valid_items_action(QMenu* menu, QString label){
    auto action = construct_base_action(menu, label);
    action->setEnabled(false);
    action->setText(label);
    Theme::StoreActionIcon(action, "Icons", "circleInfoDark");
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
        bool can_add_node = view_controller->getAdoptableNodeKinds().size();
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

void ContextMenu::menu_focussed(QMenu* menu){
    auto widget_action = search_actions_.value(menu, 0);
    if(widget_action){
        widget_action->defaultWidget()->setFocus();
    }
}




void ContextMenu::update_add_node_menu(){
    auto menus = {add_node_menu, dock_add_node_menu};

    for(auto menu : menus){
        if(menu_requires_update(menu)){
            ClearMenu(menu);

            auto filter_str = menu->property("filter").toString();
            auto adoptable_node_kinds = view_controller->getAdoptableNodeKinds();
            auto node_kinds = add_node_action_hash.uniqueKeys().toSet();
            //Get rid of the kind we don't have a menu item for
            adoptable_node_kinds.intersect(node_kinds);
            
            auto label_action = add_labels.value(menu, 0);
            //Construct a set to store whats filtered.
            QList<NODE_KIND> filtered_node_kinds;

            auto filtered_view_items = view_controller->filterList(filter_str, view_controller->getNodeKindItems());
            std::sort(filtered_view_items.begin(), filtered_view_items.end(), ViewItem::SortByKind);
            for(auto view_item : filtered_view_items){
                auto node_item = (NodeViewItem*) view_item;
                filtered_node_kinds.append(node_item->getNodeKind());
            }

            if(adoptable_node_kinds.isEmpty()){
                adoptable_node_kinds.insert(NODE_KIND::NONE);
                filtered_node_kinds.append(NODE_KIND::NONE);
            }

            auto visible_node_kind_count = 0;
            for(auto node_kind : filtered_node_kinds){
                if(adoptable_node_kinds.contains(node_kind)){
                    auto action = add_node_action_hash.value(node_kind, 0);
                    if(action){
                        menu->addAction(action);
                    }
                    visible_node_kind_count++;
                }
            }
            
            auto hidden_count = adoptable_node_kinds.size() - visible_node_kind_count;

            if(hidden_count > 0){
                get_no_valid_items_action(menu, QString::number(hidden_count) + " Entities filtered");
            }

            //Place menu into the set of valid menus to avoid recalculating
            menu_updated(menu);
        }
    }
}

void updateAction(QAction* action, ViewItem* item){
    if(action && item){
        action->setText(item->getData("label").toString());
        Theme::StoreActionIcon(action, item->getIcon());
    }
}

QAction* ContextMenu::construct_base_action(QMenu* menu, QString label){
    auto action = menu ? menu->addAction(label) : new QAction(label, this);
    connect(action, &QAction::hovered, this, [=](){action_hovered(action);});
    connect(action, &QAction::triggered, [=](){action_triggered(action);});
    return action;
}

void ContextMenu::clear_menu_cache(QMenu* menu){
    if(menu){
        menu->setProperty("load_count", 10);
    }
}

QAction* ContextMenu::construct_viewitem_action(ViewItem* item, QMenu* menu){
    if(item){
        auto id = item->getID();
        auto action = construct_base_action(menu, "");
        action->setProperty("id", id);
        if(item->isNode()){
            auto node_item = (NodeViewItem*)item;
            action->setProperty("node_kind", QVariant::fromValue(node_item->getNodeKind()));
        }else if(item->isEdge()){
            auto edge_item = (EdgeViewItem*)item;
            action->setProperty("edge_kind", QVariant::fromValue(edge_item->getEdgeKind()));
        }
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
        connect(menu_action, &QAction::hovered, this, [=](){action_hovered(menu_action);});

        updateAction(menu_action, item);
        return menu;
    }
    return 0;
}

void ContextMenu::update_deploy_menu(){
    if(deploy_menu && dock_deploy_menu){
        auto edge_kind = EDGE_KIND::DEPLOYMENT;
        auto edge_direction = EDGE_DIRECTION::TARGET;
        //Get the list of existing edge points
        auto item_list = view_controller->getExistingEdgeEndPointsForSelection(edge_kind);
        //Get the deployable nodes
        auto item_map = view_controller->getValidEdges2(edge_kind);
        auto deployable_items = item_map.values(edge_direction);
        auto menus = {dock_deploy_menu, deploy_menu};

        for(auto menu : menus){
            menu->setProperty("load_all", true);
            if(menu_requires_update(menu)){
                auto labels = deploy_labels[menu];
                ClearMenu(menu);

                //Get the list of existing edge points
                auto item_list = view_controller->getExistingEdgeEndPointsForSelection(edge_kind);

                auto connected_count = item_list.count();

                //Show the current deployed label if we have any
                if(connected_count >= 0){
                    menu->addAction(labels->deployed_nodes_action);
                }

                //Set these properties temporarily so that the sub actions will have the correct properties
                menu->setProperty("action_kind", QVariant::fromValue(ACTION_KIND::REMOVE_EDGE));
                
                menu->setProperty("edge_kind", QVariant::fromValue(edge_kind));

                //Update the menus, using flattened menus
                construct_view_item_menus(menu, item_list, true, "Not currently deployed");

                if (connected_count > 1){
                    //Construct a remove all button in each menu
                    construct_remove_all_action(menu, connected_count);
                }

                //Set these properties temporarily so that the sub actions will have the correct properties
                menu->setProperty("action_kind", QVariant::fromValue(ACTION_KIND::ADD_EDGE));

                


                //Show the current deployed label if we have any
                //Get only the Hardware end of the deployment edges
                
                auto deployable_count = item_map.count();
            
                if(deployable_count > 0){
                    menu->addAction(labels->available_nodes_action);
                }

                for(auto key : item_map.uniqueKeys()){
                    menu->setProperty("edge_direction", QVariant::fromValue(key));
                    //Update the menus, using flattened menus
                    construct_view_item_menus(menu, item_map.values(key), false, "");
                }
                if(deployable_count > 0 && item_map.isEmpty()){
                    get_no_valid_items_action(menu, "No available entities");
                }

                //Place menu into the set of valid menus to avoid recalculating
                menu_updated(menu);
            }
        }
    }
}

QWidgetAction* construct_menu_label(QString label){
    auto action = new QWidgetAction(0); 
    auto label_widget = new QLabel(label);
    label_widget->setContentsMargins(4,8,4,8);
    label_widget->setAlignment(Qt::AlignCenter);
    label_widget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    action->setDefaultWidget(label_widget);
    
    return action;
}

QWidgetAction* ContextMenu::construct_menu_search(QMenu* parent){
    auto action = new QWidgetAction(0);
    action->setProperty("SearchBox", true);
    auto filter_widget = new FilterWidget();
    
    
    connect(filter_widget, &FilterWidget::filterChanged, [=](const QString & text){
        parent->setProperty("filter", text);
        parent->setProperty("load_count", 0);
        valid_menus.remove(parent);
        update_menu(parent);
    });
   
    action->setDefaultWidget(filter_widget);

    search_actions_[parent] = action;

    return action;
}


void ContextMenu::setupMenus(){
    main_menu = construct_menu("", 0);

    deploy_menu = construct_menu("Deploy", main_menu);
    
    add_edge_menu = construct_menu("Connect", main_menu);
    remove_edge_menu = construct_menu("Disconnect", main_menu);
    
    //Add a Label to thte Add Node Menu
    add_node_menu = construct_menu("Add Node", main_menu);
    add_labels[add_node_menu] = construct_menu_label("Available Entities");
    
    
    //Setup the menus that will be going in the dock
    dock_add_node_menu = construct_menu("Parts", 0, 32);
    dock_deploy_menu = construct_menu("Deploy", 0, 32);

    add_labels[dock_add_node_menu] = construct_menu_label("Available Entities");


    
    dock_add_node_menu->addAction(construct_menu_search(dock_add_node_menu));
    add_node_menu->addAction(construct_menu_search(add_node_menu));
    
    

    dock_deploy_menu->addAction(construct_menu_search(dock_deploy_menu));
    deploy_menu->addAction(construct_menu_search(deploy_menu));

    {
        auto dock_deploy_labels = new DeployLabels;
        dock_deploy_labels->deployed_nodes_action = construct_menu_label("Deployed Entities");
        dock_deploy_labels->available_nodes_action = construct_menu_label("Available Entities");
        
        deploy_labels[dock_deploy_menu] = dock_deploy_labels;
    }

    {
        auto dock_deploy_labels = new DeployLabels;
        dock_deploy_labels->deployed_nodes_action = construct_menu_label("Deployed Entities");
        dock_deploy_labels->available_nodes_action = construct_menu_label("Available Entities");
        deploy_labels[deploy_menu] = dock_deploy_labels;
    }
    
    //Get the list of adoptable node kind items
    for(auto node_view_item : view_controller->getNodeKindItems()){
        auto node_kind = node_view_item->getNodeKind();
        auto edge_kind = connect_node_edge_kinds.value(node_kind, EDGE_KIND::NONE);


        

        if(edge_kind == EDGE_KIND::NONE){
            //Make an action for the Items which don't need a submenu
            auto action = construct_viewitem_action(node_view_item);
            action->setProperty("action_kind", QVariant::fromValue(ACTION_KIND::ADD_NODE));
            add_node_action_hash[node_kind] = action;
        }else{
            //Make a menu for the Nodes which need to be constructed with an edge
            auto menu = construct_viewitem_menu(node_view_item, 0);
            menu->setProperty("edge_kind", QVariant::fromValue(edge_kind));
            menu->setProperty("action_kind", QVariant::fromValue(ACTION_KIND::ADD_NODE_WITH_EDGE));
            
            menu->addAction(construct_menu_search(menu));
            auto menu_action = menu->menuAction();

            add_node_action_hash[node_kind] = menu_action;
        }
    }
    auto empty_action = get_no_valid_items_action(0, "No Adoptable Entities");
    empty_action->setProperty("node_kind", QVariant::fromValue(NODE_KIND::NONE));
    add_node_action_hash[NODE_KIND::NONE] = empty_action;
    add_node_menu->addAction(empty_action);
    dock_add_node_menu->addAction(empty_action);

    //LOAD ALL 
    add_node_action_hash[NODE_KIND::WORKER_PROCESS]->menu()->setProperty("load_all", true);

   

    //Get the list of constructable edge kind items
    auto edge_kinds = view_controller->getEdgeKindItems();
    
    for(auto edge_view_item : edge_kinds){
        auto edge_kind = edge_view_item->getEdgeKind();
        
        auto needs_add = edge_kind != EDGE_KIND::DEPLOYMENT;
        auto needs_remove = needs_add && edge_kind != EDGE_KIND::DEFINITION;
        
        if(needs_add){
            //Construct an Add Edge Kind Menu
            auto add_edge_kind_menu = construct_viewitem_menu(edge_view_item, add_edge_menu);
            add_edge_kind_menu->setProperty("action_kind", QVariant::fromValue(ACTION_KIND::ADD_EDGE));
            
            //Construct an Add Edge Kind From Menu
            auto add_edge_kind_src_menu = construct_menu("From", add_edge_kind_menu);
            add_edge_kind_src_menu->setProperty("edge_kind", add_edge_kind_menu->property("edge_kind"));
            add_edge_kind_src_menu->setProperty("action_kind", add_edge_kind_menu->property("action_kind"));
            add_edge_kind_src_menu->setProperty("edge_direction", QVariant::fromValue(EDGE_DIRECTION::SOURCE));

            auto src_search = construct_menu_search(add_edge_kind_menu);
            search_actions_[add_edge_kind_src_menu] = src_search;
            add_edge_kind_src_menu->addAction(src_search);

            //Construct an Add Edge Kind To Menu
            auto add_edge_kind_dst_menu = construct_menu("To", add_edge_kind_menu);
            add_edge_kind_dst_menu->setProperty("edge_kind", add_edge_kind_menu->property("edge_kind"));
            add_edge_kind_dst_menu->setProperty("action_kind", add_edge_kind_menu->property("action_kind"));
            add_edge_kind_dst_menu->setProperty("edge_direction", QVariant::fromValue(EDGE_DIRECTION::TARGET));

            auto dst_search = construct_menu_search(add_edge_kind_menu);
            search_actions_[add_edge_kind_dst_menu] = dst_search;
            add_edge_kind_dst_menu->addAction(dst_search);

            //Insert these menus into the hash
            add_edge_menu_hash[edge_kind] = add_edge_kind_menu;
            
            add_edge_menu_direct_hash[{EDGE_DIRECTION::SOURCE, edge_kind}] = add_edge_kind_src_menu;
            add_edge_menu_direct_hash[{EDGE_DIRECTION::TARGET, edge_kind}] = add_edge_kind_dst_menu;
        
            //Setup the edge menu
            add_edge_kind_menu->addMenu(add_edge_kind_src_menu);
            add_edge_kind_menu->addMenu(add_edge_kind_dst_menu);
            add_edge_menu->addMenu(add_edge_kind_menu);
        }
        if(needs_remove){
            //Construct an Remove Edge Kind Menu
            auto remove_edge_kind_menu = construct_viewitem_menu(edge_view_item, remove_edge_menu);
            remove_edge_kind_menu->setProperty("action_kind", QVariant::fromValue(ACTION_KIND::REMOVE_EDGE));
            remove_edge_menu_hash[edge_kind] = remove_edge_kind_menu;
            remove_edge_menu->addMenu(remove_edge_kind_menu);
        }
    }

    //Put our custom deploy menu into the edge hash
    add_edge_menu_direct_hash[{EDGE_DIRECTION::SOURCE, EDGE_KIND::DEPLOYMENT}] = deploy_menu;
    add_edge_menu_direct_hash[{EDGE_DIRECTION::TARGET, EDGE_KIND::DEPLOYMENT}] = deploy_menu;

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
