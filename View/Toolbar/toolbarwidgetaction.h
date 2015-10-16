#ifndef TOOLBARWIDGETACTION_H
#define TOOLBARWIDGETACTION_H

#include "../../Model/node.h"
#include "toolbarwidget.h"
#include "toolbarabstractbutton.h"

#include <QWidget>
#include <QWidgetAction>
#include <QPushButton>
#include <QLabel>


class ToolbarWidgetMenu;

class ToolbarWidgetAction : public QWidgetAction
{
    Q_OBJECT
public:
    explicit ToolbarWidgetAction(QString nodeKind, QString textLabel = "", ToolbarWidgetMenu* parent = 0);
    explicit ToolbarWidgetAction(NodeItem* nodeItem, ToolbarWidgetMenu* parent = 0, bool willHaveMenu = false);

    void setMenu(ToolbarWidgetMenu* widgetMenu);
    ToolbarWidgetMenu* getMenu();

    ToolbarAbstractButton* getButton();
    QPoint getButtonPos();

    NodeItem* getNodeItem();
    int getNodeItemID();

    QString getActionKind();
    bool isDeletable();

    ToolbarWidgetMenu* getParentMenu();
    ToolbarWidgetAction* getTopMostParentAction();

protected:
    QWidget* createWidget(QWidget *parent);

signals:
    void toolbarAction_pressed();

public slots:
    void hover();
    void setParentMenuFocus(bool hasFocus);

    void actionButtonPressed();
    void actionButtonClicked();
    void resetActionButton();

private:
    NodeItem* nodeItem;
    ToolbarWidgetMenu* parentMenu;

    QString kind;
    QString label;

    ToolbarAbstractButton* actionButton;
    ToolbarWidgetMenu* widgetMenu;
    ToolbarWidgetMenu* prevWidgetMenu;

    bool willHaveMenu;
    bool deletable;
    bool parentMenuHasFocus;

};

#endif // TOOLBARWIDGETACTION_H
