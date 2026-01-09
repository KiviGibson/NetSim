#include "nodes.hxx"

void Ramp::deliver_goods(Time t){
    if ((t - 1) % _di == 0) {
        Package p = Package();
        push_package(std::move(p)); 
        send_package();
    }
}