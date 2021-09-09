#include "Game.h"
#include "Control.h"
#include "Tile_State.h"
#include <iostream>
#include <fstream>
#include "macros.h"
#include "UnicodeConverter.h"
#include <stdexcept>
#include <filesystem>
using namespace sf;
using json = nlohmann::json;
float map(float value, float istart, float istop, float ostart, float ostop) {
    return ostart + (ostop - ostart) * ((value - istart) / (istop - istart));
}

Game::Game(RenderWindow & _window, Audio & _audio, thor::ResourceHolder<Texture,std::string> & _textures,
           thor::ResourceHolder<Font,int> & _fonts , unsigned _controlHeight): window(_window),audio(_audio),textures(_textures)
{
    obstacle.setTexture(&textures["obstacle"]);
    box.setTexture(&textures["box"]);
    controlHeight = _controlHeight;
    infoSize = Vector2f(190,window.getSize().y - controlHeight);
    winnerText.setFont(_fonts[0]);
    winnerText.setStyle(Text::Bold);
    winnerText.setCharacterSize(fontSize+12);
    winnerText.setFillColor(Color::White);
    winnerDisplay.setOutlineColor(Color::White);
    winnerDisplay.setOutlineThickness(3);

    blendMode.alphaDstFactor = blendMode.DstAlpha;
    blendMode.alphaSrcFactor = blendMode.DstAlpha;

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
        animators[name].addAnimation(name,animations[name],seconds(timeThresholds[1]));
        animators[name].playAnimation(name,true);
    }
    player[0].setTexture(&textures["player1"]);
    player[1].setTexture(&textures["player2"]);


    deadzone.setFillColor(Color(250,0,0,80));


    initPlayerAnimation();

    playerInfo.setSize(infoSize);
    playerInfo.setPosition(0,0);
    playerInfo.setFillColor(Color(13,13,13));

    const float spacing = 50;// (window.getSize().y - controlHeight) / 8.0;

    for(int i=0;i<2;i++){
        player_info_transformable[i].setPosition(playerInfoLeftOffset, 5 + i * (infoSize.y / 2));
        names[i].setFont(_fonts[0]);
        names[i].setCharacterSize(fontSize);
        names[i].setStyle(Text::Bold);
        names[i].setPosition(0,50);
        fullnames[i].setFont(_fonts[0]);
        fullnames[i].setCharacterSize(minFontSize);
        fullnames[i].setStyle(Text::Bold);
        fullnames[i].setPosition(0, 50);
        heart[i].setSize(Vector2f(spacing,spacing));
        heart[i].setPosition(names[i].getPosition() + Vector2f(infoSize.x - spacing - 20, fontSize+5));
        heart[i].setTexture(&textures["heart"]);
        upgrades[i].setFont(_fonts[0]);
        upgrades[i].setCharacterSize(fontSize);
        upgrades[i].setStyle(Text::Bold);
        upgrades[i].setPosition(names[i].getPosition() + Vector2f(0,/*spacing+*/fontSize+10));
        extraHealth[i].setFont(_fonts[0]);
        extraHealth[i].setCharacterSize(fontSize-1);
        extraHealth[i].setStyle(Text::Bold);
        extraHealth[i].setPosition(heart[i].getPosition().x+heart[i].getSize().x*3/4,heart[i].getPosition().y+heart[i].getSize().y-fontSize);
    }

    heartTextureSize = textures["heart"].getSize().y;
    upgrades[0].setFillColor(player1Theme);
    upgrades[1].setFillColor(player2Theme);
    names[0].setFillColor(player1Theme);
    names[1].setFillColor(player2Theme);
    fullnames[0].setFillColor(player1Theme);
    fullnames[1].setFillColor(player2Theme);
    fullnames[0].setString("...");
    three_dot_width = fullnames[0].getGlobalBounds().width;

    darkBackground = VertexArray(PrimitiveType::Quads,4);
	Color clr = Color(0,0,0,180);
	for(int i=0;i<4;i++){
		darkBackground[i].color = clr;
	}
	darkBackground[0].position=Vector2f(0,0);
	darkBackground[1].position=Vector2f(window.getSize().x,0);
	darkBackground[2].position=static_cast<Vector2f>(window.getSize());
	darkBackground[3].position=Vector2f(0,window.getSize().y);
}
void Game::load(std::wstring logAddress){


    std::ifstream i;
    i.open(std::filesystem::path(logAddress));
    if (!i.good()) {
        throw std::ios_base::failure("failed to load file");
    }
    i >> json_;
    i.close();

    timePassed=0;
    columns = json_["initial_game_data"]["map_width"];
    rows = json_["initial_game_data"]["map_height"];
    totalTurns = json_["initial_game_data"]["last_step"];
    initialHealth = json_["initial_game_data"]["initial_health"];
    bombDelay = json_["initial_game_data"]["bomb_delay"];
    winnerIndex = json_["initial_game_data"]["winnerId"];
    winnerIndex--;
    const std::string name1 = json_["initial_game_data"]["player_1_name"];
    const std::string name2 = json_["initial_game_data"]["player_2_name"];
    names[0].setString(UnicodeConverter::to_wide(name1));
    names[1].setString(UnicodeConverter::to_wide(name2));
    fullnames[0].setString(names[0].getString());
    fullnames[1].setString(names[1].getString());
    for(int i=0;i<2;i++){
        showing_fullname[i] = false;
        names[i].setCharacterSize(fontSize);
        while((names[i].getGlobalBounds().width + playerInfoLeftOffset) >= infoSize.x && names[i].getCharacterSize() >= minFontSize)
            names[i].setCharacterSize(names[i].getCharacterSize()-1);
        truncated[i] = false;
        if ((names[i].getGlobalBounds().width + playerInfoLeftOffset) >= infoSize.x) {
            //truncate if still bigger than allowed
            int c = 0;
            float x = names[i].findCharacterPos(c).x;
            while (names[i].findCharacterPos(c).x + three_dot_width <= infoSize.x) {
                c++;
            }
            names[i].setString(names[i].getString().substring(0, c - 2) + "...");
            truncated[i] = true;
        }
    }

    timeThreshold=timeThresholds[speed];
    turn = 0;

    float widthRatio=(window.getSize().x-infoSize.x)/(float)columns;
    float heightRatio=(window.getSize().y-controlHeight)/(float)rows;
    scale = widthRatio<heightRatio ? widthRatio : heightRatio;
    startPoint.x=(window.getSize().x-infoSize.x-scale*columns)/2.0+infoSize.x;
    startPoint.y=(window.getSize().y-controlHeight-scale*rows)/2.0;
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
    bombMask.setSize(Vector2f(scale,scale));
    rTexture.create(scale,scale);
    player[0].setSize(Vector2f(scale,scale));
    player[1].setSize(Vector2f(scale,scale));
    for(int i=0;i<Resources_n::spritesCount;i++){
        std::string & name=Resources_n::sprites[i];
        shapes[name].setSize(Vector2f(scale,scale));
        animators[name].playAnimation(name,true);
    }

    updatePlayer();
    updateAnimators(Time::Zero);
}

