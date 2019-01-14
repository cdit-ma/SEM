#include "contextmenu.h"
#include "../../theme.h"

#include "../../../modelcontroller/entityfactory.h"
#include "../../Widgets/filterwidget.h"
#include "../../Widgets/loadmorewidget.h"

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
    connect_node_edge_kinds[NODE_KIND::COMPONENT_INSTANCE] = EDGE_KIND::DEFINITION;
    connect_node_edge_kinds[NODE_KIND::VECTOR_INSTANCE] = EDGE_KIND::DEFINITION;
    connect_node_edge_kinds[NODE_KIND::COMPONENT_IMPL] = EDGE_KIND::DEFINITION;
    connect_node_edge_kinds[NODE_KIND::AGGREGATE_INSTANCE] = EDGE_KIND::DEFINITION;
    connect_node_edge_kinds[NODE_KIND::MEMBER_INSTANCE] = EDGE_KIND::DEFINITION;
    connect_node_edge_kinds[NODE_KIND::PORT_PUBLISHER_IMPL] = EDGE_KIND::DEFINITION;
    
    connect_node_edge_kinds[NODE_KIND::ENUM_INSTANCE] = EDGE_KIND::DEFINITION;

    connect_node_edge_kinds[NODE_KIND::CLASS_INSTANCE] = EDGE_KIND::DEFINITION;
    connect_node_edge_kinds[NODE_KIND::FUNCTION_CALL] = EDGE_KIND::DEFINITION;
    connect_node_edge_kinds[NODE_KIND::EXTERNAL_TYPE] = EDGE_KIND::DEFINITION;
    

    connect_node_edge_kinds[NODE_KIND::PORT_SUBSCRIBER] = EDGE_KIND::AGGREGATE;
    connect_node_edge_kinds[NODE_KIND::PORT_PUBLISHER] = EDGE_KIND::AGGREGATE;
    connect_node_edge_kinds[NODE_KIND::PORT_SUBSCRIBER_DELEGATE] = EDGE_KIND::AGGREGATE;
    connect_node_edge_kinds[NODE_KIND::PORT_PUBLISHER_DELEGATE] = EDGE_KIND::AGGREGATE;
    connect_node_edge_kinds[NODE_KIND::EXTERNAL_PUBSUB_DELEGATE] = EDGE_KIND::AGGREGATE;
    connect_node_edge_kinds[NODE_KIND::EXTERNAL_SERVER_DELEGATE] = EDGE_KIND::AGGREGATE;

    connect_node_edge_kinds[NODE_KIND::PORT_REPLIER] = EDGE_KIND::AGGREGATE;
    connect_node_edge_kinds[NODE_KIND::PORT_REQUESTER] = EDGE_KIND::AGGREGATE;

    connect_node_edge_kinds[NODE_KIND::PORT_REQUESTER_IMPL] = EDGE_KIND::DEFINITION;
    connect_node_edge_kinds[NODE_KIND::PORT_REPLIER_IMPL] = EDGE_KIND::DEFINITION;
    

    action_controller = view_controller->getActionController();

    setupMenus();
    themeChanged();

    connect(Theme::theme(), &Theme::theme_Changed, this, &ContextMenu::themeChanged);
    

    connect(vc, &ViewController::ActionFinished, this, &ContextMenu::invalidate_menus);
    connect(vc, &ViewController::ActionFinished, this, &ContextMenu::update_dock_menus);

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

