#ifndef TILE_STATE_H_INCLUDED
#define TILE_STATE_H_INCLUDED

enum Tile_State {
  deadzone,
  fire,
  box,
  wall,
  bomb,
  upgrade_range,
  upgrade_health,
  upgrade_trap,
  player,
  trap,
};

bool has_state(int _state_mask, Tile_State _state_to_check);
int add_state(int _state_mask, Tile_State _state_to_add);
int remove_state(int _state_mask, Tile_State _state_to_remove);

#endif // TILE_STATE_H_INCLUDED
