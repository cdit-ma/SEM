#include "contextmenu.h"
#include "../../theme.h"

#include "../../ModelController/entityfactory.h"

#include <QDebug>
#include <QStyleFactory>
#include <QLabel>
#include <QWidgetAction>

#define MENU_ICON_SIZE 32

ContextMenu::ContextMenu(ViewController *vc){
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

    view_controller = vc;
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


    connect(Theme::theme(), &Theme::theme_Changed, this, &ContextMenu::themeChanged);
    
    connect(vc->getSelectionController(), &SelectionController::selectionChanged, this, &ContextMenu::update_add_node_menu);
    connect(vc->getSelectionController(), &SelectionController::selectionChanged, this, &ContextMenu::update_deploy_menu);

    connect(vc, &ViewController::vc_ActionFinished, this, &ContextMenu::update_add_node_menu);
    connect(vc, &ViewController::vc_ActionFinished, this, &ContextMenu::update_deploy_menu);

    update_deploy_menu();
    update_add_node_menu();
}

QMenu* ContextMenu::getAddMenu(){
    return dock_add_node_menu;
}
QMenu* ContextMenu::getDeployMenu(){
    return dock_deploy_menu;
}

void ContextMenu::themeChanged(){
    auto theme = Theme::theme();
    main_menu->setStyleSheet(theme->getMenuStyleSheet(32) + " QMenu{font-size:10pt;} QLabel{color:" + theme->getTextColorHex(Theme::CR_NORMAL) + ";}");// QMenu::item{padding: 4px 8px 4px " + QString::number(MENU_ICON_SIZE + 8)  + "px; }"
    //dock_add_node_menu->setStyleSheet(theme->getMenuStyleSheet(32) + " QMenu{font-size:10pt;} QLabel{color:" + theme->getTextColorHex(Theme::CR_DISABLED) + ";}");// QMenu::item{padding: 4px 8px 4px " + QString::number(MENU_ICON_SIZE + 8)  + "px; }"
    //dock_deploy_menu->setStyleSheet(theme->getMenuStyleSheet(32) + " QMenu{font-size:10pt;} QLabel{color:" + theme->getTextColorHex(Theme::CR_DISABLED) + ";}");// QMenu::item{padding: 4px 8px 4px " + QString::number(MENU_ICON_SIZE + 8)  + "px; }"

    //main_menu->setStyleSheet(theme->getMenuStyleSheet(32) + " QMenu{font-size:10pt;} QLabel{color:" + theme->getTextColorHex(Theme::CR_DISABLED) + ";}");// QMenu::item{padding: 4px 8px 4px " + QString::number(MENU_ICON_SIZE + 8)  + "px; }"
    
    //deploy_menu->setStyleSheet("QWidget{font-size:10pt;} " +theme->getMenuStyleSheet(32) + " QMenu{background:transparent;} QLabel{color:" + theme->getTextColorHex(Theme::CR_DISABLED) + ";}");
    //parts_menu->setStyleSheet("QWidget{font-size:10pt;} " +theme->getMenuStyleSheet(32) + " QMenu{background:transparent;} QLabel{color:" + theme->getTextColorHex(Theme::CR_DISABLED) + ";}");

    add_node_menu->setIcon(theme->getIcon("Icons", "plus"));
    add_edge_menu->setIcon(theme->getIcon("Icons", "connect"));
    remove_edge_menu->setIcon(theme->getIcon("Icons", "connectStriked"));

    for(auto node_menu : add_node_menu_hash.values()){
        auto node_kind = node_menu->property("node_kind").value<NODE_KIND>();
        auto node_kind_str = EntityFactory::getNodeKindString(node_kind);
        node_menu->setIcon(theme->getIcon("EntityIcons", node_kind_str));
    }
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

    deploy_menu->setIcon(theme->getIcon("Icons", "screen"));
    no_valid_action->setIcon(theme->getIcon("Icons", "circleInfoDark"));
}

void ContextMenu::popup(QPoint global_pos, QPointF item_pos){
    //Test the other popup
    //auto value = add_node_menu_hash[NODE_KIND::COMPONENT_INSTANCE];//remove_edge_menu_hash[EDGE_KIND::ASSEMBLY];
    //auto value = add_edge_menu_direct_hash[{EDGE_DIRECTION::TARGET, EDGE_KIND::ASSEMBLY}];
    main_menu->popup(global_pos);
    
}

