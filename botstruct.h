#ifndef BOTSTRUCT_H
#define BOTSTRUCT_H


struct _Bot {
    int iterator = 0;
    int direction = 0;
    int energy;
    int *genom;
    int old = 0;
    int x, y;
};

class Bot {
public:
    Bot(int genome_len);
    ~Bot();

    int x, y, direction;
    int energy;
    int *genom;
    int iterator;
    int old;
};

#endif // BOTSTRUCT_H
