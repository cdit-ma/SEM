#ifndef COMBOBOXTABLEDELEGATE_H
#define COMBOBOXTABLEDELEGATE_H
#include <QItemDelegate>

#include "../../Model/graphmldata.h"

class ComboBoxTableDelegate : public QItemDelegate
{
    Q_OBJECT

public:

    ComboBoxTableDelegate(QObject *parent = 0);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const;

    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const;

    void updateEditorGeometry(QWidget *editor,
                              const QStyleOptionViewItem &option, const QModelIndex &index) const;
signals:
public slots:
    void updateValue();

private:
    GraphMLData* getGraphMLData(const QModelIndex &index) const;
    QStringList getValidValueList(const QModelIndex &index) const;
};

#endif // COMBOBOXTABLEDELEGATE_H
