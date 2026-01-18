#ifndef NODES_HXX
#define NODES_HXX

#include <map>
#include "storage_types.hxx"
#include "package.hxx"
#include "helpers.hxx"
#include <optional>
#include "types.hxx"
#include <memory>

enum class ReciverType{
    STOREHOUSE,
    WORKER
};

class IPackageReceiver{
    public:
        virtual void receive_package(Package&& p) = 0;
        virtual ElementID get_id() const = 0;
        virtual IPackageStockpile::const_iterator cbegin() const = 0;
        virtual IPackageStockpile::const_iterator cend() const = 0;
        virtual IPackageStockpile::const_iterator begin() const = 0;
        virtual IPackageStockpile::const_iterator end() const = 0;
        virtual ReciverType get_reciver_type() const = 0;
        virtual ~IPackageReceiver() = default;
};



class ReceiverPreferences{
    using preferences_t = std::map<IPackageReceiver*, double>;
    using const_iterator = preferences_t::const_iterator;
    ProbabilityGenerator probality_generator;
    preferences_t preferences_;
    public:
        ReceiverPreferences(ProbabilityGenerator pg);
        void add_receiver(IPackageReceiver *r);
        void remove_receiver(IPackageReceiver* r);
        IPackageReceiver* choose_receiver(void);
        const preferences_t& get_preferences(void) {return preferences_;};
        const_iterator cbegin() const
        noexcept { return preferences_.cbegin(); };
        const_iterator cend() const
        noexcept { return preferences_.cend(); };
        const_iterator begin() const
        noexcept { return preferences_.begin(); };
        const_iterator end() const
        noexcept { return preferences_.end(); };  
};

class PackageSender{  
    private:
        std::optional<Package> buffer_ = std::nullopt;
    public:
        ReceiverPreferences receiver_preferences_;
        PackageSender() : receiver_preferences_(probability_generator) {};
        PackageSender(PackageSender&&) =default;
        void send_package(void);
        const std::optional<Package>& get_sending_buffer(void) const {return buffer_;};
        void push_package(Package&&);
        ReceiverPreferences& get_receiver_preferences(){return receiver_preferences_;};
};

class StoreHouse : public IPackageReceiver{
public:
    StoreHouse(ElementID id, std::unique_ptr<IPackageStockpile> d = std::make_unique<PackageQueue>(PackageQueueType::FIFO)) : id_(id), d_(std::move(d)) {};
    void receive_package(Package&& p) override;
    ElementID get_id() const override { return id_; }
    ReciverType get_reciver_type() const override { return ReciverType::STOREHOUSE; };
    IPackageStockpile::const_iterator cbegin() const override { return d_->cbegin(); }
    IPackageStockpile::const_iterator cend() const override { return d_->cend(); }
    IPackageStockpile::const_iterator begin() const override { return d_->begin(); }
    IPackageStockpile::const_iterator end() const override { return d_->end(); }
    IPackageStockpile* get_queue() const { return d_.get(); }
private:
    ElementID id_;
    std::unique_ptr<IPackageStockpile> d_;
};

class Worker: public PackageSender, public IPackageReceiver{
    private:
        std::unique_ptr<IPackageQueue> queue;
        std::optional<Package> current_product = std::nullopt;
        TimeOffset work_time;
        Time time;
        ElementID id;

    public:
        Worker(ElementID id, TimeOffset work_time, std::unique_ptr<IPackageQueue> queue): id(id), work_time(work_time), queue(std::move(queue)) {}
        void do_work(Time time);
        void receive_package(Package&& pck) override;
        ElementID get_current_product_id() const{return current_product.has_value()? current_product->get_id() : -1;};
        TimeOffset get_processing_duration() const{return work_time;}
        Time get_processing_start_time() const{return time;}
        ElementID get_id() const override{return id;}
        ReciverType get_reciver_type() const override { return ReciverType::STOREHOUSE; };
        IPackageStockpile::const_iterator cbegin() const override { return queue->cbegin(); }
        IPackageStockpile::const_iterator cend() const override { return queue->cend(); }
        IPackageStockpile::const_iterator begin() const override { return queue->begin(); }
        IPackageStockpile::const_iterator end() const override { return queue->end(); }
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