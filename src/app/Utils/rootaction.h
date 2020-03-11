#ifndef ROOTACTION_H
#define ROOTACTION_H

#include <QList>
#include <QAction>

class RootAction : public QAction
{
    Q_OBJECT
    
public:
    RootAction(const QString& category, const QString& text, QObject* parent = nullptr);
    
    void setIconPath(const QString& path, const QString& alias);
    QPair<QString, QString> getIconPair() const;
    QString getIconPath() const;
    QString getIconAlias() const;

    QAction* constructSubAction(bool stealth = true);

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
