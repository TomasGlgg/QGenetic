#ifndef COMMANDS_H
#define COMMANDS_H


namespace commands {
    enum Commands {
        mutate_attack = -17,        // mutate attack         = -17
        mutate,                     // mutate genome         = -16
        check_target_minerals,      // check target minerals = -15
        check_my_minerals,          // check my minerals     = -14
        check_my_level,             // check my level        = -13
        check_my_energy,            // check my energy       = -12
        share_command,              // share command         = -11
        reproduction_command,       // reproduction          = -10
        eat_command,                // eat                   = -9
        steal_command,              // steal command         = -8
        check_command,              // check                 = -7
        minerals_command,           // minerals              = -6
        photosynthesis_command,     // photosynthesis        = -5
        convert_minerals_command,   // convert minerals      = -4
        right_command,              // right                 = -3
        left_command,               // left                  = -2
        step_command                // step                  = -1
    };
}


#endif // COMMANDS_H
