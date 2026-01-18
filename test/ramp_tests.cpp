#include <gtest/gtest.h>
#include "nodes.hxx"
#include "helpers.hxx"
#include <memory>
/**
 * @brief Mock receiver to verify if and when the Ramp delivers a package.
 */
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

    IPackageStockpile::const_iterator cbegin() const override { return d_->cbegin(); }
    IPackageStockpile::const_iterator cend() const override { return d_->cend(); }
    IPackageStockpile::const_iterator begin() const override { return d_->begin(); }
    IPackageStockpile::const_iterator end() const override { return d_->end(); }
    ReciverType get_reciver_type() const override { return ReciverType::STOREHOUSE; }
private:
    ElementID id_ = 1;
    std::unique_ptr<IPackageStockpile> d_;
    int received_pkg_id_ = -1;
    int received_count_ = 0;
};

/**
 * @test Verifies that Ramp sends a package exactly every 'di' turns.
 */
TEST(RampTest, DeliveryHappensInCorrectTurns) {
    TimeOffset di = 3;
    Ramp ramp(1, di);
    MockReceiver receiver;
    ramp.get_receiver_preferences().add_receiver(&receiver);
    // Turn 1: Initial delivery
    ramp.deliver_goods(1);
    EXPECT_EQ(receiver.count(), 1);

    // Turn 2 & 3: No delivery expected
    ramp.deliver_goods(2);
    ramp.deliver_goods(3);
    EXPECT_EQ(receiver.count(), 1);

    // Turn 4: Second delivery expected (1 + 3)
    ramp.deliver_goods(4);
    EXPECT_EQ(receiver.count(), 2);
}

/**
 * @test Verifies that the Ramp buffer is emptied immediately after delivery.
 */
TEST(RampTest, PackageEntersReceiverBufferImmediately) {
    Ramp ramp(1, 1);
    MockReceiver receiver;
    ramp.get_receiver_preferences().add_receiver(&receiver);
    ramp.deliver_goods(1);
    EXPECT_FALSE(ramp.get_sending_buffer().has_value());
}