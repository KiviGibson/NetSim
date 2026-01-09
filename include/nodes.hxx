#ifndef NODES_HXX
#define NODES_HXX

#include <map>
#include "storage_types.hxx"
#include "package.hxx"
#include "helpers.hxx"
#include <optional>
#include "types.hxx"
#include <memory>


class IPackageReceiver{
    public:
        virtual void receive_package(Package&& p) = 0;
        virtual const ElementID get_id() const = 0;
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

class PackageSender{  
    private:
        std::optional<Package> buffer_ = std::nullopt;
    public:
        ReceiverPreferences receiver_preferences_;
        PackageSender() : receiver_preferences_(probability_generator) {};
        PackageSender(PackageSender&&) =default;
        void send_package(void);
        const std::optional<Package>& get_sending(void) const {return buffer_;};
        void push_package(Package&&);
        ReceiverPreferences& get_receiver_preferences(){return receiver_preferences_;};

};
class StoreHouse{};

class Worker: public PackageSender, public IPackageReceiver{
    private:
        std::unique_ptr<IPackageQueue> queue;
        std::optional<Package> current_product = std::nullopt;
        TimeOffset work_time;
        Time time;
        ElementID id;

    public:
        void do_work(Time time);
        void receive_package(Package&& pck) override;

        TimeOffset get_processing_duration() const{return work_time;}
        Time get_processing_start_time() const{return time;}
        ElementID const get_id() const override{return id;}
        IPackageQueue* get_queue() const {return queue.get();}
};

class Ramp : public PackageSender{
    private:
        ElementID _id;
        TimeOffset _di;
    public:
        Ramp(ElementID id,TimeOffset di) : _id(id), _di(di){};
        void deliver_goods(Time t);
        TimeOffset get_delivery_interval(void) const{return _di;};
        ElementID get_id(void) const{return _id;};
};

#endif