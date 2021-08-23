#include "Control.h"
#include "Game.h"
#include <string>
#include <iostream>

using namespace sf;



Control::Control(RenderWindow & _window, Manager& _manager,Game & _game, Audio & _audio, int _width,int _height, thor::ResourceHolder<Texture,std::string> & _textures) :
                window(_window),game(_game),audio(_audio),width(_width),height(_height),textures(_textures),manager(_manager)
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
    //Vector2i mousePositioni = Mouse::getPosition(window);
    Vector2f mousePosition = static_cast<Vector2f>(Mouse::getPosition(window));
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
                if(game.setTurn(game.getTurn()-1)){
                    audio.play(Audio::Click);
                }
                else
                    audio.play(Audio::Failed);
            if(i==4) //forward
                if(game.setTurn(game.getTurn()+1))
                    audio.play(Audio::Click);
                else
                    audio.play(Audio::Failed);
        }
    }
    for(int i=0;i<2;i++){
        if(soundButtons[i].getGlobalBounds().contains(mousePosition)){
            toggleSound(i);
        }
    }
}

void Control::manageKey(Event::KeyEvent key, bool released)
{
    if (!released) {
        switch (key.code)
        {
        case Keyboard::Key::Space:
        case Keyboard::Key::Enter:
            playing = !playing;
            game.setPlaying(playing);
            audio.play(playing ? Audio::Sounds::Play : Audio::Sounds::Pause);
            break;

        case Keyboard::Key::D:
        case Keyboard::Key::Right: {
            int step = 1;
            if (key.control) {
                step = 5;
            }
            if (game.setTurn(game.getTurn() + step)) {
                audio.play(Audio::Click);
            }
            else {
                audio.play(Audio::Failed);
            }
            break;
        }
        case Keyboard::Key::A:
        case Keyboard::Key::Left: {
            int step = 1;
            if (key.control) {
                step = 5;
            }
            if (game.setTurn(game.getTurn() - step)) {
                audio.play(Audio::Click);
            }
            else {
                audio.play(Audio::Failed);
            }
            break;
        }
        default:
            if (Keyboard::Key::Num0 <= key.code && key.code <= Keyboard::Key::Num9) {
                if (game.setTurn((key.code - Keyboard::Key::Num0) / 10.0)) {
                    audio.play(Audio::Click);
                }
                else {
                    audio.play(Audio::Failed);
                }
            }
            else if (Keyboard::Key::Numpad0 <= key.code && key.code <= Keyboard::Key::Numpad9) {
                if (game.setTurn((key.code - Keyboard::Key::Numpad0) / 10.0)) {
                    audio.play(Audio::Click);
                }
                else {
                    audio.play(Audio::Failed);
                }
            }
            break;
        }
    }
    else { // on release
        switch (key.code)
        {
        case Keyboard::Key::Escape:
            if (music) {
                music = false;
                audio.stop(Audio::Music);
                buttons[5].setTexture(&textures["speed_button_" + std::to_string(speed)]);
                soundButtons[0].setTexture(&textures["music_off"]);
            }
            playing = false;
            game.setPlaying(false);
            game.changeSpeed(1);
            manager.setState(Manager::State::menu);
            break;

        case Keyboard::Key::W:
        case Keyboard::Key::Up:
        case Keyboard::Key::S:
        case Keyboard::Key::Down:
            if (key.code == Keyboard::Key::Up || key.code == Keyboard::Key::W) {
                speed = speed % 3 + 1;
            }
            else {
                speed = (speed + 1) % 3 + 1;
            }
            buttons[5].setTexture(&textures["speed_button_" + std::to_string(speed)]);
            game.changeSpeed(speed);
            audio.play(Audio::Click);
            break;

        case Keyboard::Key::E:
            toggleSound(1);
            break;

        case Keyboard::Key::Q:
            toggleSound(0);
            break;

        case Keyboard::Key::R:
            game.setTurn(0);
            audio.play(Audio::Rewind);
            break;

        default:
            break;
        }
    }
}

void Control::manageMouse(Event::MouseButtonEvent mouseButton, bool released) {
    if (!released) {
        if (mouseButton.button == Mouse::Button::Left) {
            update();
        }
    }
    else {

    }
}

void Control::toggleSound(bool sound_selected)
{
    if (!sound_selected) {
        music = !music;
        soundButtons[sound_selected].setTexture(music ? &textures["music_on"] : &textures["music_off"]);
        if (music)
            audio.play(Audio::Music);
        else
            audio.stop(Audio::Music);
    }
    else {
        sound = !sound;
        soundButtons[sound_selected].setTexture(sound ? &textures["sound_on"] : &textures["sound_off"]);
        audio.setSound(sound);
        audio.play(Audio::Click);
    }
}
