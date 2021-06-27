#include "Control.h"
#include "Game.h"
#include <string>
#include <iostream>

using namespace sf;



Control::Control(RenderWindow & _window,Game & _game,Audio & _audio, int _width, int _height,thor::ResourceHolder<Texture,std::string> & _textures) :
                window(_window),game(_game),audio(_audio),width(_width),height(_height),textures(_textures)
{


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
    buttons[0].setTexture(&textures["rewind_button"]);
    buttons[1].setTexture(&textures["backward_button"]);
    buttons[2].setTexture(&textures["pause_button"]);
    buttons[3].setTexture(&textures["play_button"]);
    buttons[4].setTexture(&textures["forward_button"]);
    buttons[5].setTexture(&textures["speed_button_1"]);

    float const _scale=30.0;
    float const offset = (width-2*_scale)/4.0;
    for(int i=0;i<2;i++){
        soundButtons[i].setSize(Vector2f(_scale,_scale));
        soundButtons[i].setPosition((2*i+1)*offset+(i)*_scale,height-_scale-5);
    }
    soundButtons[0].setTexture(&textures["music_off"]);
    soundButtons[1].setTexture(&textures["sound_on"]);

}

void Control::draw(){
    window.draw(background);
    for(int i=0;i<BUTTONS;i++)
        window.draw(buttons[i]);
    for(int i=0;i<2;i++)
        window.draw(soundButtons[i]);
}

void Control::update(){
    Vector2i mousePositioni = Mouse::getPosition();
    Vector2f mousePosition = Vector2f(mousePositioni.x,mousePositioni.y);
    for(int i=0;i<BUTTONS;i++){
        if(buttons[i].getGlobalBounds().contains(mousePosition)){
            if(i==0){ //rewind
                game.setTurn(0);
                audio.play(Audio::Rewind);
            }
            if(i==2){ //stop
                playing=false;
                game.setPlaying(playing);
                audio.play(Audio::Pause);
            }
            if(i==3){ //play
                playing=true;
                game.setPlaying(playing);
                audio.play(Audio::Play);
            }
            if(i==5){ //change speed
                speed=speed%3+1;
                buttons[5].setTexture(&textures["speed_button_"+std::to_string(speed)]);
                game.changeSpeed(speed);
                audio.play(Audio::Click);
            }
            if(i==1) //backward
                if(playing==false){
                    if(game.setTurn(game.getTurn()-1)){
                        audio.play(Audio::Click);
                    }
                    else
                        audio.play(Audio::Failed);
                }
                else
                    audio.play(Audio::Failed);
            if(i==4) //forward
                if(playing==false){
                    if(game.setTurn(game.getTurn()+1)){
                        audio.play(Audio::Click);
                    }
                    else
                        audio.play(Audio::Failed);
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
                soundButtons[i].setTexture(music?&textures["music_on"]:&textures["music_off"]);
                if(music)
                    audio.play(Audio::Music);
                else
                    audio.stop(Audio::Music);
            }
            if(i==1){
                sound=!sound;
                soundButtons[i].setTexture(sound?&textures["sound_on"]:&textures["sound_off"]);
                audio.setSound(sound);
                audio.play(Audio::Click);
            }
        }
    }
}
