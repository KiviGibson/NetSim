#ifndef PACKAGE_HXX
#define PACKAGE_HXX

#include "types.hxx"
#include <set>
class Package {
    private:
        ElementID id_;
        static std::set<ElementID> assigned;
        static std::set<ElementID> freed;
    public:
        Package();
        Package(Package&& pck) : id_(pck.id_) {};
        Package(ElementID id): id_{id} {assigned.insert(id_);}
        Package& operator=(Package&& pck) noexcept;
        const ElementID get_id()const{return id_;}
        ~Package();
    };
    
#endif
