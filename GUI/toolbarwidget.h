#ifndef TOOLBARWIDGET_H
#define TOOLBARWIDGET_H

#include "nodeitem.h"

#include <QWidget>
#include <QToolButton>


class ToolbarWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ToolbarWidget(QWidget *parent = 0);

    void connectToView();
    void setNodeItem(NodeItem* item);

signals:
    void goToDefinition(Node* node);
    void goToImplementation(Node* node);

public slots:
    void goToDefinition();
    void goToImplementation();

private:
    void setupToolBar();

    NodeItem* nodeItem;

    QToolButton* addChildButton;
    QToolButton* deleteButton;
    QToolButton* connectButton;

    QToolButton* definitionButton;
    QToolButton* implementationButton;

};

#endif // TOOLBARWIDGET_H
