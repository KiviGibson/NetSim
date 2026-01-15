#include "package.hxx"

std::set<ElementID> Package::freed = {};
std::set<ElementID> Package::assigned = {}; 

Package::Package(){
    if (freed.empty()){
        if(assigned.empty()) id_ = 1;
        else id_ = *assigned.end() + 1;
    }
    else{
        id_ = *freed.begin();
        freed.erase(id_);
    }
    assigned.insert(id_);
}

Package& Package::operator=(Package&& pck) noexcept{
    if (this == &pck)
        return *this;
    assigned.erase(id_);
    freed.insert(id_);
    id_ = pck.id_;
    assigned.insert(id_);
    return *this;
}

Package::~Package(){
    freed.insert(id_);
    assigned.erase(id_);
}