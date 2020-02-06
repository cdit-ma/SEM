//
// Created by Cathlyn on 4/02/2020.
//

#ifndef MEDEA_TRIGGERITEMDELEGATE_H
#define MEDEA_TRIGGERITEMDELEGATE_H

#include <QStyledItemDelegate>

class TriggerItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit TriggerItemDelegate(QObject* parent = nullptr);

    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

protected:
    void setEditorData(QWidget* editor, const QModelIndex& index) const override;
    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;

};


#endif //MEDEA_TRIGGERITEMDELEGATE_H
