#include "medeasubwindow.h"
#include "View/nodeview.h"
#include <QHeaderView>
#include <QDebug>

#define RIGHT_PANEL_WIDTH 230.0

MedeaSubWindow::MedeaSubWindow(QWidget *parent) :
    QDialog(parent)
{
    Qt::WindowFlags flags = 0;
    flags |= Qt::WindowMaximizeButtonHint;
    flags |= Qt::WindowCloseButtonHint;
    flags |= Qt::WindowMinimizeButtonHint;

    this->setWindowModality(Qt::NonModal);
    dataTable = 0;
    dataTableBox = 0;
    setWindowFlags(flags);
    resize(650,400);

    setWindowTitle("MEDEA Sub View");

    setupLayout();
}

void MedeaSubWindow::setupLayout()
{
    mainLayout = new QHBoxLayout();
    mainLayout->setMargin(0);
    setLayout(mainLayout);
}

void MedeaSubWindow::updateDataTable()
{
    if(dataTable){
        QAbstractItemModel* tableModel = dataTable->model();
        if(tableModel){
            dataTableBox->setVisible(true);
            // calculate the required height
            int height = 0;
            for (int i = 0; i < tableModel->rowCount(); i++) {
                height += dataTable->rowHeight(i);
            }

            int vOffset = dataTable->horizontalHeader()->size().height();
            int maxHeight = dataTable->height();
            int newHeight = height + vOffset;

            // check if the datatable should be hidden or if its height needs restricting
            if (maxHeight == 0) {
                dataTableBox->setVisible(false);
            }
            dataTableBox->setFixedSize(dataTable->width() + 5, newHeight);

            // update the visible region of the groupbox to fit the dataTable
            if (dataTable->height() == 0) {
                dataTableBox->setVisible(false);
            } else {
                updateWidgetMask(dataTableBox, dataTable);
            }

            // align the contents of the datatable
            dataTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
            dataTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Interactive);
            dataTable->horizontalHeader()->resizeSection(1, dataTable->width()/4);
            dataTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);

        }else{
            dataTableBox->setVisible(false);
        }
    }
}

void MedeaSubWindow::updateWidgetMask(QWidget *widget, QWidget *maskWidget)
{
    if (widget->mask().isEmpty()) {
        return;
    }

    QPointF pos = maskWidget->pos();
    widget->clearMask();
    widget->setMask(QRegion(pos.x(),
                            pos.y(),
                            maskWidget->width(),
                            maskWidget->height(),
                            QRegion::Rectangle));
}

void MedeaSubWindow::setAttributeModel(AttributeTableModel *model)
{
    if(dataTable){
        dataTable->clearSelection();
        dataTable->setModel(model);
        updateDataTable();
    }
}


void MedeaSubWindow::setNodeView(NodeView *view)
{
    nodeView = view;
    connect(view, SIGNAL(destroyed()), this, SLOT(deleteLater()));
    connect(this, SIGNAL(window_Resized()), view, SLOT(centralizedItemMoved()));
    connect(view, SIGNAL(view_ClearSubViewAttributeTable()), this, SLOT(setAttributeModel()));
    connect(view, SIGNAL(view_SetAttributeModel(AttributeTableModel*)), this, SLOT(setAttributeModel(AttributeTableModel*)));
    if(mainLayout){
        mainLayout->addWidget(view,1);
        setupDataTable();
        nodeView->setParent(this);
    }
}

void MedeaSubWindow::setupDataTable()
{
    if(mainLayout){
        QHBoxLayout* horizontalLayout = new QHBoxLayout();
        QVBoxLayout* verticalLayout = new QVBoxLayout();

        nodeView->setLayout(horizontalLayout);
        horizontalLayout->addStretch();
        horizontalLayout->addLayout(verticalLayout);

        verticalLayout->setMargin(0);
        horizontalLayout->setMargin(0);
        mainLayout->setContentsMargins(0,0,5,0);

        dataTableBox = new QWidget(this);



        dataTable = new QTableView();


        dataTable->setEnabled(false);

        dataTable->setFixedWidth(RIGHT_PANEL_WIDTH);
        dataTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        QVBoxLayout *tableLayout = new QVBoxLayout();
        tableLayout->setMargin(0);
        tableLayout->addWidget(dataTable);

        dataTableBox->setLayout(tableLayout);

        //dataTableBox->setFixedWidth(RIGHT_PANEL_WIDTH);
        //dataTableBox->setContentsMargins(0,0,0,0);
        dataTableBox->setLayout(tableLayout);
        dataTableBox->setStyleSheet("background-color: rgba(0,0,0,0);"
                                    "border: 0px;"
                                    "margin: 0px;"
                                    "padding: 0px;"
                                    );
        dataTable->setStyleSheet("background-color: white;");


        //dataTableBox->setVisible(false);
        verticalLayout->addWidget(dataTableBox, 0, Qt::AlignLeft | Qt::AlignVCenter);

        updateWidgetMask(dataTableBox, dataTable);
    }
}

void MedeaSubWindow::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    emit window_Resized();
}
