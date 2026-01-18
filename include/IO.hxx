#ifndef IO_HXX
#define IO_HXX
#include <iostream>
#include <map>
#include <string>
#include "factory.hxx"

enum class FactoryNode{
    LOADING_RAMP,
    WORKER,
    STOREHOUSE,
    LINK
};

struct parsedLineData{
    FactoryNode element_type;
    std::map<std::string,std::string> params;
};

std::map<std::string,FactoryNode> enumToString = {
    {"LOADING_RAMP",FactoryNode::LOADING_RAMP  }, 
    {"WORKER",FactoryNode::WORKER } , 
    {"STOREHOUSE" , FactoryNode::STOREHOUSE} , 
    {"LINK", FactoryNode::LINK },
};

parsedLineData parse_line(std::string line);
std::map<std::string,std::string> parseParams(std::stringstream& ss);
    
Factory load_factory_structure(std::istream& iss);
void save_factory_structure(const Factory& factory, std::ostream& os);

#endif