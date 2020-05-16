#include "botstruct.h"

Bot::Bot(){
    iterator = 0;
    direction = 0;
    old = 0;
    x = 0;
    y = 0;
    energy = 10;
}

Bot::~Bot() {
    genom.clear();
    genom.shrink_to_fit();
}
