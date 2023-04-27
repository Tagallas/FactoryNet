#include "factory.hpp"

#include <istream>
#include <string>
#include <sstream>

template <class Node>
void NodeCollection<Node>::remove_by_id(ElementID id){
    collection_.remove_if([id](const Node& el) {return el.get_id() == id;});
}

void Factory::do_deliveres(Time t){
    std::for_each(ramp_collection_.begin(), ramp_collection_.end(), [&t](Ramp& ramp){ramp.deliver_goods(t);});
}

void Factory::do_package_passing(){
    std::for_each(ramp_collection_.begin(), ramp_collection_.end(), [](Ramp& ramp){ramp.send_package();});

    std::for_each(worker_collection_.begin(), worker_collection_.end(), [](Worker& worker){worker.send_package();});
}

void Factory::do_work(Time t){
    std::for_each(worker_collection_.begin(), worker_collection_.end(), [&t](Worker& worker){worker.do_work(t);});
}

template <class Node>
void Factory::remove_receiver(NodeCollection<Node>& collection, ElementID id){

    auto iter = collection.find_by_id(id);
    //(*iter).get_preferences().
    auto receiver_ptr = dynamic_cast<IPackageReceiver*>(iter);

    for(auto & ramp : ramp_collection_)
    {
        auto & _preferences = ramp.receiver_preferences_.get_preferences();
        for(auto _preference : _preferences)
        {
            if(_preference.first == receiver_ptr)
            {
                ramp.receiver_preferences_.remove_receiver(receiver_ptr);
                break;
            }
        }
    }

    for(auto & worker : worker_collection_)
    {
        auto & _preferences = worker.receiver_preferences_.get_preferences();
        for(auto _preference  : _preferences)
        {
            if(_preference.first == receiver_ptr)
            {
                worker.receiver_preferences_.remove_receiver(receiver_ptr);
                break;
            }
        }
    }
}


bool Factory::has_reachable_storehouse(const PackageSender* sender, std::map<const PackageSender*, NodeColor>& node_colors){
    if (node_colors[sender] == NodeColor::VERIFIED)
        return true;

    node_colors[sender] = NodeColor::VISITED;
    auto preferences = sender->receiver_preferences_.get_preferences();
    if (preferences.empty())
        throw std::logic_error("Nadawca nie ma zdefiniowanych odbiorców");

    bool other_receiver = false;
    for (auto& el : preferences){
        auto& receiver = el.first;

        if (receiver->get_receiver_type() == ReceiverType::STOREHOUSE)
            other_receiver = true;
        else{
            IPackageReceiver* receiver_ptr = receiver;
            auto worker_ptr = dynamic_cast<Worker*>(receiver_ptr);
            auto sendrecv_ptr = dynamic_cast<PackageSender*>(worker_ptr);

            if(sendrecv_ptr == sender)
                continue;
            other_receiver=true;

            if (node_colors[sendrecv_ptr] == NodeColor::UNVISITED)
                has_reachable_storehouse(sendrecv_ptr, node_colors);
        }
    }
    node_colors[sender] = NodeColor::VERIFIED;

    if (other_receiver)
        return true;
    else{
        throw std::logic_error("Nadawca nie ma żadnego odbiorcy poza sobą");
    }

}

bool Factory::is_consistent() {

    std::map<const PackageSender*, NodeColor> color;

    for(auto ptr = worker_collection_.begin(); ptr!= worker_collection_.end();ptr++){
        Worker* worker = &(*ptr);
        auto sender = dynamic_cast<PackageSender*>(worker);
        color[sender] = NodeColor::UNVISITED;
    }

    for(auto ptr = ramp_collection_.begin(); ptr!= ramp_collection_.end();ptr++){
        Ramp* ramp = &(*ptr);
        auto sender = dynamic_cast<PackageSender*>(ramp);
        color[sender] = NodeColor::UNVISITED;
    }
    try{
        for(auto ptr = ramp_collection_.begin();ptr != ramp_collection_.end();ptr++){
            Ramp* ramp = &(*ptr);
            auto sender = dynamic_cast<PackageSender*>(ramp);
            has_reachable_storehouse(sender,color);
        }
    }
    catch (const std::logic_error&) {
        return false;
    }

    return true;
}

