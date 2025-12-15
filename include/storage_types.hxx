#ifndef PACKAGE_STORAGE_HXX
#define PACKAGE_SORAGE_HXX

#include "package.hxx"
#include <list>
enum class PackageQueueType {
    FIFO,
    LIFO
};

class IPackageStockpile {
    public:
        using const_iterator = std::list<Package>::const_iterator;
        
        virtual ~IPackageStockpile() = default;
        virtual void push(Package&& m) = 0;
        virtual bool empty() const = 0;
        virtual size_t size() const = 0;
        virtual const_iterator begin() const = 0;
        virtual const_iterator end() const = 0;
        virtual const_iterator cbegin() const = 0;
        virtual const_iterator cend() const = 0;
};

class IPackageQueue : public IPackageStockpile{
    public:
        ~IPackageQueue() override = default;
        virtual Package pop() = 0;
        virtual PackageQueueType get_queue_type() const = 0;
};

class PackageQueue: public IPackageQueue{
    PackageQueueType type_;
    std::list<Package> package_list_;
    public:
    PackageQueue() = delete;
    explicit PackageQueue(PackageQueueType type):type_(type), package_list_(){}; 
    ~PackageQueue() override = default;

    void push(Package&& pck) override {
        package_list_.emplace_back(std::move(pck));
    }

    size_t size() const override{
        return package_list_.size();
    }
    
    bool empty() const override{
        return package_list_.empty();
    }

    const_iterator begin() const override {
        return package_list_.cbegin();
    }

    const_iterator end() const override {
        return package_list_.cend();
    }

    const_iterator cbegin() const override {
        return package_list_.cbegin();
    }

    const_iterator cend() const override {
        return package_list_.cend();
    }

    Package pop() override;

    PackageQueueType get_queue_type() const override{
        return type_;
    }
};

#endif