#include "gtest/gtest.h"
#include "factory.hxx"
#include "IO.hxx"
#include "simulation.hxx"
#include "raports.hxx"
#include <iostream>
#include <fstream>

int main(int argc, char** argv) {
    if (argc != 2){
        std::cout << "wymagany argument test lub <path> dla symulacji";
        return 1;
    }
    if(argv[1] == "test"){
        ::testing::InitGoogleTest(&argc, argv);
        return RUN_ALL_TESTS();
    }else{
        std::fstream plik(argv[1]);
        Factory factory = load_factory_structure(plik);
        IntervalReportNotifier notif = IntervalReportNotifier(1);
        Time time = 20;
        generate_structure_report(factory, std::cout);
        simulate(factory, time, [&notif](Factory& f, TimeOffset t) {
            generate_simulation_turn_report(f,std::cout, t);
        });
    }
}