bool has_reachable_storehouse(const PackageSender* sender, std::map<const PackageSender*, NodeColor>& node_colors){
    if(node_colors[sender] == NodeColor::VERIFIED){
        return true;
    }
    node_colors[sender] = NodeColor::VISITED;
    if(sender -> receiver_preferences_.get_preferences().empty()){
        throw std::logic_error("Sender hasn't got any receivers");
    }
    bool sender_has_one_receiver_other_than_himself = false;
    for(auto receiver : sender -> receiver_preferences_.get_preferences()){
        if(receiver.first->get_receiver_type() == ReceiverType::STOREHOUSE){
            sender_has_one_receiver_other_than_himself = true;
        }
        else if(receiver.first->get_receiver_type() == ReceiverType::WORKER){
            IPackageReceiver* receiver_ptr = receiver.first;
            auto worker_ptr = dynamic_cast<class Worker*>(receiver_ptr);
            auto send_rec_ptr = dynamic_cast<PackageSender*>(worker_ptr);
            if(send_rec_ptr == sender){
                continue;
            }
            sender_has_one_receiver_other_than_himself = true;
            if(node_colors[send_rec_ptr] == NodeColor::UNVISITED){
                has_reachable_storehouse(send_rec_ptr,node_colors);
            }
        }
    }
    node_colors[sender] = NodeColor::VERIFIED;
    if(sender_has_one_receiver_other_than_himself){
        return true;
    }else{
        throw std::logic_error("Error");
    }
}

enum class ElementType{
    RAMP,WORKER,STOREHOUSE,LINK
};
struct ParsedLineData{
    ElementType element_type;
    std::map<std::string, std::string> parameters;
};

std::vector<std::string> split(const std::string &text, char sep) {
    std::vector<std::string> tokens;
    std::size_t start = 0, end = 0;
    while ((end = text.find(sep, start)) != std::string::npos) {
        tokens.push_back(text.substr(start, end - start));
        start = end + 1;
    }
    tokens.push_back(text.substr(start));
    return tokens;
}

ParsedLineData parse_line(std::string line) {
    std::vector<std::string> tokens;
    std::string token;

    std::istringstream token_stream(line);
    char delimiter = ' ';

    while (std::getline(token_stream, token, delimiter)) {
        tokens.push_back(token);
    }
    std::map<std::string, std::string> tag_map;
    std::vector<std::string> key_val;
    for(std::size_t i =1; i< std::size(tokens) ; i++){
        key_val = split(tokens[i],'=');
        tag_map.insert(std::pair<std::string,std::string>(key_val[0],key_val[1]));
    }
    ParsedLineData p;
    p.parameters = std::move(tag_map);
    if(tokens[0] == "LOADING_RAMP"){
        p.element_type = ElementType::RAMP;
    }else if(tokens[0] == "WORKER"){
        p.element_type = ElementType::WORKER;
    }else if(tokens[0] == "STOREHOUSE"){
        p.element_type = ElementType::STOREHOUSE;
    }else if(tokens[0] == "LINK"){
        p.element_type = ElementType::LINK;
    }

    return p;
}

