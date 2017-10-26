#include "idlparser.h"
#include <QCommandLineParser>
#include <iostream>
#include <fstream>

bool writeFile(const std::string &file_path, const std::string &data){
    std::ofstream file_out(file_path); 

    if(file_out){
        file_out << data;
        file_out.close();
        return true;
    }else{
        
        return false;
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("idl2graphml");

    QCommandLineParser parser;
    parser.addHelpOption();
    QCommandLineOption import_options({"i", "import"}, "Import IDL project.", "The idl file path");
    QCommandLineOption export_options({"e", "export"}, "Export as a graphml file.", "The graphml file path");

    parser.addOption(import_options);
    parser.addOption(export_options);
    parser.process(app);

    bool got_values = parser.isSet(import_options);
    
    if(!got_values){
        parser.showHelp(1);
    }

    
    auto import_idl_path = parser.value(import_options).toStdString();
    auto export_graphml_path = parser.value(export_options).toStdString();
    auto should_export = parser.isSet(export_options);
    auto result = IdlParser::IdlParser::ParseIdl(import_idl_path, true);
    auto success = result.first;

    if(success){
        std::cerr << "IDL Parser: Successfully parsed file '" << import_idl_path << "'" << std::endl;
    }else{
        std::cerr << "IDL Parser: Failed to parsed file '" << import_idl_path << "'" << std::endl;
    }

    if(success && should_export){
        if(writeFile(export_graphml_path, result.second)){
            std::cerr << "IDL Parser: Exported file '" << export_graphml_path << "'" << std::endl;
        }else{
            std::cerr << "IDL Parser: Cannot export file '" << export_graphml_path << "'" << std::endl;
        }
    }else if(success){
        std::cout << result.second;
    }
      
    return success;
}
