#include <math.h>
#include "tile_state.h"

using namespace std;


bool has_state(int _state_mask, Tile_State _state_to_check){
    return _state_mask & int(pow(2.0, _state_to_check));
}
int add_state(int _state_mask, Tile_State _state_to_add){
    return _state_mask | int(pow(2.0, _state_to_add));
}
int remove_state(int _state_mask, Tile_State _state_to_remove){
    return _state_mask & !(int(pow(2.0, _state_to_remove)));
}
