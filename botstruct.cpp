#include "botstruct.h"


Bot::Bot(uint genome_len, int _x, int _y) {
    for (uint i = 0; i<genome_len; i++) {
        genome.push_back(0);
    }
    x = _x;
    y = _y;
    hashxy(this);
}

Bot::~Bot() {
    genome.clear();
}

int Bot::getX() { return x; }
int Bot::getY() { return y; }

void Bot::move(int *xy) {
    assert(type != KILLED);
    x = xy[0];
    y = xy[1];
    hashxy(this);
}

ulong hashxy(int x, int y) {
    ulong result = x;
    result = result << 16 | y;
    return result;
}

ulong hashxy(Bot *bot) {
    ulong result = bot->getX();
    result = result << 16 | bot->getY();
    bot->hash = result;
    return result;
}

ulong hashxy(int *xy) {
    ulong result = xy[0];
    result = result << 16 | xy[1];
    return result;
}
