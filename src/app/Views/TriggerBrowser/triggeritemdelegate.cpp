//
// Created by Cathlyn on 4/02/2020.
//

#include "triggeritemdelegate.h"
#include "triggertablemodel.h"

#include <QComboBox>
#include <QLineEdit>
#include <QDebug>

TriggerItemDelegate::TriggerItemDelegate(QObject* parent)
    : QStyledItemDelegate(parent) {}


QWidget* TriggerItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (index.isValid()) {
        const auto &data = index.data(TriggerTableModel::TableRole::KeyRole);
        if (data.isNull() || !data.isValid()) {
            return QStyledItemDelegate::createEditor(parent, option, index);
        }
        const auto &key = data.value<TriggerTableModel::TableKey>();
        switch (key) {
            case TriggerTableModel::TableKey::Type: {
                auto combo_box = new QComboBox(parent);
                combo_box->addItems(TriggerTableModel::getTriggerTypes());
                return combo_box;
            }
            case TriggerTableModel::TableKey::Condition: {
                auto combo_box = new QComboBox(parent);
                combo_box->addItems(TriggerTableModel::getTriggerConditions());
                return combo_box;
            }
            case TriggerTableModel::TableKey::Value: {
                auto double_lineedit = new QLineEdit(parent);
                double_lineedit->setValidator(new QDoubleValidator(0, 100, 2, parent));
                return double_lineedit;
            }
            case TriggerTableModel::TableKey::ReTrigger: {
                auto combo_box = new QComboBox(parent);
                combo_box->addItems(QStringList({"true", "false"}));
                return combo_box;
            }
            case TriggerTableModel::TableKey::WaitPeriod: {
                auto uint_lineedit = new QLineEdit(parent);
                uint_lineedit->setValidator(new QIntValidator(0, 1000000, parent));
                return uint_lineedit;
            }
        }
    }
    return QStyledItemDelegate::createEditor(parent, option, index);
}


void TriggerItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if (editor == nullptr || !index.isValid()) {
        return;
    }

    const auto& data = index.data(TriggerTableModel::TableRole::KeyRole);
    if (data.isNull() || !data.isValid()) {
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
    } else if (line_edit != nullptr) {
        model->setData(index, line_edit->text(), Qt::DisplayRole);
    }

    QStyledItemDelegate::setModelData(editor, model, index);
}