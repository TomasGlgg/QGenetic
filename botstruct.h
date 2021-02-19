#ifndef BOTSTRUCT_H
#define BOTSTRUCT_H
#include <vector>


class Bot {
public:
    Bot(unsigned int genome_len);
    ~Bot();

    int x, y;
    unsigned int direction;
    unsigned int energy;
    std::vector<int> genome;
    unsigned int iterator;
    unsigned int old;
};

#endif // BOTSTRUCT_H
