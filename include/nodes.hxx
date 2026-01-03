#ifndef WORKER_HPP
#define WORKER_HPP

#include "storage_types.hxx"
#include "types.hxx"
#include "package.hxx"
#include <memory>
#include <optional>

class ReciverPrefrences{};
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