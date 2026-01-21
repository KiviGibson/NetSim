#include <gtest/gtest.h>
#include "factory.hxx"
#include "nodes.hxx"

class FactoryTest : public ::testing::Test {
protected:
    Factory factory;
};

// ==================== TESTY NodeCollection ====================

TEST_F(FactoryTest, NodeCollection_AddAndFind) {
    factory.add_ramp(Ramp(1, 5));
    
    auto it = factory.find_ramp_by_id(1);
    ASSERT_NE(it, factory.ramp_cend());
    EXPECT_EQ(it->get_id(), 1);
}

TEST_F(FactoryTest, NodeCollection_FindNonExistent) {
    factory.add_ramp(Ramp(1, 5));
    
    auto it = factory.find_ramp_by_id(999);
    EXPECT_EQ(it, factory.ramp_cend());
}

TEST_F(FactoryTest, NodeCollection_RemoveById) {
    factory.add_ramp(Ramp(1, 5));
    factory.add_ramp(Ramp(2, 10));
    
    factory.remove_ramp(1);
    
    EXPECT_EQ(factory.find_ramp_by_id(1), factory.ramp_cend());
    EXPECT_NE(factory.find_ramp_by_id(2), factory.ramp_cend());
}

// ==================== TESTY remove_receiver ====================

TEST_F(FactoryTest, RemoveNonExistentNode_DoesNotThrow) {
    factory.add_worker(Worker(1, 3, std::make_unique<PackageQueue>(PackageQueueType::FIFO)));
    
    // Usuwanie nieistniejącego węzła nie powinno rzucać wyjątku
    EXPECT_NO_THROW(factory.remove_worker(999));
}

// ==================== TESTY is_consistent ====================

TEST_F(FactoryTest, IsConsistent_ValidPath_RampToStorehouse) {
    factory.add_ramp(Ramp(1, 5));
    factory.add_storehouse(StoreHouse(1));
    
    auto ramp_it = factory.find_ramp_by_id(1);
    auto store_it = factory.find_storehouse_by_id(1);
    
    ramp_it->receiver_preferences_.add_receiver(&(*store_it));
    
    EXPECT_TRUE(factory.is_consistent());
}

TEST_F(FactoryTest, IsConsistent_ValidPath_RampWorkerStorehouse) {
    factory.add_ramp(Ramp(1, 5));
    factory.add_worker(Worker(1, 3, std::make_unique<PackageQueue>(PackageQueueType::FIFO)));
    factory.add_storehouse(StoreHouse(1));
    
    auto ramp_it = factory.find_ramp_by_id(1);
    auto worker_it = factory.find_worker_by_id(1);
    auto store_it = factory.find_storehouse_by_id(1);
    
    ramp_it->receiver_preferences_.add_receiver(&(*worker_it));
    worker_it->receiver_preferences_.add_receiver(&(*store_it));
    
    EXPECT_TRUE(factory.is_consistent());
}

TEST_F(FactoryTest, IsConsistent_NoConnections_Fails) {
    factory.add_ramp(Ramp(1, 5));
    
    // Rampa bez połączeń
    EXPECT_FALSE(factory.is_consistent());
}


TEST_F(FactoryTest, IsConsistent_MultiplePathsToStorehouse) {
    factory.add_ramp(Ramp(1, 5));
    factory.add_worker(Worker(1, 3, std::make_unique<PackageQueue>(PackageQueueType::FIFO)));
    factory.add_worker(Worker(2, 3, std::make_unique<PackageQueue>(PackageQueueType::FIFO)));
    factory.add_storehouse(StoreHouse(1));
    
    auto ramp_it = factory.find_ramp_by_id(1);
    auto w1 = factory.find_worker_by_id(1);
    auto w2 = factory.find_worker_by_id(2);
    auto store_it = factory.find_storehouse_by_id(1);
    
    // Ramp -> W1 -> W2 -> Storehouse
    //          \-> Storehouse
    ramp_it->receiver_preferences_.add_receiver(&(*w1));
    w1->receiver_preferences_.add_receiver(&(*w2));
    w1->receiver_preferences_.add_receiver(&(*store_it));
    w2->receiver_preferences_.add_receiver(&(*store_it));
    
    EXPECT_TRUE(factory.is_consistent());
}