void Game::update(Time DeltaTime){
    if(playing)
        timePassed+=DeltaTime.asSeconds();
    if(turn==totalTurns)
        timePassed=0.0;
    while(timePassed/timeThreshold>=1){
        timePassed=timePassed-timeThreshold;
        turn++;
        setupTurn();
        if(turn==totalTurns+1){
            turn--;
        }
    }
    updateAnimators(DeltaTime);
    updatePlayer();
    draw();
}
void Game::updateAnimators(Time DeltaTime){
    Time realDeltaTime = seconds(DeltaTime.asSeconds()*timeThresholds[1]/timeThreshold);
    for(int i=0;i<Resources_n::spritesCount;i++){
        std::string & name=Resources_n::sprites[i];
        animators[name].update(playing?realDeltaTime:Time::Zero);
        animators[name].animate(shapes[name]);
    }
    for(int i=0;i<2;i++){
        std::string name = "player"+std::to_string(i+1);
        animators[name].update(playing?realDeltaTime:Time::Zero);
        animators[name].animate(player[i]);
    }
}

void Game::updateMouse(Vector2f position, bool F_pressed)
{
    for (int i = 0; i < 2; i++) {
        if (names[i].getGlobalBounds().contains(player_info_transformable[i].getInverseTransform() * position)) {
            showing_fullname[i] = true;
        }
        else {
            showing_fullname[i] = F_pressed;
        }
    }
}

