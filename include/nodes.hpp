#ifndef NETSIM_NODES_HPP
#define NETSIM_NODES_HPP
#include "package.hpp"
#include "storage_types.hpp"
#include "helpers.hpp"

#include <memory>
#include <optional>
#include <map>
#include <tuple>
#include <stdexcept>

enum class ReceiverType{
    WORKER,
    STOREHOUSE
};

class IPackageReceiver{
public:
    using const_iterator = IPackageStockpile::const_iterator;

    virtual void receive_package(Package&& p) = 0;
    [[nodiscard]] virtual ElementID get_id() const = 0;
    //#if (defined EXERCISE_ID && EXERCISE_ID != EXERCISE_ID_NODES)
    //[[nodiscard]] virtual ReceiverType get_receiver_type() const = 0;///zaomentować do nodes
    //#endif
    [[nodiscard]] virtual const_iterator cbegin() const = 0;
    [[nodiscard]] virtual const_iterator begin() const = 0;
    [[nodiscard]] virtual const_iterator end() const = 0;
    [[nodiscard]] virtual const_iterator cend() const = 0;///default destructor
};


class ReceiverPreferences{
public:
    using preferences_t = std::map<IPackageReceiver*, double>;
    using const_iterator = preferences_t::const_iterator;

    explicit ReceiverPreferences(ProbabilityGenerator pg = probability_generator): pg_(std::move(pg)){}

    [[nodiscard]] const_iterator begin() const {return preferences_.begin(); }
    [[nodiscard]] const_iterator cbegin() const {return preferences_.cbegin(); }
    [[nodiscard]] const_iterator end() const {return preferences_.end(); }
    [[nodiscard]] const_iterator cend() const {return preferences_.cend(); }

    void add_receiver(IPackageReceiver* r);
    void remove_receiver(IPackageReceiver* r);
    IPackageReceiver* choose_receiver();
    [[nodiscard]] const preferences_t& get_preferences() const {return preferences_;}///tutaj jest inaczej i jest mateda empty

private:
    preferences_t preferences_;
    ProbabilityGenerator pg_;
};

class PackageSender{
public:
    PackageSender()=default;
    PackageSender(PackageSender&&)=default;///bez konstruktorów
    void send_package();
    [[nodiscard]] const std::optional<Package>& get_sending_buffer() const {return buffer_; }///bez constów jest też
    void push_package(Package&& package) {buffer_.emplace(std::move(package));}/// bez std::move

    ReceiverPreferences receiver_preferences_;
private:
    std::optional<Package> buffer_ = std::nullopt;
};

class Worker: public IPackageReceiver, public PackageSender{
public:
    Worker(ElementID id, TimeOffset pd, std::unique_ptr<IPackageQueue> v)
        : id_(id), pd_(pd), queue_(std::move(v)) {}

    void do_work(Time t);
    [[nodiscard]] TimeOffset get_processing_duration()const {return pd_;}
    [[nodiscard]] Time get_package_processing_start_time()const {return start_time_;}
    [[nodiscard]] const std::optional<Package>& get_package_processing_buffer() const {return processing_package_;};
    [[nodiscard]] IPackageQueue* get_queue() const {return queue_.get();};

    [[nodiscard]] const_iterator cbegin() const override {return queue_->cbegin();}
    [[nodiscard]] const_iterator begin() const override {return queue_->begin();}
    [[nodiscard]] const_iterator cend() const override {return queue_->cend();}
    [[nodiscard]] const_iterator end() const override {return queue_->end();}

    [[nodiscard]] ElementID get_id()const override{return id_;}
    void receive_package(Package&&)override;
    //#if (defined EXERCISE_ID && EXERCISE_ID != EXERCISE_ID_NODES)
    //[[nodiscard]] ReceiverType get_receiver_type() const override {return ReceiverType::WORKER; }///zakomentować do nodes
    //#endif
private:
    ElementID id_;
    TimeOffset pd_;
    std::unique_ptr<IPackageQueue> queue_;
    Time start_time_ = 0;
    ReceiverType type_ = ReceiverType::WORKER;

    std::optional<Package> processing_package_;
};

class Storehouse: public IPackageReceiver{
public:
    explicit Storehouse(ElementID id, std::unique_ptr<IPackageStockpile> p = std::make_unique<PackageQueue>(PackageQueueType::FIFO))///lekko inaczej
        : id_(id),list_(std::move(p)) {}

    void push(Package&& package) {list_->push(std::move(package));}///nie ma tego
    [[nodiscard]] bool empty() const {return list_->empty();}
    [[nodiscard]] std::size_t size() const {return list_->size();}

    [[nodiscard]] const_iterator cbegin() const override {return list_->cbegin();}
    [[nodiscard]] const_iterator begin() const override {return list_->begin();}
    [[nodiscard]] const_iterator cend() const override {return list_->cend();}
    [[nodiscard]] const_iterator end() const override {return list_->end();}

    void receive_package(Package&& p) override {list_->push(std::move(p)); }
    [[nodiscard]] ElementID get_id() const override {return id_;}
    //#if (defined EXERCISE_ID && EXERCISE_ID != EXERCISE_ID_NODES)
    //[[nodiscard]] ReceiverType get_receiver_type() const override {return ReceiverType::STOREHOUSE; }///zakomentować dla nodes
    //#endif
private:
    ElementID id_;
    std::unique_ptr<IPackageStockpile> list_;///Package queue nie Ipackage stock pile
    ReceiverType type_ = ReceiverType::STOREHOUSE;
};

class Ramp: public PackageSender{
public:
    Ramp(ElementID id, TimeOffset di)
        :id_(id), di_(di) {}
    void deliver_goods(Time t);
    [[nodiscard]] TimeOffset get_delivery_interval()const {return di_;}
    [[nodiscard]] ElementID get_id()const {return id_;}
private:
    ElementID id_;
    TimeOffset di_;///start time
};




#endif //NETSIM_NODES_HPP
