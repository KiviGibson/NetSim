
#include "factory.hxx"

// --- Implementacja metod prywatnych i pomocniczych ---

// Szablonowa metoda usuwająca odbiorcę i czyszcząca referencje do niego
template <typename Node>
void Factory::remove_receiver(NodeCollection<Node>& collection, ElementID id) {
    // 1. Znajdź węzeł w kolekcji
    auto it = collection.find_by_id(id);
    if (it == collection.end()) {
        return; // Nie ma takiego elementu
    }

    // 2. Uzyskaj wskaźnik na interfejs IPackageReceiver tego węzła
    // Rzutujemy adres elementu iteratora na IPackageReceiver*
    IPackageReceiver* receiver_ptr = dynamic_cast<IPackageReceiver*>(&(*it));
    
    if (receiver_ptr) {
        // 3. Usuń wskaźnik z preferencji wszystkich Ramp
        for (auto& ramp : ramps_) {
            ramp.receiver_preferences_.remove_receiver(receiver_ptr);
        }

        // 4. Usuń wskaźnik z preferencji wszystkich Robotników
        for (auto& worker : workers_) {
            worker.receiver_preferences_.remove_receiver(receiver_ptr);
        }
    }

    // 5. Usuń fizycznie węzeł z kolekcji
    collection.remove_by_id(id);
}

bool has_reachable_storehouse(const PackageSender* sender, std::map<const PackageSender*, NodeColor>& node_colors) {
    if (node_colors[sender] == NodeColor::VERIFIED) {
        return true;
    }

    node_colors[sender] = NodeColor::VISITED;

    if (sender->receiver_preferences_.get_preferences().empty()) {
        throw std::logic_error("Sender does not have any receivers");
    }

    for (const auto& receiver : sender->receiver_preferences_.get_preferences()) {
        if (receiver.first->get_reciver_type() == ReciverType::STOREHOUSE) {
            return true;
        } else if (receiver.first->get_reciver_type() == ReciverType::WORKER) {
            PackageSender* sendrecv_ptr = dynamic_cast<PackageSender*>(dynamic_cast<class Worker*>(receiver.first));

            if (sendrecv_ptr == sender) {
                continue;
            }

            if (node_colors[sendrecv_ptr] == NodeColor::UNVISITED && has_reachable_storehouse(sendrecv_ptr, node_colors)) {
                return true;
            }
        }
    }

    node_colors[sender] = NodeColor::VERIFIED;
    throw std::logic_error("Error");
}

// --- Implementacja publicznego API Factory ---

void Factory::remove_worker(ElementID id) {
    remove_receiver(workers_, id);
}

void Factory::remove_storehouse(ElementID id) {
    remove_receiver(storehouses_, id);
}

void Factory::do_deliveries(Time t) {
    for (auto& ramp : ramps_) {
        ramp.deliver_goods(t);
    }
}

void Factory::do_package_passing(void) {
    for (auto& ramp : ramps_) {
        ramp.send_package();
    }
    for (auto& worker : workers_) {
        worker.send_package();
    }
}

void Factory::do_work(Time t) {
    for (auto& worker : workers_) {
        worker.do_work(t);
    }
}

bool Factory::is_consistent() const {
    std::map<const PackageSender*, NodeColor> kolor;

    auto set_unvisited_colors = [&kolor](const auto& n) {
        for (const auto& item : n) {
            const PackageSender* sender = dynamic_cast<const PackageSender*>(&item);
            kolor[sender] = NodeColor::UNVISITED;
        }
    };

    set_unvisited_colors(workers_);
    set_unvisited_colors(ramps_);

    try {
        for (const auto& ramp : ramps_) {
            const PackageSender* sender = dynamic_cast<const PackageSender*>(&ramp);
            has_reachable_storehouse(sender, kolor);
        }
    } catch (const std::logic_error&) {
        return false;
    }
    return true;
}
