#include "contextmenu.h"
#include "../../theme.h"

#include "../../ModelController/entityfactory.h"

#include <QProxyStyle>
#include <QDebug>

#define MENU_ICON_SIZE 32
class BigMenuStyle : public QProxyStyle{
    public:
    int pixelMetric(PixelMetric metric, const QStyleOption* option = 0, const QWidget* widget = 0) const{
        auto s = QProxyStyle::pixelMetric(metric, option, widget);
        if (metric == QStyle::PM_SmallIconSize) {
            s = MENU_ICON_SIZE;
        }
        return s;
    }
 };

 

ContextMenu::ContextMenu(ViewController *vc){
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

    setupMenus();
    themeChanged();

    connect(Theme::theme(), &Theme::theme_Changed, this, &ContextMenu::themeChanged);
    connect(main_menu, &QMenu::triggered, this, &ContextMenu::action_triggered);
}

void ContextMenu::themeChanged(){
    auto theme = Theme::theme();
    main_menu->setStyleSheet(theme->getMenuStyleSheet() + "QMenu::item { padding: 2px " + QString::number(MENU_ICON_SIZE) + "px 2px " + QString::number(MENU_ICON_SIZE + 4) + "px; }");

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
    
}

void ContextMenu::popup(QPoint global_pos, QPointF item_pos){
    main_menu->popup(global_pos);
}

QMenu* construct_menu(QMenu* parent, QString label){
    auto menu = parent ? parent->addMenu(label) : new QMenu(label);
    menu->setStyle(new BigMenuStyle);
    return menu;
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

                qCritical() << "CONNECTING: " << src_ids << " TO " << dst_ids;
                emit view_controller->vc_constructEdges(src_ids, dst_ids, edge_kind);
            }
            break;
        }
        default:
        break;
    }
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
                        parent_menu = get_view_item_menu(menu, parent_view_item);
                        sub_menus.insert(parent_id, parent_menu);
                    }
                }
                auto action = get_view_item_action(parent_menu, view_item);
    
                //Set the properties on the action
                action->setProperty("node_kind", menu->property("node_kind"));
                action->setProperty("edge_kind", menu->property("edge_kind"));
                action->setProperty("action_kind", menu->property("action_kind"));
                action->setProperty("edge_direction", menu->property("edge_direction"));
            }
        }

        //auto total_count = src_menu->count() + dst_menu->count();
        src_menu->menuAction()->setVisible(src_menu->actions().count());
        dst_menu->menuAction()->setVisible(dst_menu->actions().count());
    }
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
                    parent_menu = get_view_item_menu(menu, parent_view_item);
                    sub_menus.insert(parent_id, parent_menu);
                }
            }
            auto action = get_view_item_action(parent_menu, view_item);

            //Set the properties on the action
            action->setProperty("node_kind", menu->property("node_kind"));
            action->setProperty("edge_kind", menu->property("edge_kind"));
            action->setProperty("action_kind", menu->property("action_kind"));
        }
    }
}
void ContextMenu::update_add_edge_menu(QMenu* menu){
    auto visible_edge_kinds = view_controller->getValidEdgeKindsForSelection().toSet();
    
    for(auto edge_menu : add_edge_menu_hash.values()){
        auto edge_kind = edge_menu->property("edge_kind").value<EDGE_KIND>();
        edge_menu->menuAction()->setVisible(visible_edge_kinds.contains(edge_kind));
    }
}

void ContextMenu::update_add_node_menu(QMenu* menu){
    auto visible_node_kinds = view_controller->getAdoptableNodeKinds2().toSet();

    for(auto node_menu : add_node_menu_hash.values()){
        auto node_kind = node_menu->property("node_kind").value<NODE_KIND>();

        node_menu->menuAction()->setVisible(visible_node_kinds.contains(node_kind));
    }

    for(auto node_action : add_node_action_hash.values()){
        auto node_kind = node_action->property("node_kind").value<NODE_KIND>();
        node_action->setVisible(visible_node_kinds.contains(node_kind));
    }
}

QAction* ContextMenu::get_view_item_action(QMenu* parent, ViewItem* item){
    if(item){
        auto id = item->getID();
        auto action = parent->addAction(item->getData("label").toString());
        action->setIcon(Theme::theme()->getIcon(item->getIcon()));
        action->setProperty("id", id);
        return action;
    }
    return 0;
}

