#include <gtest/gtest.h>
#include "nodes.hxx"
#include "helpers.hxx"

/**
 * @brief Mock receiver to verify if and when the Ramp delivers a package.
 */
class MockReceiver : public IPackageReceiver {
public:
    void receive_package(Package&& pkg) override {
        received_count++;
        last_pkg_id = pkg.get_id();
    }
    const ElementID get_id() const override { return 1; };
    int count(){return received_count;};
    int received_count = 0;
    int last_pkg_id = -1;
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

    // Act: deliver goods
    ramp.deliver_goods(1);

    // Assert: Package should be in receiver, and Ramp's buffer should be empty
    EXPECT_EQ(receiver.count(), 1);
    EXPECT_FALSE(ramp.get_sending().has_value()); // This failed because send_package() was not called
}