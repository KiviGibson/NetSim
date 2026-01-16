#include "nodes.hxx"
#include <gtest/gtest.h>
#include "package.hxx"
#include "storage_types.hxx"

class MockReceiver : public IPackageReceiver {
public:
    void receive_package(Package&& pkg) override {
        received_pkg_id_ = pkg.get_id();
        received_count_++;
        
    }
    
    // Required by the IPackageReceiver interface
    ElementID get_id() const override { return id_; };
    IPackageStockpile::const_iterator cbegin() const override { return d_->cbegin(); }
    IPackageStockpile::const_iterator cend() const override { return d_->cend(); }
    IPackageStockpile::const_iterator begin() const override { return d_->begin(); }
    IPackageStockpile::const_iterator end() const override { return d_->end(); }
    IPackageStockpile* get_queue() const { return d_.get(); }
    ReciverType get_reciver_type() const override {return ReciverType::STOREHOUSE; }
    // Test helper methods
    int last_id() const { return received_pkg_id_; }
    int count() const { return received_count_; }

private:
    ElementID id_ = 1;
    int received_pkg_id_ = -1;
    int received_count_ = 0;
    std::unique_ptr<IPackageStockpile> d_;
};

TEST(WorkerTest, WorkerTestRecivingPackageTest){
    Worker workerFIFO = Worker(1, 1, std::make_unique<PackageQueue>(PackageQueueType::FIFO));
    Worker workerLIFO = Worker(2, 1, std::make_unique<PackageQueue>(PackageQueueType::LIFO));
    workerFIFO.receive_package(Package(1)); // 1
    workerFIFO.receive_package(Package(2)); // 2
    Package p = std::move(workerFIFO.get_queue()->pop());
    EXPECT_EQ(p.get_id(), 1);
    p = std::move(workerFIFO.get_queue()->pop());
    EXPECT_EQ(p.get_id(), 2);
    workerLIFO.receive_package(Package(4)); // 3
    workerLIFO.receive_package(Package(3)); // 4
    p = std::move(workerLIFO.get_queue()->pop());
    EXPECT_EQ(p.get_id(), 3);
    p = std::move(workerLIFO.get_queue()->pop());
    EXPECT_EQ(p.get_id(), 4);
};

TEST(WorkerTest, WorkerTestWorkCycleTest){
    Worker worker = Worker(1, 3, std::make_unique<PackageQueue>(PackageQueueType::FIFO));
    worker.receive_package(Package(1));

    MockReceiver mock;
    worker.receiver_preferences_.add_receiver(&mock);
    worker.do_work(1);
    EXPECT_EQ(mock.count(), 0);
    worker.do_work(2);
    EXPECT_EQ(mock.count(), 0);
    worker.do_work(3);
    EXPECT_EQ(mock.count(), 0);
    worker.do_work(4);
    EXPECT_EQ(mock.count(), 1);
    worker.do_work(5);
    worker.receive_package(Package(2));
    worker.do_work(6);
    worker.do_work(7);
    worker.do_work(8);
    EXPECT_EQ(mock.count(), 1);
    worker.do_work(9);
    EXPECT_EQ(mock.count(), 2);
};