void Game::draw(RenderTarget* target){
    if (target == nullptr) {
        target = &this->window;
    }
    target->draw(vertices,&textures["floor"]);
    for(int i=0;i<rows;i++){
        for(int j=0;j<columns;j++){
            int mask = json_["steps"][turn]["map_data"][i][j];
            bool isBox=false;
            if(has_state(mask,Tile_State::wall)){
                obstacle.setPosition(j*scale+startPoint.x,i*scale+startPoint.y);
                target->draw(obstacle);
            }
            if(has_state(mask,Tile_State::box)){
                box.setPosition(j*scale+startPoint.x,i*scale+startPoint.y);
                target->draw(box);
                isBox=true;
            }
            if(!isBox && has_state(mask,Tile_State::upgrade_health)){
                shapes["upgrade_health"].setPosition(j*scale+startPoint.x,i*scale+startPoint.y);
                target->draw(shapes["upgrade_health"]);
            }
            if(!isBox && has_state(mask,Tile_State::upgrade_range)){
                shapes["upgrade_power"].setPosition(j*scale+startPoint.x,i*scale+startPoint.y);
                target->draw(shapes["upgrade_power"]);
            }
            if(!isBox && has_state(mask,Tile_State::upgrade_trap)){
                shapes["upgrade_trap"].setPosition(j*scale+startPoint.x,i*scale+startPoint.y);
                target->draw(shapes["upgrade_trap"]);
            }
            if(has_state(mask,Tile_State::trap)){
                shapes["trap"].setPosition(j*scale+startPoint.x,i*scale+startPoint.y);
                target->draw(shapes["trap"]);
            }
        }
    }


    //draw bombs
    for(int i=0;i<json_["steps"][turn]["bombs"].size();i++){
        int x = json_["steps"][turn]["bombs"][i]["y"];
        int y = json_["steps"][turn]["bombs"][i]["x"];
        int stepsPassed = json_["steps"][turn]["bombs"][i]["steps_passed"];
        int alpha = map(stepsPassed,0,bombDelay,0,100);
        bombMask.setFillColor(Color(255,0,0,alpha));

        rTexture.clear(Color(0,0,0,0));
        rTexture.draw(shapes["bomb"]);
        rTexture.draw(bombMask,sf::RenderStates(blendMode));
        rTexture.display();
        const sf::Texture& texture = rTexture.getTexture();
        sf::Sprite sprite(texture);
        sprite.setPosition(x*scale+startPoint.x,y*scale+startPoint.y);
        target->draw(sprite);
    }

    for(int i=0;i<2;i++)
        target->draw(player[i]);


    for(int i=0;i<rows;i++){
        for(int j=0;j<columns;j++){
            int mask = json_["steps"][turn]["map_data"][i][j];
            if(has_state(mask,Tile_State::fire)){
                shapes["fire"].setPosition(j*scale+startPoint.x,i*scale+startPoint.y);
                target->draw(shapes["fire"]);
            }
            if(has_state(mask,Tile_State::deadzone)){
                deadzone.setPosition(j*scale+startPoint.x,i*scale+startPoint.y);
                target->draw(deadzone);
            }
        }
    }

    target->draw(playerInfo);
    for(int i=0;i<2;i++){
        target->draw(heart[i],        player_info_transformable[i].getTransform());
        target->draw(upgrades[i],     player_info_transformable[i].getTransform());
        target->draw(extraHealth[i],  player_info_transformable[i].getTransform());
        if (truncated[i] && showing_fullname[i]) {
            target->draw(fullnames[i],player_info_transformable[i].getTransform());
        }
        else {
            target->draw(names[i],    player_info_transformable[i].getTransform());
        }
    }
    if(turn == totalTurns)
        displayWinner(target);
}

