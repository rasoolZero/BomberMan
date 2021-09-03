#include "Control.h"
#include "Game.h"
#include <string>
#include <iostream>
#include <cmath>

using namespace sf;



Control::Control(RenderWindow & _window, Manager& _manager,Game & _game, Audio & _audio, int _width,int _height, thor::ResourceHolder<Texture,std::string> & _textures) :
                window(_window),game(_game),audio(_audio),width(_width),height(_height),textures(_textures),manager(_manager)
    ,turn_seek{ {_window.getSize().x - 40.f, 9.f} }
{


    background.setPosition(0, window.getSize().y - height);
    background.setSize(Vector2f(width,height));
    background.setFillColor(Color(20,20,20));

    //float const scale = std::min((width-20)/2.0,(height-300)/double(BUTTONS));
    float const scale = std::min((width - 300) / 2.0, 40.0);
    float const side_offset = window.getSize().y - scale * 1.125;
    float const side_margin = 30;
    float const left_offset = (width/2.0)-(BUTTONS/2)*side_margin-(side_margin/2)-(BUTTONS/2)*scale;
    for(int i=0;i<BUTTONS;i++){
        buttons[i].setSize(Vector2f(scale,scale));
        buttons[i].setPosition(left_offset + i * side_margin + i * scale, side_offset);
    }
    buttons[0].setTexture(&textures["rewind_button"]);
    buttons[1].setTexture(&textures["backward_button"]);
    //buttons[2].setTexture(&textures["pause_button"]);
    buttons[2].setTexture(&textures["play_button"]);
    buttons[3].setTexture(&textures["forward_button"]);
    buttons[4].setTexture(&textures["speed_button_1"]);
    for(int i=0;i<2;i++){
        soundButtons[i].setSize(Vector2f(scale/2.0,scale/2.0));
        soundButtons[i].setPosition(side_margin/2.0*(i+1) + scale/2.0*i , side_offset + scale/4.0);
    }
    soundButtons[0].setTexture(&textures["music_off"]);
    soundButtons[1].setTexture(&textures["sound_on"]);

    turn_seek.setPosition(20, window.getSize().y - (height + 58) );
    bar_transformable.setOrigin(turn_seek.getPosition());

    active_area.top     = window.getSize().y * 0.875;
    active_area.height  = window.getSize().y * 0.125;
    active_area.width   = window.getSize().x;
    active_area.left    = 0;
}

void Control::load(int turnCount)
{
    turn_seek.setTurnCount(turnCount);
    for (int i = 0; i < BUTTONS; i++) {
        button_transformable[i].setPosition((active_position - inactive_position));
        buttons[i].setFillColor(Color(255, 255, 255, 0));
    }
    bar_transformable.setScale(1, 0.5);
    bar_transformable.setPosition(turn_seek.getPosition() + (active_position - inactive_position) * 1.125f);
}

void Control::draw(RenderTarget* target){
    if (target == nullptr) {
        target = &this->window;
    }
    target->draw(background);
    for(int i=0;i<BUTTONS;i++)
        target->draw(buttons[i], button_transformable[i].getTransform());
    for(int i=0;i<2;i++)
        target->draw(soundButtons[i], button_transformable[i + 1].getTransform());
    target->draw(turn_seek, bar_transformable.getTransform());
}

void Control::update(Time DeltaTime){
    if (playing && !adjusting) {
        turn_seek.advance(static_cast<double>(DeltaTime.asSeconds()) / game.getTurnTime());
    }
    bool changed = false;
    if (active_area.contains(Mouse::getPosition()) && progress < 1.0) {
        progress += DeltaTime / activation_time;
        changed = true;
    }
    else if (!active_area.contains(Mouse::getPosition()) && progress > 0.0) {
        progress -= DeltaTime / activation_time;
        changed = true;
    }
    if (changed) {
        if (progress > 1.0) {
            progress = 1.0;
        }
        else if (progress < 0.0) {
            progress = 0.0;
        }
        for (int i = 0; i < BUTTONS; i++) {
            button_transformable[i].setPosition((active_position - inactive_position) * (1.f - powf(progress, 1 + abs(2 - i) / 2.f)));
            Color t = buttons[i].getFillColor();
            t.a = 255 * pow(progress, 2);
            buttons[i].setFillColor(t);
        }
        bar_transformable.setScale(1, 0.5 + 0.5 * progress);
        bar_transformable.setPosition(turn_seek.getPosition() + (active_position - inactive_position) * (1.f - progress) * 1.125f);
    }
}

