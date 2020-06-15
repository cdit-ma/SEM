#include "graphmlprinter.h"

#include <QSet>

#include "Entities/data.h"
#include "Entities/node.h"
#include "Entities/edge.h"
#include "Entities/key.h"
namespace helper{
    //Helper functions
    QString SanitizeString(const QString& str){
        const static QString str_illegal_and("&");
        const static QString str_safe_and("&amp;");
        const static QString str_illegal_gt(">");
        const static QString str_safe_gt("&gt;");
        const static QString str_illegal_lt("<");
        const static QString str_safe_lt("&lt;");
        const static QString str_illegal_dq("\"");
        const static QString str_safe_dq("&quot;");
        const static QString str_illegal_sq("\'");
        const static QString str_safe_sq("&apos;");

        QString sanitized_string = str;
        sanitized_string.replace(str_illegal_and, str_safe_and);
        sanitized_string.replace(str_illegal_gt, str_safe_gt);
        sanitized_string.replace(str_illegal_lt, str_safe_lt);
        sanitized_string.replace(str_illegal_dq, str_safe_dq);
        sanitized_string.replace(str_illegal_sq, str_safe_sq);
        return sanitized_string;
    };
};

GraphmlPrinter::GraphmlPrinter(const QSet<ExportFlags>& flags):
    flags_(flags){};

GraphmlPrinter::GraphmlPrinter(){

}

QString GraphmlPrinter::ToGraphml(const QList<Entity*>& selection){
    //Definition list of Entities
    QSet<Key*> keys;
    QSet<Node*> top_nodes;
    QSet<Node*> all_nodes;
    QSet<Edge*> edges;
    
    //Get the nodes/edges out of the selection
    for(auto entity : selection){
        if(entity){
            if(entity->isNode()){
                auto node = (Node*) entity;
                
                top_nodes += node;
                all_nodes += node;
                all_nodes += node->getAllChildren();
                edges += node->getAllEdges();
            }else if(entity->isEdge()){
                edges += (Edge*) entity;
            }
        }
    }

    //Get the keys which are needed for all nodes/edges
    std::for_each(all_nodes.cbegin(), all_nodes.cend(), [&keys](const Node* n){keys += n->getKeys();});
    std::for_each(edges.cbegin(), edges.cend(), [&keys](const Edge* e){keys += e->getKeys();});

    QString xml;
    QTextStream stream(&xml);

    //Output the inital stuff
    stream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    stream << "<graphml xmlns=\"http://graphml.graphdrawing.org/xmlns\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"http://graphml.graphdrawing.org/xmlns\">\n";

    //Construct a list of keys, and sort
    auto key_list = keys.toList();
    std::sort(key_list.begin(), key_list.end(), GraphML::SortByID);
    
    //Export all Keys at the top
    for(auto key : key_list){
        ToGraphmlStream(*key, stream, 1);
    }

    {

        stream << "\n\t<graph edgedefault=\"directed\" id=\"g0\">\n";
        for(auto node : top_nodes){
            ToGraphmlStream(*node, stream, 2);
        }
        
        //Sort the edges to be lowest to highest IDS for determinism
        auto edge_list = edges.toList();
        std::sort(edge_list.begin(), edge_list.end(), [](const Entity* e1, const Entity* e2){
            return e1->getID() > e2->getID();
        });

        const auto export_edges = IsFlagSet(ExportFlags::EXPORT_ALL_EDGES);;

        for(auto entity : edge_list){
            auto edge = (Edge*) entity;

            auto src = edge->getSource();
            auto dst = edge->getDestination();

            //Only export the edge if we contain both sides, unless we should export all
            bool export_edge = export_edges;
            
            if(!export_edge){
                bool contains_src = all_nodes.contains(src);
                bool contains_dst = all_nodes.contains(dst);
                if(contains_src && contains_dst){
                    export_edge = true;
                }else{
                    //If we don't contain both
                    switch(edge->getEdgeKind()){
                        case EDGE_KIND::AGGREGATE:
                        case EDGE_KIND::ASSEMBLY:
                        case EDGE_KIND::DEPLOYMENT:{
                            export_edge = true;
                            break;
                        }
                        case EDGE_KIND::DEFINITION:{
                            export_edge = contains_src;
                            break;
                        }
                        default:
                            break;
                    }
                }
            }
            if(export_edge){
                ToGraphmlStream(*edge, stream, 2);
            }
        }
        stream << "\t</graph>\n";
    }
    stream << "</graphml>";
    return xml;
}

