#ifndef BOTSTRUCT_H
#define BOTSTRUCT_H


struct Bot {
    int iterator = 0;
    int direction = 0;
    int energy;
    int *genom;
    int old = 0;
    int *xy;
    bool alive = true;
};

#endif // BOTSTRUCT_H
