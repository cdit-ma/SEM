#include <QCoreApplication>
#include "utils.h"
#include "../ModelController/modelcontroller.h"
#include "../ModelController/entityfactory.h"
#include <iostream>
#include <QDebug>
#include <QHash>

struct Entity{
    int id;
};

struct Node: public Entity{
    NODE_KIND kind;
    int parent_id;
    QList<int> children_id;
};

struct Edge: public Entity{
    EDGE_KIND kind;
    int src_id;
    int dst_id;
};

Node* get_model();
Node* get_parent(Node* node);


QHash<int, Entity*> entities;
int root_id = -1;
ModelController* model_controller = 0;

Entity* get_entity(int id){
    return entities.value(id, 0);
};

Node* get_node(int id){
    auto entity = get_entity(id);
    if(entity){
        return (Node*) entity;
    }
    return 0;
}

void node_constructed(int parent_id, int id, NODE_KIND kind){
    auto parent = get_node(parent_id);
    auto entity = new Node();
    entity->id = id;
    entity->kind = kind;
    entity->parent_id = parent_id;

    if(parent){
        parent->children_id.push_back(id);
    }else if(kind == NODE_KIND::MODEL){
        root_id = id;
    }

    entities[id] = entity;
};

void edge_constructed(int id, EDGE_KIND kind, int src_id, int dst_id){

};

void entity_destructed(int ID, GRAPHML_KIND kind){
    auto entity = get_entity(ID);
    if(entity){
        switch(kind){
            case GRAPHML_KIND::NODE:{
                auto node = (Node*) entity;
                auto parent = get_parent(node);
                if(parent){
                    parent->children_id.removeAll(ID);
                }
            }
            default:
                break;
        }
        entities.remove(ID);
        delete entity;
    }
};

Node* get_model(){
    return get_node(root_id);
};

Node* get_parent(Node* node){
    return get_node(node ? node->parent_id : -1);
};

Node* get_child(Node* node, QString label){
    if(node){
        for(auto id : node->children_id){
            auto child_label = model_controller->getEntityDataValue(id, "label").toString();
            
            if(child_label == label){
                return get_node(id);
            }
        }
    }
    return 0;
};

QString get_current_path(Node* current_node){
    QString path;

    auto node = current_node;
    
    while(node){
        auto label = model_controller->getEntityDataValue(node->id, "label").toString();
        if(node != current_node){
            path = "/" + path;
        }
        path = label + path;

        node = get_parent(node);
    }
    return path;
}

Node* resolve_path(Node* current_node, QString path){
    auto current = current_node;
    //Select root as parent
    if(path.startsWith("/")){
        current = get_model();
    }
    auto tokens = path.split("/");

    for(auto token: tokens){
        if(!token.isEmpty()){
            if(token == ".."){
                //Go up a parent.
                current = get_parent(current);
            }else if(token == "."){
                //Ignore
            }else{
                current = get_child(current, token);
            }
        }

        if(!current){
            break;
        }
    }

    return current;
};


bool is_descendant(Node* parent, Node* child){
    if(parent == child){
        return true;
    }
    for(auto id : parent->children_id){
        auto p = get_node(id);
        qCritical() << id;
        if(p){
            if(p == child){
                return true;
            }else{
                return is_descendant(p, child);
            }
        }
    }
    return false;
}

Node* add_node(Node* current_node, QStringList args);
Node* rm(Node* current_node, QStringList args);
Node* ls(Node* current_node, QStringList args);
Node* cd(Node* current_node, QStringList args);
Node* import_model(Node* current_node, QStringList args);
Node* export_model(Node* current_node, QStringList args);


Node* add_node(Node* current_node, QStringList args){
    if(args.length() == 1){
        auto node_kind_str = args[0];
        auto node_kind = EntityFactory::getNodeKind(node_kind_str);

        if(node_kind != NODE_KIND::NONE){
            model_controller->constructNode(current_node->id, node_kind, QPointF());
        }
    }
    return current_node;
};


