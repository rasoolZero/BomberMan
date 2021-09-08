#ifndef PLAYER_ACTION_H_INCLUDED
#define PLAYER_ACTION_H_INCLUDED

enum Player_Action
{
    go_left,
    go_right,
    go_up,
    go_down,
    stay,
    place_bomb,
    place_trap_left,
    place_trap_right,
    place_trap_up,
    place_trap_down,
    init_action,
    no_action,
    no_action_timeout,
};


#endif // PLAYER_ACTION_H_INCLUDED
