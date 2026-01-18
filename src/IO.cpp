#include "IO.hxx"
#include <sstream>
std::map<std::string,FactoryNode> enumToString = {
    {"LOADING_RAMP",FactoryNode::LOADING_RAMP  }, 
    {"WORKER",FactoryNode::WORKER } , 
    {"STOREHOUSE" , FactoryNode::STOREHOUSE} , 
    {"LINK", FactoryNode::LINK },
};

parsedLineData parse_line(std::string line){
    if((line.size() >0 && line.find(";")==0) || line.find_first_not_of(" \t\n\v\f\r")== std::string::npos){
        throw std::logic_error("Incorrect String Input");
    }
    std::stringstream ss(line);
    std::string firstWord;
    ss>>firstWord;
    
    if(enumToString.find(firstWord) == enumToString.end()){
        throw 505;
    }

    std::map<std::string,std::string> params = parseParams(ss);
    parsedLineData pld;
    pld.element_type = enumToString[firstWord];
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

std::vector<std::string> split(const std::string txt, char delimiter){
    std::vector<std::string> tokens;
    std::string token;
    for(const auto& letter: txt){
        if(letter == delimiter){
            tokens.push_back(token);
            token = "";
        }else token += letter;
    }
    return tokens;
}

void link(Factory& f, const std::map<std::string, std::string>& params){
    auto src_str = split(params.at("src"), '-');
    auto dest_str = split(params.at("dest"), '-');
    IPackageReceiver*  rec = nullptr; 
    if(src_str[0] == "store"){
        rec = &*f.find_storehouse_by_id(std::stoi(src_str[1]));
    }else if(src_str[0] == "worker"){
        rec = &*f.find_worker_by_id(std::stoi(src_str[1]));
    }
    if(dest_str[0] == "ramp"){
        f.find_ramp_by_id(std::stoi(dest_str[1]))->get_receiver_preferences().add_receiver(rec);
    }else if(dest_str[0] == "worker"){
        f.find_worker_by_id(std::stoi(dest_str[1]))->get_receiver_preferences().add_receiver(rec);
    }
}

Factory load_factory_structure(std::istream& is){
    Factory factory;
    std::string line;
    while(std::getline(is, line)){
        if(line.empty() || line[0] == ';') continue;
        parsedLineData data = parse_line(line);
        switch(data.element_type){
            case FactoryNode::LOADING_RAMP:{
                ElementID id = std::stoi(data.params.at("id"));
                TimeOffset di = std::stoi(data.params.at("delivery-interval"));
                factory.add_ramp(Ramp(id, di));
                break;
            }
            case FactoryNode::WORKER:{
                ElementID id = std::stoi(data.params.at("id"));
                TimeOffset di = std::stoi(data.params.at("processing-time"));
                PackageQueueType type_t = (data.params.at("package")=="FIFO")? PackageQueueType::FIFO : PackageQueueType::LIFO;
                factory.add_worker(Worker(id, di, std::make_unique<PackageQueue>(type_t)));
                break;
            }
            case FactoryNode::STOREHOUSE:{
                ElementID id = std::stoi(data.params.at("id"));
                factory.add_storehouse(StoreHouse(id));
                break;
            }
            case FactoryNode::LINK:{
                link(factory, data.params);
                break;
            }
        }
    }

    return factory;
}

void save_factory_structure(const Factory& factory, std::ostream& os){

}