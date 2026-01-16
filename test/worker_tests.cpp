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
    
    // Test helper methods
    int last_id() const { return received_pkg_id_; }
    int count() const { return received_count_; }

private:
    ElementID id_ = 1;
    int received_pkg_id_ = -1;
    int received_count_ = 0;
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

};

