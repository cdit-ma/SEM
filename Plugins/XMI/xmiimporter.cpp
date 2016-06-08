#include "xmiimporter.h"
#include <QTreeView>
#include <QStandardItem>
#include <QXmlStreamReader>
#include <QFile>
#include <QFileInfo>
#include <QPushButton>
#include <QVBoxLayout>
#include <QDialog>
#include <QDebug>
#include "../../View/theme.h"
#include "GUI/XMITreeViewDialog.h"
#include <QStringBuilder>

XMIImporter::XMIImporter(CUTSManager *cutsManager, QWidget *parent):QObject(parent)
{
    parentWidget = parent;
    this->cutsManager = cutsManager;

    connect(this, SIGNAL(requestXMLFromXMI(QString)), cutsManager, SLOT(executeXMI2XML(QString)));
    connect(cutsManager, SIGNAL(gotXMIXML(bool,QString,QString)), this, SLOT(gotXMIXML(bool,QString,QString)));

    connect(this, SIGNAL(requestGraphMLFromXMI(QString,QStringList)), cutsManager, SLOT(executeXMI2GraphML(QString,QStringList)));
    connect(cutsManager, SIGNAL(gotXMIGraphML(bool,QString,QString)), this, SIGNAL(gotXMIGraphML(bool,QString,QString)));

}

void XMIImporter::importXMI(QString XMIPath)
{
    filePath = XMIPath;
    emit loadingStatus(true, "Transforming XMI 2 XML");
    emit requestXMLFromXMI(XMIPath);
}


void XMIImporter::gotXMIXML(bool success, QString errorString, QString outputxml)
{
    emit loadingStatus(false);

    QXmlStreamReader xml(outputxml);

    XMITreeModel *model = new XMITreeModel();

    QStandardItem *currentItem = model->invisibleRootItem();
    while(!xml.atEnd()){
        //Read each line of the xml document.
        xml.readNext();

        //Get the tagName
        QString tagName = xml.name().toString();


         if(xml.isStartElement()){
             QString id = xml.attributes().value("id").toString();
             QString name = xml.attributes().value("name").toString();

             if(tagName == "model"){
                 name = "model";
             }else if(tagName == "class"){
                 QString attributeCount = xml.attributes().value("attribute_count").toString();
                 name += " [" % attributeCount % "]";
             }

             QStandardItem *newItem = new QStandardItem(name);
             newItem->setData(name, XMI_NAME);
             newItem->setData(tagName, XMI_TYPE);
             newItem->setData(id, XMI_ID);
             newItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
             currentItem->appendRow(newItem);
             currentItem = newItem;
         }
         if(xml.isEndElement()){
             //Go up a parent.
             currentItem = currentItem->parent();
         }
    }

    XMITreeViewDialog* dialog = new XMITreeViewDialog(model, parentWidget);
    connect(dialog, SIGNAL(importClasses(QStringList)), this, SLOT(importSelectedXMI(QStringList)));
}

void XMIImporter::importSelectedXMI(QStringList selectedClasses)
{
    emit loadingStatus(true, "Transforming XMI 2 Graphml");
    emit requestGraphMLFromXMI(filePath, selectedClasses);
}
