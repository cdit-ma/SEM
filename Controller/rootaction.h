#ifndef ROOTACTION_H
#define ROOTACTION_H

#include <QAction>
class RootAction : public QAction
{
    Q_OBJECT
public:
    RootAction(QString text, QObject* parent = 0);
    void setIconPath(QString path, QString alias);
    QString getIconPath();
    QString getIconAlias();
    QAction* getStealthAction();
private slots:
    void actionChanged();
private:
    QAction* stealthAction;
    QString iconPath;
    QString iconAlias;
};

#endif // ROOTACTION_H
