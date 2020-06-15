#ifndef WELCOMESCREENWIDGET_H
#define WELCOMESCREENWIDGET_H

#include <QWidget>
#include <QToolButton>
#include <QToolBar>
#include <QLabel>

class ActionController;

class WelcomeScreenWidget : public QWidget
{
    Q_OBJECT

public:
    explicit WelcomeScreenWidget(ActionController* action_controller, QWidget *parent = nullptr);

signals:
    void actionTriggered(QAction*);

private:
    void themeChanged();
    void recentProjectsUpdated();

    QToolButton* getButton(QAction* action, Qt::ToolButtonStyle style = Qt::ToolButtonTextBesideIcon);

    ActionController* action_controller = nullptr;

    QToolButton* recent_project_label = nullptr;
    QToolBar* recent_project_toolbar = nullptr;
    QToolBar* left_toolbar = nullptr;
    QToolBar* bottom_toolbar = nullptr;

    QLabel* medea_icon = nullptr;
    QLabel* medea_label = nullptr;
    QLabel* medea_version_label = nullptr;
};

#endif // WELCOMESCREENWIDGET_H