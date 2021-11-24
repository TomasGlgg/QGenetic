#ifndef ENUMS_H
#define ENUMS_H


namespace commands {
    enum Commands {
        mutate_attack = -18,        // mutate attack         = -19
        mutate,                     // mutate genome         = -18
        check_target_minerals,      // check target minerals = -17
        check_my_minerals,          // check my minerals     = -16
        check_my_level,             // check my level        = -15
        check_my_energy,            // check my energy       = -14
        align,                      // align horizontally    = -13
        share,                      // share                 = -12
        reproduction,               // reproduction          = -11
        eat,                        // eat                   = -10
        steal,                      // steal                 = -9
        check,                      // check                 = -8
        minerals,                   // minerals              = -7
        photosynthesis,             // photosynthesis        = -6
        convert_minerals,           // convert minerals      = -5
        change_dir,                 // change directory (absolutely) = -4
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
