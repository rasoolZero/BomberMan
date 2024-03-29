#include "Resources_n.h"

namespace Resources_n{
    int const texturesCount=20;
    std::string textures[texturesCount]={"speed_button_1","speed_button_2","speed_button_3","rewind_button","play_button","pause_button","forward_button","backward_button",
                        "sound_on","sound_off","music_on","music_off","floor","box","obstacle","player1","player2","heart", "logo","icon"};

    int const spritesCount=6;
    std::string sprites[spritesCount]={"upgrade_power","upgrade_trap","upgrade_health","fire","bomb","trap"};

    int const shadersCount = 1;
    std::string shaders[shadersCount] = { "blur" };

    int const soundsCount=9;
    std::string sounds[soundsCount] = {"Pause","Play","Rewind","Click","Failed","Capture","Intro","Boom","Winner"};
    std::string music = "Music.flac";
}
