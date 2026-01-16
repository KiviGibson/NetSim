#include "nodes.hxx"

void Worker::do_work(Time t){
    if(!current_product.has_value() && !queue->empty()){
        current_product.emplace(queue->pop());
        time = t;
    }
    if(time + work_time == t){
        push_package(Package(current_product.value().get_id()));
        send_package();
        current_product.reset();
    }
}

void Worker::receive_package(Package&& pck){
    queue->push(std::move(pck));
}