//
// Created by Cathlyn on 4/02/2020.
//

#include "triggeritemdelegate.h"
#include "triggertablemodel.h"

#include <QComboBox>
#include <QLineEdit>

TriggerItemDelegate::TriggerItemDelegate(QObject* parent)
    : QStyledItemDelegate(parent) {}


QWidget* TriggerItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    auto combobox = new QComboBox(parent);
    combobox->addItems(QStringList({"CPU_util", "Mem_util", "temperature"}));
    return combobox;
/*
    if (index.isValid()) {
        const auto& data = index.data(TriggerTableModel::TableKey);
        if (data.isNull() || !data.isValid()) {
            return QStyledItemDelegate::createEditor(parent, option, index);
        }

        if (data.canConvert<TriggerTableModel::VerticalSection>()) {
            const auto& section = data.value<TriggerTableModel::VerticalSection>();
            switch (section) {
                case TriggerTableModel::VerticalSection::Type: {
                    auto combo_box = new QComboBox(parent);
                    combo_box->addItems(QStringList({"CPU_util", "Mem_util", "temperature"}));
                    return combo_box;
                }
                case TriggerTableModel::VerticalSection::Condition: {
                    auto combo_box = new QComboBox(parent);
                    combo_box->addItems(QStringList({"<", "==", ">", "<=", ">=", "!="}));
                    return combo_box;
                }
                case TriggerTableModel::VerticalSection::Value: {
                    auto double_lineedit = new QLineEdit(parent);
                    double_lineedit->setValidator(new QDoubleValidator(0, 100, 2, parent));
                    return double_lineedit;
                }
                case TriggerTableModel::VerticalSection::ReTrigger: {
                    auto combo_box = new QComboBox(parent);
                    combo_box->addItems(QStringList({"true", "false"}));
                    return combo_box;
                }
                case TriggerTableModel::VerticalSection::WaitPeriod: {
                    auto uint_lineedit = new QLineEdit(parent);
                    uint_lineedit->setValidator(new QIntValidator(0, 1000000, parent));
                    return uint_lineedit;
                }
            }
        }
    }
    return QStyledItemDelegate::createEditor(parent, option, index); */
}

/*
void TriggerItemDelegate::destroyEditor(QWidget *editor, const QModelIndex &index) const
{
    QStyledItemDelegate::destroyEditor(editor, index);
}
*/


void TriggerItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if (editor == nullptr || !index.isValid()) {
        return;
    }
/*    const auto& data = index.data(TriggerTableModel::TableKey);
    if (data.isNull() || !data.isValid()) {
        return;
    }
*/
    auto section = 0; //data.value<int>();
    const auto& section_value = index.data(section).toString();

    auto combo_box = qobject_cast<QComboBox*>(editor);
    auto line_edit = qobject_cast<QLineEdit*>(editor);

    if (combo_box != nullptr) {
        combo_box->setCurrentText(section_value);
        /*
        switch (section) {
            case TriggerTableModel::VerticalSection::Type: {

            }
            case TriggerTableModel::VerticalSection::Condition: {

            }
            case TriggerTableModel::VerticalSection::ReTrigger: {

            }
            default:
                return;
        }
         */
    } else if (line_edit != nullptr) {

        line_edit->setText(section_value);

        /*
        switch (section) {
            case TriggerTableModel::VerticalSection::Value: {

            }
            case TriggerTableModel::VerticalSection::WaitPeriod: {

            }
            default:
                return;
        }
         */
    }
    QStyledItemDelegate::setEditorData(editor, index);
}


void TriggerItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QStyledItemDelegate::setModelData(editor, model, index);
}