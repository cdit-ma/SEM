#include "idlparser.h"
#include <iostream>
#include <fstream>
#include <boost/program_options.hpp>
#include <vector>

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
    std::string import_idl_path;
    std::string export_graphml_path;
    bool should_export = false;

    boost::program_options::options_description options("idl2graphml");
    options.add_options()("import,i", boost::program_options::value<std::string>(&import_idl_path)->required(), "IDL file paths to import");
    options.add_options()("export,e", boost::program_options::value<std::string>(&export_graphml_path), "Graphml file path");
    options.add_options()("help,h", "Display help");

    //Construct a variable_map
	boost::program_options::variables_map vm;
	
	try{
		//Parse Argument variables
		boost::program_options::store(boost::program_options::parse_command_line(argc, argv, options), vm);
        boost::program_options::notify(vm);
        should_export = !vm["export"].empty();
	}catch(boost::program_options::error& e) {
        std::cerr << "Arg Error: " << e.what() << std::endl << std::endl;
		std::cout << options << std::endl;
        return 1;
    }

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