QAction* ConstructDisabledAction(const QString& str, QObject* obj){
    auto action = new QAction(str, obj);
    action->setEnabled(false);
    return action;
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
    main_menu->setStyleSheet(theme->getMenuStyleSheet(icon_size.width()) + " QLabel{color:" + theme->getTextColorHex(ColorRole::DISABLED) + ";}"
    "QMenu::item:disabled {"
           "color:" + theme->getAltTextColorHex() + ";"
           "}");

    //Set Icons for top level icons
    add_node_menu->setIcon(theme->getIcon("Icons", "plus"));
    add_edge_menu->setIcon(theme->getIcon("Icons", "connect"));
    deploy_menu->setIcon(theme->getIcon("Icons", "screenTwoTone"));

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

    QIcon connect_to_icon;
    QIcon connect_from_icon;
    QIcon disconnect_icon;
    disconnect_icon.addPixmap(theme->getImage("Icons", "connectStriked", QSize(), theme->getTextColor()));
    connect_to_icon.addPixmap(theme->getImage("Icons", "connectTo", QSize(), theme->getTextColor()));
    connect_from_icon.addPixmap(theme->getImage("Icons", "connectFrom", QSize(), theme->getTextColor()));

    for(auto title_action : title_actions.values()){
        title_action->connect_to_title->setIcon(connect_to_icon);
        title_action->connect_from_title->setIcon(connect_from_icon);
        title_action->disconnect_title->setIcon(disconnect_icon);
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


    // update chart menu icons
    chart_data_kind_menu->setIcon(theme->getIcon("Icons", "chart"));
    for (auto action : chart_data_kind_menu->actions()) {
        action->setIcon(theme->getIcon("ToggleIcons", action->text()));
    }
}



void ContextMenu::update_edge_menu(QMenu* parent_menu, QMenu* menu, QList<ViewItem*> connect_source_items, QList<ViewItem*> connect_target_items, QList<ViewItem*> disconnect_items){
    if(menu && menu_requires_update(menu)){
        //Get the Titles for this menu
        auto titles = title_actions.value(menu, 0);
        if(parent_menu){
            menu->setProperty("filter", parent_menu->property("filter"));
        }
        ClearMenu(menu);
            
        auto connect_size = connect_source_items.size() + connect_target_items.size();
        auto disconnect_size = disconnect_items.size();
        auto total_size = connect_size + disconnect_size;

        //Should show connect
        bool show_disconnect = disconnect_size > 0;
        bool show_connect = connect_size > 0 || disconnect_size == 0;

        if(show_disconnect){
            //Add the disconnect title
            menu->addAction(titles->disconnect_title);
            
            //Temporarily set the menu action kind so that any action we are to it will know to remove
            menu->setProperty("action_kind", QVariant::fromValue(ACTION_KIND::REMOVE_EDGE));
            construct_view_item_menus(menu, disconnect_items, false, true);

            if(disconnect_size > 1){
                //Construct a remove all buutton
                construct_remove_all_action(menu, disconnect_size);
            }
        }

        if(show_disconnect && show_connect){
            menu->addSeparator();
        }
        
        if(show_connect){
            //Add the connect title
            if(connect_source_items.size()){
                menu->addAction(titles->connect_from_title);
            }
            
            //Temporarily set the menu action kind so that any action we are to it will know to add
            menu->setProperty("action_kind", QVariant::fromValue(ACTION_KIND::ADD_EDGE));
            menu->setProperty("edge_direction", QVariant::fromValue(EDGE_DIRECTION::SOURCE));
            construct_view_item_menus(menu, connect_source_items, false, false);

            if(connect_target_items.size()){
                if(connect_source_items.size()){
                    menu->addSeparator();
                }
                menu->addAction(titles->connect_to_title);
            }
            menu->setProperty("edge_direction", QVariant::fromValue(EDGE_DIRECTION::TARGET));
            construct_view_item_menus(menu, connect_target_items, false, false);


            if(connect_size == 0){
                
                get_no_valid_items_action(menu, "No Entities");
            }
        }

        menu->menuAction()->setVisible(total_size > 0);
        
        if(parent_menu){
            parent_menu->addAction(menu->menuAction());
        }
        

        menu_updated(menu);
    }


}
void ContextMenu::popup_menu(QMenu* menu, QPoint pos){
    if(menu){
        emit menu->aboutToShow();
        menu->popup(pos);
    }
}

void ContextMenu::popup_edge_menu(QPoint global_pos, EDGE_KIND edge_kind, EDGE_DIRECTION edge_direction){
    popup_menu(add_edge_menu_direct_hash.value({edge_direction, edge_kind}, 0), global_pos);
}
void ContextMenu::popup_add_menu(QPoint global_pos, int index){
    node_position = NodePosition::INDEX;
    node_index = index;
    popup_menu(add_node_menu, global_pos);
}

void ContextMenu::popup(QPoint global_pos, QPointF item_pos){
    node_position = NodePosition::POSITION;
    model_point = item_pos;
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
    model_point = QPointF();
    for(auto menu : {dock_add_node_menu, dock_deploy_menu}){
        update_menu(menu);
    }
}

void ContextMenu::update_menu(QMenu* menu){

    if(menu == main_menu){
        return update_main_menu();
    }else if(menu == add_edge_menu){
        return update_add_edge_menu();
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
                case ACTION_KIND::ADD_EDGE:
                case ACTION_KIND::REMOVE_EDGE:{
                    auto edge_menu = add_edge_menu_hash.value(edge_kind, 0);
                    if(edge_menu){
                        return populate_dynamic_add_edge_menu(edge_menu);
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

                if(item->hasData("isExpanded")){
                    //Expand!
                    emit view_controller->SetData(parent_id, "isExpanded", true);
                }

                switch(node_position){
                    case NodePosition::INDEX:{
                        emit view_controller->ConstructNodeAtIndex(parent_id, node_kind, node_index);
                        node_index = -1;
                        break;
                    }
                    case NodePosition::POSITION:{
                        emit view_controller->ConstructNodeAtPos(parent_id, node_kind, model_point);
                        break;
                    }
                }
            }
            break;
        }
        case ACTION_KIND::ADD_NODE_WITH_EDGE:{
            auto item = view_controller->getSelectionController()->getActiveSelectedItem();
            if(item){
                auto parent_id = item->getID();
                emit view_controller->SetData(parent_id, "isExpanded", true);

                switch(node_position){
                    case NodePosition::INDEX:{
                        emit view_controller->ConstructConnectedNodeAtIndex(parent_id, node_kind, id, edge_kind, node_index);
                        node_index = -1;
                        break;
                    }
                    case NodePosition::POSITION:{
                        emit view_controller->ConstructConnectedNodeAtPos(parent_id, node_kind, id, edge_kind, model_point);
                        break;
                    }
                }
                
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
                    auto remove_source = action->property("remove_source").value<bool>();
                    auto remove_target = action->property("remove_target").value<bool>();

                    QSet<EDGE_DIRECTION> edge_directions;
                    if(remove_source){
                        edge_directions.insert(EDGE_DIRECTION::SOURCE);
                    }
                    if(remove_target){
                        edge_directions.insert(EDGE_DIRECTION::TARGET);
                    }
                    emit view_controller->DestructAllEdges(selection, edge_kind, edge_directions);
                }else{
                    emit view_controller->DestructEdges(selection, {id}, edge_kind);
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

        auto connect_map = view_controller->getValidEdges(edge_kind);
        auto disconnect_map = view_controller->getExistingEndPointsOfSelection(edge_kind);

        auto is_empty = connect_map.isEmpty() && disconnect_map.isEmpty();

        if(src_menu){
            update_edge_menu(menu, src_menu, connect_map.values(EDGE_DIRECTION::SOURCE), {}, disconnect_map.values(EDGE_DIRECTION::TARGET));
        }
        if(dst_menu){
            update_edge_menu(menu, dst_menu, {}, connect_map.values(EDGE_DIRECTION::TARGET), disconnect_map.values(EDGE_DIRECTION::SOURCE));
        }

        if(is_empty){
            get_no_valid_items_action(menu);
        }

        //Add menu into the set of validated menus to avoid recalculating unnecessarily
        menu_updated(menu);
    }
}

void ContextMenu::construct_view_item_menus(QMenu* menu, QList<ViewItem*> view_items, bool flatten_menu, bool add_empty){
    if(menu){
        menu->setUpdatesEnabled(false);
        auto filter_str = menu->property("filter").toString();
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

        //We should use a parent menu hiarachy, if we have more than 1 parent, and the user doesn't want flattened menus
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

        auto load_more_action = get_load_more_action(menu);

        if(all_loaded){
            menu->removeAction(load_more_action);
        }else{
            menu->addAction(load_more_action);
        }
        
        if(hidden_count > 0){
            get_no_valid_items_action(menu, QString::number(hidden_count) + " Entities filtered");
        }

        //Add an invalid item
        if(add_empty && view_items.empty()){
            get_no_valid_items_action(menu, "No Entities");
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

        update_menu(menu);
    }
}


QAction* ContextMenu::construct_remove_all_action(QMenu* menu, int number){
    auto remove_all = construct_base_action(menu, "Disconnect All (" + QString::number(number) + ")");
    remove_all->setIcon(Theme::theme()->getIcon("Icons", "bin"));
    remove_all->setProperty("node_kind", menu->property("node_kind"));
    remove_all->setProperty("edge_kind", menu->property("edge_kind"));
    remove_all->setProperty("action_kind", menu->property("action_kind"));
    remove_all->setProperty("remove_source", menu->property("remove_source"));
    remove_all->setProperty("remove_target", menu->property("remove_target"));

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
        auto selected_ids = view_controller->getSelectionController()->getSelectionIDs();
        auto v_e = view_controller->getValidEdgeKinds(selected_ids);
        
        auto visible_edge_kinds = v_e.first + v_e.second;
        
        for(auto edge_menu : add_edge_menu_hash.values()){
            auto edge_kind = edge_menu->property("edge_kind").value<EDGE_KIND>();
            edge_menu->menuAction()->setVisible(visible_edge_kinds.contains(edge_kind));
        }

        //Add menu into the set of validated menus to avoid recalculating unnecessarily
        menu_updated(add_edge_menu);
    }
}

void ContextMenu::update_main_menu(){
    if(menu_requires_update(main_menu)){
        bool can_add_node = view_controller->getValidNodeKinds().size();

        auto selected_ids = view_controller->getSelectionController()->getSelectionIDs();
        auto valid_edge_kinds = view_controller->getValidEdgeKinds(selected_ids);
        auto existant_edge_kinds = view_controller->getCurrentEdgeKinds();
        
        auto edge_kinds = valid_edge_kinds.first + valid_edge_kinds.second + existant_edge_kinds;

        bool can_add_edge = false;

        //Check if we have an Add Edge item for any of the valid edge kinds
        for(auto edge_kind : edge_kinds){
            if(add_edge_menu_hash.contains(edge_kind)){
                can_add_edge = true;
                break;
            }
        }
        for(auto edge_menu : add_edge_menu_hash.values()){
            auto edge_kind = edge_menu->property("edge_kind").value<EDGE_KIND>();
            auto edge_kind_visible = edge_kinds.contains(edge_kind);
            if(edge_kind_visible){
                can_add_edge = true;
            }
            edge_menu->menuAction()->setVisible(edge_kind_visible);
        }

        //Show and hide the menu actions for the sub menus
        add_node_menu->menuAction()->setVisible(can_add_node);

        add_edge_menu->menuAction()->setVisible(can_add_edge);
        deploy_menu->menuAction()->setVisible(edge_kinds.contains(EDGE_KIND::DEPLOYMENT));
        
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
    menu->show();
    auto widget_action = search_actions_.value(menu, 0);
    if(widget_action){
        menu->setActiveAction(widget_action);
        widget_action->defaultWidget()->setFocus();
    }
}




void ContextMenu::update_add_node_menu(){
    auto menus = {add_node_menu, dock_add_node_menu};

    for(auto menu : menus){
        if(menu_requires_update(menu)){
            ClearMenu(menu);

            auto filter_str = menu->property("filter").toString();
            auto adoptable_node_kinds = view_controller->getValidNodeKinds();
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

        auto connect_map = view_controller->getValidEdges(EDGE_KIND::DEPLOYMENT);
        auto disconnect_map = view_controller->getExistingEndPointsOfSelection(EDGE_KIND::DEPLOYMENT);

        auto menus = {dock_deploy_menu, deploy_menu};

        for(auto menu : menus){
            menu->setProperty("load_all", true);
            update_edge_menu(0, menu, connect_map.values(EDGE_DIRECTION::SOURCE), connect_map.values(EDGE_DIRECTION::TARGET), disconnect_map.values());
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

QWidgetAction* ContextMenu::get_load_more_action(QMenu* parent){
    auto action = load_more_actions_.value(parent, 0);

    if(!action){
        action = new QWidgetAction(0);
        //action->setProperty("LoadMore", true);
        auto add_more_widget = new LoadMoreWidget();

        connect(add_more_widget, &LoadMoreWidget::LoadMore, [=](){
            load_more_actions(parent);
            parent->show();
        });

        action->setDefaultWidget(add_more_widget);
        load_more_actions_[parent] = action;
    }
    return action;
}



void ContextMenu::setupMenus()
{
    main_menu = construct_menu("", 0);

    deploy_menu = construct_menu("Deploy", main_menu);
    
    add_edge_menu = construct_menu("Connect", main_menu);
    
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
            auto menu_titles = new DeployTitles;
            menu_titles->connect_to_title =  ConstructDisabledAction("CONNECT TO", this);
            menu_titles->connect_from_title =  ConstructDisabledAction("CONNECT FROM", this);
            menu_titles->disconnect_title = ConstructDisabledAction("DISCONNECT", this);
            title_actions[dock_deploy_menu] = menu_titles;
    }


    {
        auto menu_titles = new DeployTitles;
        menu_titles->connect_to_title =  ConstructDisabledAction("CONNECT TO", this);
        menu_titles->connect_from_title =  ConstructDisabledAction("CONNECT FROM", this);
        menu_titles->disconnect_title = ConstructDisabledAction("DISCONNECT", this);
        title_actions[deploy_menu] = menu_titles;
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

    if(add_node_action_hash.contains(NODE_KIND::FUNCTION_CALL)){
        add_node_action_hash[NODE_KIND::FUNCTION_CALL]->menu()->setProperty("load_all", true);
    }

   

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
            add_edge_kind_src_menu->setProperty("remove_source", true);

            {
                auto menu_titles = new DeployTitles;
                menu_titles->connect_to_title =  ConstructDisabledAction("CONNECT TO", this);
                menu_titles->connect_from_title =  ConstructDisabledAction("CONNECT FROM", this);
                menu_titles->disconnect_title = ConstructDisabledAction("DISCONNECT", this);
                title_actions[add_edge_kind_src_menu] = menu_titles;
            }

            auto src_search = construct_menu_search(add_edge_kind_menu);
            search_actions_[add_edge_kind_src_menu] = src_search;
            add_edge_kind_src_menu->addAction(src_search);

            //Construct an Add Edge Kind To Menu
            auto add_edge_kind_dst_menu = construct_menu("To", add_edge_kind_menu);
            add_edge_kind_dst_menu->setProperty("edge_kind", add_edge_kind_menu->property("edge_kind"));
            add_edge_kind_dst_menu->setProperty("action_kind", add_edge_kind_menu->property("action_kind"));
            add_edge_kind_dst_menu->setProperty("edge_direction", QVariant::fromValue(EDGE_DIRECTION::TARGET));
            add_edge_kind_dst_menu->setProperty("remove_target", true);

            {
                auto menu_titles = new DeployTitles;
                menu_titles->connect_to_title =  ConstructDisabledAction("CONNECT TO", this);
                menu_titles->connect_from_title =  ConstructDisabledAction("CONNECT FROM", this);
                menu_titles->disconnect_title = ConstructDisabledAction("DISCONNECT", this);
                title_actions[add_edge_kind_dst_menu] = menu_titles;
            }

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
    }

    deploy_menu->setProperty("edge_kind", QVariant::fromValue(EDGE_KIND::DEPLOYMENT));
    deploy_menu->setProperty("remove_source", true);
    deploy_menu->setProperty("remove_target", true);
    
    dock_deploy_menu->setProperty("edge_kind", QVariant::fromValue(EDGE_KIND::DEPLOYMENT));
    dock_deploy_menu->setProperty("remove_source", true);
    dock_deploy_menu->setProperty("remove_target", true);

    //Put our custom deploy menu into the edge hash
    add_edge_menu_direct_hash[{EDGE_DIRECTION::SOURCE, EDGE_KIND::DEPLOYMENT}] = deploy_menu;
    add_edge_menu_direct_hash[{EDGE_DIRECTION::TARGET, EDGE_KIND::DEPLOYMENT}] = deploy_menu;


    // setup chart menu
    chart_data_kind_menu = construct_menu("View In Chart", main_menu);
    for (auto kind : GET_TIMELINE_DATA_KINDS()) {
        if (kind == TIMELINE_DATA_KIND::PORT_LIFECYCLE ||
            kind == TIMELINE_DATA_KIND::WORKLOAD ||
            kind == TIMELINE_DATA_KIND::CPU_UTILISATION ||
            kind == TIMELINE_DATA_KIND::MEMORY_UTILISATION)
        {
            auto action = chart_data_kind_menu->addAction(GET_TIMELINE_DATA_KIND_STRING(kind));
            action->setProperty("dataKind", (uint)kind);
        }
    }
    // connect chart menu
    connect(chart_data_kind_menu, &QMenu::triggered, [=](QAction* action) {
        if (view_controller) {
            auto item = view_controller->getSelectionController()->getActiveSelectedItem();
            if (item)
                emit view_controller->vc_viewItemInChart(item, (TIMELINE_DATA_KIND)action->property("dataKind").toUInt());
        }
    });


    //Setup the main_menu
    main_menu->addMenu(add_node_menu);
    main_menu->addAction(action_controller->edit_delete->constructSubAction(true));
    main_menu->addSeparator();

    main_menu->addMenu(deploy_menu);
    main_menu->addMenu(add_edge_menu);
    main_menu->addSeparator();
    main_menu->addAction(action_controller->view_viewConnections->constructSubAction(true));
    main_menu->addAction(action_controller->model_getCodeForComponent->constructSubAction(true));
    main_menu->addSeparator();
    main_menu->addAction(action_controller->toolbar_replicateCount->constructSubAction(true));
    main_menu->addSeparator();
    //main_menu->addAction(action_controller->chart_viewInChart->constructSubAction(true));
    main_menu->addMenu(chart_data_kind_menu);
    main_menu->addSeparator();
    main_menu->addAction(action_controller->view_viewInNewWindow->constructSubAction(true));
    main_menu->addAction(action_controller->toolbar_wiki->constructSubAction(true));
}
