
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

// Helper DFS do sprawdzania spójności
// Zwraca true, jeśli z 'sender' da się dojść do jakiegokolwiek magazynu
bool has_reachable_storehouse(const PackageSender* sender, std::map<const PackageSender*, NodeColor>& node_colors) {
    // Jeśli węzeł już zweryfikowany pozytywnie wcześniej -> OK
    if (node_colors[sender] == NodeColor::VERIFIED) {
        return true;
    }
    
    // Oznacz jako odwiedzony (w trakcie przetwarzania)
    node_colors[sender] = NodeColor::VISITED;

    // Pobierz preferencje (mapa <IPackageReceiver*, double>)
    // Uwaga: get_preferences() musi być const, jeśli nie jest w nodes.hpp, może być problem.
    // Zakładam, że w nodes.hpp jest metoda: const preferences_t& get_preferences(void) const;
    // Jeśli w nodes.hpp nie ma const przy tej metodzie, trzeba by użyć const_cast (brzydkie) 
    // lub zakładać, że nodes.hpp pozwala na dostęp. W przesłanym kodzie nodes.hpp get_preferences nie było const,
    // ale w kodzie fabryki operujemy na const PackageSender*.
    // Przyjmuję, że możemy pobrać preferencje.
    
    // Ponieważ sender jest const, musimy użyć const_cast jeśli metoda w nodes.hpp nie jest const-correct,
    // ale zakładamy, że logika na to pozwala. Dla bezpieczeństwa w tym przykładzie:
    auto& preferences = const_cast<PackageSender*>(sender)->receiver_preferences_.get_preferences();

    if (preferences.empty()) {
        throw std::logic_error("Nadawca nie posiada zadnego polaczenia wyjsciowego.");
    }

    bool has_other_receiver = false; // Czy posiada odbiorcę innego niż on sam

    for (const auto& pair : preferences) {
        IPackageReceiver* receiver = pair.first;
        
        // --- ZASTĘPSTWO ZA ENUMA: RTTI (dynamic_cast) ---
        
        // 1. Sprawdzamy czy to Magazyn
        if (receiver->get_reciver_type() == ReciverType::STOREHOUSE) {
            has_other_receiver = true; // Znaleziono magazyn -> ścieżka poprawna
        } 
        else {
            // 2. Skoro nie magazyn, sprawdzamy czy to Robotnik
            Worker* worker_ptr = dynamic_cast<Worker*>(receiver);
            if (worker_ptr != nullptr) {
                // Rzutowanie w górę na PackageSender, aby kontynuować DFS
                PackageSender* sendrecv_ptr = dynamic_cast<PackageSender*>(worker_ptr);
                
                // Sprawdzenie czy nie wskazuje na samego siebie
                if (sendrecv_ptr == sender) {
                    continue; 
                }
                
                has_other_receiver = true;

                // Rekurencja, jeśli jeszcze nie odwiedzono
                if (node_colors[sendrecv_ptr] == NodeColor::UNVISITED) {
                    has_reachable_storehouse(sendrecv_ptr, node_colors);
                }
            }
        }
    }

    node_colors[sender] = NodeColor::VERIFIED;

    if (has_other_receiver) {
        return true;
    } else {
        throw std::logic_error("Brak osiagalnego magazynu (mozliwa petla bez wyjscia).");
    }
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

bool Factory::is_consistent(void) {
    std::map<const PackageSender*, NodeColor> node_colors;

    
    for (const auto& ramp : ramps_) {
        node_colors[&ramp] = NodeColor::UNVISITED;
    }
    for (const auto& worker : workers_) {
        node_colors[&worker] = NodeColor::UNVISITED;
    }

    try {
        
        for (const auto& ramp : ramps_) {
            has_reachable_storehouse(&ramp, node_colors);
        }
    } catch (const std::logic_error&) {
        
        return false;
    }

    return true;
}