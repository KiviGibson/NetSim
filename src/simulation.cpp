#include "simulation.hxx"

void simulate(Factory& f, TimeOffset to, std::function<void(Factory& f, TimeOffset t)> rf){
    if(!f.is_consistent()) throw std::logic_error("Non-consistent factory");
    for(Time t; t < to; t++){
        f.do_deliveries(t);
        f.do_package_passing();
        f.do_work(t);
        rf(f, t);
    }
}