QMenu* ContextMenu::construct_menu(QString label, QMenu* parent_menu){
    auto menu = parent_menu ? parent_menu->addMenu(label) : new QMenu(label);
    menu->setStyle(new CustomMenuStyle(32));
    return menu;
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
    set_hovered_id(0);
}

void ContextMenu::populate_dynamic_add_edge_menu(QMenu* menu){
    if(menu){
        auto edge_kind = menu->property("edge_kind").value<EDGE_KIND>();
        
        auto src_menu = add_edge_menu_direct_hash[{EDGE_DIRECTION::SOURCE, edge_kind}];
        auto dst_menu = add_edge_menu_direct_hash[{EDGE_DIRECTION::TARGET, edge_kind}];

        //Clear all the old items
        src_menu->clear();
        dst_menu->clear();

        //Keep a hash to keep track of previous 
        QHash<int, QMenu*> sub_menus;

        auto item_map = view_controller->getValidEdges2(edge_kind);


        for(auto direction : item_map.uniqueKeys()){
            auto menu = add_edge_menu_direct_hash.value({direction, edge_kind}, 0);
            for(auto view_item : item_map.values(direction)){
                auto parent_view_item = view_item->getParentItem();
                QMenu* parent_menu = menu;
                
                if(parent_view_item){
                    auto parent_id = parent_view_item->getID();
                    //Construct
                    parent_menu = sub_menus.value(parent_id, 0);
                    if(!parent_menu){
                        parent_menu = construct_viewitem_menu(parent_view_item, menu);
                        sub_menus.insert(parent_id, parent_menu);
                    }
                }
                auto action = construct_viewitem_action(view_item);

                action->setCheckable(true);
                action->setChecked(true);
                
                
                
                //Set the properties on the action
                action->setProperty("node_kind", menu->property("node_kind"));
                action->setProperty("edge_kind", menu->property("edge_kind"));
                action->setProperty("action_kind", menu->property("action_kind"));
                action->setProperty("edge_direction", menu->property("edge_direction"));
                parent_menu->addAction(action);
            }
        }

        src_menu->menuAction()->setVisible(!src_menu->isEmpty());
        dst_menu->menuAction()->setVisible(!dst_menu->isEmpty());
        //Add an invalid item
        if(menu->isEmpty()){
            menu->addAction(get_no_valid_items_action());
        }
    }
}

