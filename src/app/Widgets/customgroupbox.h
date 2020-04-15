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
    explicit CustomGroupBox(const QString& title = "", QWidget *parent = nullptr);

    virtual void setTitle(const QString& title);
    QString getTitle();

    void setCheckable(bool checkable);
    void setChecked(bool checked);
    bool isChecked();

    QAction* addWidget(QWidget* widget);
    QAction* insertWidget(QAction* beforeAction, QWidget* widget);

public slots:
    void themeChanged();

private:
    void setupLayout();

    QToolButton* groupTitleButton = nullptr;
    QToolBar* widgetsToolbar = nullptr;
    QToolBar* topToolbar = nullptr;

};

#endif // CUSTOMGROUPBOX_H
