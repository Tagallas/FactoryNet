#ifndef NETSIM_STORAGE_TYPES_HPP
#define NETSIM_STORAGE_TYPES_HPP

#include<list>

#include "package.hpp"

enum class PackageQueueType{
    FIFO,
    LIFO
};

class IPackageStockpile{
public:
    using const_iterator = std::list<Package>::const_iterator;

    virtual void push(Package&&) = 0;
    [[nodiscard]] virtual bool empty() const = 0;
    [[nodiscard]] virtual std::size_t size() const = 0;

    [[nodiscard]] virtual IPackageStockpile::const_iterator begin()const = 0;
    [[nodiscard]] virtual IPackageStockpile::const_iterator end()const = 0;
    [[nodiscard]] virtual IPackageStockpile::const_iterator cbegin() const = 0;
    [[nodiscard]] virtual IPackageStockpile::const_iterator cend() const = 0;

    virtual ~IPackageStockpile() = default;
};


class IPackageQueue: public IPackageStockpile{
public:
    virtual Package pop() = 0;
    [[nodiscard]] virtual PackageQueueType get_queue_type() const = 0;///protected: queue_type ale to źle raczej
};

class PackageQueue: public IPackageQueue{
public:
    explicit PackageQueue(PackageQueueType type)
        : type_(type), list_(std::list<Package>()) {}

    void push(Package&& package) override;
    [[nodiscard]] bool empty() const override {return list_.empty();}
    [[nodiscard]] std::size_t size() const override {return list_.size();}

    [[nodiscard]] IPackageStockpile::const_iterator begin() const override {return list_.begin();}
    [[nodiscard]] IPackageStockpile::const_iterator end() const override {return list_.end();}
    [[nodiscard]] IPackageStockpile::const_iterator cbegin() const override {return list_.cbegin();}
    [[nodiscard]] IPackageStockpile::const_iterator cend() const override {return list_.cend();}

    Package pop() override;
    [[nodiscard]] PackageQueueType get_queue_type() const override {return type_;}

    ~PackageQueue()override {list_.begin()->reset();};///nie ma
private:
    PackageQueueType type_;
    std::list<Package> list_;
};


#endif //NETSIM_STORAGE_TYPES_HPP
