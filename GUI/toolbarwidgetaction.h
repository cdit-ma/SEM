#ifndef TOOLBARWIDGETACTION_H
#define TOOLBARWIDGETACTION_H

#include "../Model/node.h"
#include "toolbarwidget.h"

#include <QWidgetAction>
#include <QPushButton>
#include <QLabel>


class ToolbarWidgetAction : public QWidgetAction
{
    Q_OBJECT
public:
    explicit ToolbarWidgetAction(QString nodeKind,  ToolbarWidget* parent = 0);
    explicit ToolbarWidgetAction(Node* node, QWidget *parent = 0);

    void activate();

    Node* getNode();
    QString getKind();

protected:
    QWidget* createWidget(QWidget *parent);

signals:

public slots:
    void hover();
    void actionButtonClicked();

private:

    Node* node;
    QString kind;
    QString label;
    QPushButton* actionButton;

    bool actionHovered;

    QLabel* textLabel;

};

#endif // TOOLBARWIDGETACTION_H
