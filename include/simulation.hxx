#ifndef SIM_HXX
#define SIM_HXX
#include "factory.hxx"
#include "types.hxx"

class IntervalReportNotifier{
    TimeOffset to;
public:
    explicit IntervalReportNotifier(TimeOffset to): to(to){};
    bool should_generate(Time t){ return t == 1 || t % to == 1;};
};

class SpecificReportNotifier{
    std::set<Time> turns;
public:
    explicit SpecificReportNotifier(const std::set<Time> turns): turns(turns){};
    bool should_generate(Time t){ return turns.find(t) != turns.cend(); };
};

void simulate(Factory& f, TimeOffset to, std::function<void(Factory&, TimeOffset)>&& rf);

#endif