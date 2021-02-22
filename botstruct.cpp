#include "botstruct.h"

Bot::Bot(uint genome_len) {
    for (uint i = 0; i<genome_len; i++) {
        genome.push_back(0);
    }
    iterator = 0;
    direction = 0;
    old = 0;
    x = 0;
    y = 0;
    energy = 10;
}

Bot::~Bot() {
    genome.clear();
}