TEST_F(FactoryTest, IsConsistent_MultipleRamps) {
    factory.add_ramp(Ramp(1, 5));
    factory.add_ramp(Ramp(2, 10));
    factory.add_storehouse(StoreHouse(1));
    
    auto ramp1_it = factory.find_ramp_by_id(1);
    auto ramp2_it = factory.find_ramp_by_id(2);
    auto store_it = factory.find_storehouse_by_id(1);
    
    ramp1_it->receiver_preferences_.add_receiver(&(*store_it));
    ramp2_it->receiver_preferences_.add_receiver(&(*store_it));
    
    EXPECT_TRUE(factory.is_consistent());
}

TEST_F(FactoryTest, IsConsistent_OneRampHasNoPath_Fails) {
    factory.add_ramp(Ramp(1, 5));
    factory.add_ramp(Ramp(2, 10));
    factory.add_storehouse(StoreHouse(1));
    
    auto ramp1_it = factory.find_ramp_by_id(1);
    auto store_it = factory.find_storehouse_by_id(1);
    
    // Tylko Ramp1 ma połączenie, Ramp2 nie ma
    ramp1_it->receiver_preferences_.add_receiver(&(*store_it));
    
    EXPECT_FALSE(factory.is_consistent());
}

TEST_F(FactoryTest, IsConsistent_EmptyFactory) {
    // Pusta fabryka - co powinno zwrócić?
    // Według specyfikacji sprawdzamy każdą rampę, więc brak ramp = success
    EXPECT_TRUE(factory.is_consistent());
}

TEST_F(FactoryTest, IsConsistent_ComplexValidNetwork) {
    // Złożona sieć: 2 Rampy, 3 Workers, 2 Storehouses
    factory.add_ramp(Ramp(1, 5));
    factory.add_ramp(Ramp(2, 10));
    factory.add_worker(Worker(1, 3, std::make_unique<PackageQueue>(PackageQueueType::FIFO)));
    factory.add_worker(Worker(2, 3, std::make_unique<PackageQueue>(PackageQueueType::FIFO)));
    factory.add_worker(Worker(3, 3, std::make_unique<PackageQueue>(PackageQueueType::FIFO)));
    factory.add_storehouse(StoreHouse(1));
    factory.add_storehouse(StoreHouse(2));
    
    auto ramp1 = factory.find_ramp_by_id(1);
    auto ramp2 = factory.find_ramp_by_id(2);
    auto w1 = factory.find_worker_by_id(1);
    auto w2 = factory.find_worker_by_id(2);
    auto w3 = factory.find_worker_by_id(3);
    auto s1 = factory.find_storehouse_by_id(1);
    auto s2 = factory.find_storehouse_by_id(2);
    
    // Ramp1 -> W1 -> W3 -> S1
    // Ramp2 -> W2 -> S2
    ramp1->receiver_preferences_.add_receiver(&(*w1));
    ramp2->receiver_preferences_.add_receiver(&(*w2));
    w1->receiver_preferences_.add_receiver(&(*w3));
    w2->receiver_preferences_.add_receiver(&(*s2));
    w3->receiver_preferences_.add_receiver(&(*s1));
    
    EXPECT_TRUE(factory.is_consistent());
}

// ==================== TESTY Iteratorów ====================

