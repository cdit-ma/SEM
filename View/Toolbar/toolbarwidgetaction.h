#ifndef TOOLBARWIDGETACTION_H
#define TOOLBARWIDGETACTION_H

#include "../../Model/node.h"
#include "toolbarwidget.h"
#include "toolbarwidgetbutton.h"

#include <QWidget>
#include <QWidgetAction>
#include <QPushButton>
#include <QLabel>


class ToolbarWidgetMenu;

class ToolbarWidgetAction : public QWidgetAction
{
    Q_OBJECT
public:
    explicit ToolbarWidgetAction(QString nodeKind, QString textLabel = "", QWidget *parent = 0);
    explicit ToolbarWidgetAction(NodeItem* nodeItem, QWidget *parent = 0, bool willHaveMenu = false);

    void setMenu(ToolbarWidgetMenu* widgetMenu);
    ToolbarWidgetMenu* getMenu();

    NodeItem* getNodeItem();
    QString getKind();

    bool isDeletable();

    QPushButton* getButton();
    QPoint getButtonPos();

protected:
    QWidget* createWidget(QWidget *parent);

signals:
    void toolbarAction_pressed();

public slots:
    void hover();
    void endHover();
    void actionButtonPressed();
    void actionButtonClicked();

    void menuOpened();
    void menuClosed();

private:
    NodeItem* nodeItem;

    QString kind;
    QString label;

    ToolbarWidgetButton* actionButton;
    ToolbarWidgetMenu* widgetMenu;
    ToolbarWidgetMenu* prevWidgetMenu;

    bool willHaveMenu;
    bool deletable;

};

#endif // TOOLBARWIDGETACTION_H
