#ifndef XMIIMPORTER_H
#define XMIIMPORTER_H
#include <QObject>
#include "xmitreemodel.h"
#include "../CUTS/cutsmanager.h"



class XMIImporter: public QObject{
    Q_OBJECT

public:
    XMIImporter(CUTSManager* cutsManager,QWidget* parent=0);

signals:
    void loadingStatus(bool loading, QString statusText="");
    void requestXMLFromXMI(QString XMIPath);
    void requestGraphMLFromXMI(QString XMIPath, QStringList selectedClassIDs);

    void importGraphml(QString);
    void gotXMIGraphML(bool, QString, QString);
private slots:
    void importXMI(QString XMIPath);
    void gotXMIXML(bool success, QString errorString, QString outputxml);

    void importSelectedXMI(QStringList selectedClasses);

private:
    QString filePath;
    QWidget* parentWidget;
    CUTSManager* cutsManager;
};

#endif //XMIIMPORTER_H
