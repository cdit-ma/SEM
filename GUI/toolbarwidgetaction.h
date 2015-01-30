#ifndef TOOLBARWIDGETACTION_H
#define TOOLBARWIDGETACTION_H

#include "../Model/node.h"

#include <QWidgetAction>
#include <QPushButton>


class ToolbarWidgetAction : public QWidgetAction
{
    Q_OBJECT
public:
    explicit ToolbarWidgetAction(QString nodeKind, QWidget *parent = 0);
    explicit ToolbarWidgetAction(Node* node, QWidget *parent = 0);

    Node* getNode();
    QString getKind();

protected:
    QWidget* createWidget(QWidget *parent);

signals:

public slots:
    void actionButtonPressed();

private:
    Node* node;
    QString kind;
    QString label;
    QPushButton* actionButton;

};

#endif // TOOLBARWIDGETACTION_H
