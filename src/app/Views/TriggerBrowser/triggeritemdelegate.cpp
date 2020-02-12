//
// Created by Cathlyn on 4/02/2020.
//

#include "triggeritemdelegate.h"
#include "../../../modelcontroller/Entities/TriggerDefinitions/trigger.h"

#include <QComboBox>
#include <QLineEdit>

TriggerItemDelegate::TriggerItemDelegate(QObject* parent)
    : QStyledItemDelegate(parent) {}


QWidget* TriggerItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (index.isValid()) {
        const auto& data = index.model()->headerData(index.row(), Qt::Vertical, Qt::DisplayRole);
        if (data.isNull() || !data.isValid()) {
            return QStyledItemDelegate::createEditor(parent, option, index);
        }
        const auto& key = data.toString();
        if (key == Trigger::getTableKeyString(Trigger::TableKey::Type)) {
            auto combo_box = new QComboBox(parent);
            combo_box->addItems(Trigger::getTriggerTypes());
            return combo_box;
        } else if (key == Trigger::getTableKeyString(Trigger::TableKey::Condition)) {
            auto combo_box = new QComboBox(parent);
            combo_box->addItems(Trigger::getTriggerConditions());
            return combo_box;
        } else if (key == Trigger::getTableKeyString(Trigger::TableKey::Value)) {
            auto double_lineedit = new QLineEdit(parent);
            double_lineedit->setValidator(new QDoubleValidator(0, 100, 2, parent));
            return double_lineedit;
        } else if (key == Trigger::getTableKeyString(Trigger::TableKey::SingleActivation)) {
            auto combo_box = new QComboBox(parent);
            combo_box->addItems(QStringList({"false", "true"}));
            return combo_box;
        }
    }
    return QStyledItemDelegate::createEditor(parent, option, index);
}


void TriggerItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if (editor == nullptr || !index.isValid()) {
        return;
    }
    auto&& current_val = index.data(Qt::DisplayRole).toString();
    auto combo_box = qobject_cast<QComboBox*>(editor);
    auto line_edit = qobject_cast<QLineEdit*>(editor);
    if (combo_box != nullptr) {
        combo_box->setCurrentText(current_val);
    } else if (line_edit != nullptr) {
        line_edit->setText(current_val);
    }
    QStyledItemDelegate::setEditorData(editor, index);
}


void TriggerItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if (editor == nullptr || model == nullptr || !index.isValid()) {
        return;
    }
    auto combo_box = qobject_cast<QComboBox*>(editor);
    auto line_edit = qobject_cast<QLineEdit*>(editor);
    if (combo_box != nullptr) {
        model->setData(index, combo_box->currentText(), Qt::DisplayRole);
        model->setData(index, combo_box->currentText(), Qt::EditRole);
    } else if (line_edit != nullptr) {
        model->setData(index, line_edit->text(), Qt::DisplayRole);
        model->setData(index, line_edit->text(), Qt::EditRole);
    }
    QStyledItemDelegate::setModelData(editor, model, index);
}