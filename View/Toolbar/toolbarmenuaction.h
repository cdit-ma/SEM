#ifndef TOOLBARMENUACTION_H
#define TOOLBARMENUACTION_H

#include "toolbarwidget.h"
#include <QAction>

class ToolbarMenu;

class ToolbarMenuAction : public QAction
{
    Q_OBJECT

public:
    explicit ToolbarMenuAction(NodeItem* item, QWidget* parent = 0);
    explicit ToolbarMenuAction(QString kind, QWidget* parent = 0,
                               QString displayedText = "", QString iconPath = ":/Items/");

    NodeItem* getNodeItem();
    QString getNodeItemID();
    QString getActionKind();
    bool isDeletable();

private:
    NodeItem* nodeItem;
    QString actionKind;
    bool deletable;
};

#endif // TOOLBARMENUACTION_H
