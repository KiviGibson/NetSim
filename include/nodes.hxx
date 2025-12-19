#ifndef NODES_HXX
#define NODES_HXX

#include <map>
#include "storage_types.hxx"
#include "package.hxx"
#include "helpers.hxx"

class IPackageReceiver{
    virtual void receive_package(Package&& p) = 0;
    virtual const ElementID get_id() const =0;
};



class ReceiverPreferences{
    using preferences_t = std::map<IPackageReceiver*, double>;
    using const_iterator = preferences_t::const_iterator;
    private:
        ProbabilityGenerator probality_generator;
        preferences_t preferences;
    public:
        ReceiverPreferences(ProbabilityGenerator pg);
        void add_receiver(IPackageReceiver *r);
        void remove_receiver(IPackageReceiver* r);
        IPackageReceiver* choose_receiver(void);
        const preferences_t& get_preferences(void) {return preferences;};
};

#endif