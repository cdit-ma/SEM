#ifndef CONTEXTMENU_H
#define CONTEXTMENU_H

#include <QHash>
#include <QMenu>
#include <QAction>
#include <QWidgetAction>

#include "../../Controllers/ViewController/viewcontroller.h"

enum class ACTION_KIND {
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

enum class NodePosition {
    INDEX,
    POSITION
};

class ContextMenu : public QObject
{
    Q_OBJECT

public:
    explicit ContextMenu(ViewController *vc);

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
    
    void update_trigger_edge_menu();

    void update_trigger_edge_menu();

    void update_chart_menu();

    void populate_dynamic_add_node_menu(QMenu* menu);
    void populate_dynamic_add_edge_menu(QMenu* menu);

    QMenu* construct_menu(const QString& label, QMenu* parent_menu, int icon_size = 0);

    void popup_menu(QMenu* menu, QPoint pos);
    void construct_view_item_menus(QMenu* menu, const QList<ViewItem*>& view_items, bool flatten_menu = false, bool add_empty = true);

    void update_edge_menu(QMenu* parent_menu, QMenu* menu, const QList<ViewItem*>& connect_source_items, const QList<ViewItem*>& connect_target_items, const QList<ViewItem*>& disconnect_items);

    QWidgetAction* construct_menu_search(QMenu* parent);
    QWidgetAction* get_load_more_action(QMenu* parent);

    bool menu_requires_update(QMenu* menu);
    void menu_updated(QMenu* menu);
    void menu_focussed(QMenu* menu);

    QAction* get_no_valid_items_action(QMenu* menu, const QString& label = "No Valid Entities");

    QAction* construct_base_action(QMenu* menu, const QString& label);
    QAction* construct_remove_all_action(QMenu* menu, int number);
    QAction* construct_viewitem_action(ViewItem* item, QMenu* menu = nullptr);
    QMenu* construct_viewitem_menu(ViewItem* item, QMenu* parent_menu = nullptr);
    
    ViewController* view_controller = nullptr;
    ActionController* action_controller = nullptr;

    QMenu* main_menu = nullptr;
    QMenu* add_node_menu = nullptr;
    QMenu* add_edge_menu = nullptr;
    QMenu* deploy_menu = nullptr;
    QMenu* dock_add_node_menu = nullptr;
    QMenu* dock_deploy_menu = nullptr;
    QMenu* chart_data_kind_menu = nullptr;
    QMenu* trigger_edge_menu = nullptr;

    QHash <NODE_KIND, QAction*> add_node_action_hash;
    QHash <EDGE_KIND, QMenu*> add_edge_menu_hash;
    QHash <QPair<EDGE_DIRECTION, EDGE_KIND> , QMenu*> add_edge_menu_direct_hash;
    QHash<NODE_KIND, EDGE_KIND> connect_node_edge_kinds;

    QSet<QMenu*> valid_menus;

    struct DeployTitles {
        QAction* disconnect_title = nullptr;
        QAction* connect_to_title = nullptr;
        QAction* connect_from_title = nullptr;
    };

    QHash <QMenu*, QWidgetAction*> add_labels;
    QHash<QMenu*, QWidgetAction*> load_more_actions_;
    QHash<QMenu*, QWidgetAction*> search_actions_;
    QHash <QMenu*, DeployTitles*> title_actions;

    NodePosition node_position = NodePosition::INDEX;
    QPointF model_point;

    int node_index = -1;
    int current_hovered_id = 0;
};

#endif // CONTEXTMENU_H
