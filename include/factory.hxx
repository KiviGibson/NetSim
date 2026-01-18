#ifndef FACTORY_HPP
#define FACTORY_HPP

#include <list>
#include <algorithm>
#include <vector>
#include <map>
#include <stdexcept>
#include "nodes.hxx"
template <typename Node>
class NodeCollection {
public:
    // Aliasy typów wymagane przez treść zadania
    using container_t = std::list<Node>;
    using iterator = typename container_t::iterator;
    using const_iterator = typename container_t::const_iterator;

private:
    container_t nodes_;

public:
    // Iteratory
    iterator begin() { return nodes_.begin(); }
    iterator end() { return nodes_.end(); }
    const_iterator begin() const { return nodes_.begin(); }
    const_iterator end() const { return nodes_.end(); }
    const_iterator cbegin() const { return nodes_.cbegin(); }
    const_iterator cend() const { return nodes_.cend(); }

    // Dodawanie węzła (przenoszenie własności)
    void add(Node&& node) {
        nodes_.emplace_back(std::move(node));
    }

    // Wyszukiwanie po ID (wersja modyfikowalna)
    iterator find_by_id(ElementID id) {
        return std::find_if(nodes_.begin(), nodes_.end(),
            [id](const Node& node) { return node.get_id() == id; });
    }

    // Wyszukiwanie po ID (wersja const)
    const_iterator find_by_id(ElementID id) const {
        return std::find_if(nodes_.cbegin(), nodes_.cend(),
            [id](const Node& node) { return node.get_id() == id; });
    }

    // Usuwanie po ID
    void remove_by_id(ElementID id) {
        auto it = find_by_id(id);
        if (it != nodes_.end()) {
            nodes_.erase(it);
        }
    }
};

enum class NodeColor { UNVISITED, VISITED, VERIFIED };

class Factory {
private:
    // Kolekcje węzłów
    NodeCollection<Ramp> ramps_;
    NodeCollection<Worker> workers_;
    NodeCollection<StoreHouse> storehouses_;

    // Pomocnicza metoda szablonowa do usuwania odbiorcy (StoreHouse/Worker)
    // i czyszczenia powiązań u nadawców.
    template <typename Node>
    void remove_receiver(NodeCollection<Node>& collection, ElementID id);

public:
    // --- RAMPS ---
    void add_ramp(Ramp&& r) { ramps_.add(std::move(r)); }
    void remove_ramp(ElementID id) { ramps_.remove_by_id(id); }
    NodeCollection<Ramp>::iterator find_ramp_by_id(ElementID id) { return ramps_.find_by_id(id); }
    NodeCollection<Ramp>::const_iterator find_ramp_by_id(ElementID id) const { return ramps_.find_by_id(id); }
    NodeCollection<Ramp>::const_iterator ramp_cbegin() const { return ramps_.cbegin(); }
    NodeCollection<Ramp>::const_iterator ramp_cend() const { return ramps_.cend(); }

    // --- WORKERS ---
    void add_worker(Worker&& w) { workers_.add(std::move(w)); }
    void remove_worker(ElementID id); 
    NodeCollection<Worker>::iterator find_worker_by_id(ElementID id) { return workers_.find_by_id(id); }
    NodeCollection<Worker>::const_iterator find_worker_by_id(ElementID id) const { return workers_.find_by_id(id); }
    NodeCollection<Worker>::const_iterator worker_cbegin() const { return workers_.cbegin(); }
    NodeCollection<Worker>::const_iterator worker_cend() const { return workers_.cend(); }

    // --- StoreHouseS ---
    void add_storehouse(StoreHouse&& s) { storehouses_.add(std::move(s)); }
    void remove_storehouse(ElementID id);
    NodeCollection<StoreHouse>::iterator find_storehouse_by_id(ElementID id) { return storehouses_.find_by_id(id); }
    NodeCollection<StoreHouse>::const_iterator find_storehouse_by_id(ElementID id) const { return storehouses_.find_by_id(id); }
    NodeCollection<StoreHouse>::const_iterator storehouse_cbegin() const { return storehouses_.cbegin(); }
    NodeCollection<StoreHouse>::const_iterator storehouse_cend() const { return storehouses_.cend(); }

    // --- Logika biznesowa ---
    bool is_consistent(void);
    void do_deliveries(Time t);
    void do_package_passing(void);
    void do_work(Time t);
};

#endif