#include "IO.hxx"
#include <sstream>

parsedLineData parse_line(std::string line){
    if((line.size() >0 && line.find(";")==0) || line.find_first_not_of(" \t\n\v\f\r")== std::string::npos){
        throw std::logic_error("Bladny string");
    }
    std::stringstream ss(line);
    std::string firstWord;
    ss>>firstWord;
    enumToString.find(firstWord);
    if(!node){
        throw 505;
    }
    std::map<std::string,std::string> params = parseParams(ss);
    parsedLineData pld;
    pld.element_type = node;
    pld.params = std::move(params);
    return pld;
}


std::map<std::string,std::string> parseParams(std::stringstream& ss){
    std::map<std::string,std::string> mp;
    std::string token;

    while (ss>>token) {
        std::size_t pos = token.find("=");
        if(pos != std::string::npos){
            std::string key = token.substr(0, pos);
            std::string value = token.substr(pos+1);
            mp[key] = value;
        }
    }
    return mp;
}

Factory load_factory_structure(std::istream& iss){

}
void save_factory_structure(const Factory& factory, std::ostream& os){

}