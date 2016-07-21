#ifndef MEDEADOCKWIDGET_H
#define MEDEADOCKWIDGET_H

#include <QDockWidget>
#include "../../View/docktitlebarwidget.h"

class MedeaWindowNew;
class MedeaDockWidget : public QDockWidget
{
    Q_OBJECT
    friend class MedeaWindowManager;
protected:
    MedeaDockWidget(QString title="", Qt::DockWidgetArea initalArea = Qt::TopDockWidgetArea, QWidget* parent=0);
    ~MedeaDockWidget();
public:
    int getID();
    Qt::DockWidgetArea getDockWidgetArea();
    void setSourceWindow(MedeaWindowNew* window);
    MedeaWindowNew* getSourceWindow();

    bool isProtected();
    void setProtected(bool protect);
    void setWidget(QWidget* widget);
    void setCurrentWindow(MedeaWindowNew* window);
    MedeaWindowNew* getCurrentWindow();

    void setIcon(QPixmap icon);
    void setTitle(QString title, Qt::Alignment alignment = Qt::AlignCenter);
    QString getTitle();

    void setActive(bool focussed);
    bool isActive();

    void setCloseVisible(bool visible);
    void setMaximizeVisible(bool visible);
    void setPopOutVisible(bool visible);

    void setMaximizeToggled(bool toggled);
    void setPopOutToggled(bool toggled);
signals:
    void maximizeWidget(bool maximize);
    void popOutWidget();
    void closeWidget();
private slots:
    void themeChanged();
    void showContextMenu(const QPoint &point);
private:
    void updateTitleLabelStyle();
    void setActionVisible(DockTitleBarWidget::DOCK_ACTION action, bool visible);
    void setActionToggled(DockTitleBarWidget::DOCK_ACTION action, bool toggled);
    QAction* getAction(DockTitleBarWidget::DOCK_ACTION action);

    DockTitleBarWidget* titleBar;
    Qt::DockWidgetArea initialArea;

    MedeaWindowNew* sourceWindow;
    MedeaWindowNew* currentWindow;
protected:
    bool eventFilter(QObject *object, QEvent *event);
private:
    bool _isProtected;
    bool _isActive;
    QString labelStyle_Normal;
    QString labelStyle_Focussed;
    QString highlightedTextColor;
    int ID;
    static int _DockWidgetID;

};


#endif // MEDEADOCKWIDGET_H