QString GraphmlPrinter::ToGraphml(const Node& node){
    QString xml;
    QTextStream stream(&xml);
    ToGraphmlStream(node, stream, 0);
    return xml;
}

void GraphmlPrinter::ToGraphmlStream(const Node& node, QTextStream& stream, int indent_depth){
    const auto tab = QString("\t").repeated(indent_depth);
    
    stream << tab;
    stream << "<node id=\"" << node.getID() << "\">\n";

    DatasToGraphmlStream(node, stream, indent_depth + 1);

    const auto& children = node.getChildren(0);
    //Children are in a <graph>
    if(children.size()){
        stream << tab << "\t<graph id=\"g" << node.getID() << "\">\n";
        for(auto child : children){
            ToGraphmlStream(*child, stream, indent_depth + 2);
        }
        stream << tab << "\t</graph>\n";
    }
    stream << tab << "</node>\n";
}

void GraphmlPrinter::DatasToGraphmlStream(const Entity& entity, QTextStream& stream, int indent_depth){
    auto data_list = entity.getData();
    
    //Sort the Data
    std::sort(data_list.begin(), data_list.end(), Data::SortByKey);
    for(auto data : data_list){
        ToGraphmlStream(*data, stream, indent_depth);
    }
}

void GraphmlPrinter::ToGraphmlStream(const Edge& edge, QTextStream& stream, int indent_depth){
    const auto tab = QString("\t").repeated(indent_depth);
    
    stream << tab << "<edge id=\"" << edge.getID() << "\" source=\"" << edge.getSourceID() << "\" target=\"" << edge.getDestinationID() << "\">\n";
    DatasToGraphmlStream(edge, stream, indent_depth + 1);
    stream << tab << "</edge>\n";
}

QString GraphmlPrinter::GetKeyID(const Key& key){
    if(IsFlagSet(ExportFlags::HUMAN_READABLE_DATA)){
        //Use the name if we are human readible
        return key.getName();
    }
    return QString::number(key.getID());
}

bool GraphmlPrinter::ExportKey(const Key& key){
    if(IsFlagSet(ExportFlags::STRIP_VISUAL_DATA) && IsKeyVisual(key.getName())){
        return false;
    }
    return true;
}

bool GraphmlPrinter::IsKeyVisual(const QString& key_name){
    static const QSet<QString> visual_key_names({KeyName::X, KeyName::Y, KeyName::Width, KeyName::Height, KeyName::IsExpanded});
    return visual_key_names.contains(key_name);
}

void GraphmlPrinter::ToGraphmlStream(const Key& key, QTextStream& stream, int indent_depth){
    if(ExportKey(key)){
        stream << QString("\t").repeated(indent_depth);
        stream << "<key";
        stream << " attr.name=\"" << key.getName() << "\"";
        stream << " attr.type=\"" << Key::getGraphMLTypeName(key.getType()) << "\"";
        stream << " id=\"" << GetKeyID(key) << "\"/>\n";
    }
}

void GraphmlPrinter::ToGraphmlStream(const Data& data, QTextStream& stream, int indent_depth)
{
    const auto& key = *(data.getKey());

    if (ExportKey(key)) {
        auto data_string = helper::SanitizeString(data.getValue().toString());
        if (key.getName() == KeyName::ProcessesToLog) {
            const static QString search_str("\n");
            const static QString replace_str(",");
            //Replace all new lines with comma's
            data_string.replace(search_str, replace_str);
        }
        stream << QString("\t").repeated(indent_depth);
        stream << "<data key=\"" << GetKeyID(key) << "\">";
        stream << data_string;
        stream << "</data>\n";
    }
}

bool GraphmlPrinter::IsFlagSet(const ExportFlags& flag) const{
    return flags_.contains(flag);
}