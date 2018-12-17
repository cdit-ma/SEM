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
    explicit WelcomeScreenWidget(ActionController* action_controller, QWidget *parent = 0);

signals:
    void actionTriggered(QAction*);
private:
    void themeChanged();
    void recentProjectsUpdated();
private:
    QToolButton* getButton(QAction* action, Qt::ToolButtonStyle style = Qt::ToolButtonTextBesideIcon);

    ActionController* action_controller = 0;
    QToolButton* recent_project_label = 0;
    QToolBar* recent_project_toolbar = 0;
    QToolBar* left_toolbar = 0;
    QToolBar* bottom_toolbar = 0;
    QLabel* medea_icon = 0;
    QLabel* medea_label = 0;
    QLabel* medea_version_label = 0;
};

#endif // WELCOMESCREENWIDGET_H
