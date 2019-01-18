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

enum class NodePosition{
    INDEX,
    POSITION
};

class ContextMenu : public QObject{
    Q_OBJECT
public:
    ContextMenu(ViewController *vc);
    void popup_edge_menu(QPoint global_pos, EDGE_KIND edge_kind, EDGE_DIRECTION edge_direction);
    void popup_add_menu(QPoint global_pos, int index);
    void popup(QPoint global_pos, QPointF item_pos);

    QMenu* getAddMenu();
    QMenu* getDeployMenu();
private:
    void load_more_actions(QMenu* menu);
    void invalidate_menus();
    void clear_hover();
    void set_hovered_id(int id);
    void themeChanged();
    void setupMenus();
    void action_triggered(QAction* action);
    void action_hovered(QAction* action);
    
    void update_dock_menus();
    void update_menu(QMenu* menu);

    void update_main_menu();
    void update_add_edge_menu();

    void update_add_node_menu();
    void update_deploy_menu();

    void update_chart_menu();

    void populate_dynamic_add_node_menu(QMenu* menu);
    void populate_dynamic_add_edge_menu(QMenu* menu);

    QMenu* construct_menu(QString label, QMenu* parent_menu, int icon_size=0);
private:
    void popup_menu(QMenu* menu, QPoint pos);
    void construct_view_item_menus(QMenu* menu, QList<ViewItem*> view_items, bool flatten_menu = false, bool add_empty = true);

    //void update_edge_menu(QMenu* parent_menu, QMenu* menu, QList<ViewItem*> connect_items, QList<ViewItem*> disconnect_items);
    void update_edge_menu(QMenu* parent_menu, QMenu* menu, QList<ViewItem*> connect_source_items, QList<ViewItem*> connect_target_items, QList<ViewItem*> disconnect_items);

    QWidgetAction* construct_menu_search(QMenu* parent);
    QWidgetAction* get_load_more_action(QMenu* parent);

    bool menu_requires_update(QMenu* menu);
    void menu_updated(QMenu* menu);
    void menu_focussed(QMenu* menu);


    QAction* get_no_valid_items_action(QMenu* menu, QString label="No Valid Entities");

    QAction* construct_base_action(QMenu* menu, QString label);
    QAction* construct_remove_all_action(QMenu* menu, int number);
    QAction* construct_viewitem_action(ViewItem* item, QMenu* menu=0);
    QAction* get_viewitem_menu(ACTION_KIND kind, ViewItem* item);
    QMenu* construct_viewitem_menu(ViewItem* item, QMenu* parent_menu = 0);
    
    ViewController* view_controller = 0;
    ActionController* action_controller = 0;
    
    QMenu* get_add_edge_menu(EDGE_DIRECTION direction, EDGE_KIND edge_kind);
    QMenu* get_remove_edge_menu(EDGE_KIND edge_kind);

    QMenu* main_menu = 0;
    QMenu* add_node_menu = 0;
    QMenu* add_edge_menu = 0;
    //QMenu* remove_edge_menu = 0;
    QMenu* deploy_menu = 0;

    QMenu* dock_add_node_menu = 0;
    QMenu* dock_deploy_menu = 0;

    QMenu* chart_data_kind_menu = 0;

    QHash <EDGE_KIND, QMenu*> add_edge_menu_hash;
    QHash <EDGE_KIND, QMenu*> remove_edge_menu_hash;
    
    QHash <QPair<EDGE_DIRECTION, EDGE_KIND> , QMenu*> add_edge_menu_direct_hash;
    
    
    QHash <NODE_KIND, QAction*> add_node_action_hash;

    QSet<QMenu*> valid_menus;

    struct DeployTitles{
        QAction* disconnect_title = 0;
        QAction* connect_to_title = 0;
        QAction* connect_from_title = 0;
    };

    QHash<QMenu*, QWidgetAction*> load_more_actions_;
    QHash<QMenu*, QWidgetAction*> search_actions_;
    QHash <QMenu*, DeployTitles*> title_actions;
    QHash <QMenu*, QWidgetAction*> add_labels;

    QHash<NODE_KIND, EDGE_KIND> connect_node_edge_kinds;

    NodePosition node_position = NodePosition::INDEX;
    QPointF model_point;
    int node_index = -1;

    int current_hovered_id = 0;
};

/*inline uint qHash(TIMELINE_DATA_KIND key, uint seed)
{
    return ::qHash(static_cast<uint>(key), seed);
}
Q_DECLARE_METATYPE(TIMELINE_DATA_KIND)*/

#endif // CONTEXTMENU_H
