#ifndef ENUMS_H
#define ENUMS_H


namespace commands {
    enum Commands {
        mutate_attack = -18,        // mutate attack         = -18
        mutate,                     // mutate genome         = -17
        check_target_minerals,      // check target minerals = -16
        check_my_minerals,          // check my minerals     = -15
        check_my_level,             // check my level        = -14
        check_my_energy,            // check my energy       = -13
        align,                      // align horizontally    = -12
        share,                      // share                 = -11
        reproduction,               // reproduction          = -10
        eat,                        // eat                   = -9
        steal,                      // steal                 = -8
        check,                      // check                 = -7
        minerals,                   // minerals              = -6
        photosynthesis,             // photosynthesis        = -5
        convert_minerals,           // convert minerals      = -4
        right,                      // right                 = -3
        left,                       // left                  = -2
        step                        // step                  = -1
    };
}

enum BotType {
    ALIVE,
    ORGANIC,
    DEAD
};

enum Direction {
    UP,
    UP_RIGHT,
    RIGHT,
    RIGHT_DOWN,
    DOWN,
    DOWN_LEFT,
    LEFT,
    LEFT_UP
};


#endif // ENUMS_H
