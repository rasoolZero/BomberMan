#include "Game.h"
#include "Control.h"
#include "Tile_State.h"
#include <iostream>
using namespace sf;
using json = nlohmann::json;

Game::Game(RenderWindow & _window,json & _json,thor::ResourceHolder<Texture,std::string> & _textures , unsigned offset):
                window(_window),json_(_json),textures(_textures)
{
    columns = _json["initial_game_data"]["map_width"];
    rows = _json["initial_game_data"]["map_height"];
    totalTurns = _json["initial_game_data"]["last_turn"];
    initialHealth = _json["initial_game_data"]["initial_health"];
    turn = 0;


    float widthRatio=(_window.getSize().x-offset)/(float)columns;
    float heightRatio=_window.getSize().y/(float)rows;
    scale = widthRatio<heightRatio ? widthRatio : heightRatio;
    startPoint.x=(_window.getSize().x-offset-scale*columns)/2.0+offset;
    startPoint.y=(_window.getSize().y-scale*rows)/2.0;
    Vector2u textureSize = textures["floor"].getSize();
    vertices.setPrimitiveType(Quads);
    vertices.resize(columns * rows * 4);
    for(int i=0;i<rows;i++){
        for(int j=0;j<columns;j++){
            Vertex* quad = &vertices[(i + j * columns) * 4];
            quad[0].position = Vector2f(i*scale+startPoint.x,j*scale+startPoint.y);
            quad[1].position = Vector2f((i+1)*scale+startPoint.x,j*scale+startPoint.y);
            quad[2].position = Vector2f((i+1)*scale+startPoint.x,(j+1)*scale+startPoint.y);
            quad[3].position = Vector2f(i*scale+startPoint.x,(j+1)*scale+startPoint.y);

            quad[0].texCoords = Vector2f(0,0);
            quad[1].texCoords = Vector2f(textureSize.x,0);
            quad[2].texCoords = Vector2f(textureSize.x,textureSize.y);
            quad[3].texCoords = Vector2f(0,textureSize.y);
        }
    }



    obstacle.setTexture(&textures["obstacle"]);
    obstacle.setSize(Vector2f(scale,scale));
    box.setTexture(&textures["box"]);
    box.setSize(Vector2f(scale,scale));
    deadzone.setFillColor(Color(250,0,0,100));
    deadzone.setSize(Vector2f(scale,scale));
    powerup1.setTexture(&textures["powerup1"]);
    powerup2.setTexture(&textures["powerup2"]);
    powerup1.setSize(Vector2f(scale,scale));
    powerup2.setSize(Vector2f(scale,scale));
    fire.setTexture(&textures["fire"]);
    fire.setSize(Vector2f(scale,scale));



    int textureEdge=textures["powerup1"].getSize().y;
    int parts=textures["powerup1"].getSize().x/textureEdge;
    for(int i=0;i<parts;i++)
        powerupAnimation.addFrame(1.f,IntRect(i*textureEdge,0,textureEdge,textureEdge));

    textureEdge=textures["fire"].getSize().y;
    parts=textures["fire"].getSize().x/textureEdge;
    for(int i=0;i<parts;i++)
        fireAnimation.addFrame(1.f,IntRect(i*textureEdge,0,textureEdge,textureEdge));

    powerupAnimator.addAnimation("powerup1",powerupAnimation,seconds(timeThresholds[1]));
    powerupAnimator.addAnimation("powerup2",powerupAnimation,seconds(timeThresholds[2]));
    powerupAnimator.addAnimation("powerup3",powerupAnimation,seconds(timeThresholds[3]));
    powerupAnimator.playAnimation("powerup1",true);

    fireAnimator.addAnimation("fire1",fireAnimation,seconds(timeThresholds[1]));
    fireAnimator.addAnimation("fire2",fireAnimation,seconds(timeThresholds[2]));
    fireAnimator.addAnimation("fire3",fireAnimation,seconds(timeThresholds[3]));
    fireAnimator.playAnimation("fire1",true);
}

void Game::update(){
    timePassed+=DeltaTime.asSeconds();
    if(!playing || turn==totalTurns-1)
        timePassed=0.0;
    if(timePassed>=timeThreshold){
        timePassed=0.0;
        turn++;
        if(turn==totalTurns){
            turn--;
        }
    }
    powerupAnimator.update(playing?DeltaTime:seconds(0));
    powerupAnimator.animate(powerup1);
    powerupAnimator.animate(powerup2);
    fireAnimator.update(playing?DeltaTime:seconds(0));
    fireAnimator.animate(fire);
    draw();
}

void Game::draw(){
    window.draw(vertices,&textures["floor"]);
    for(int i=0;i<rows;i++){
        for(int j=0;j<columns;j++){
            int mask = json_["turns"][turn]["map_data"][i][j];
            bool isBox=false;
            if(has_state(mask,Tile_State::wall)){
                obstacle.setPosition(j*scale+startPoint.x,i*scale+startPoint.y);
                window.draw(obstacle);
            }
            if(has_state(mask,Tile_State::box)){
                box.setPosition(j*scale+startPoint.x,i*scale+startPoint.y);
                window.draw(box);
                isBox=true;
            }
            if(!isBox && has_state(mask,Tile_State::upgrade_health)){
                powerup1.setPosition(j*scale+startPoint.x,i*scale+startPoint.y);
                window.draw(powerup1);
            }
            if(!isBox && has_state(mask,Tile_State::upgrade_power)){
                powerup2.setPosition(j*scale+startPoint.x,i*scale+startPoint.y);
                window.draw(powerup2);
            }
            if(has_state(mask,Tile_State::fire_origin) || has_state(mask,Tile_State::fire_x) || has_state(mask,Tile_State::fire_y)){
                fire.setPosition(j*scale+startPoint.x,i*scale+startPoint.y);
                window.draw(fire);
            }







            if(has_state(mask,Tile_State::deadzone)){
                deadzone.setPosition(j*scale+startPoint.x,i*scale+startPoint.y);
                window.draw(deadzone);
            }
        }
    }
}

void Game::changeSpeed(int _speed){
    this->speed=_speed;
    timeThreshold = timeThresholds[this->speed];
    if(playing){
        powerupAnimator.playAnimation("powerup"+std::to_string(speed),true);
        fireAnimator.playAnimation("fire"+std::to_string(speed),true);
    }
}
void Game::setPlaying(bool _playing){
    playing=_playing;
}

bool Game::setTurn(unsigned _turn){
    if(_turn >= totalTurns)
        return false;
    if(_turn < 0)
        return false;
    turn = _turn;
    timePassed=0;
    return true;
}

