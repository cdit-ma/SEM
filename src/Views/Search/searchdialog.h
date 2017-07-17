#ifndef SEARCHDIALOG_H
#define SEARCHDIALOG_H

#include <QScrollArea>
#include <QLabel>
#include <QLineEdit>
#include <QToolBar>
#include <QToolButton>
#include <QComboBox>
#include <QVBoxLayout>
#include <QActionGroup>
#include <QSplitter>

#include "../../theme.h"
#include "../../Controllers/ViewController/viewitem.h"
#include "../../Utils/filtergroup.h"
#include "searchitemwidget.h"

class SearchDialog : public QWidget
{
    Q_OBJECT
public:
    explicit SearchDialog(QWidget *parent = 0);

    void searchResults(QString query, QMap<QString, ViewItem*> results);

signals:
    void itemHoverEnter(int ID);
    void itemHoverLeave(int ID);

    void centerOnViewItem(int ID);
    void popupViewItem(int ID);

    void keyButtonChecked(QString key);
    void searchButtonClicked();
    void refreshButtonClicked();

public slots:
    void themeChanged();
    void keyButtonChecked(bool checked);

    void searchItemSelected(int ID);
    void viewItemDestructed(int ID);

    void centerOnSelectedItem();
    void popupSelectedItem();

    void resetPanel();

private:
    void updateKeyButtonIcons();
    void setupLayout();
    void setupFilterGroups();
    void clear();

    SearchItemWidget* constructSearchItem(ViewItem* item);
    void constructKeyButton(QString key, QString text = "", bool checked = false, bool addToGroup = true);

    QWidget* mainWidget;

    QLabel* queryLabel;
    QLabel* searchLabel;
    QLabel* scopeLabel;
    QLabel* infoLabel;
    QSplitter* displaySplitter;

    QToolButton* centerOnButton;
    QToolButton* popupButton;
    QToolButton* searchButton;
    QToolButton* refreshButton;

    QToolBar* keysToolbar;
    QToolBar* topToolbar;
    QToolBar* bottomToolbar;
    QVBoxLayout* keysLayout;

    QActionGroup* staticKeysActionGroup;
    QActionGroup* dynamicKeysActionGroup;

    QVBoxLayout* resultsLayout;
    QHash<int, SearchItemWidget*> searchItems;

    int selectedSearchItemID;
};

#endif // SEARCHDIALOG_H
