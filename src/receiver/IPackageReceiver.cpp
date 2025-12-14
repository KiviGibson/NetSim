
#include "include/package.hxx"


class IPackageReceiver{
    virtual void receive_package(Package&& p) = 0;
    virtual ElementID get_id() const = 0;
};