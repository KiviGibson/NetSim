#include <gtest/gtest.h>
#include "factory.hpp"
#include "nodes.hpp"

// Fixture dla testów NodeCollection
class NodeCollectionTest : public ::testing::Test {
protected:
    NodeCollection<Worker> workers;
    NodeCollection<Storehouse> storehouses;
    
    void SetUp() override {
        // Metoda wywoływana przed każdym testem
    }
    
    void TearDown() override {
        // Metoda wywoływana po każdym teście
    }
};

// ========================================
// TESTY WYSZUKIWANIA PO ID
// ========================================

TEST_F(NodeCollectionTest, FindById_TwoObjects_ReturnsCorrectFirst) {
    // Arrange: Dodaj 2 obiekty Worker o różnych ID
    workers.add(Worker(1, 1, std::make_unique<PackageQueue>(PackageQueueType::FIFO)));
    workers.add(Worker(2, 1, std::make_unique<PackageQueue>(PackageQueueType::FIFO)));
    
    // Act: Wyszukaj pierwszy obiekt
    auto it = workers.find_by_id(1);
    
    // Assert: Sprawdź czy znaleziono poprawny obiekt
    ASSERT_NE(it, workers.end());
    EXPECT_EQ(it->get_id(), 1);
}

TEST_F(NodeCollectionTest, FindById_TwoObjects_ReturnsCorrectSecond) {
    // Arrange: Dodaj 2 obiekty Worker o różnych ID
    workers.add(Worker(1, 1, std::make_unique<PackageQueue>(PackageQueueType::FIFO)));
    workers.add(Worker(2, 1, std::make_unique<PackageQueue>(PackageQueueType::FIFO)));
    
    // Act: Wyszukaj drugi obiekt
    auto it = workers.find_by_id(2);
    
    // Assert: Sprawdź czy znaleziono poprawny obiekt
    ASSERT_NE(it, workers.end());
    EXPECT_EQ(it->get_id(), 2);
}

TEST_F(NodeCollectionTest, FindById_TwoObjects_NonExistentIdReturnsEnd) {
    // Arrange: Dodaj 2 obiekty Worker
    workers.add(Worker(1, 1, std::make_unique<PackageQueue>(PackageQueueType::FIFO)));
    workers.add(Worker(2, 1, std::make_unique<PackageQueue>(PackageQueueType::FIFO)));
    
    // Act: Szukaj nieistniejącego ID
    auto it = workers.find_by_id(999);
    
    // Assert: Powinien zwrócić iterator end()
    EXPECT_EQ(it, workers.end());
}

TEST_F(NodeCollectionTest, FindById_ConstVersion_TwoObjects_ReturnsCorrect) {
    // Arrange: Dodaj 2 obiekty Storehouse
    storehouses.add(Storehouse(10));
    storehouses.add(Storehouse(20));
    
    // Act: Użyj const wersji find_by_id
    const auto& const_storehouses = storehouses;
    auto it = const_storehouses.find_by_id(20);
    
    // Assert: Sprawdź czy znaleziono poprawny obiekt
    ASSERT_NE(it, const_storehouses.end());
    EXPECT_EQ(it->get_id(), 20);
}

// ========================================
// TESTY USUWANIA PO ID
// ========================================

TEST_F(NodeCollectionTest, RemoveById_TwoObjects_RemovesCorrectFirst) {
    // Arrange: Dodaj 2 obiekty Worker
    workers.add(Worker(1, 1, std::make_unique<PackageQueue>(PackageQueueType::FIFO)));
    workers.add(Worker(2, 1, std::make_unique<PackageQueue>(PackageQueueType::FIFO)));
    
    // Act: Usuń pierwszy obiekt
    workers.remove_by_id(1);
    
    // Assert: Sprawdź czy został usunięty poprawny obiekt
    auto it1 = workers.find_by_id(1);
    auto it2 = workers.find_by_id(2);
    
    EXPECT_EQ(it1, workers.end()); // Obiekt 1 nie powinien istnieć
    ASSERT_NE(it2, workers.end()); // Obiekt 2 powinien istnieć
    EXPECT_EQ(it2->get_id(), 2);
    
    // Sprawdź rozmiar kolekcji
    EXPECT_EQ(std::distance(workers.begin(), workers.end()), 1);
}

