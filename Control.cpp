#include "Control.h"
#include <string.h>
#include <iostream>

using namespace sf;

Control::Control(RenderWindow & _window, int _width, int _height) : window(_window),width(_width),height(_height)
{
    for(int i=0;i<3;i++){
        speedButtons[i].loadFromFile("assets/buttons/speed_button_"+std::to_string(i+1)+".png");
        speedButtons[i].setSmooth(true);
    }
    rewindButton.loadFromFile("assets/buttons/rewind_button.png");
    playButton.loadFromFile("assets/buttons/play_button.png");
    pauseButton.loadFromFile("assets/buttons/paused_button.png");
    rewindButton.setSmooth(true);
    playButton.setSmooth(true);
    pauseButton.setSmooth(true);

    background.setPosition(0,0);
    background.setSize(Vector2f(width,height));
    background.setFillColor(Color(50,50,50));

    float scale = std::min((height-20)/2.0,(width-300)/4.0);
    float top_offset = (height - scale)/2.0;
    float const side_margin = 30;
    float side_offset = (width/2.0)-side_margin-(side_margin/2);
    for(int i=0;i<4;i++){
        buttons[i].setSize(Vector2f(scale,scale));
        buttons[i].setPosition(side_offset+i*side_margin,top_offset);
    }
    buttons[0].setTexture(&rewindButton);
    buttons[1].setTexture(&pauseButton);
    buttons[2].setTexture(&playButton);
    buttons[3].setTexture(speedButtons);

}
