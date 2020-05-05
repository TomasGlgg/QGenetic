#include "botstruct.h"

Bot::Bot(int genome_len){
    genom = new int[genome_len];
    iterator = 0;
    direction = 0;
    old = 0;
    x = 0;
    y = 0;
}

Bot::~Bot(){
//    delete [] genom;
//    genom = nullptr;
}
