#ifndef TOOLBARWIDGETACTION_H
#define TOOLBARWIDGETACTION_H

#include "../Model/node.h"
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

    QPushButton* getButton();
    QPoint getButtonPos();

protected:
    QWidget* createWidget(QWidget *parent);

signals:

public slots:
    void hover();
    void actionButtonClicked();
    void actionButtonUnclicked();

private:
    Node* node;

    QString kind;
    QString label;

    QPushButton* actionButton;
    ToolbarWidgetMenu* widgetMenu;

    bool willHaveMenu;

};

#endif // TOOLBARWIDGETACTION_H
