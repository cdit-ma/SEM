#include "XMITreeViewDialog.h"
#include "../../../View/theme.h"
#include <QDebug>


XMITreeViewDialog::XMITreeViewDialog(XMITreeModel *treeModel, QWidget *parent):QDialog(parent)
{
    //Set local parameters.
    this->parentWidget = parent;
    this->treeModel = treeModel;
    //Attach the tree model to the GUI
    treeModel->setParent(this);

    connect(treeModel, SIGNAL(selectionChanged(Qt::CheckState)), this, SLOT(selectionChanged(Qt::CheckState)));

    setWindowTitle("Select XMI Elements to Import");
    setWindowIcon(Theme::theme()->getIcon("", "UML.gif"));

    //Disable Help Button
    setWindowFlags(windowFlags() & (~Qt::WindowContextHelpButtonHint));
    setupLayout();

    treeView->setModel(treeModel);
    //Make Widget Modal
    setModal(true);
    //Select all
    treeModel->selectAll();
    show();
}


void XMITreeViewDialog::selectionChanged(Qt::CheckState overallState)
{
    bool anySelected = overallState >= Qt::PartiallyChecked;
    bool allSelected = overallState == Qt::Checked;

    button_selectNone->setEnabled(anySelected);
    button_importSelection->setEnabled(anySelected);
    button_selectAll->setDisabled(allSelected);
}

void XMITreeViewDialog::importSelection()
{
    QStringList classIDs = treeModel->getSelectedClassIDs();
    //Close the GUI
    accept();
    //Emit the signal to start the importing of elements.
    emit importClasses(classIDs);
}

void XMITreeViewDialog::setupLayout()
{
    layout_main = new QVBoxLayout(this);

    treeView = new QTreeView(this);

    layout_buttons = new QHBoxLayout();
    button_selectAll = new QPushButton("Select All", this);
    button_selectNone = new QPushButton("Select None", this);
    button_importSelection = new QPushButton("Import Selection", this);
    button_cancelImport = new QPushButton("Cancel", this);

    //Add Buttons to button layout.
    layout_buttons->addWidget(button_selectAll);
    layout_buttons->addWidget(button_selectNone);
    layout_buttons->addStretch();
    layout_buttons->addWidget(button_importSelection);
    layout_buttons->addWidget(button_cancelImport);

    //Add elements to the dialog layout.
    layout_main->addWidget(treeView, 1);
    layout_main->addLayout(layout_buttons);

    connect(button_selectAll, SIGNAL(clicked(bool)), treeModel, SLOT(selectAll()));
    connect(button_selectNone, SIGNAL(clicked(bool)), treeModel, SLOT(selectNone()));
    connect(button_cancelImport, SIGNAL(clicked(bool)), this, SIGNAL(rejected()));
    connect(button_importSelection, SIGNAL(clicked(bool)), this, SLOT(importSelection()));
}
