#ifndef BASEDOCKWIDGET_H
#define BASEDOCKWIDGET_H

#include <QDockWidget>
#include "docktitlebar.h"

class BaseWindow;
class BaseDockWidget : public QDockWidget
{
    Q_OBJECT
public:
    enum DOCKWIDGET_TYPE {MDW_VIEW, MDW_TOOL};
    friend class WindowManager;
protected:
    BaseDockWidget(DOCKWIDGET_TYPE type);
    ~BaseDockWidget();

public:
    void setTitleIcon(QString path, QString alias);
    int getID();
    DOCKWIDGET_TYPE getDockType();
    Qt::DockWidgetArea getDockWidgetArea();
    void setDockWidgetArea(Qt::DockWidgetArea area);
    void setSourceWindow(BaseWindow* window);
    BaseWindow* getSourceWindow();

    virtual void themeChanged() = 0;

    QPair<QString, QString> getIcon();
    DockTitleBar* getTitleBar();
    void setTitleBarIconSize(int height);
    bool isProtected();
    void setProtected(bool protect);
    virtual void setWidget(QWidget* widget);
    void setCurrentWindow(BaseWindow* window);
    BaseWindow* getCurrentWindow();

    void setIcon(QPair<QString, QString> pair);
    void setIcon(QString, QString);
    void setTitle(QString title, Qt::Alignment alignment = Qt::AlignLeft);
    QString getTitle();

    virtual void setActive(bool focussed);
    bool isActive();

    void setMaximized(bool maximized);

    void setFocusEnabled(bool enabled);
    bool isFocusEnabled();
    void setIconVisible(bool visible);

    void setCloseVisible(bool visible);
    void setHideVisible(bool visible);
    void setMaximizeVisible(bool visible);
    void setPopOutVisible(bool visible);
    void setProtectVisible(bool visible);

    void setMaximizeToggled(bool toggled);
    void setMaximizeEnabled(bool enabled);
    void setPopOutToggled(bool toggled);
    void setProtectToggled(bool toggled);

    void close();
signals:
    void iconChanged();
    void titleChanged();
    void dockSetActive(bool);
    void req_Maximize(int ID, bool maximize);
    void req_Visible(int ID, bool visible);
    void req_PopOut(int ID);
    void req_Close(int ID);
private slots:
    void title_Maximize(bool maximize);
    void title_Visible(bool visible);
    void title_PopOut(bool);
    void title_Close(bool);


    void _visibilityChanged(bool visible);

    void destruct();
    void showContextMenu(const QPoint &point);

private:
    void closeOrHide();
    void setActionVisible(DockTitleBar::DOCK_ACTION action, bool visible);
    void setActionToggled(DockTitleBar::DOCK_ACTION action, bool toggled);
    void setActionEnabled(DockTitleBar::DOCK_ACTION action, bool enabled);
    QAction* getAction(DockTitleBar::DOCK_ACTION action);

    DockTitleBar* titleBar;
    Qt::DockWidgetArea initialArea;

    BaseWindow* sourceWindow;
    BaseWindow* currentWindow;

    QPair<QString, QString> titleIcon;

    QFrame* borderFrame;

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


    // QWidget interface
protected:
    void resizeEvent(QResizeEvent *event);
};


#endif // BASEDOCKWIDGET_H
