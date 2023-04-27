#include "package.hpp"

Package::Package() {
    if (freed_IDs.empty()) {
        ID_ = *(assigned_IDs.end())+1;
        //BLANK_ID_=assigned_IDs.size()+1;
        assigned_IDs.insert(ID_);
    }
    else if (assigned_IDs.empty()){
        ID_ = 1;
        assigned_IDs.insert(ID_);
    }
    else {
        ID_=*freed_IDs.begin();
        freed_IDs.erase(ID_);
        assigned_IDs.insert(ID_);
    }

}

Package::Package(ElementID id) {
        ID_ = id;
        assigned_IDs.insert(id);
        freed_IDs.erase(id);
}

Package::Package(Package&& package) noexcept{///tego nie ma
    ID_ = package.get_id();
    BLANK_ID_ = ID_;
    assigned_IDs.insert(ID_);
    freed_IDs.erase(ID_);
}


Package::~Package() {///bez blank id
    if(ID_!=BLANK_ID_){
        assigned_IDs.erase(ID_);
        freed_IDs.insert(ID_);
    }
    else
        BLANK_ID_=-1;
}
