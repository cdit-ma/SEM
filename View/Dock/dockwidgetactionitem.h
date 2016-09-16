#ifndef DockWidgetActionItem_H
#define DockWidgetActionItem_H

#include <QLabel>
#include <QAction>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include "dockwidgetitem.h"

class DockWidgetActionItem : public DockWidgetItem
{
    Q_OBJECT

public:
    explicit DockWidgetActionItem(QAction* action, QWidget* parent = 0);

    QAction* getAction();
    DOCKITEM_KIND getItemKind();

    void setSubActionRequired(bool required);
    bool requiresSubAction();

    void setProperty(const char *name, const QVariant &value);
    QVariant getProperty(const char *name);

    void highlightItem(bool highlight);

public slots:
    void actionChanged();
    void themeChanged();
    void updateDisplayedText(QString text);

protected:
    void enterEvent(QEvent* event);
    void leaveEvent(QEvent* event);

private:
    void setupLayout();
    void updateStyleSheet();

    QAction* dockAction;
    int dockActionID;

    bool subActionRequired;
    bool highlighted;

    QLabel* textLabel;
    QLabel* iconLabel;
    QLabel* arrowLabel;
    QLabel* imageLabel;

    Theme* theme;
    QString backgroundColorHex;
    QString colorHex;
};

#endif // DockWidgetActionItem_H
