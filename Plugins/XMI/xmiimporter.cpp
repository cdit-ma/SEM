#include "xmiimporter.h"
#include <QTreeView>
#include <QStandardItem>
#include <QXmlStreamReader>
#include <QFile>
#include <QFileInfo>
#include <QPushButton>
#include <QVBoxLayout>
#include <QDialog>
#include "../../View/theme.h"
#include "GUI/XMITreeViewDialog.h"

XMIImporter::XMIImporter(QWidget *parent):QObject(parent)
{
    parentWidget = parent;

}

void XMIImporter::importXMIFile(QString xmi_file)
{
    QFile xmlFile(xmi_file);
    QFileInfo xmlFileInfo(xmlFile);

    if(!xmlFile.open(QIODevice::ReadOnly | QIODevice::Text)){
        return;
    }

    QXmlStreamReader xml(&xmlFile);

    XMITreeModel *model = new XMITreeModel();

    QStandardItem *currentItem = model->invisibleRootItem();
    while(!xml.atEnd()){
        //Read each line of the xml document.
        xml.readNext();

        //Get the tagName
        QString tagName = xml.name().toString();


         if(xml.isStartElement()){
             QString name = xml.attributes().value("name").toString();
             if(tagName == "doc"){
                 name = xmlFileInfo.fileName();
             }

             QString id = xml.attributes().value("id").toString();

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



}
