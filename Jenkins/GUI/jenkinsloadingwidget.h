#ifndef JENKINSLOADINGWIDGET_H
#define JENKINSLOADINGWIDGET_H

#include <QWidget>
#include <QLabel>

class JenkinsLoadingWidget : public QWidget
{
    Q_OBJECT
public:
    JenkinsLoadingWidget(QWidget *parent = 0);
    void setWaiting(bool waiting = false);
    void hideLoadingBar();

public slots:
    void authenticationFinished();
private:
    QWidget* iconBar;
    QWidget* loadingBar;
    QLabel* loadingLabel;

};

#endif // JENKINSLOADINGWIDGET_H
