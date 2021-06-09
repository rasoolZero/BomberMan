#include "Control.h"
#include <string.h>
#include <iostream>

using namespace sf;

Control::Control(RenderWindow & _window,Audio & _audio, int _width, int _height) : window(_window),audio(_audio),width(_width),height(_height)
{
    for(int i=0;i<3;i++){
        speedButtons[i].loadFromFile("assets/buttons/speed_button_"+std::to_string(i+1)+".png");
        speedButtons[i].setSmooth(true);
    }
    rewindButton.loadFromFile("assets/buttons/rewind_button.png");
    playButton.loadFromFile("assets/buttons/play_button.png");
    pauseButton.loadFromFile("assets/buttons/paused_button.png");
    forwardButton.loadFromFile("assets/buttons/forward_button.png");
    backwardButton.loadFromFile("assets/buttons/backward_button.png");
    soundOn.loadFromFile("assets/buttons/sound_on.png");
    soundOff.loadFromFile("assets/buttons/sound_off.png");
    musicOn.loadFromFile("assets/buttons/music_on.png");
    musicOff.loadFromFile("assets/buttons/music_off.png");
    rewindButton.setSmooth(true);
    playButton.setSmooth(true);
    pauseButton.setSmooth(true);
    forwardButton.setSmooth(true);
    backwardButton.setSmooth(true);
    soundOn.setSmooth(true);
    soundOff.setSmooth(true);
    musicOn.setSmooth(true);
    musicOff.setSmooth(true);

    background.setPosition(0,0);
    background.setSize(Vector2f(width,height));
    background.setFillColor(Color(50,50,50));

    float const scale = std::min((width-20)/2.0,(height-300)/double(BUTTONS));
    float const left_offset = (width - scale)/2.0;
    float const side_margin = 30;
    float const side_offset = (height/2.0)-(BUTTONS/2-1)*side_margin-(side_margin/2)-(BUTTONS/2)*scale;
    for(int i=0;i<BUTTONS;i++){
        buttons[i].setSize(Vector2f(scale,scale));
        buttons[i].setPosition(left_offset,side_offset+i*side_margin+i*scale);
    }
    buttons[0].setTexture(&rewindButton);
    buttons[1].setTexture(&backwardButton);
    buttons[2].setTexture(&pauseButton);
    buttons[3].setTexture(&playButton);
    buttons[4].setTexture(&forwardButton);
    buttons[5].setTexture(speedButtons);

    float const _scale=30.0;
    float const offset = (width-2*_scale)/4.0;
    for(int i=0;i<2;i++){
        soundButtons[i].setSize(Vector2f(_scale,_scale));
        soundButtons[i].setPosition((2*i+1)*offset+(i)*_scale,height-_scale-5);
    }
    soundButtons[0].setTexture(&musicOff);
    soundButtons[1].setTexture(&soundOn);

}

void Control::draw(){
    window.draw(background);
    for(int i=0;i<BUTTONS;i++)
        window.draw(buttons[i]);
    for(int i=0;i<2;i++)
        window.draw(soundButtons[i]);
}

void Control::update(int * stateCounter=0){
    Vector2i mousePositioni = Mouse::getPosition();
    Vector2f mousePosition = Vector2f(mousePositioni.x,mousePositioni.y);
    for(int i=0;i<BUTTONS;i++){
        if(buttons[i].getGlobalBounds().contains(mousePosition)){
            if(i==0){ //rewind
                *stateCounter=0;
                audio.play(Audio::Rewind);
            }
            if(i==2){ //stop
                playing=false;
                audio.play(Audio::Pause);
            }
            if(i==3){ //play
                playing=true;
                audio.play(Audio::Play);
            }
            if(i==5){ //change speed
                speed=speed%3+1;
                buttons[5].setTexture(speedButtons+speed-1);
                if(speed==1)
                    frameThreshold=15;
                if(speed==2)
                    frameThreshold=10;
                if(speed==3)
                    frameThreshold=5;
                audio.play(Audio::Click);
            }
            if(i==1) //backward
                if(playing==false){
                    if(*stateCounter>0){
                        (*stateCounter)--;
                        audio.play(Audio::Click);
                    }
                    else
                        audio.play(Audio::Failed);
                }
                else
                    audio.play(Audio::Failed);
            if(i==4) //forward
                if(playing==false){
                    (*stateCounter)++;
                    audio.play(Audio::Click);
                }
                else
                    audio.play(Audio::Failed);
            break;
        }
    }
    for(int i=0;i<2;i++){
        if(soundButtons[i].getGlobalBounds().contains(mousePosition)){
            if(i==0){
                music=!music;
                soundButtons[i].setTexture(music?&musicOn:&musicOff);
                if(music)
                    audio.play(Audio::Music);
                else
                    audio.stop(Audio::Music);
            }
            if(i==1){
                sound=!sound;
                soundButtons[i].setTexture(sound?&soundOn:&soundOff);
                audio.setSound(sound);
                audio.play(Audio::Click);
            }
        }
    }
}
