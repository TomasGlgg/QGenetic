#ifndef BOTSTRUCT_H
#define BOTSTRUCT_H
#include <vector>

struct _Bot {
    int iterator = 0;
    int direction = 0;
    int energy;
    std::vector<int> genom;
    int old = 0;
    int x, y;
};

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
