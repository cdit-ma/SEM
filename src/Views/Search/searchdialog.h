#ifndef SEARCHDIALOG_H
#define SEARCHDIALOG_H

#include <QDialog>
#include <QScrollArea>
#include <QLabel>
#include <QLineEdit>
#include <QToolBar>
#include <QToolButton>
#include <QComboBox>
#include <QVBoxLayout>
#include <QActionGroup>
#include <QSplitter>

#include "../../Controllers/ViewController/viewitem.h"
#include "searchitemwidget.h"

class SearchDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SearchDialog(QWidget *parent = 0);

    void searchResults(QString query, QMap<QString, ViewItem*> results);

signals:
    void itemHoverEnter(int ID);
    void itemHoverLeave(int ID);
    void keyButtonChecked(QString key);
    void centerOnViewItem(int ID);
    void popupViewItem(int ID);

public slots:
    void themeChanged();
    void keyButtonChecked(bool checked);

    void searchItemSelected(int ID);
    void viewItemDestructed(int ID);

    void centerOnSelectedItem();
    void popupSelectedItem();

    void resetDialog();

private:
    void setupLayout();
    void clear();

    SearchItemWidget* constructSearchItem(ViewItem* item);
    void constructKeyButton(QString key, QString text = "", bool checked = false);

    QLabel* queryLabel;
    QLabel* searchLabel;
    QLabel* scopeLabel;
    QLabel* infoLabel;
    QSplitter* displaySplitter;

    QToolButton* centerOnButton;
    QToolButton* popupButton;

    QLineEdit* searchLineEdit;
    QToolButton* searchButton;
    QComboBox* scopeComboBox;

    QToolBar* keysToolbar;
    QToolBar* buttonsToolbar;
    QVBoxLayout* keysLayout;
    QActionGroup* keysActionGroup;

    QVBoxLayout* resultsLayout;
    QHash<int, SearchItemWidget*> searchItems;

    int selectedSearchItemID;
};

#endif // SEARCHDIALOG_H
