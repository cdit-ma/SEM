#ifndef CONTEXTMENU_H
#define CONTEXTMENU_H

#include <QMenu>
#include <QHash>
#include <QAction>

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
private:
    void themeChanged();
    void setupMenus();
    void action_triggered(QAction* action);

    void update_add_edge_menu(QMenu* menu);
    void update_add_node_menu(QMenu* menu);
    void populate_dynamic_add_node_menu(QMenu* menu);
    void populate_dynamic_add_edge_menu(QMenu* menu);
    QMenu* construct_menu(QMenu* parent, QString label);
private:
    QAction* get_view_item_action(QMenu* parent, ViewItem* item);
    QMenu* get_view_item_menu(QMenu* parent, ViewItem* item);
    
    QHash<int, QAction*> view_item_actions;
    QHash<int, QMenu*> view_item_menu;

    ViewController* view_controller;
    
    QMenu* get_add_edge_menu(EDGE_DIRECTION direction, EDGE_KIND edge_kind);
    QMenu* get_remove_edge_menu(EDGE_KIND edge_kind);

    QAction* add_node_action = 0;
    QAction* add_edge_action = 0;
    QAction* remove_edge_action = 0;
    
    QMenu* main_menu = 0;
    QMenu* add_node_menu = 0;
    QMenu* add_edge_menu = 0;
    QMenu* remove_edge_menu = 0;
    
    QHash <EDGE_KIND, QMenu*> remove_edge_menu_hash;
    QHash <EDGE_KIND, QMenu*> add_edge_menu_hash;
    
    QHash <QPair<EDGE_DIRECTION, EDGE_KIND> , QMenu*> add_edge_menu_direct_hash;
    
    QHash <NODE_KIND, QMenu*> add_node_menu_hash;
    QHash <NODE_KIND, QAction*> add_node_action_hash;



    QHash<NODE_KIND, EDGE_KIND> connect_node_edge_kinds;
};

#endif // CONTEXTMENU_H