TEST(FactoryUpelTest, IsConsistentMissingLink2) {
// R -> W1 -> S
// W1 -> W2 -> W2

    Factory factory;
    factory.add_ramp(Ramp(1, 1));
    factory.add_worker(Worker(1, 1, std::make_unique<PackageQueue>(PackageQueueType::FIFO)));
    factory.add_worker(Worker(2, 1, std::make_unique<PackageQueue>(PackageQueueType::FIFO)));
    factory.add_storehouse(StoreHouse(1));

    Ramp& r = *(factory.find_ramp_by_id(1));
    r.receiver_preferences_.add_receiver(&(*factory.find_worker_by_id(1)));

    Worker& w1 = *(factory.find_worker_by_id(1));
    w1.receiver_preferences_.add_receiver(&(*factory.find_storehouse_by_id(1)));
    w1.receiver_preferences_.add_receiver(&(*factory.find_worker_by_id(2)));

    Worker& w2 = *(factory.find_worker_by_id(2));
    w2.receiver_preferences_.add_receiver(&(*factory.find_worker_by_id(2)));

    EXPECT_FALSE(factory.is_consistent());
}

TEST_F(FactoryTest, Iterators_RampIteration) {
    factory.add_ramp(Ramp(1, 5));
    factory.add_ramp(Ramp(2, 10));
    factory.add_ramp(Ramp(3, 15));
    
    int count = 0;
    for (auto it = factory.ramp_cbegin(); it != factory.ramp_cend(); ++it) {
        count++;
        EXPECT_TRUE(it->get_id() >= 1 && it->get_id() <= 3);
    }
    
    EXPECT_EQ(count, 3);
}

TEST_F(FactoryTest, Iterators_WorkerIteration) {
    factory.add_worker(Worker(1, 5, std::make_unique<PackageQueue>(PackageQueueType::FIFO)));
    factory.add_worker(Worker(2, 3, std::make_unique<PackageQueue>(PackageQueueType::FIFO)));
    
    int count = 0;
    for (auto it = factory.worker_cbegin(); it != factory.worker_cend(); ++it) {
        count++;
    }
    
    EXPECT_EQ(count, 2);
}

TEST_F(FactoryTest, Iterators_StorehouseIteration) {
    factory.add_storehouse(StoreHouse(1));
    factory.add_storehouse(StoreHouse(2));
    
    int count = 0;
    for (auto it = factory.storehouse_cbegin(); it != factory.storehouse_cend(); ++it) {
        count++;
    }
    
    EXPECT_EQ(count, 2);
}

// ==================== TESTY Edge Cases ====================

TEST_F(FactoryTest, EdgeCase_WorkerOnlyConnectedToItself) {
    factory.add_ramp(Ramp(1, 5));
    factory.add_worker(Worker(1, 3, std::make_unique<PackageQueue>(PackageQueueType::FIFO)));
    factory.add_storehouse(StoreHouse(1));
    
    auto ramp_it = factory.find_ramp_by_id(1);
    auto worker_it = factory.find_worker_by_id(1);
    auto store_it = factory.find_storehouse_by_id(1);
    
    // Worker ma dwa połączenia: do siebie i do magazynu
    ramp_it->receiver_preferences_.add_receiver(&(*worker_it));
    worker_it->receiver_preferences_.add_receiver(&(*worker_it)); // self
    worker_it->receiver_preferences_.add_receiver(&(*store_it));  // valid path
    
    // To POWINNO być spójne, bo mimo self-loop jest ścieżka do magazynu
    EXPECT_TRUE(factory.is_consistent());
}


TEST_F(FactoryTest, IsConsistentCorrect) {
    // R -> W -> S

    Factory factory;
    factory.add_ramp(Ramp(1, 1));
    factory.add_worker(Worker(1, 1, std::make_unique<PackageQueue>(PackageQueueType::FIFO)));
    factory.add_storehouse(StoreHouse(1));

    Ramp& r = *(factory.find_ramp_by_id(1));
    r.receiver_preferences_.add_receiver(&(*factory.find_worker_by_id(1)));

    Worker& w = *(factory.find_worker_by_id(1));
    w.receiver_preferences_.add_receiver(&(*factory.find_storehouse_by_id(1)));

    EXPECT_TRUE(factory.is_consistent());
}