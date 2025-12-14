#ifndef IPACKAGE_RECEIVER_HPP
#define IPACKAGE_RECEIVER_HPP

#include "package.hxx"

class IPackageReceiver {
public:
    virtual ~IPackageReceiver() = default;
    virtual void receive_package(Package&& p) = 0;
    virtual ElementID get_id() const = 0;
};

#endif // IPACKAGE_RECEIVER_HPP
