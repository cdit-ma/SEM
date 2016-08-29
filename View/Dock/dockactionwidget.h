#ifndef DOCKACTIONWIDGET_H
#define DOCKACTIONWIDGET_H

#include <QPushButton>
#include <QLabel>
#include <QAction>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include "../theme.h"

class DockActionWidget : public QPushButton
{
    Q_OBJECT
public:
    explicit DockActionWidget(QAction* action, QWidget *parent = 0);

    QAction* getAction();

    void setSubActionRequired(bool required);
    bool requiresSubAction();

    void setProperty(const char *name, const QVariant &value);
    QVariant getProperty(const char *name);

signals:

public slots:
    void actionChanged();
    void themeChanged();

protected:
    void enterEvent(QEvent* event);
    void leaveEvent(QEvent* event);

private:
    void setupLayout();

    QVBoxLayout* mainLayout;
    QLabel* textLabel;
    QLabel* iconLabel;
    QLabel* arrowLabel;
    QLabel* imageLabel;

    QAction* dockAction;
    bool subActionRequired;

    Theme* theme;

};

#endif // DOCKACTIONWIDGET_H