void ContextMenu::popuplate_dynamic_remove_edge_menu(QMenu* menu){
    if(menu){
        auto edge_kind = menu->property("edge_kind").value<EDGE_KIND>();

        //Clear all the old items
        menu->clear();

        
        //Keep a hash to keep track of previous 
        QHash<int, QMenu*> sub_menus;

        auto item_list = view_controller->getExistingEdgeEndPointsForSelection(edge_kind);

        for(auto view_item : item_list){
            auto parent_view_item = view_item->getParentItem();
            QMenu* parent_menu = menu;
            
            if(parent_view_item){
                auto parent_id = parent_view_item->getID();
                //Construct
                parent_menu = sub_menus.value(parent_id, 0);
                if(!parent_menu){
                    parent_menu = construct_viewitem_menu(parent_view_item, menu);
                    menu->addMenu(parent_menu);
                    sub_menus.insert(parent_id, parent_menu);
                }
            }
            auto action = construct_viewitem_action(view_item);
            
            
            //Set the properties on the action
            action->setProperty("node_kind", menu->property("node_kind"));
            action->setProperty("edge_kind", menu->property("edge_kind"));
            action->setProperty("action_kind", menu->property("action_kind"));
            parent_menu->addAction(action);
        }
        auto visible_count = item_list.count();
        
        //Add an invalid item
        if(visible_count == 0){
            menu->addAction(get_no_valid_items_action());
        }else if (visible_count > 1){
            construct_remove_all_action(menu, visible_count);
        }
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
    if(menu){
        auto node_kind = menu->property("node_kind").value<NODE_KIND>();
        auto edge_kind = menu->property("edge_kind").value<EDGE_KIND>();
    
        //Clear all the old items
        menu->clear();

        //Keep a hash to keep track of previous 
        QHash<int, QMenu*> sub_menus;

        for(auto view_item : view_controller->getConstructableNodeDefinitions(node_kind, edge_kind)){
            auto parent_view_item = view_item->getParentItem();
            QMenu* parent_menu = menu;
            
            if(parent_view_item){
                auto parent_id = parent_view_item->getID();
                //Construct
                parent_menu = sub_menus.value(parent_id, 0);
                if(!parent_menu){
                    parent_menu = construct_viewitem_menu(parent_view_item, menu);
                    menu->addMenu(parent_menu);
                    sub_menus.insert(parent_id, parent_menu);
                }
            }
            auto action = construct_viewitem_action( view_item);

            //Set the properties on the action
            action->setProperty("node_kind", menu->property("node_kind"));
            action->setProperty("edge_kind", menu->property("edge_kind"));
            action->setProperty("action_kind", menu->property("action_kind"));

            parent_menu->addAction(action);
        }

        //Add an invalid item
        if(menu->isEmpty()){
            menu->addAction(get_no_valid_items_action());
        }
    }
}

QAction* ContextMenu::get_no_valid_items_action(QString label){
    no_valid_action->setText(label);
    return no_valid_action;
}

void ContextMenu::update_add_edge_menu(){
    auto visible_edge_kinds = view_controller->getValidEdgeKindsForSelection().toSet();
    
    for(auto edge_menu : add_edge_menu_hash.values()){
        auto edge_kind = edge_menu->property("edge_kind").value<EDGE_KIND>();
        edge_menu->menuAction()->setVisible(visible_edge_kinds.contains(edge_kind));
    }
}

void ContextMenu::update_remove_edge_menu(){
    auto visible_edge_kinds = view_controller->getExistingEdgeKindsForSelection().toSet();
    
    for(auto edge_menu : remove_edge_menu_hash.values()){
        auto edge_kind = edge_menu->property("edge_kind").value<EDGE_KIND>();
        edge_menu->menuAction()->setVisible(visible_edge_kinds.contains(edge_kind));
    }
}

void ContextMenu::update_main_menu(){
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
}

void ContextMenu::update_add_node_menu(){
    auto visible_node_kinds = view_controller->getAdoptableNodeKinds2().toSet();

    //Get all the actions for the node kinds. Some of these actions are menu actions themselves
    for(auto node_action : add_node_action_hash.values()){
        auto node_kind = node_action->property("node_kind").value<NODE_KIND>();
        node_action->setVisible(visible_node_kinds.contains(node_kind));
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
    connect(action, &QAction::triggered, [=](){action_triggered(action);});
    return action;
}

QAction* ContextMenu::construct_viewitem_action(ViewItem* item){
    if(item){
        auto id = item->getID();
        
        auto action = construct_base_action(0, 0);

        action->setProperty("id", id);
        if(item->isNode()){
            auto node_item = (NodeViewItem*)item;
            action->setProperty("node_kind", QVariant::fromValue(node_item->getNodeKind()));
        }else if(item->isEdge()){
            auto edge_item = (EdgeViewItem*)item;
            action->setProperty("edge_kind", QVariant::fromValue(edge_item->getEdgeKind()));
        }

        connect(action, &QAction::hovered, [=](){action_hovered(action);});
        
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

        connect(menu_action, &QAction::hovered, [=](){action_hovered(menu_action);});

        updateAction(menu_action, item);
        return menu;
    }
    return 0;
}

void ContextMenu::update_deploy_menu(){
    if(deploy_menu && dock_deploy_menu){
        auto edge_kind = EDGE_KIND::DEPLOYMENT;
        
        auto menu_labels = deploy_labels[deploy_menu];
        auto dock_labels = deploy_labels[dock_deploy_menu];
        
        deploy_menu->clear();
        dock_deploy_menu->clear();
        
        //Get the existing nodes
        auto item_list = view_controller->getExistingEdgeEndPointsForSelection(edge_kind);
        auto deployed_item_count = item_list.count();
        qSort(item_list.begin(), item_list.end(), ViewItem::SortByKind);

        //Show the current deployed label if we have any
        if(deployed_item_count > 0){
            deploy_menu->addAction(menu_labels->deployed_nodes_action);
            dock_deploy_menu->addAction(dock_labels->deployed_nodes_action);
        }

        
        for(auto view_item : item_list){
            //Get the remove_edge action for this item
            auto remove_edge = get_deploy_viewitem_action(ACTION_KIND::REMOVE_EDGE, view_item);
            deploy_menu->addAction(remove_edge);
            dock_deploy_menu->addAction(remove_edge);
        }

        //Add an invalid item
        if(deployed_item_count > 1){
            auto action1 = construct_remove_all_action(deploy_menu, deployed_item_count);
            auto action2 = construct_remove_all_action(dock_deploy_menu, deployed_item_count);
            action1->setProperty("action_kind", QVariant::fromValue(ACTION_KIND::REMOVE_EDGE));
            action2->setProperty("action_kind", QVariant::fromValue(ACTION_KIND::REMOVE_EDGE));
            action1->setProperty("edge_kind", QVariant::fromValue(EDGE_KIND::DEPLOYMENT));
            action2->setProperty("edge_kind", QVariant::fromValue(EDGE_KIND::DEPLOYMENT));
        }

        //Get the deployable nodes
        auto item_map = view_controller->getValidEdges2(EDGE_KIND::DEPLOYMENT);
        
        

        QHash<int, QMenu*> sub_menus;
        
        //Get only the Hardware end of the deployment edges
        auto hardware = item_map.values(EDGE_DIRECTION::TARGET);
        //Sort by kind
        qSort(hardware.begin(), hardware.end(), ViewItem::SortByKind);

        //Show the available deployed label if we have any
        if(hardware.size()){
            deploy_menu->addAction(menu_labels->available_nodes_action);
            dock_deploy_menu->addAction(dock_labels->available_nodes_action);
        }
        
        for(auto view_item : hardware){
            auto node_view_item = (NodeViewItem*)view_item;
            auto parent_view_item = (NodeViewItem*)view_item->getParentItem();
            
            QMenu* parent_menu = 0;
            
            auto id = view_item->getID();

            auto node_kind = node_view_item->getNodeKind();
            auto parent_node_kind = parent_view_item->getNodeKind();

            auto menu_node = parent_view_item;

            if(node_kind == NODE_KIND::HARDWARE_CLUSTER){
                menu_node = node_view_item;
            }else if(parent_node_kind != NODE_KIND::HARDWARE_CLUSTER){
                //Don't make submenus
                menu_node = 0;
            }

            if(menu_node){
                auto menu_node_id = menu_node->getID();
                //Construct
                parent_menu = sub_menus.value(menu_node_id, 0);
                if(!parent_menu){
                    parent_menu = construct_viewitem_menu(menu_node, deploy_menu);
                    
                    dock_deploy_menu->addAction(parent_menu->menuAction());

                    sub_menus.insert(menu_node_id, parent_menu);
                }
            }

            auto add_edge_action = get_deploy_viewitem_action(ACTION_KIND::ADD_EDGE, view_item);
            add_edge_action->setProperty("edge_direction", QVariant::fromValue(EDGE_DIRECTION::TARGET));
            add_edge_action->setProperty("edge_kind", QVariant::fromValue(EDGE_KIND::DEPLOYMENT));
            if(sub_menus.contains(id)){
                add_edge_action->setText("Any Node");
            }

            if(parent_menu){
                parent_menu->addAction(add_edge_action);
            }else{
                deploy_menu->addAction(add_edge_action);
                dock_deploy_menu->addAction(add_edge_action);
            }
        }
    }
}

QWidgetAction* construct_menu_label(QString label){
    auto label_widget = new QLabel(label);
    label_widget->setAlignment(Qt::AlignCenter);
    label_widget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    auto action = new QWidgetAction(0); 
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
    dock_add_node_menu = construct_menu("Parts", 0);
    dock_deploy_menu = construct_menu("Deploy", 0);
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
        connect(add_edge_kind_menu, &QMenu::aboutToShow, [=](){populate_dynamic_add_edge_menu(add_edge_kind_menu);});
        connect(remove_edge_kind_menu, &QMenu::aboutToShow, [=](){popuplate_dynamic_remove_edge_menu(remove_edge_kind_menu);});
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

    //Construct a place holder image
    no_valid_action = new QAction();
    no_valid_action->setEnabled(false);
}