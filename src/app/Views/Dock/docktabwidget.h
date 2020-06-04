#ifndef DOCKTABWIDGET_H
#define DOCKTABWIDGET_H

#include <QWidget>
#include <QStackedWidget>
#include <QScrollArea>

#include "../../Controllers/ViewController/viewcontroller.h"

class DockTabWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DockTabWidget(ViewController* vc, QWidget* parent = nullptr);

protected:
    void refreshSize();
    bool eventFilter(QObject *object, QEvent *event) override;

private:
    void themeChanged();
    void setupLayout();
    void setupDocks();
    void dockActionTriggered(QAction* action);

    ViewController* view_controller = nullptr;

    QMenu* add_part_menu = nullptr;
    QMenu* deploy_menu = nullptr;
    
    QScrollArea* deploy_dock = nullptr;
    QScrollArea* parts_dock = nullptr;

    QAction* parts_action = nullptr;
    QAction* deploy_action = nullptr;
    QToolBar* toolbar = nullptr;
    
    QStackedWidget* stack_widget = nullptr;
};

#endif // DOCKTABWIDGET_H
