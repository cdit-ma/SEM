#ifndef TOOLBARWIDGETACTION_H
#define TOOLBARWIDGETACTION_H

#include "../Model/node.h"
#include "toolbarwidget.h"

#include <QWidget>
#include <QWidgetAction>
#include <QPushButton>
#include <QLabel>


class ToolbarWidgetAction : public QWidgetAction
{
    Q_OBJECT
public:
    explicit ToolbarWidgetAction(QString nodeKind,  QWidget *parent = 0);
    explicit ToolbarWidgetAction(Node* node, QWidget *parent = 0, QString actionKind = "");

    void setMenu(QMenu* menu);
    QMenu* getMenu();

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
    QMenu* menu;

};

#endif // TOOLBARWIDGETACTION_H
