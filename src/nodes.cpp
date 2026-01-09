#include "nodes.hxx"

void Worker::do_work(Time t){{
    if(!current_product.has_value() and !queue->empty()){}
        current_product.emplace(queue->pop());
        time = t;
    }
    if(time + work_time == t){
        // sender function
        current_product.reset();
    }
}

void Worker::recive_package(Package&& pck){
    queue->push(std::move(pck));
}