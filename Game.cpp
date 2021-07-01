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
    obstacle.setSize(Vector2f(scale,scale));
    obstacle.setTexture(&textures["obstacle"]);
    box.setSize(Vector2f(scale,scale));
    box.setTexture(&textures["box"]);

    for(int i=0;i<Resources_n::spritesCount;i++){
        std::string & name=Resources_n::sprites[i];

        shapes.insert(std::pair<std::string,RectangleShape>(name,RectangleShape(Vector2f(scale,scale))));
        shapes[name].setTexture(&textures[name]);
        animations.insert(std::pair<std::string,thor::FrameAnimation>(name,thor::FrameAnimation()));
        int textureEdge=textures[name].getSize().y;
        int parts=textures[name].getSize().x/textureEdge;
        for(int j=0;j<parts;j++)
            animations[name].addFrame(1.f,IntRect(j*textureEdge,0,textureEdge,textureEdge));
        animators.insert(std::pair<std::string,thor::Animator<sf::RectangleShape, std::string>>(name,thor::Animator<sf::RectangleShape, std::string>()));
        for(int j=1;j<=3;j++)
            animators[name].addAnimation(name+std::to_string(j),animations[name],seconds(timeThresholds[j]));

        animators[name].playAnimation(name+std::to_string(1),true);
    }

    deadzone.setFillColor(Color(250,0,0,80));
    deadzone.setSize(Vector2f(scale,scale));
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
    for(int i=0;i<Resources_n::spritesCount;i++){
        std::string & name=Resources_n::sprites[i];
        animators[name].update(playing?DeltaTime:seconds(0));
        animators[name].animate(shapes[name]);
    }
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
                shapes["powerup1"].setPosition(j*scale+startPoint.x,i*scale+startPoint.y);
                window.draw(shapes["powerup1"]);
            }
            if(!isBox && has_state(mask,Tile_State::upgrade_power)){
                shapes["powerup2"].setPosition(j*scale+startPoint.x,i*scale+startPoint.y);
                window.draw(shapes["powerup2"]);
            }
            if(has_state(mask,Tile_State::mine)){
                shapes["mine"].setPosition(j*scale+startPoint.x,i*scale+startPoint.y);
                window.draw(shapes["mine"]);
            }
            if(has_state(mask,Tile_State::bomb)){
                shapes["bomb"].setPosition(j*scale+startPoint.x,i*scale+startPoint.y);
                window.draw(shapes["bomb"]);
            }
            if(has_state(mask,Tile_State::fire_origin) || has_state(mask,Tile_State::fire_x) || has_state(mask,Tile_State::fire_y)){
                shapes["fire"].setPosition(j*scale+startPoint.x,i*scale+startPoint.y);
                window.draw(shapes["fire"]);
            }





            if(has_state(mask,Tile_State::deadzone)){
                deadzone.setPosition(j*scale+startPoint.x,i*scale+startPoint.y);
                window.draw(deadzone);
            }
        }
    }
}

void Game::changeSpeed(int _speed){
    speed=_speed;
    timeThreshold = timeThresholds[speed];
    std::cout << "test " << speed << std::endl;
    for(int i=0;i<Resources_n::spritesCount;i++){
        std::string & name = Resources_n::sprites[i];
        animators[name].playAnimation(name+std::to_string(speed),true);
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

