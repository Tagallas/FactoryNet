#ifndef NETSIM_PACKAGE_HPP
#define NETSIM_PACKAGE_HPP

#include <set>

#include "types.hpp"
class Package{
public:
    Package();
    explicit Package(ElementID);
    Package(Package&&)noexcept;///nie ma
    Package(Package&)=delete;///nie ma
    Package& operator=(Package&&)=default;///tego nie ma
    Package& operator=(Package&)=delete;/// tego nie ma
    /// jest bool operator ==
    ~Package();

    static void reset() {assigned_IDs.clear(); freed_IDs.clear();}///tego nie ma
    [[nodiscard]] ElementID get_id() const {return ID_;}
private:
    inline static std::set<ElementID> assigned_IDs;///nie ma inline
    inline static std::set<ElementID> freed_IDs;
    ElementID ID_;
    inline static ElementID BLANK_ID_=-1;
};


#endif //NETSIM_PACKAGE_HPP
