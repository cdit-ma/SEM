#ifndef XMIIMPORTER_H
#define XMIIMPORTER_H
#include <QObject>
#include "xmitreemodel.h"




class XMIImporter: public QObject{
    Q_OBJECT

public:
    XMIImporter(QWidget* parent=0);

private slots:
    void importXMIFile(QString xmi_file);

private:
    QWidget* parentWidget;
};

#endif //XMIIMPORTER_H
