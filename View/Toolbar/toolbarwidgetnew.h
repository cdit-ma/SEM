#ifndef TOOLBARWIDGETNEW_H
#define TOOLBARWIDGETNEW_H

#include "../../Controller/actioncontroller.h"
#include "../theme.h"

#include <QWidget>
#include <QFrame>
#include <QToolBar>
#include <QAction>
#include <QVBoxLayout>

class ToolbarWidgetNew : public QWidget
{
    Q_OBJECT
public:
    explicit ToolbarWidgetNew(ActionController *ac, QWidget *parent = 0);

signals:

public slots:
    void themeChanged();
    void setVisible(bool visible);

private:
    void setupToolbar();

    ActionController* actionController;
    QSize iconSize;

    QFrame* mainFrame;
    QFrame* shadowFrame;
    QToolBar* toolbar;
};

#endif // TOOLBARWIDGETNEW_H
