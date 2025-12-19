#include "nodes.hxx"


void PackageSender::push_package(Package&& package){
    if (!buffer_.has_value()) {
        buffer_ = std::move(package);
    }
}

void PackageSender::send_package(){
    if (!buffer_.has_value()) {
        return;
    }
    if (receiver_preferences_.get_preferences().empty()) {
        return;
    }

    
    IPackageReceiver* receiver = receiver_preferences_.choose_receiver();

    if (receiver != nullptr) {
        receiver->receive_package(std::move(*buffer_));
        buffer_.reset();
    }
}