void Game::displayWinner(RenderTarget* target){
    std::wstring textToDisplay;
    sf::Color color;
    if(winnerIndex==1){
        color = player2Theme;
        color.a = 200;
    }
    else{
        color = player1Theme;
        color.a = 200;
    }
	textToDisplay = names[winnerIndex].getString().toWideString();
    textToDisplay+=std::wstring(L" WINS! ");
    winnerText.setString(textToDisplay);
    winnerText.setPosition((target->getSize().x-winnerText.getGlobalBounds().width)/2,(target->getSize().y-winnerText.getGlobalBounds().height)/2);
    winnerDisplay.setFillColor(color);
    winnerDisplay.setSize(Vector2f(winnerText.getGlobalBounds().width+10,winnerText.getGlobalBounds().height*3));
    winnerDisplay.setPosition((target->getSize().x-winnerDisplay.getSize().x)/2.0,(target->getSize().y-winnerDisplay.getSize().y)/2.0);
	target->draw(darkBackground);
    target->draw(winnerDisplay);
    target->draw(winnerText);
	target->draw(player[winnerIndex]);
}

void Game::changeSpeed(int _speed){
    speed=_speed;
    float timeRatio = timePassed / timeThreshold;
    timeThreshold = timeThresholds[speed];
    timePassed = timeThreshold * timeRatio;
}
void Game::setPlaying(bool _playing){
    playing=_playing;
    if(playing && timePassed==0.0)
        setupTurn();
}

void Game::showFullName(bool player, bool show)
{
    showing_fullname[player] = show;
}

bool Game::setTurn(int _turn){
    if (_turn > totalTurns) {
        if (turn == totalTurns) {
            return false;
        }
        turn = totalTurns;
        setupTurn();
    }
    else if (_turn < 0) {
        if (turn == 0) {
            return false;
        }
        turn = 0;
    }
    else {
        turn = _turn;
        setupTurn();
    }
    timePassed=0;
    updatePlayer();
    return true;
}

bool Game::setTurn(double progress) // progress between 0 and 1
{
#ifdef DEBUGGING
    assert(0 <= progress && progress <= 1);
#endif // DEBUGGING
    return setTurn(static_cast<int>(progress * totalTurns));
}

Vector2f Game::getPlayerPosition(int _turn,int player){
    int x;
    int y;
    if(_turn == -1){
        x = json_["initial_game_data"][std::string("player_")+std::to_string(player+1)+std::string("_init_y")];
        y = json_["initial_game_data"][std::string("player_")+std::to_string(player+1)+std::string("_init_x")];
    }
    else{
        x = json_["steps"][_turn]["players_data"][player]["y_position"];
        y = json_["steps"][_turn]["players_data"][player]["x_position"];
    }
    return Vector2f(x*scale+startPoint.x,y*scale+startPoint.y);
}

void Game::updatePlayer(){
    for(int i=0;i<2;i++){
        health[i] = json_["steps"][turn]["players_data"][i]["health"];
        std::string bombPower = std::to_string(int(json_["steps"][turn]["players_data"][i]["bomb_power_level"]));
        std::string trapCount = std::to_string(int(json_["steps"][turn]["players_data"][i]["traps_left"]));
        upgrades[i].setString("POWER: "+bombPower+"\nTRAPS:  "+trapCount);

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
            int actionTaken = json_["steps"][turn]["players_data"][i]["action_taken"];
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

    for(int i=0;i<8;i++){
        animators["player1"].addAnimation(playerAnimations[i],animations[playerAnimations[i]],seconds(timeThresholds[1]));
        animators["player2"].addAnimation(playerAnimations[i],animations[playerAnimations[i]],seconds(timeThresholds[1]));
    }
}

void Game::setupTurn(){
    if(turn == totalTurns){
        audio.play(Audio::Winner);
    }
    if(turn>0){
        bool flag = false;
        for(int i=0;i<json_["steps"][turn-1]["bombs"].size();i++){
            if(json_["steps"][turn-1]["bombs"][i]["steps_passed"] == bombDelay){
                flag = true;
                break;
            }
        }
        if(flag){
            if(playing)
                audio.play(Audio::Boom);
                animators["fire"].playAnimation("fire", false);
        }
    }
}
