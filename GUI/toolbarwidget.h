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

public slots:

private:
    void setupToolBar();

    NodeItem* nodeItem;

    QToolButton* addChildButton;
    QToolButton* deleteButton;
    QToolButton* connectButton;

};

#endif // TOOLBARWIDGET_H
