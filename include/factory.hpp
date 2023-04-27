#ifndef NETSIM_FACTORY_HPP
#define NETSIM_FACTORY_HPP

#include <sstream>

#include "nodes.hpp"

enum class NodeColor { UNVISITED, VISITED, VERIFIED };

template <class Node>
class NodeCollection{
public:
    using container_t = typename std::list<Node>;
    using iterator = typename container_t::iterator;
    using const_iterator = typename container_t::const_iterator;

    void add(Node&& node){collection_.push_back(std::move(node)); }
    void remove_by_id(ElementID id);

    const_iterator cbegin() const {return collection_.cbegin(); }
    const_iterator cend() const {return collection_.cend(); }
    iterator begin() {return collection_.begin(); }
    iterator end() {return collection_.end(); }

    NodeCollection<Node>::iterator find_by_id(ElementID id) {return std::find_if(collection_.begin(), collection_.end(), [id](const auto& elem){return elem.get_id()==id;}); }
    NodeCollection<Node>::const_iterator find_by_id(ElementID id) const {return std::find_if(collection_.cbegin(), collection_.cend(), [id](const auto& elem){return elem.get_id()==id;}); }

private:
    container_t collection_;
};

class Factory{
public:
    void add_ramp(Ramp&& r) {ramp_collection_.add(std::move(r)); }
    void remove_ramp(ElementID id) {ramp_collection_.remove_by_id(id); }
    NodeCollection<Ramp>::iterator find_ramp_by_id(ElementID id) {return ramp_collection_.find_by_id(id); }
    NodeCollection<Ramp>::const_iterator find_ramp_by_id(ElementID id) const{return ramp_collection_.find_by_id(id); }
    NodeCollection<Ramp>::const_iterator ramp_cend() const {return ramp_collection_.cend(); }
    NodeCollection<Ramp>::const_iterator ramp_cbegin() const {return ramp_collection_.cbegin(); }

    void add_worker(Worker&& r) {worker_collection_.add(std::move(r)); }
    void remove_worker(ElementID id) {remove_receiver(worker_collection_, id); }
    NodeCollection<Worker>::iterator find_worker_by_id(ElementID id) {return worker_collection_.find_by_id(id); }
    NodeCollection<Worker>::const_iterator find_worker_by_id(ElementID id) const{return worker_collection_.find_by_id(id); }
    NodeCollection<Worker>::const_iterator worker_cend() const {return worker_collection_.cend(); }
    NodeCollection<Worker>::const_iterator worker_cbegin() const {return worker_collection_.cbegin(); }

    void add_storehouse(Storehouse&& r) {storehouse_collection_.add(std::move(r)); }
    void remove_storehouse(ElementID id) {remove_receiver(storehouse_collection_, id); }
    NodeCollection<Storehouse>::iterator find_storehouse_by_id(ElementID id) {return storehouse_collection_.find_by_id(id); }
    NodeCollection<Storehouse>::const_iterator find_storehouse_by_id(ElementID id) const{return storehouse_collection_.find_by_id(id); }
    NodeCollection<Storehouse>::const_iterator storehouse_cend() const {return storehouse_collection_.cend(); }
    NodeCollection<Storehouse>::const_iterator storehouse_cbegin() const {return storehouse_collection_.cbegin(); }

    bool has_reachable_storehouse(const PackageSender* sender, std::map<const PackageSender*, NodeColor>& node_colors);
    bool is_consistent();
    void do_deliveres(Time t);
    void do_package_passing();
    void do_work(Time t);

private:
    template<class Node>
    void remove_receiver(NodeCollection<Node>& collection, ElementID id);

    NodeCollection<Ramp> ramp_collection_;
    NodeCollection<Worker> worker_collection_;
    NodeCollection<Storehouse> storehouse_collection_;
};

bool has_reachable_storehouse(const PackageSender* sender, std::map<const PackageSender*, NodeColor>& node_colors);
Factory load_factory_structure(std::istream& is);
void save_factory_structure( Factory& factory, std::ostream& os);

#endif //NETSIM_FACTORY_HPP