Node* rm(Node* node, QStringList args){
    if(args.length() == 1){
        auto path = args[0];
        
        QList<int> ids;
        if(path == "*"){
            //Remove all children of node.
            ids = node->children_id;
        }else{
            auto n = resolve_path(node, path);
            if(n){
                ids << n->id;
            }

            while(is_descendant(n, node)){
                node = get_parent(node);
                if(!node){
                    node = 0;
                }
            }
        }
        if(ids.length() > 0){
            model_controller->remove(ids);
        }
    }
    return node;
};

Node* ls(Node* current_node, QStringList args){
    for(auto id : current_node->children_id){
        qCritical().noquote().nospace() << "\e[31m" << model_controller->getEntityDataValue(id, "label").toString() << "\e[0m";
    }
    return current_node;
};

Node* cd(Node* current_node, QStringList args){
    if(args.length() >= 1){
        auto path = args[0];
        current_node = resolve_path(current_node, path);
    }
    return current_node;
};

Node* import_model(Node* current_node, QStringList args){
    /*
    if(args.length() >= 1)
     auto path = token.split(" ")[1];
            QStringList files;
            QString file_data = Utils::readTextFile(path);
            files << file_data;
            model_controller->importProjects(files);*/
    return current_node;
};

Node* export_model(Node* current_node, QStringList args){
    return current_node;
};


int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);



    model_controller = new ModelController();
    QObject::connect(model_controller, &ModelController::NodeConstructed, &node_constructed);
    QObject::connect(model_controller, &ModelController::EntityDestructed, &entity_destructed);

    
    model_controller->SetupController();

    QHash<QString, std::function<Node*(Node*, QStringList args)> > command_fns;
    std::string s;

    command_fns["add"] = &add_node;
    command_fns["rm"] = &rm;
    command_fns["ls"] = &ls;
    command_fns["cd"] = &cd;

    auto current_node = get_model();
    while(true){
        std::string s;
        {
            std::cout << get_current_path(current_node).toStdString() << "> ";
            QDebug debug(QtDebugMsg);
            std::getline(std::cin, s);
        }

        QString token = QString::fromStdString(s);  

        auto tokens = token.split(' ');

        if(tokens.size() > 0){
            auto command = tokens[0].toLower();
            tokens.pop_front();
            auto args = tokens;
            if(command_fns.contains(command)){
                auto node = command_fns[command](current_node, args);
                if(node){
                    current_node = node;
                }
            }
        }
    }


     /*
        }else if(token.startsWith("import")){
            auto path = token.split(" ")[1];
            QStringList files;
            QString file_data = Utils::readTextFile(path);
            files << file_data;
            model_controller->importProjects(files);
        }else if(token.startsWith("export")){
            auto path = token.split(" ")[1];
            if(Utils::writeTextFile(path, model_controller->getProjectAsGraphML())){
                qCritical() << "Written: " << path;
            };
        }else if(token == "cat"){
            auto id = c_node->id;
            for(auto key : model_controller->getEntityKeys(id)){
                qCritical() << "\t " << key << " = " << model_controller->getEntityDataValue(id, key).toString();    
            }
        }else if(token.startsWith("add")){
            auto remainder = token.split(" ")[1];
            if(remainder == "--list"){
                for(auto kind : model_controller->getAdoptableNodeKinds(c_node->id)){
                    qCritical() << "\t" << EntityFactory::getNodeKindString(kind);
                }
            }else{
                add_node(c_node, remainder);
            }
        }else if(token.startsWith("set")){
            auto tokens = token.split(" ");
            auto name = tokens[1];
            auto value = tokens[2];
            
            model_controller->setData(c_node->id, name, value);
        }else if(token == "undo"){
            model_controller->undo();
        }else if(token == "redo"){
            model_controller->redo();
        }else if(token.startsWith("rm")){
            auto node = rm(c_node, token);
            if(node){
                c_node = node;
            }
        }
        else if(token == "exit"){
            break;
        }
    }*/
    delete model_controller;

    return 0;
}