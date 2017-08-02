#ifndef CUSTOMGROUPBOX_H
#define CUSTOMGROUPBOX_H

#include <QFrame>
#include <QToolBar>
#include <QToolButton>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

class CustomGroupBox : public QFrame
{
    Q_OBJECT
public:
    explicit CustomGroupBox(QString title = "", QWidget *parent = 0);

    virtual void setTitle(QString title);
    QString getTitle();

    void setChecked(bool checked);
    bool isChecked();

    QAction* addWidget(QWidget* widget);
    QAction* insertWidget(QAction* beforeAction, QWidget* widget);

    QAction* getTopAction();

public slots:
    void themeChanged();

private:
    void setupLayout();

    QString groupTitle;
    QToolButton* groupTitleButton;
    QToolBar* widgetsToolbar;

};

#endif // CUSTOMGROUPBOX_H
