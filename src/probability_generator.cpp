#include "helpers.hxx"

#include <cstdlib>
#include <random>

std::random_device rd;
std::mt19937 rng(rd());

double default_probability_generator() {
    // Generuj liczby pseudolosowe z przedziału [0, 1); 10 bitów losowości.
    return std::generate_canonical<double, 10>(rng);
}

ProbabilityGenerator probability_generator = default_probability_generator;
