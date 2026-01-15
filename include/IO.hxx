#include <iostream>
#include <map>
#include <string>
#include "factory.hxx"
class IO{
private:
    parsedLineData parse_line(std::string line);
    std::map<std::string,std::string> parseParams(std::stringstream& ss);
    
public:
    Factory load_factory_structure(std::istream& is);
    void save_factory_structure(const Factory& factory, std::ostream& os);
};

enum factoryNode{
    LOADING_RAMP,
    WORKER,
    STOREHOUSE,
    LINK
};

std::map<std::string,factoryNode> enumToString = {
    {"LOADING_RAMP",LOADING_RAMP  }, 
    {"WORKER",WORKER } , 
    {"STOREHOUSE" , STOREHOUSE} , 
    {"LINK", LINK }
};

struct parsedLineData{
    factoryNode element_type;
    std::map<std::string,std::string> params;
};