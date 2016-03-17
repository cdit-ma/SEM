#ifndef TOOLBARMENUACTION_H
#define TOOLBARMENUACTION_H

#include "toolbarwidget.h"
#include <QAction>

class ToolbarMenu;

class ToolbarMenuAction : public QAction
{
    Q_OBJECT

public:
    explicit ToolbarMenuAction(NodeItem* item, ToolbarMenuAction *parent_action = 0, QWidget* parent = 0);
    explicit ToolbarMenuAction(QString kind, ToolbarMenuAction *parent_action = 0, QWidget* parent = 0,
                               QString displayedText = "", QString prefixPath = "", QString aliasPath = "");

    ToolbarMenuAction* getParentAction();
    QString getParentActionKind();

    NodeItem* getNodeItem();
    int getNodeItemID();

    QString getActionKind();
    bool isDeletable();

public slots:
    void themeChanged();
private:
    void updateIcon();
    ToolbarMenuAction* parentAction;
    NodeItem* nodeItem;
    QString prefixPath;
    QString actionKind;
    bool deletable;
};

#endif // TOOLBARMENUACTION_H
