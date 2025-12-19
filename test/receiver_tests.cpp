#include <gtest/gtest.h>
#include <numeric>
#include "nodes.hxx"

// Helpers generator helps us to fixed score of random choice
double constant_probability = 0.0;
double mock_generator() { return constant_probability; }

// Dummy receiver class to tests
class FakeReceiver : public IPackageReceiver {
public:
    void receive_package(Package&& p) override{}
    const ElementID get_id() const override{ return 1; }
};

// --- TESTS ---

TEST(ReceiverPreferencesTest, IsProbabilityDistributedEqually) {
    ReceiverPreferences rp(mock_generator);
    FakeReceiver r1, r2, r3;

    rp.add_receiver(&r1);
    rp.add_receiver(&r2);
    rp.add_receiver(&r3);

    auto prefs = rp.get_preferences();
    
    // We check if all of the receivers has the same probability
    for (auto const& [receiver, prob] : prefs) {
        EXPECT_NEAR(prob, 1.0 / 3.0, 0.0001);
    }
}

TEST(ReceiverPreferencesTest, IsProbabilitySumEqualOne) {
    ReceiverPreferences rp(mock_generator);
    FakeReceiver r1, r2, r3, r4;

    rp.add_receiver(&r1);
    rp.add_receiver(&r2);
    rp.add_receiver(&r3);
    rp.remove_receiver(&r1);

    auto prefs = rp.get_preferences();
    double sum = 0.0;
    for (auto const& [receiver, prob] : prefs) {
        sum += prob;
    }

    EXPECT_NEAR(sum, 1.0, 0.0001);
}

TEST(ReceiverPreferencesTest, ThrowsWhenRemovingLastReceiver) {
    ReceiverPreferences rp(mock_generator);
    FakeReceiver r1;

    rp.add_receiver(&r1);


    EXPECT_THROW(rp.remove_receiver(&r1), std::invalid_argument);
}

TEST(ReceiverPreferencesTest, ChooseReceiverCorrectly) {
    ReceiverPreferences rp(mock_generator);
    FakeReceiver r1, r2;

    rp.add_receiver(&r1);
    rp.add_receiver(&r2);

    // We have to check which pointer is smaller in memory cause our map is sorting them through memory adress
    IPackageReceiver* first = (&r1 < &r2) ? &r1 : &r2;
    IPackageReceiver* second = (&r1 < &r2) ? &r2 : &r1;

    // 1 Scenario
    constant_probability = 0.2;
    EXPECT_EQ(rp.choose_receiver(), first);

    // 2 Scenario
    constant_probability = 0.8;
    EXPECT_EQ(rp.choose_receiver(), second);
}

TEST(ReceiverPreferencesTest, AddingExistingReceiverDoesNothing) {
    ReceiverPreferences rp(mock_generator);
    FakeReceiver r1;

    rp.add_receiver(&r1);
    rp.add_receiver(&r1); // Próba dodania tego samego

    EXPECT_EQ(rp.get_preferences().size(), 1);
}