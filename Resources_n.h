#include <string>
#ifndef RESOURCES_N_H_INCLUDED
#define RESOURCES_N_H_INCLUDED
namespace Resources{
    int const texturesCount=15;
    std::string textures[texturesCount]={"speed_button_1","speed_button_2","speed_button_3","rewind_button","play_button","pause_button","forward_button","backward_button",
                        "sound_on","sound_off","music_on","music_off","floor","box","obstacle"};

    int const spritesCount=5;
    std::string sprites[spritesCount]={"powerup1","powerup2","fire","bomb","mine"};

    int const soundsCount=6;
    std::string sounds[soundsCount] = {"Pause","Play","Rewind","Click","Failed","Music"};
}


#endif // RESOURCES_N_H_INCLUDED
