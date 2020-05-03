#ifndef BOTSTRUCT_H
#define BOTSTRUCT_H


struct Bot {
    int iterator = 0;
    int direction = 1;
    int energy;
    int *genom;
    int old = 0;
    int xy[2];
};

#endif // BOTSTRUCT_H
