#include <gtest/gtest.h>
#include "nodes.hxx"
#include "helpers.hxx"
#include <memory>
/**
 * @brief Mock receiver for verifying package delivery.
 * This class tracks if a package was received and stores its ID for verification.
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
 * @test Verifies that the buffer becomes empty after a package is successfully sent.
 */
TEST(PackageSenderTest, BufferIsEmptyAfterSuccessfulSend) {
    // 1. Arrange: Create sender with a constant generator and one receiver
    ProbabilityGenerator constant_gen = []() { return 0.0; };
    PackageSender sender;
    
    // Assign preferences with a predictable generator
    sender.receiver_preferences_ = ReceiverPreferences(constant_gen);
    
    MockReceiver receiver;
    sender.get_receiver_preferences().add_receiver(&receiver);

    // 2. Act: Push a package into the sender's buffer and try to send it
    sender.push_package(Package(1));
    
    // Verify that the package is actually in the buffer before sending
    ASSERT_TRUE(sender.get_sending().has_value()); 
    
    sender.send_package();

    // 3. Assert: The buffer should now be empty (std::nullopt)
    EXPECT_FALSE(sender.get_sending().has_value());
    EXPECT_EQ(receiver.count(), 1);
    EXPECT_EQ(receiver.last_id(), 1);
}

/**
 * @test Verifies that the package stays in the buffer if there are no receivers.
 * This prevents data loss in the simulation.
 */
TEST(PackageSenderTest, BufferRemainsFullIfNoReceiversAreDefined) {
    // 1. Arrange: Setup sender with NO receivers
    ProbabilityGenerator constant_gen = []() { return 0.0; };
    PackageSender sender;
    sender.receiver_preferences_ = ReceiverPreferences(constant_gen);

    // 2. Act: Push package and attempt to send
    sender.push_package(Package(500));
    
    // This call should not crash and should not clear the buffer
    sender.send_package();

    // 3. Assert: Buffer should still hold the package
    EXPECT_TRUE(sender.get_sending().has_value());
    if (sender.get_sending().has_value()) {
        EXPECT_EQ(sender.get_sending()->get_id(), 500);
    }
}

/**
 * @test Verifies the push_package logic: it should not overwrite an existing package.
 */
TEST(PackageSenderTest, PushPackageDoesNotOverwriteExistingPackage) {
    PackageSender sender;

    // 1. Act: Attempt to push two different packages
    sender.push_package(Package(10)); // Should be accepted
    sender.push_package(Package(20)); // Should be ignored because buffer is full

    // 2. Assert: The first package (ID 10) must remain in the buffer
    ASSERT_TRUE(sender.get_sending().has_value());
    EXPECT_EQ(sender.get_sending()->get_id(), 10);
}

/**
 * @test Verifies that send_package() does nothing when the buffer is empty.
 */
TEST(PackageSenderTest, SendPackageOnEmptyBufferDoesNothing) {
    PackageSender sender;
    MockReceiver receiver;
    sender.get_receiver_preferences().add_receiver(&receiver);

    // 1. Act: Attempt to send without pushing any package first
    sender.send_package();

    // 2. Assert: Receiver should not have received anything
    EXPECT_EQ(receiver.count(), 0);
    EXPECT_FALSE(sender.get_sending().has_value());
}