Factory load_factory_structure(std::istream& is){
    Factory factory;
    std::string line;
    ParsedLineData pld;
    std::list<Ramp>::iterator ramp;
    std::list<Worker>::iterator worker_in;
    std::list<Storehouse>::iterator store;
    std::list<Worker>::iterator worker_out;
    std::vector<std::string> key_val;
    while (std::getline(is, line)) {
        if(*line.data() != ';' and line != ""){
            pld = parse_line(line);
            switch(pld.element_type) {
                case ElementType::RAMP:{
                    factory.add_ramp(Ramp(std::stoi(pld.parameters["id"]), std::stoi(pld.parameters["delivery-interval"])));
                    break;
                }
                case ElementType::WORKER:{
                    if(pld.parameters["queue-type"] == "LIFO"){
                        factory.add_worker(Worker(std::stoi(pld.parameters["id"]), std::stoi(pld.parameters["processing-time"]), std::make_unique<PackageQueue>(PackageQueueType::LIFO)));
                    }
                    else{
                        factory.add_worker(Worker(std::stoi(pld.parameters["id"]), std::stoi(pld.parameters["processing-time"]), std::make_unique<PackageQueue>(PackageQueueType::FIFO)));
                    }
                    break;
                }
                case ElementType::STOREHOUSE:{
                    factory.add_storehouse(Storehouse(std::stoi(pld.parameters["id"])));
                    break;
                }
                case ElementType::LINK:{

                    key_val = split(pld.parameters["src"], '-');
                    if (key_val[0] == "ramp"){
                        ramp = factory.find_ramp_by_id(std::stoi(key_val[1]));

                        key_val = split(pld.parameters["dest"], '-');
                        if(key_val[0] == "store"){
                            store = factory.find_storehouse_by_id(std::stoi(key_val[1]));
                            ramp->receiver_preferences_.add_receiver(&*store);
                        }
                        else if(key_val[0] == "worker"){
                            worker_out = factory.find_worker_by_id(std::stoi(key_val[1]));
                            ramp->receiver_preferences_.add_receiver(&*worker_out);
                        }
                    }
                    else if(key_val[0] == "worker"){
                        worker_in = factory.find_worker_by_id(std::stoi(key_val[1]));
                        key_val = split(pld.parameters["dest"], '-');
                        if(key_val[0] == "store"){
                            store = factory.find_storehouse_by_id(std::stoi(key_val[1]));
                            worker_in->receiver_preferences_.add_receiver(&*store);
                        }
                        else if(key_val[0] == "worker"){
                            worker_out = factory.find_worker_by_id(std::stoi(key_val[1]));
                            worker_in->receiver_preferences_.add_receiver(&*worker_out);
                        }
                    }
                    break;
                }
            }
        }
    }

    return factory;

}
void save_factory_structure(Factory& factory, std::ostream& os) {
    os.flush();
    for(auto it = factory.ramp_cbegin(); it != factory.ramp_cend(); it++){
        if(it == factory.ramp_cbegin()){
            os << "LOADING_RAMP id=" << std::to_string(it->get_id()) << " delivery-interval=" << std::to_string(it->get_delivery_interval());

        }
        else{
            os << "\n" << "LOADING_RAMP id=" << std::to_string(it->get_id()) << " delivery-interval=" << std::to_string(it->get_delivery_interval());
        }
    }

    std::string queue_type;
    for(auto it = factory.worker_cbegin(); it != factory.worker_cend(); it++){
        switch (it->get_queue()->get_queue_type()){
            case PackageQueueType::LIFO:{
                queue_type = "LIFO";
                break;
            }
            case PackageQueueType::FIFO:{
                queue_type = "FIFO";
                break;
            }
            default:
                break;
        }
        os << "\n" << "WORKER id=" << std::to_string(it->get_id()) << " processing-time=" << std::to_string(it->get_processing_duration()) << " queue-type=" << queue_type;
    }
    for(auto it = factory.storehouse_cbegin(); it != factory.storehouse_cend(); it++){
        os<<"\n"<<"STOREHOUSE id="<<std::to_string(it->get_id());
    }
    std::string receiver_type;
    for(auto it = factory.ramp_cbegin(); it != factory.ramp_cend(); it++){
        for(auto iterator = it->receiver_preferences_.cbegin(); iterator != it->receiver_preferences_.cend(); iterator++){
            switch (iterator->first->get_receiver_type()){
                case ReceiverType::WORKER:{
                    receiver_type = "worker";
                    break;
                }
                case ReceiverType::STOREHOUSE:{
                    receiver_type = "store";
                    break;
                }
                default:
                    break;
            }
            os << "\n" << "LINK src=ramp-" << std::to_string(it->get_id()) << " dest=" << receiver_type << "-" << std::to_string(iterator->first->get_id());
        }
    }
    for(auto it = factory.worker_cbegin(); it != factory.worker_cend(); it++){
        for(auto iterator = it->receiver_preferences_.cbegin(); iterator != it->receiver_preferences_.cend(); iterator++){
            switch (iterator->first->get_receiver_type()) {
                case ReceiverType::WORKER:{
                    receiver_type = "worker";
                    break;
                }
                case ReceiverType::STOREHOUSE:{
                    receiver_type = "store";
                    break;
                }
                default:
                    break;
            }
            os << "\n" << "LINK src=worker-" << std::to_string(it->get_id()) << " dest=" << receiver_type << "-" << std::to_string(iterator->first->get_id());
        }
    }
}
