#ifndef WINDOWITEM_H
#define WINDOWITEM_H

#include "../../Widgets/Windows/basewindow.h"
#include "../../Controllers/WindowManager/windowmanager.h"
#include "viewmanagerwidget.h"

#include <QToolBar>
#include <QLabel>
#include <QWidget>
#include <QVBoxLayout>

class WindowItem : public QWidget
{
     Q_OBJECT

public:
    WindowItem(ViewManagerWidget* manager, BaseWindow* window);

private slots:
    void themeChanged();
    void titleChanged(const QString& title = "");

    void dockWidgetAdded(BaseDockWidget* widget);

private:
    void setupLayout();

    ViewManagerWidget* manager = nullptr;
    BaseWindow* window = nullptr;

    QLabel* label = nullptr;
    QAction* closeAction = nullptr;
    QWidget* dockContainer = nullptr;
    QToolBar* windowToolbar = nullptr;

    QVBoxLayout* viewContainerLayout = nullptr;
    QVBoxLayout* toolContainerLayout = nullptr;
};

#endif // WINDOWITEM_H