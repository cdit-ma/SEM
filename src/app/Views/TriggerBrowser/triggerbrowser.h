//
// Created by Cathlyn on 3/02/2020.
//

#ifndef MEDEA_TRIGGERBROWSER_H
#define MEDEA_TRIGGERBROWSER_H

#include "triggeritemmodel.h"

#include <QWidget>
#include <QListView>
#include <QTableView>
#include <QSplitter>
#include <QToolBar>

class TriggerBrowser : public QWidget
{
    Q_OBJECT

public:
    explicit TriggerBrowser(QWidget* parent = nullptr);

private slots:
    void themeChanged();

    void currentTriggerChanged(const QModelIndex& current, const QModelIndex& previous);
    void triggerListDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles = QVector<int>());

    void addTrigger();
    void removeTrigger();

private:
    void setupLayout();

    TriggerItemModel* trigger_item_model_ = nullptr;

    QListView* trigger_list_view_ = nullptr;
    QTableView* trigger_fields_view_ = nullptr;
    QSplitter* horizontal_splitter_ = nullptr;

    QToolBar* toolbar_ = nullptr;
    QAction* add_trigger_action_ = nullptr;
    QAction* remove_trigger_action_ = nullptr;

    QString trigger_item_text_;
};

#endif //MEDEA_TRIGGERBROWSER_H
