#ifndef MEDEADOCKWIDGET_H
#define MEDEADOCKWIDGET_H

#include <QDockWidget>
#include "../../View/docktitlebarwidget.h"

class MedeaWindowNew;
class MedeaDockWidget : public QDockWidget
{
    Q_OBJECT
public:
    MedeaDockWidget(QString title="", QWidget* parent=0);

    void setSourceWindow(MedeaWindowNew* window);
    MedeaWindowNew* getSourceWindow();

    void setCurrentWindow(MedeaWindowNew* window);
    MedeaWindowNew* getCurrentWindow();

    void setIcon(QPixmap icon);
    void setTitle(QString title, Qt::Alignment alignment = Qt::AlignCenter);
    QString getTitle();


    void setCloseVisible(bool visible);
    void setMaximizeVisible(bool visible);
    void setPopOutVisible(bool visible);

    void setMaximizeToggled(bool toggled);
    void setPopOutToggled(bool toggled);

signals:
    void maximizeWidget(bool maximize);
    void popOutWidget(bool popout);
    void closeWidget();
private slots:
    void themeChanged();
private:
    void setActionVisible(DockTitleBarWidget::DOCK_ACTION action, bool visible);
    void setActionToggled(DockTitleBarWidget::DOCK_ACTION action, bool toggled);
    QAction* getAction(DockTitleBarWidget::DOCK_ACTION action);

    DockTitleBarWidget* titleBar;

    MedeaWindowNew* sourceWindow;
    MedeaWindowNew* currentWindow;
};

#endif // MEDEADOCKWIDGET_H