void Control::manageKey(Event::KeyEvent key, bool released)
{
    if (!released) {
        switch (key.code)
        {
        case Keyboard::Key::D:
        case Keyboard::Key::Right: {
            int step = 1;
            if (key.control) {
                step = 5;
            }
            setTurn(game.getTurn() + step);
            break;
        }
        case Keyboard::Key::A:
        case Keyboard::Key::Left: {
            int step = 1;
            if (key.control) {
                step = 5;
            }
            setTurn(game.getTurn() - step);
            break;
        }
        case Keyboard::Key::F:
            game.showFullName(0, 1);
            game.showFullName(1, 1);
            break;

        default:
            break;
        }
    }
    else { // on release
        switch (key.code)
        {
        case Keyboard::Key::Space:
        case Keyboard::Key::Enter:
            playing = !playing;
            setPlaying();
            audio.play(playing ? Audio::Sounds::Play : Audio::Sounds::Pause);
            break;

        case Keyboard::Key::Escape:
            if (music) {
                music = false;
                audio.stop(Audio::Music);
                buttons[4].setTexture(&textures["speed_button_" + std::to_string(speed)]);
                soundButtons[0].setTexture(&textures["music_off"]);
            }
            playing = false;
            setPlaying();
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
            buttons[4].setTexture(&textures["speed_button_" + std::to_string(speed)]);
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
            setTurn(0);
            audio.play(Audio::Rewind);
            break;

        case Keyboard::Key::F:
            game.showFullName(0, 0);
            game.showFullName(1, 0);
            game.updateMouse(static_cast<Vector2f>(Mouse::getPosition()));
            break;

        default:
            if (Keyboard::Key::Num0 <= key.code && key.code <= Keyboard::Key::Num9) {
                setTurn((key.code - Keyboard::Key::Num0) / 10.0);
            }
            else if (Keyboard::Key::Numpad0 <= key.code && key.code <= Keyboard::Key::Numpad9) {
                setTurn((key.code - Keyboard::Key::Numpad0) / 10.0);
            }
            break;
        }
    }
}

void Control::manageMouse(Event::MouseButtonEvent mouseButton, bool released) {
    Vector2f pos(mouseButton.x, mouseButton.y);
    if (!released) {
        if (mouseButton.button == Mouse::Button::Left) {
            //update();
            for (int i = 0; i < BUTTONS; i++) {
                if (buttons[i].getGlobalBounds().contains(pos)) {
                    if (i == 0) { //rewind
                        setTurn(0);
                        audio.play(Audio::Rewind);
                    }
                    else if (i == 2) { //play / pause
                        playing = !playing;
                        setPlaying();
                        audio.play(playing ? Audio::Sounds::Play : Audio::Sounds::Pause);
                    }
                    else if (i == 4) { //change speed
                        speed = speed % 3 + 1;
                        buttons[4].setTexture(&textures["speed_button_" + std::to_string(speed)]);
                        game.changeSpeed(speed);
                        audio.play(Audio::Click);
                    }
                    else if (i == 1) { //backward
                        setTurn(game.getTurn() - 1);
                    }
                    else if (i == 3) { //forward
                        setTurn(game.getTurn() + 1);
                    }
                    return;
                }
            }
            for (int i = 0; i < 2; i++) {
                if (soundButtons[i].getGlobalBounds().contains(pos)) {
                    toggleSound(i);
                    return;
                }
            }
            adjusting = turn_seek.contains(pos);
            if (adjusting) {
                turn_seek.setProgress(pos.x);
                setPlaying();
                setTurn(turn_seek.getValue());
            }
        }
    }
    else { // on release
        if (mouseButton.button == Mouse::Button::Left) {
            if (adjusting) {
                adjusting = false;
                setPlaying();
                audio.play(Audio::Sounds::Click);
            }
        }
    }
}

void Control::updateMouse(Event::MouseMoveEvent mouseMove)
{
    if (adjusting) {
        turn_seek.setProgress(mouseMove.x);
        //assert(turn_seek.getValue() == int(turn_seek.getProgress() * game.getLastTurn()));
        setTurn(turn_seek.getValue());
    }
    else {
        game.updateMouse(Vector2f(mouseMove.x, mouseMove.y), Keyboard::isKeyPressed(Keyboard::Key::F));
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

void Control::setTurn(int turn)
{
    if (game.setTurn(turn)) {
        turn_seek.setValue(turn);
        if (!adjusting) {
            audio.play(Audio::Click);
        }
    }
    else {
        audio.play(Audio::Failed);
    }
}

void Control::setTurn(double progress)
{
    if (game.setTurn(progress)) {
        turn_seek.setValue(game.getTurn());
        if (!adjusting) {
            audio.play(Audio::Click);
        }
    }
    else {
        audio.play(Audio::Failed);
    }
}

void Control::setPlaying()
{
    buttons[2].setTexture(playing ? &textures["pause_button"] : &textures["play_button"]);
    game.setPlaying(playing && !adjusting);
}
