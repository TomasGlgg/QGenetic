#include "botstruct.h"

Bot::Bot(unsigned int genome_len) {
    genome.resize(genome_len);
    iterator = 0;
    direction = 0;
    old = 0;
    x = 0;
    y = 0;
    energy = 10;
}

Bot::~Bot() {
    genome.clear();
    genome.shrink_to_fit();
}
