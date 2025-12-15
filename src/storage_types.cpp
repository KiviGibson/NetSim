#include "storage_types.hxx"

Package PackageQueue::pop(){
    Package pck;
    switch(type_){
        case PackageQueueType::LIFO:
        pck = std::move(package_list_.back());
        package_list_.pop_back();
        break;
        case PackageQueueType::FIFO:
        pck = std::move(package_list_.front());
        package_list_.pop_front();
        break;
    }
    return pck;    
}