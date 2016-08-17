#ifndef SELECTIONCONTROLLER_H
#define SELECTIONCONTROLLER_H
#include <QObject>
#include "../../Controller/selectionhandler.h"


class MedeaViewDockWidget;
class MedeaNodeViewDockWidget;
class SelectionController : public QObject
{
    friend class SelectionHandler;
    Q_OBJECT
public:
    SelectionController(ViewController* controller);

    SelectionHandler* constructSelectionHandler(QObject* object);
    void registerSelectionHandler(QObject* object, SelectionHandler* handler);
    void unregisterSelectionHandler(QObject* object, SelectionHandler* handler);


    QVector<ViewItem*> getSelection();
    QList<int> getSelectionIDs();
    int getSelectionCount();
    ViewItem* getFirstSelectedItem();
    int getFirstSelectedItemID();

signals:
    void itemActiveSelectionChanged(ViewItem* item, bool isActive);
    void selectionChanged(int selected);
    void viewItemDeleted(ViewItem* item);
    void clearSelection();
    void selectAll();
private slots:
    void activeViewDockWidgetChanged(MedeaViewDockWidget* widget);
    void cycleActiveSelectionBackward();
    void cycleActiveSelectionForward();
    void removeSelectionHandler();
private:
    QVector<ViewItem *> getOrderedSelection(QList<int> selection);
    void cycleActiveSelectedItem(bool forward);
    void setCurrentViewDockWidget(MedeaViewDockWidget* dock);
    void setCurrentSelectionHandler(SelectionHandler* handler);

    MedeaNodeViewDockWidget* currentViewDockWidget;
    SelectionHandler* currentHandler;

    QHash<QObject*, int> selectionHandlerIDLookup;
    QHash<int, SelectionHandler*> selectionHandlers;
    ViewController* viewController;
};

#endif // SELECTIONCONTROLLER_H
