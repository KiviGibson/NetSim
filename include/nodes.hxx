#ifndef WORKER_HPP
#define WORKER_HPP

#include "storage_types.hxx"
#include "types.hxx"
#include "package.hxx"
#include <memory>
#include <optional>

class ReciverPrefrences {
public:
    using preferences_t = std::map<IpackageReciver *, double>;
    using const_iterator = preferences_t::const_iterator;

    explicit ReciverPreferences(ProbabilityGenerator
    pg = probability_generator
    ) :

    generate_probability_ (
    std::move(pg)) {};

    void add_reciver(IPackageReciver *reciver);

    void remove_reciver(IPackageReciver reciver);

    IPackageReceiver *choose_reciver();

    const preferences_t &get_preferences() const {
        return this->preferences_;
    };

    const_iterator cbegin() const

    noexcept { return preferences_.cbegin(); };

    const_iterator cend() const

    noexcept { return preferences_.cend(); };

    const_iterator begin() const

    noexcept { return preferences_.begin(); };

    const_iterator end() const

    noexcept { return preferences_.end(); };
private:
    preferences_t preferences_;
    ProbabilityGenerator generate_probability_;
};
class PackageSender{};
class Ramp{};
class IPackageReciver{
public:
    virtual void recive_package(Package&& p) = 0;
    virtual ElementID get_id() const = 0;
    virtual IPackageStockpile::const_iterator cbegin() const = 0;
    virtual IPackageStockpile::const_iterator cend() const = 0;
    virtual IPackageStockpile::const_iterator begin() const = 0;
    virtual IPackageStockpile::const_iterator end() const = 0;
    virtual ReciverType get_reciver_type() const = 0;
    virtual ~IPackageReciver() = default;
};
class StoreHouse : public IPackageReciver{
public:
    StoreHouse(ElementID id,
               std::unique_ptr<IPackageStockpile> d = std::make_unique<PackageQueue>(PackageQueueType::FIFO)) : id_(id), d_(std::move(d)) {}
    void receive_package(Package&& p) override;
    ElementID get_id() const override { return id_; }
    ReceiverType get_receiver_type() const override { return ReciverType::STOREHOUSE; };
    IPackageStockpile::const_iterator cbegin() const override { return d_->cbegin(); }
    IPackageStockpile::const_iterator cend() const override { return d_->cend(); }
    IPackageStockpile::const_iterator begin() const override { return d_->begin(); }
    IPackageStockpile::const_iterator end() const override { return d_->end(); }
    IPackageStockpile* get_queue() const { return d_.get(); }
private:
    ElementID id_;
    std::unique_ptr<IPackageStockpile> d_;
};


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