#include "simulation.hxx"
#include <iostream>
void simulate(Factory& f, TimeOffset to, std::function<void(Factory&, TimeOffset)>&& rf){
    if(!f.is_consistent()) throw std::logic_error("Non-consistent factory");
    for(Time t = 0; t < to; t++){
        f.do_deliveries(t);
        f.do_package_passing();
        f.do_work(t);
        rf(f, t);
    }
}