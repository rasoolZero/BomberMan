#include "Game.h"
#include "Control.h"
#include "Tile_State.h"
#include <iostream>
#include <fstream>
using namespace sf;
using json = nlohmann::json;
float map(float value, float istart, float istop, float ostart, float ostop) {
    return ostart + (ostop - ostart) * ((value - istart) / (istop - istart));
}

Game::Game(RenderWindow & _window,thor::ResourceHolder<Texture,std::string> & _textures,
           thor::ResourceHolder<Font,int> & _fonts , unsigned _offset): window(_window),textures(_textures)
{
    obstacle.setTexture(&textures["obstacle"]);
    box.setTexture(&textures["box"]);
    offset = _offset;


    for(int i=0;i<Resources_n::spritesCount;i++){
        std::string & name=Resources_n::sprites[i];

        shapes.insert(std::pair<std::string,RectangleShape>(name,RectangleShape()));
        shapes[name].setTexture(&textures[name]);
        animations.insert(std::pair<std::string,thor::FrameAnimation>(name,thor::FrameAnimation()));
        int textureEdge=textures[name].getSize().y;
        int frames=textures[name].getSize().x/textureEdge;
        for(int j=0;j<frames;j++)
            animations[name].addFrame(1.f,IntRect(j*textureEdge,0,textureEdge,textureEdge));
        animators.insert(std::pair<std::string,ANIMATOR>(name,ANIMATOR()));
        for(int j=1;j<=3;j++)
            animators[name].addAnimation(name+std::to_string(j),animations[name],seconds(timeThresholds[j]));

        animators[name].playAnimation(name+std::to_string(speed),true);
    }
    player[0].setTexture(&textures["player1"]);
    player[1].setTexture(&textures["player2"]);


    deadzone.setFillColor(Color(250,0,0,80));


    initPlayerAnimation();

    int playerInfoBoxWidth = window.getSize().x - offset;
    int playerInfoBoxY = window.getSize().y-playerInfoBoxHeight;
    playerInfo.setSize(Vector2f(playerInfoBoxWidth,playerInfoBoxHeight));
    playerInfo.setPosition(offset,playerInfoBoxY);
    playerInfo.setFillColor(Color(10,10,10));

    for(int i=0;i<2;i++){
        heart[i].setSize(Vector2f(playerInfoBoxHeight,playerInfoBoxHeight));
        heart[i].setPosition(offset + (playerInfoBoxWidth/2)*i + (playerInfoBoxWidth/2-heart[i].getSize().y)/2 , playerInfoBoxY);
        heart[i].setTexture(&textures["heart"]);
        upgrades[i].setFont(_fonts[0]);
        upgrades[i].setCharacterSize(fontSize);
        upgrades[i].setStyle(Text::Bold);
        upgrades[i].setPosition(offset + (playerInfoBoxWidth/2)*i + (playerInfoBoxWidth/2-heart[i].getSize().x)/2 + heart[i].getSize().x + fontSize,playerInfoBoxY+(playerInfoBoxHeight)/2-upgrades[i].getLocalBounds().height);
        names[i].setFont(_fonts[0]);
        names[i].setCharacterSize(fontSize);
        names[i].setStyle(Text::Bold);
        extraHealth[i].setFont(_fonts[0]);
        extraHealth[i].setCharacterSize(fontSize-3);
        extraHealth[i].setStyle(Text::Bold);
        extraHealth[i].setPosition(heart[i].getPosition().x+heart[i].getSize().x*3/4,window.getSize().y-fontSize);
        FloatRect const bound = names[i].getLocalBounds();
        names[i].setPosition(offset + (playerInfoBoxWidth/2)*i + (((playerInfoBoxWidth/2-heart[i].getSize().x)/2)-bound.width)/2 ,playerInfoBoxY+(playerInfoBoxHeight)/2-bound.height);
    }

    heartTextureSize = textures["heart"].getSize().y;
    upgrades[0].setFillColor(Color(156,40,40));
    upgrades[1].setFillColor(Color(40,40,156));
    names[0].setFillColor(Color(156,40,40));
    names[1].setFillColor(Color(40,40,156));
}
void Game::load(std::string logAddress){

    std::ifstream i(logAddress);
    i >> json_;
    i.close();

    timePassed=0;
    columns = json_["initial_game_data"]["map_width"];
    rows = json_["initial_game_data"]["map_height"];
    totalTurns = json_["initial_game_data"]["last_turn"];
    initialHealth = json_["initial_game_data"]["initial_health"];
    const std::string name1 = "test name 1"; // replace these with actual names from json
    const std::string name2 = "test name 2";
    names[0].setString(name1);
    names[1].setString(name2);

    timeThreshold=timeThresholds[speed];
    turn = 0;

    float widthRatio=(window.getSize().x-offset)/(float)columns;
    float heightRatio=(window.getSize().y-playerInfoBoxHeight)/(float)rows;
    scale = widthRatio<heightRatio ? widthRatio : heightRatio;
    startPoint.x=(window.getSize().x-offset-scale*columns)/2.0+offset;
    startPoint.y=(window.getSize().y-playerInfoBoxHeight-scale*rows)/2.0;
    Vector2u textureSize = textures["floor"].getSize();
    vertices.setPrimitiveType(Quads);
    vertices.resize(columns * rows * 4);
    for(int i=0;i<columns;i++){
        for(int j=0;j<rows;j++){
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
    box.setSize(Vector2f(scale,scale));
    deadzone.setSize(Vector2f(scale,scale));
    player[0].setSize(Vector2f(scale,scale));
    player[1].setSize(Vector2f(scale,scale));
    for(int i=0;i<Resources_n::spritesCount;i++){
        std::string & name=Resources_n::sprites[i];
        shapes[name].setSize(Vector2f(scale,scale));
        animators[name].playAnimation(name+std::to_string(speed),true);
    }

    updatePlayer();
}

void Game::update(Time DeltaTime){
    timePassed+=DeltaTime.asSeconds();
    if(!playing || turn==totalTurns)
        timePassed=0.0;
    if(timePassed>=timeThreshold){
        timePassed=0.0;
        turn++;
        if(turn==totalTurns+1){
            turn--;
        }
    }
    for(int i=0;i<Resources_n::spritesCount;i++){
        std::string & name=Resources_n::sprites[i];
        animators[name].update(playing?DeltaTime:seconds(0));
        animators[name].animate(shapes[name]);
    }
    for(int i=0;i<2;i++){
        std::string name = "player"+std::to_string(i+1);
        animators[name].update(playing?DeltaTime:seconds(0));
        animators[name].animate(player[i]);
    }
    updatePlayer();
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
            if(!isBox && has_state(mask,Tile_State::upgrade_range)){
                shapes["powerup2"].setPosition(j*scale+startPoint.x,i*scale+startPoint.y);
                window.draw(shapes["powerup2"]);
            }
            if(!isBox && has_state(mask,Tile_State::upgrade_mine)){
                shapes["powerup3"].setPosition(j*scale+startPoint.x,i*scale+startPoint.y);
                window.draw(shapes["powerup3"]);
            }
            if(has_state(mask,Tile_State::mine)){
                shapes["mine"].setPosition(j*scale+startPoint.x,i*scale+startPoint.y);
                window.draw(shapes["mine"]);
            }
            if(has_state(mask,Tile_State::bomb)){
                shapes["bomb"].setPosition(j*scale+startPoint.x,i*scale+startPoint.y);
                window.draw(shapes["bomb"]);
            }

            for(int i=0;i<2;i++)
                window.draw(player[i]);

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
    window.draw(playerInfo);
    for(int i=0;i<2;i++){
        window.draw(heart[i]);
        window.draw(upgrades[i]);
        window.draw(names[i]);
        window.draw(extraHealth[i]);
    }
}

void Game::changeSpeed(int _speed){
    speed=_speed;
    timeThreshold = timeThresholds[speed];
    for(int i=0;i<Resources_n::spritesCount;i++){
        std::string & name = Resources_n::sprites[i];
        animators[name].playAnimation(name+std::to_string(speed),true);
    }
    for(int i=0;i<2;i++){
        std::string name = "player"+std::to_string(i+1);
        if(animators[name].isPlayingAnimation()){
            std::string animation = animators[name].getPlayingAnimation();
            animation.pop_back();
            animation+=std::to_string(speed);
            animators[name].playAnimation(animation,true);
        }
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
    updatePlayer();
    return true;
}

Vector2f Game::getPlayerPosition(int _turn,int player){
    int x;
    int y;
    if(_turn == -1){
        x = json_["initial_game_data"][std::string("player_")+std::to_string(player+1)+std::string("_init_x")];
        y = json_["initial_game_data"][std::string("player_")+std::to_string(player+1)+std::string("_init_y")];
    }
    else{
        x = json_["turns"][_turn]["players_data"][player]["x_position"];
        y = json_["turns"][_turn]["players_data"][player]["y_position"];
    }
    return Vector2f(x*scale+startPoint.x,y*scale+startPoint.y);
}

void Game::updatePlayer(){
    for(int i=0;i<2;i++){
        health[i] = json_["turns"][turn]["players_data"][i]["health"];
        std::string bombPower = std::to_string(int(json_["turns"][turn]["players_data"][i]["bomb_power_level"]));
        std::string mineCount = std::to_string(int(json_["turns"][turn]["players_data"][i]["mines_left"]));
        upgrades[i].setString("POWER:"+bombPower+"  MINES:"+mineCount);

        int part;
        if(health[i]>initialHealth){
            part = 0;
            extraHealth[i].setString(std::string("+")+std::to_string(health[i]-initialHealth));
        }
        else{
            part = int(map(health[i],0,initialHealth,3,0));
            extraHealth[i].setString("");
        }
        heart[i].setTextureRect(IntRect(part*heartTextureSize,0,heartTextureSize,heartTextureSize));

        Vector2f currentPosition = Game::getPlayerPosition(turn-1,i);
        Vector2f nextPosition = Game::getPlayerPosition(turn,i);
        Vector2f path = nextPosition-currentPosition;
        Vector2f position = currentPosition + (path*timePassed/timeThreshold);

        std::string nextAnimation,name="player"+std::to_string(i+1);
        if(path.x<0)
            nextAnimation = "walk_left";
        else if(path.x>0)
            nextAnimation = "walk_right";
        else if(path.y<0)
            nextAnimation = "walk_up";
        else if(path.y>0)
            nextAnimation = "walk_down";
        else{
            int actionTaken = json_["turns"][turn]["players_data"][i]["action_taken"];
            Player_Action action = static_cast<Player_Action>(actionTaken);

            if(static_cast<int>(action) < 4){
                if(action==Player_Action::go_down)
                    nextAnimation = "stand_down";
                if(action==Player_Action::go_left)
                    nextAnimation = "stand_left";
                if(action==Player_Action::go_up)
                    nextAnimation = "stand_up";
                if(action==Player_Action::go_right)
                    nextAnimation = "stand_right";
            }
            else{
                if(animators[name].isPlayingAnimation()){
                    std::string currentAnimation = animators[name].getPlayingAnimation();
                    if(currentAnimation.find("left")!=std::string::npos)
                        nextAnimation = "stand_left";
                    if(currentAnimation.find("right")!=std::string::npos)
                        nextAnimation = "stand_right";
                    if(currentAnimation.find("down")!=std::string::npos)
                        nextAnimation = "stand_down";
                    if(currentAnimation.find("up")!=std::string::npos)
                        nextAnimation = "stand_up";
                }
                else{
                    nextAnimation = "stand_right";
                }
            }
        }
        nextAnimation+=std::to_string(speed);
        if(animators[name].isPlayingAnimation()){
            if(animators[name].getPlayingAnimation()!=nextAnimation)
                animators[name].playAnimation(nextAnimation,true);
        }
        else
            animators[name].playAnimation(nextAnimation,true);

        player[i].setPosition(position);
    }

}



void Game::initPlayerAnimation(){
    const std::string playerAnimations[8] =  {"walk_down","walk_left","walk_right","walk_up",
                                        "stand_down","stand_left","stand_right","stand_up"};
    for(int i=0;i<8;i++){
        std::string name = playerAnimations[i];
        animations.insert(std::pair<std::string,thor::FrameAnimation>(name,thor::FrameAnimation()));
    }
    int edge = textures["player1"].getSize().y/4;
    int frames = textures["player1"].getSize().x/edge;
    for(int j=0;j<4;j++){
        for(int k=0;k<frames;k++){
            animations[playerAnimations[j]].addFrame(1.f,IntRect(k*edge,j*edge,edge,edge));
        }
        animations[playerAnimations[j+4]].addFrame(1.f,IntRect((frames/2)*edge,j*edge,edge,edge));
    }
    for(int i=0;i<2;i++)
        animators.insert(std::pair<std::string,ANIMATOR>("player"+std::to_string(i+1),ANIMATOR()));

    for(int i=0;i<8;i++)
        for(int j=1;j<=3;j++){
            animators["player1"].addAnimation(playerAnimations[i]+std::to_string(j),animations[playerAnimations[i]],seconds(timeThresholds[j]));
            animators["player2"].addAnimation(playerAnimations[i]+std::to_string(j),animations[playerAnimations[i]],seconds(timeThresholds[j]));
        }

}
