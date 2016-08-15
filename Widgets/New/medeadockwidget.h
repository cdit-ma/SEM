#ifndef MEDEADOCKWIDGET_H
#define MEDEADOCKWIDGET_H

#include <QDockWidget>
#include "../../View/docktitlebarwidget.h"

class MedeaWindowNew;
class MedeaDockWidget : public QDockWidget
{
public:
    enum DOCKWIDGET_TYPE {MDW_VIEW, MDW_TOOL};
    Q_OBJECT
    friend class MedeaWindowManager;
protected:
    MedeaDockWidget(DOCKWIDGET_TYPE type);
    ~MedeaDockWidget();

public:
    int getID();
    DOCKWIDGET_TYPE getDockType();
    Qt::DockWidgetArea getDockWidgetArea();
    void setDockWidgetArea(Qt::DockWidgetArea area);
    void setSourceWindow(MedeaWindowNew* window);
    MedeaWindowNew* getSourceWindow();

    bool isProtected();
    void setProtected(bool protect);
    virtual void setWidget(QWidget* widget);
    void setCurrentWindow(MedeaWindowNew* window);
    MedeaWindowNew* getCurrentWindow();

    void setIcon(QPair<QString, QString> pair);
    void setIcon(QString prefix, QString alias);
    void setTitle(QString title, Qt::Alignment alignment = Qt::AlignLeft);
    QString getTitle();

    void setActive(bool focussed);
    bool isActive();

    void setFocusEnabled(bool enabled);
    bool isFocusEnabled();
    void setCloseVisible(bool visible);
    void setHideVisible(bool visible);
    void setMaximizeVisible(bool visible);
    void setPopOutVisible(bool visible);
    void setProtectVisible(bool visible);

    void setMaximizeToggled(bool toggled);
    void setPopOutToggled(bool toggled);
    void setProtectToggled(bool toggled);

signals:
    void maximizeWidget(bool maximize);
    void popOutWidget();
    void closeWidget();

private slots:
    void destruct();
    void themeChanged();
    void showContextMenu(const QPoint &point);

private:
    void updateActiveStyleSheet();
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
    DOCKWIDGET_TYPE type;
    bool _isProtected;
    bool _isActive;
    bool _isFocusEnabled;
    QString highlightedTextColor;
    int ID;
    static int _DockWidgetID;

};


#endif // MEDEADOCKWIDGET_H
