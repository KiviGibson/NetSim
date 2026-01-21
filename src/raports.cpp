#include "raports.hxx"

void generate_structure_report(const Factory& f, std::ostream os){
    os << "\n == Loading Ramps == \n\n";
    std::set<ElementID> workers;
    for(auto it = f.ramp_cbegin(); it != f.ramp_cend(); it++){
        os << "Loading Ramp #" << std::to_string(it->get_id());
        os << "\n Delivery interval = " << std::to_string(it->get_delivery_interval());
        os << "\n Receivers:\n";
        for(auto rit = it->receiver_preferences_.cbegin(); rit != it->receiver_preferences_.cend(); rit++){
            switch(rit->first->get_reciver_type()){
                case ReciverType::WORKER: workers.insert(rit->first->get_id()); break;
                case ReciverType::STOREHOUSE:os << "\tstorehouse #" << std::to_string(rit->first->get_id());  break;
            }
        }
        for(auto worker: workers){
            os << "\tworker #" << std::to_string(worker);
        }
        os << "\n";
    }

    os<< "\n == Workers == \n\n";
    for(auto it = f.worker_cbegin(); it != f.worker_cend(); it++){
        os << "Worker #" << std::to_string(it->get_id());
        os << "\n Processing Time = " << std::to_string(it->get_processing_duration());
        os << "\n Queue Time = " << (it->get_queue()->get_queue_type() == PackageQueueType::FIFO)? "FIFO" : "LIFO";
        for(auto rit = it->receiver_preferences_.cbegin(); rit != it->receiver_preferences_.cend(); rit++){
            switch(rit->first->get_reciver_type()){
                case ReciverType::WORKER: workers.insert(rit->first->get_id()); break;
                case ReciverType::STOREHOUSE:os << "\tstorehouse #" << std::to_string(rit->first->get_id());  break;
            }
        }
        for(auto worker: workers){
            os << "\tworker #" << std::to_string(worker);
        }
        os << "\n";
    }
    os << "\n == StoreHouses == \n\n";
    for(auto it = f.storehouse_cbegin(); it != f.storehouse_cend(); it++){
        os << "StoreHouse #" << std::to_string(it->get_id()) << "\n\n";
    }
}

void generate_simulation_turn_report(const Factory& f, std::ostream& os, Time t){
    os << "=== [ Turn: <<" << std::to_string(t) << " ] ==\n\n";
    os << "== Workers ==\n";
    for(auto it = f.worker_cbegin(); it != f.worker_cend(); it++){
        os << "Worker #" << std::to_string(it->get_id()) << "\n";
        os << "Current Product = "<< ((it->get_current_product_id() != 0)? std::to_string(it->get_current_product_id()): "(empty)") << "\n";
        if (it->get_queue()->empty()) os << "Queue: (empty)\n";
        else{
            os << "Queue =";
            for(auto pck = it->get_queue()->cbegin(); pck != it->get_queue()->cend(); pck++){
                os << " #" << pck->get_id() <<",";
            }
        }
        os << "/n";
        os << "Sending = " << ((it->get_sending_buffer().has_value())? "#"+it->get_sending_buffer()->get_id() : "(empty)") << "\n";
    }
    os << "\n == StoreHouses == \n\n";
    for(auto it = f.storehouse_cbegin(); it != f.storehouse_cend(); it++){
        os << "StoreHouse #" << std::to_string(it->get_id()) << "\n";
        if (it->get_queue()->empty()) os << "Stock = (empty)\n";
        else{
            os << "Stock =";
            for(auto pck = it->get_queue()->cbegin(); pck != it->get_queue()->cend(); pck++){
                os << " #" << std::to_string(pck->get_id()) << ",";
            }
        }
        os << "\n\n";
    }
}