QMenu* ContextMenu::get_view_item_menu(QMenu* parent, ViewItem* item){
    if(item){
        auto id = item->getID();
        auto menu = parent->addMenu(item->getData("label").toString());
        menu->setIcon(Theme::theme()->getIcon(item->getIcon()));
        menu->setProperty("id", id);
        return menu;
    }
    return 0;
}


void ContextMenu::setupMenus(){
    main_menu = construct_menu(0, "");
    add_node_menu = construct_menu(main_menu, "Add Node");
    add_edge_menu = construct_menu(main_menu, "Connect");
    remove_edge_menu = construct_menu(main_menu, "Disconnect");


    connect(add_node_menu, &QMenu::aboutToShow, [=](){update_add_node_menu(add_node_menu);});

    for(auto node_kind : EntityFactory::getNodeKinds()){
        auto node_kind_str = EntityFactory::getNodeKindString(node_kind);

        auto required_edge_kind = connect_node_edge_kinds.value(node_kind, EDGE_KIND::NONE);

        if(required_edge_kind == EDGE_KIND::NONE){
            //Make an action
            auto add_node_action = add_node_menu->addAction(node_kind_str);
            add_node_action->setProperty("node_kind", QVariant::fromValue(node_kind));
            add_node_action->setProperty("action_kind", QVariant::fromValue(ACTION_KIND::ADD_NODE));
            add_node_action_hash[node_kind] = add_node_action;
        }else{
            auto add_node_kind_menu = construct_menu(add_node_menu, node_kind_str);
            add_node_kind_menu->setProperty("node_kind", QVariant::fromValue(node_kind));
            add_node_kind_menu->setProperty("edge_kind", QVariant::fromValue(required_edge_kind));
            add_node_kind_menu->setProperty("action_kind", QVariant::fromValue(ACTION_KIND::ADD_NODE_WITH_EDGE));
            add_node_menu_hash[node_kind] = add_node_kind_menu;

            connect(add_node_kind_menu, &QMenu::aboutToShow, [=](){populate_dynamic_add_node_menu(add_node_kind_menu);});
        }
    }

    for(auto edge_kind : EntityFactory::getEdgeKinds()){
        auto edge_kind_str = EntityFactory::getEdgeKindString(edge_kind);
        edge_kind_str.remove(0, 5);
        
        auto add_edge_kind_menu = construct_menu(add_edge_menu, edge_kind_str);
        add_edge_kind_menu->setProperty("edge_kind", QVariant::fromValue(edge_kind));

        auto add_edge_kind_src_menu =  construct_menu(add_edge_kind_menu, "From");
        add_edge_kind_src_menu->setProperty("edge_kind", add_edge_kind_menu->property("edge_kind"));
        add_edge_kind_src_menu->setProperty("action_kind", QVariant::fromValue(ACTION_KIND::ADD_EDGE));
        add_edge_kind_src_menu->setProperty("edge_direction", QVariant::fromValue(EDGE_DIRECTION::SOURCE));
        
        auto add_edge_kind_dst_menu = construct_menu(add_edge_kind_menu, "To");
        add_edge_kind_dst_menu->setProperty("edge_kind", add_edge_kind_menu->property("edge_kind"));
        add_edge_kind_dst_menu->setProperty("action_kind", QVariant::fromValue(ACTION_KIND::ADD_EDGE));
        add_edge_kind_dst_menu->setProperty("edge_direction", QVariant::fromValue(EDGE_DIRECTION::TARGET));

        add_edge_menu_direct_hash[{EDGE_DIRECTION::SOURCE, edge_kind}] = add_edge_kind_src_menu;
        add_edge_menu_direct_hash[{EDGE_DIRECTION::TARGET, edge_kind}] = add_edge_kind_dst_menu;
        add_edge_menu_hash[edge_kind] = add_edge_kind_menu;


        auto remove_edge_kind_menu = construct_menu(remove_edge_menu, edge_kind_str);
        remove_edge_kind_menu->setProperty("edge_kind", QVariant::fromValue(edge_kind));
        remove_edge_menu_hash[edge_kind] = remove_edge_kind_menu;

        connect(add_edge_kind_menu, &QMenu::aboutToShow, [=](){populate_dynamic_add_edge_menu(add_edge_kind_menu);});
        
    }
    connect(add_edge_menu, &QMenu::aboutToShow, [=](){update_add_edge_menu(add_edge_menu);});
}