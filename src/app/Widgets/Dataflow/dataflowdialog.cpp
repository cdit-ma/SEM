#include "dataflowdialog.h"
#include "dataflowgraphicsview.h"
#include "../../theme.h"

#include "EntityItems/componentinstancegraphicsitem.h"
#include "EntityItems/portinstancegraphicsitem.h"
#include "../DockWidgets/basedockwidget.h"

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
 * @param expState
 */
void DataflowDialog::displayExperimentState(const AggServerResponse::ExperimentState &expState)
{
    // Clear previous items
    clear();

    qDebug() << "---------------------------------------------------------------";
    qDebug() << "nodes#: " << expState.nodes.size();

    for (const auto& n : expState.nodes) {
        qDebug() << "containers#: " << n.containers.size();
        for (const auto& c : n.containers) {
            qDebug() << "comp inst#: " << c.component_instances.size();
            for (const auto& inst : c.component_instances) {

                qDebug() << "Component Instance: " << inst.name << " - " << inst.path;
                auto c_instItem = new ComponentInstanceGraphicsItem(inst);
                view_->scene()->addItem(c_instItem);

                for (const auto& port : inst.ports) {
                    qDebug() << "port: " << port.name << " - " << port.path;
                    auto p_instItem = new PortInstanceGraphicsItem(port);
                    c_instItem->addPortInstanceItem(p_instItem);
                }

            }
        }
    }

    qDebug() << "---------------------------------------------------------------";
}


/**
 * @brief DataflowDialog::clear
 * This clears all the graphics items in the scene
 */
void DataflowDialog::clear()
{
    view_->scene()->clear();
}
