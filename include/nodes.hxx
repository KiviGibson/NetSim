#ifndef WORKER_HPP
#define WORKER_HPP

#include "storage_types.hxx"
#include "types.hxx"
#include "package.hxx"
#include <memory>
#include <optional>

class ReciverPrefrences{};
public:
    using preferences_t = std::map<IpackageReceiver*, double>;
    using const_iterator = preferences_t::const_iterator;

    explicit ReceiverPreferences(ProbabilityGenerator pg = probability_generator) : generate_probability_(
            std::move(pg)) {};
    void add_receiver(IPackageReceiver* receiver);

    void remove_receiver(IPackageReceiver receiver);

    IPackageReceiver* choose_receiver();

    const preferences_t& get_preferences() const {
        return this->preferences_;
    };

    const_iterator cbegin() const noexcept { return preferences_.cbegin(); };
    const_iterator cend() const noexcept { return preferences_.cend(); };
    const_iterator begin() const noexcept { return preferences_.begin(); };
    const_iterator end() const noexcept { return preferences_.end(); };
private:
    preferences_t preferences_;
    ProbabilityGenerator generate_probability_;

class PackageSender{};
class Ramp{};
class IPackageReciver{};
class StoreHouse{};

class Worker: public PackageSender, public IPackageReciver{
    private:
        std::unique_ptr<IPackageQueue> queue;
        std::optional<Package> current_product = std::nullopt;
        TimeOffset work_time;
        Time time;
        ElementID id;

    public:
        void do_work(Time time);
        void recive_package(Package&& pck) override;

        TimeOffset get_processing_duration() const{return work_time;}
        Time get_processing_start_time() const{return time;}
        ElementID get_id() const override{return id;}
        IPackageQueue* get_queue() const {return queue.get();}
};

#endif