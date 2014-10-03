#ifndef PROJECTWINDOW_H
#define PROJECTWINDOW_H

#include <QMdiSubWindow>
#include "../Controller/newcontroller.h"
#include "nodeview.h"

class ProjectWindow : public QMdiSubWindow
{
    Q_OBJECT
public:
    explicit ProjectWindow(QWidget *parent = 0);
    ~ProjectWindow();
    NodeView* getView();
    NewController* getController();
signals:

public slots:

private:
    NodeView* view;
    NewController* controller;



    // QWidget interface
protected:
    void closeEvent(QCloseEvent *);
};

#endif // PROJECTWINDOW_H
