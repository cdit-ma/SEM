#ifndef PROJECTWINDOW_H
#define PROJECTWINDOW_H

#include <QMdiSubWindow>
#include "../Controller/newcontroller.h"
#include "nodeview.h"
#include <QThread>
class ProjectWindow : public QMdiSubWindow
{
    Q_OBJECT
public:
    explicit ProjectWindow(QWidget *parent = 0);
    ~ProjectWindow();
    NodeView* getView();
    NewController* getController();
signals:
    void updateFilters(QStringList filters);

public slots:
    void appendFilterString(QString filter);
    void removeFilterString(QString filter);
    void clearFilters();

private:
    QStringList filters;
    QThread *thread;
    NodeView* view;
    NewController* controller;



    // QWidget interface
protected:
    void closeEvent(QCloseEvent *);
};

#endif // PROJECTWINDOW_H
