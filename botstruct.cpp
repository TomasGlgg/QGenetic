#include "botstruct.h"

Bot::Bot(uint genome_len) {
    for (uint i = 0; i<genome_len; i++) {
        genome.push_back(0);
    }
    iterator = 0;
    old = 0;
    minerals = 0;
    type = ALIVE;
}

Bot::~Bot() {
    genome.clear();
}
