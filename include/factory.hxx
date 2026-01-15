#include "nodes.hxx"
class Factory{
public:
    void add_ramp(Ramp&& ramp);
    void add_worker(Worker&& worker);
    void add_storehouse(StoreHouse&& store_house);
    NodeColection<Ramp>::iterator find_ramp_by_id(ElementID id);
    NodeColection<Worker>::iterator find_worker_by_id(ElementID id);
    NodeColection<StoreHouse>::iterator find_storehouse_by_id(ElementID id);

};

template<typename Node>
class NodeColection{
public:
    void add(Node&& node);
    void remove_by_id(ElementID id);
    NodeCollection<Node>::iterator find_by_id(ElementID id);
    const NodeCollection<Node>::const_iterator find_by_id(ElementID id); 
};