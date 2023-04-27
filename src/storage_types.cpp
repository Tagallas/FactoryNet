#include "storage_types.hpp"

void PackageQueue::push(Package&& package){
    list_.emplace_back(std::move(package));
}

Package PackageQueue::pop() {
    Package p;
    switch (type_) {
        case PackageQueueType::FIFO:
            p = std::move(*list_.begin());
            list_.pop_front();
            break;
        case PackageQueueType::LIFO:
            p = std::move(*list_.end());
            list_.pop_back();
            break;
    }
    return p;
}