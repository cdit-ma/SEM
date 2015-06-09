#ifndef JENKINSLOADINGWIDGET_H
#define JENKINSLOADINGWIDGET_H

#include <QWidget>

class JenkinsLoadingWidget : public QWidget
{
    Q_OBJECT
public:
    JenkinsLoadingWidget(QWidget *parent = 0);
    void hideLoadingBar();
private:
    QWidget* iconBar;
    QWidget* loadingBar;

};

#endif // JENKINSLOADINGWIDGET_H
