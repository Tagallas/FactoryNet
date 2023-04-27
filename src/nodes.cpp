#include "nodes.hpp"

IPackageReceiver* ReceiverPreferences::choose_receiver(){
    double random = pg_();
    double prev=0;
    for(auto& receiver:preferences_){
        prev+=receiver.second;
        if (random <= prev)///bez =
            return receiver.first;
    }

    throw std::logic_error("No receiever was chosen");
}
void ReceiverPreferences::add_receiver(IPackageReceiver* r){
    preferences_.emplace(r,1);
    double probability = 1.0/ double(preferences_.size());
    for (auto& el: preferences_){
        el.second=probability;
    }

}

void ReceiverPreferences::remove_receiver(IPackageReceiver* r){///moje lepsze chyba
    if (preferences_.find(r) != preferences_.end()) {
        preferences_.erase(r);
        if (!preferences_.empty()){
            double probability = 1.0 / double(preferences_.size());
            for (auto& el: preferences_) {
                el.second = probability;
            }
        }
    }

}

void PackageSender::send_package(){
    if(buffer_.has_value()){
        IPackageReceiver* receiver = receiver_preferences_.choose_receiver();
        receiver->receive_package(std::move(*buffer_));///std move zmienia na buffer_->get_id()
        buffer_.reset();
    }
}

void Worker::do_work(Time t){///to jest inaczej całkiem
    if (!processing_package_ && !queue_->empty()){
        processing_package_.emplace(std::move(queue_->pop()));
        start_time_=t;
    }

    if (start_time_+pd_-1==t){
        push_package(std::move(*processing_package_));
        processing_package_.reset();
       // start_time_=0;
    }
}

void Worker::receive_package(Package&& package) {
    //PackageSender::push_package(std::move(package));
    queue_->push(std::move(package));
}

void Ramp::deliver_goods(Time t) {///inaczej całkiem
    if (t % di_ == 1)
        push_package(Package());
}