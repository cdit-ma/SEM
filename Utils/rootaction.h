#ifndef ROOTACTION_H
#define ROOTACTION_H

#include <QList>
#include <QObject>
#include <QAction>

class RootAction : public QAction
{
    Q_OBJECT
public:
    RootAction(QString category, QString text, QObject* parent = 0);
    void setIconPath(QString path, QString alias);
    QPair<QString, QString> getIconPair() const;
    QString getIconPath() const;
    QString getIconAlias() const;
    QString getCategory() const;

    QAction* constructSubAction(bool stealth=true);

private slots:
    void actionChanged();
    void actionRemoved(QObject* obj);
private:
    void copyActionState(QAction* action, bool stealth);
    QList<QAction*> subActions;
    QList<QAction*> stealthActions;

    QString iconPath;
    QString category;
    QString iconAlias;
};

#endif // ROOTACTION_H
