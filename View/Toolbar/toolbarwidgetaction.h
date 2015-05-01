#ifndef TOOLBARWIDGETACTION_H
#define TOOLBARWIDGETACTION_H

#include "../../Model/node.h"
#include "toolbarwidget.h"

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
    explicit ToolbarWidgetAction(Node* node, QWidget *parent = 0, QString actionKind = "");

    void setMenu(ToolbarWidgetMenu* widgetMenu);
    ToolbarWidgetMenu* getMenu();

    Node* getNode();
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
    void actionButtonPressed();
    void actionButtonClicked();

    void menuOpened();
    void menuClosed();

private:
    Node* node;

    QString kind;
    QString label;

    QPushButton* actionButton;
    ToolbarWidgetMenu* widgetMenu;
    ToolbarWidgetMenu* prevWidgetMenu;

    bool willHaveMenu;
    bool deletable;

};

#endif // TOOLBARWIDGETACTION_H
