#ifndef CONTEXTMENU_H
#define CONTEXTMENU_H

#include <QMenu>
#include <QHash>
#include <QAction>
#include <QWidgetAction>

#include "../../Controllers/ViewController/viewcontroller.h"
enum class ACTION_KIND{
    ADD_NODE,
    ADD_NODE_WITH_EDGE,
    ADD_EDGE,
    REMOVE_EDGE,
};
inline uint qHash(ACTION_KIND key, uint seed)
{
    return ::qHash(static_cast<uint>(key), seed);
}
Q_DECLARE_METATYPE(ACTION_KIND);


class ContextMenu : public QObject{
    Q_OBJECT
public:
    ContextMenu(ViewController *vc);
    void popup(QPoint global_pos, QPointF item_pos);
    QMenu* getAddMenu();
    QMenu* getDeployMenu();
private:
    void clear_hover();
    void set_hovered_id(int id);
    void themeChanged();
    void setupMenus();
    void action_triggered(QAction* action);
    void action_hovered(QAction* action);
    
    void update_main_menu();
    void update_add_edge_menu();
    void update_remove_edge_menu();
    void update_add_node_menu();
    void update_deploy_menu();


    void populate_dynamic_add_node_menu(QMenu* menu);
    void populate_dynamic_add_edge_menu(QMenu* menu);
    void popuplate_dynamic_remove_edge_menu(QMenu* menu);

    QMenu* construct_menu(QString label, QMenu* parent_menu);
private:
    QAction* get_no_valid_items_action(QString label="No Valid Entities");
    QAction* construct_base_action(QMenu* menu, QString label);
    QAction* construct_remove_all_action(QMenu* menu, int number);
    QAction* construct_viewitem_action(ViewItem* item);
    QAction* get_deploy_viewitem_action(ACTION_KIND kind, ViewItem* item);
    QAction* get_viewitem_menu(ACTION_KIND kind, ViewItem* item);
    QMenu* construct_viewitem_menu(ViewItem* item, QMenu* parent_menu = 0);
    
    QHash<int, QAction*> view_item_actions;
    QHash<int, QMenu*> view_item_menu;

    ViewController* view_controller = 0;
    ActionController* action_controller = 0;
    
    QMenu* get_add_edge_menu(EDGE_DIRECTION direction, EDGE_KIND edge_kind);
    QMenu* get_remove_edge_menu(EDGE_KIND edge_kind);

    QMenu* main_menu = 0;
    QMenu* add_node_menu = 0;
    QMenu* add_edge_menu = 0;
    QMenu* remove_edge_menu = 0;
    QMenu* deploy_menu = 0;

    QMenu* dock_add_node_menu = 0;
    QMenu* dock_deploy_menu = 0;

    QAction* no_valid_action = 0;

    
    QHash <EDGE_KIND, QMenu*> remove_edge_menu_hash;
    QHash <EDGE_KIND, QMenu*> add_edge_menu_hash;
    
    QHash <QPair<EDGE_DIRECTION, EDGE_KIND> , QMenu*> add_edge_menu_direct_hash;
    
    
    QHash <QPair<int, ACTION_KIND>, QAction*> deploy_view_item_hash; 
    
    QHash <NODE_KIND, QMenu*> add_node_menu_hash;
    QHash <NODE_KIND, QAction*> add_node_action_hash;

    QHash <int, QAction*> node_action_hash;

    struct DeployLabels{
        QWidgetAction* deployed_nodes_action = 0;
        QWidgetAction* available_nodes_action = 0;
    };
    QHash <QMenu*, DeployLabels*> deploy_labels;

    QHash<NODE_KIND, EDGE_KIND> connect_node_edge_kinds;

    int current_hovered_id = 0;
};

#endif // CONTEXTMENU_H
