#include "dataflowdialog.h"
#include "dataflowgraphicsview.h"
#include "../DockWidgets/basedockwidget.h"
#include "../../theme.h"

#include "EntityItems/componentinstancegraphicsitem.h"
#include "GraphicsItems/edgeitem.h"

#include <QVBoxLayout>
#include <QGraphicsRectItem>
#include <QDockWidget>

/**
 * @brief DataflowDialog::DataflowDialog
 * @param parent
 */
DataflowDialog::DataflowDialog(QWidget *parent) : QFrame(parent)
{
    view_ = new DataflowGraphicsView(this);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setMargin(0);
    mainLayout->addWidget(view_);

    connect(Theme::theme(), &Theme::theme_Changed, this, &DataflowDialog::themeChanged);
    themeChanged();
}


/**
 * @brief DataflowDialog::setExperimentInfo
 * @param exp_name
 * @param exp_run_id
 */
void DataflowDialog::setExperimentInfo(const QString &exp_name, quint32 exp_run_id)
{
    if (parentWidget()) {
        auto parent_dockwidget = qobject_cast<BaseDockWidget*>(parentWidget());
        parent_dockwidget->setTitle("Pulse [" + exp_name + " #" + QString::number(exp_run_id) + "]");
    }
}


/**
 * @brief DataflowDialog::themeChanged
 */
void DataflowDialog::themeChanged()
{
    Theme* theme = Theme::theme();
    setStyleSheet(theme->getScrollBarStyleSheet());
    view_->setStyleSheet("padding: 0px; border: 1px solid " + theme->getDisabledBackgroundColorHex() + ";");
    view_->setBackgroundBrush(theme->getBackgroundColor());
}


/**
 * @brief DataflowDialog::displayExperimentState
 * @param exp_state
 */
void DataflowDialog::displayExperimentState(const AggServerResponse::ExperimentState& exp_state)
{
    // Clear previous items
    clear();

    qDebug() << "---------------------------------------------------------------";
    //qDebug() << "EXPERIMENT STATE:";
    //qDebug() << "nodes#: " << expState.nodes.size();

    QHash<QString, PortInstanceGraphicsItem*> port_instances;

    for (const auto& n : exp_state.nodes) {
        //qDebug() << "containers#: " << n.containers.size();
        for (const auto& c : n.containers) {
            //qDebug() << "comp inst#: " << c.component_instances.size();
            for (const auto& inst : c.component_instances) {

                qDebug() << "Component Instance: " << inst.name << " - " << inst.path;
                auto c_instItem = new ComponentInstanceGraphicsItem(inst);
                addItemToScene(c_instItem);

                qDebug() << "Comp Inst scene rect: " << c_instItem->sceneBoundingRect();

                for (const auto& port : inst.ports) {
                    qDebug() << "Port: " << port.name << " - " << port.path;
                    auto p_instItem = new PortInstanceGraphicsItem(port);
                    port_instances[port.graphml_id] = p_instItem;
                    c_instItem->addPortInstanceItem(p_instItem);
                    connect(c_instItem, &ComponentInstanceGraphicsItem::itemMoved, p_instItem, &PortInstanceGraphicsItem::itemMoved);

                    /*
                    // TODO - Figure out why the sceneBoundingRect is empty
                    qDebug() << "Port scene rect: " << p_instItem->sceneBoundingRect();
                    qDebug() << "Port mapped rect: " << view_->mapToScene(p_instItem->boundingRect().toRect());
                    qDebug() << "Port geometry: " << p_instItem->geometry();
                    */
                }
            }
        }
    }

    // Construct the edges
    constructEdgeItems(port_instances, exp_state.port_connections);
    qDebug() << "---------------------------------------------------------------";
}


/**
 * @brief DataflowDialog::constructEdgeItems
 * @param exp_state
 */
void DataflowDialog::constructEdgeItems(const QHash<QString, PortInstanceGraphicsItem*>& port_instances, const QVector<AggServerResponse::PortConnection>& port_connections)
{
    for (const auto& p_c : port_connections) {
        auto from_port = port_instances.value(p_c.from_port_graphml, nullptr);
        auto to_port = port_instances.value(p_c.to_port_graphml, nullptr);
        if (!(from_port && to_port)) {
            qWarning("DataflowDialog::displayExperimentState - Failed to construct edge; from_port/to_port is null.");
            continue;
        }
        auto edge_item = new MEDEA::EdgeItem(from_port, to_port);
        connect(from_port, &PortInstanceGraphicsItem::itemMoved, [edge_item]{ edge_item->updateSourcePos(); });
        connect(to_port, &PortInstanceGraphicsItem::itemMoved, [edge_item]{ edge_item->updateDestinationPos(); });
        addItemToScene(edge_item);
        qDebug() << "Edge scene rect: " << edge_item->sceneBoundingRect();
    }
}


/**
 * @brief DataflowDialog::clear
 * This clears all the graphics items in the scene
 */
void DataflowDialog::clear()
{
    view_->scene()->clear();
}


/**
 * @brief DataflowDialog::addItemToScene
 * @param item
 * @throws std::invalid_argument
 */
void DataflowDialog::addItemToScene(QGraphicsItem *item)
{
    if (item) {
        view_->scene()->addItem(item);
    } else {
        throw std::invalid_argument("DataflowDialog::addItemToScene - Trying to add a null item");
    }
}