TEST_F(NodeCollectionTest, RemoveById_TwoObjects_RemovesCorrectSecond) {
    // Arrange: Dodaj 2 obiekty Worker
    workers.add(Worker(1, 1, std::make_unique<PackageQueue>(PackageQueueType::FIFO)));
    workers.add(Worker(2, 1, std::make_unique<PackageQueue>(PackageQueueType::FIFO)));
    
    // Act: Usuń drugi obiekt
    workers.remove_by_id(2);
    
    // Assert: Sprawdź czy został usunięty poprawny obiekt
    auto it1 = workers.find_by_id(1);
    auto it2 = workers.find_by_id(2);
    
    ASSERT_NE(it1, workers.end()); // Obiekt 1 powinien istnieć
    EXPECT_EQ(it1->get_id(), 1);
    EXPECT_EQ(it2, workers.end()); // Obiekt 2 nie powinien istnieć
    
    // Sprawdź rozmiar kolekcji
    EXPECT_EQ(std::distance(workers.begin(), workers.end()), 1);
}

TEST_F(NodeCollectionTest, RemoveById_TwoObjects_RemoveNonExistentDoesNothing) {
    // Arrange: Dodaj 2 obiekty Worker
    workers.add(Worker(1, 1, std::make_unique<PackageQueue>(PackageQueueType::FIFO)));
    workers.add(Worker(2, 1, std::make_unique<PackageQueue>(PackageQueueType::FIFO)));
    
    // Act: Usuń nieistniejący obiekt
    workers.remove_by_id(999);
    
    // Assert: Oba obiekty powinny nadal istnieć
    auto it1 = workers.find_by_id(1);
    auto it2 = workers.find_by_id(2);
    
    ASSERT_NE(it1, workers.end());
    ASSERT_NE(it2, workers.end());
    EXPECT_EQ(it1->get_id(), 1);
    EXPECT_EQ(it2->get_id(), 2);
    
    // Sprawdź rozmiar kolekcji
    EXPECT_EQ(std::distance(workers.begin(), workers.end()), 2);
}

TEST_F(NodeCollectionTest, RemoveById_TwoObjects_RemoveBothLeavesEmpty) {
    // Arrange: Dodaj 2 obiekty Storehouse
    storehouses.add(Storehouse(10));
    storehouses.add(Storehouse(20));
    
    // Act: Usuń oba obiekty
    storehouses.remove_by_id(10);
    storehouses.remove_by_id(20);
    
    // Assert: Kolekcja powinna być pusta
    EXPECT_EQ(storehouses.begin(), storehouses.end());
    EXPECT_EQ(std::distance(storehouses.begin(), storehouses.end()), 0);
}

// ========================================
// TESTY DODATKOWE - ITERATORY
// ========================================

TEST_F(NodeCollectionTest, Iterators_TwoObjects_CanIterate) {
    // Arrange: Dodaj 2 obiekty
    workers.add(Worker(1, 1, std::make_unique<PackageQueue>(PackageQueueType::FIFO)));
    workers.add(Worker(2, 1, std::make_unique<PackageQueue>(PackageQueueType::FIFO)));
    
    // Act & Assert: Iteruj przez kolekcję
    std::vector<ElementID> ids;
    for (const auto& worker : workers) {
        ids.push_back(worker.get_id());
    }
    
    EXPECT_EQ(ids.size(), 2);
    EXPECT_TRUE(std::find(ids.begin(), ids.end(), 1) != ids.end());
    EXPECT_TRUE(std::find(ids.begin(), ids.end(), 2) != ids.end());
}

TEST_F(NodeCollectionTest, Add_MultipleObjects_MaintainsOrder) {
    // Arrange & Act: Dodaj 3 obiekty w określonej kolejności
    workers.add(Worker(5, 1, std::make_unique<PackageQueue>(PackageQueueType::FIFO)));
    workers.add(Worker(3, 1, std::make_unique<PackageQueue>(PackageQueueType::FIFO)));
    workers.add(Worker(7, 1, std::make_unique<PackageQueue>(PackageQueueType::FIFO)));
    
    // Assert: Sprawdź kolejność dodawania
    auto it = workers.begin();
    EXPECT_EQ(it->get_id(), 5);
    ++it;
    EXPECT_EQ(it->get_id(), 3);
    ++it;
    EXPECT_EQ(it->get_id(), 7);
}