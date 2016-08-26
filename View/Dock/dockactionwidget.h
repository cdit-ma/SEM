#ifndef DOCKACTIONWIDGET_H
#define DOCKACTIONWIDGET_H

#include <QPushButton>
#include <QLabel>
#include <QAction>
#include <QVBoxLayout>
#include <QHBoxLayout>

class DockActionWidget : public QPushButton
{
    Q_OBJECT
public:
    explicit DockActionWidget(QAction* action, QWidget *parent = 0);
    void requiresSubAction(bool required);
    QAction* getAction();

signals:

public slots:
    void actionChanged();
    void themeChanged();

private:
    void setupLayout();

    QVBoxLayout* mainLayout;
    QLabel* textLabel;
    QLabel* iconLabel;
    QLabel* arrowLabel;
    QLabel* imageLabel;

    QAction* dockAction;
    bool subActionRequired;

};

#endif // DOCKACTIONWIDGET_H
