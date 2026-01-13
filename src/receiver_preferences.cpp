#include "nodes.hxx"
#include <stdexcept>

ReceiverPreferences::ReceiverPreferences(ProbabilityGenerator pg){
    ReceiverPreferences::probality_generator = pg;
}

void ReceiverPreferences::add_receiver(IPackageReceiver* r){
    if(ReceiverPreferences::preferences.count(r)){
        //That means this receiver is already in ReceiverPreferences
        return;
    }
    ReceiverPreferences:preferences[r] =0.0;
    double new_probability = 1.0 / ReceiverPreferences::preferences.size();
    for (auto& [receiver, prob] : preferences) {
        prob = new_probability;
    }
}

void ReceiverPreferences::remove_receiver(IPackageReceiver* r) {
    if (preferences.size() <= 1) {
        throw std::invalid_argument("You cannot remove element if there is only 1 element");
    }

    // 2. Usuń odbiorcę
    if (preferences.erase(r) > 0) {
        // 3. Jeśli coś usunięto, znormalizuj pozostałe wagi
        double new_probability = 1.0 / preferences.size();
        for (auto& [receiver, prob] : preferences) {
            prob = new_probability;
        }
    }
}

IPackageReceiver* ReceiverPreferences::choose_receiver() {
    double random_val = ReceiverPreferences::probality_generator(); 
    
    double cumulative_sum = 0.0;
    for (const auto& [receiver, prob] : preferences) {
        cumulative_sum += prob;
        if (random_val <= cumulative_sum) {
            return receiver;
        }
    }
    return preferences.rbegin()->first;
}