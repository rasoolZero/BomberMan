#ifndef TILE_STATE_H_INCLUDED
#define TILE_STATE_H_INCLUDED

enum Tile_State
{
    ground_normal,
    ground_broken,
    deadzone,
    fire_x,
    fire_y,
    fire_origin,
    box,
    box_broken,
    wall,
    bomb,
    bomb_blown,
    mine,
    mine_blown,
    upgrade_range,
    upgrade_health,
    upgrade_mine,
    player,
};

bool has_state(int _state_mask, Tile_State _state_to_check);
int add_state(int _state_mask, Tile_State _state_to_add);
int remove_state(int _state_mask, Tile_State _state_to_remove);


#endif // TILE_STATE_H_INCLUDED
