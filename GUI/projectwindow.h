#ifndef PROJECTWINDOW_H
#define PROJECTWINDOW_H

#include <QMdiSubWindow>
#include "../Controller/newcontroller.h"
#include "nodeview.h"
#include "filterbutton.h"
#include "nodetableview.h"
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
    void updateFilterButtons(QVector<FilterButton*>);
    void updateAspectButtons(QVector<FilterButton*>);
    void setTableView(QAbstractTableModel* model);

public slots:

    void treeViewItemSelected(QModelIndex index);
    void treeViewItemCentered(QModelIndex index);

    void selectedProject();
    void appendFilterString(QString filter);
    void removeFilterString(QString filter);
    void clearFilters();

    void appendAspectString(QString aspect);
    void removeAspectString(QString aspect);
    void clearAspects();

    void updateWindowTitle(QString title);

    void setVisibleAspects(QStringList aspects);

    void view_DialogWarning(MESSAGE_TYPE type, QString output);

private:
    QVector<FilterButton*> appliedFilterButtons;
    QStringList appliedFilters;
    QVector<FilterButton*> appliedAspectButtons;
    QStringList visibleAspects;
    QThread *thread;
    NodeView* view;
    //NodeTableView* tableView;
    NodeView* view2;
    NewController* controller;



    // QWidget interface
protected:
    void closeEvent(QCloseEvent *);
};

#endif // PROJECTWINDOW_H
