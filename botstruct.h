#ifndef BOTSTRUCT_H
#define BOTSTRUCT_H
#include <vector>


class Bot {
public:
    Bot();
    ~Bot();

    int x, y, direction;
    int energy;
    std::vector<int> genom;
    int iterator;
    int old;
};

#endif // BOTSTRUCT_H
