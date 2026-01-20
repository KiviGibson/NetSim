#include "IO.hxx"
#include <sstream>
std::map<std::string,FactoryNode> enumToString = {
    {"LOADING_RAMP",FactoryNode::LOADING_RAMP  }, 
    {"WORKER",FactoryNode::WORKER } , 
    {"STOREHOUSE" , FactoryNode::STOREHOUSE} , 
    {"LINK", FactoryNode::LINK },
};


std::vector<std::string> split(const std::string& txt, char delimiter){
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

parsedLineData parse_line(std::string& line) {
    std::vector<std::string> tokens;
    std::string token;

    std::istringstream token_stream(line);
    char delimiter = ' ';


    while (std::getline(token_stream, token, delimiter))
        tokens.push_back(token);

    parsedLineData parsed_data;

    try {
        parsed_data.element_type = enumToString.at(tokens[0]);

        std::for_each(std::next(tokens.cbegin()), tokens.cend(), [&](const std::string& parameter_str) {
            auto key_value = split(parameter_str, '=');
            parsed_data.params[key_value[0]] = key_value[1];
        });
    } catch (std::out_of_range& ex) {
        throw std::exception();
    }

    return parsed_data;
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



void link(Factory& f, const std::map<std::string, std::string>& params) {
    // 1. Poprawione nazwy zmiennych - src to nadawca, dest to odbiorca
    auto src_str = split(params.at("src"), '-');
    auto dest_str = split(params.at("dest"), '-');
    
    // 2. Walidacja rozmiaru
    if (src_str.size() < 2) {
        throw std::logic_error("Nieprawidłowy format linku");
    }
    
    // 3. Znajdź odbiorcę (receiver)
    IPackageReceiver* rec = nullptr;
    
    if (dest_str[0] == "store") {
        auto storehouse_it = f.find_storehouse_by_id(std::stoi(dest_str[1]));
        if (storehouse_it == f.storehouse_cend()) {
            throw std::logic_error("Nie znaleziono magazynu o ID: " + dest_str[1]);
        }
        rec = &const_cast<IPackageReceiver&>(static_cast<const IPackageReceiver&>(*storehouse_it));
    }
    else if (dest_str[0] == "worker") {
        auto worker_it = f.find_worker_by_id(std::stoi(dest_str[1]));
        if (worker_it == f.worker_cend()) {
            throw std::logic_error("Nie znaleziono robota o ID: " + dest_str[1]);
        }
        rec = &const_cast<IPackageReceiver&>(static_cast<const IPackageReceiver&>(*worker_it));
    }
    else {
        throw std::logic_error("Nieznany typ odbiorcy: " + dest_str[0]);
    }
    
    if (rec == nullptr) {
        throw std::logic_error("Brak odbiorcy");
    }
    
    // 4. Znajdź nadawcę (sender) i dodaj odbiorcę do jego preferencji
    if (src_str[0] == "ramp") {
        auto ramp_it = f.find_ramp_by_id(std::stoi(src_str[1]));
        if (ramp_it == f.ramp_cend()) {
            throw std::logic_error("Nie znaleziono rampy o ID: " + src_str[1]);
        }
        
        // Użyj metody, która zwraca NIECONST referencję do preferences
        // Zakładając, że masz metodę:
        // ReceiverPreferences& get_receiver_preferences() { return receiver_preferences_; }
        ramp_it->receiver_preferences_.add_receiver(rec);
    }
    else if (src_str[0] == "worker") {
        auto worker_it = f.find_worker_by_id(std::stoi(src_str[1]));
        if (worker_it == f.worker_cend()) {
            throw std::logic_error("Nie znaleziono robota o ID: " + src_str[1]);
        }
        worker_it->receiver_preferences_.add_receiver(rec);
    }
    else {
        throw std::logic_error("Nieznany typ nadawcy: " + src_str[0]);
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
std::string queue_type_str(PackageQueueType package_queue_type) {
    switch(package_queue_type) {
        case PackageQueueType::FIFO:
            return "FIFO";
        case PackageQueueType::LIFO:
            return "LIFO";
    }
    return {};
}
void link_stream_fill(std::stringstream& link_stream, const PackageSender& package_sender, ElementID package_sender_id, std::string&& package_sender_name) {
    auto prefs = package_sender.receiver_preferences_.get_preferences();

    std::for_each(prefs.cbegin(), prefs.cend(), [&](const std::pair<IPackageReceiver*, double>& key_value) {
        link_stream << "LINK src=" << package_sender_name << "-" << package_sender_id << " ";
        const IPackageReceiver* package_receiver = key_value.first;
        ReciverType receiver_type = package_receiver->get_reciver_type();

        std::string receiver_type_str = receiver_type == ReciverType::WORKER ? "worker" : "store";
        link_stream << "dest=" << receiver_type_str << "-" << package_receiver->get_id() << '\n';
        std::cout << link_stream.str();
    });
}

void save_factory_structure(const Factory& factory, std::ostream& os){

}