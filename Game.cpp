#include "Game.h"
#include "Control.h"
#include "Tile_State.h"
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
}

void Game::update(){
    timePassed+=DeltaTime.asSeconds();
    if(!playing || turn==totalTurns-1)
        timePassed=0.0;
    if(timePassed>=timeThreshold){
        timePassed=0.0;
        turn++;
        if(turn==totalTurns)
            turn--;
    }
    draw();
}

void Game::draw(){
    window.draw(vertices,&textures["floor"]);
    for(int i=0;i<rows;i++){
        for(int j=0;j<columns;j++){
            int mask = json_["turns"][turn]["map_data"][i][j];
            if(has_state(mask,Tile_State::wall)){
                obstacle.setPosition(j*scale+startPoint.x,i*scale+startPoint.y);
                window.draw(obstacle);
            }
            if(has_state(mask,Tile_State::box)){
                box.setPosition(j*scale+startPoint.x,i*scale+startPoint.y);
                window.draw(box);
            }
        }
    }
}

void Game::changeSpeed(int speed){
    if(speed==1)
        timeThreshold=0.5;
    if(speed==2)
        timeThreshold=0.25;
    if(speed==3)
        timeThreshold=0.15;
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

