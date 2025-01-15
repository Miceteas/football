#include "team.h"
#include "player.h"

#define PLAYERSIZE 30


std::string Team::getName() {
    return name;
}

gf::Color4f Team::getColor() {
    return color;
}

void changeSetup(int setup) {
    setup = setup;
}

void Team::addPlayer(Player *p) {
    players.push_back(p);
}

void Team::putPlayerBench(Player *p) {
    bench.push_back(p);
}

void Team::showActivePlayers(gf::RenderTarget& target) {
    for (Player *p : players) {
        p->render(target); 
    }
}

std::vector<Player*> Team::getPlayers(){
    return players;
}

void Team::showBenchPlayers(gf::RenderTarget& target) {
    for (Player *p : bench) {
        p->render(target);
    }
}

void Team::initPlayers() {
    players.push_back(new Player(100.0f, PLAYERSIZE, gf::Vector2f(0.0f, 0.0f), Role::GOALKEEPER, color, M_PI*3/2));
    players.push_back(new Player(90.0f, PLAYERSIZE, gf::Vector2f(10.0f, 20.0f), Role::DEFENDER, color, M_PI*3/2));
    players.push_back(new Player(90.0f, PLAYERSIZE, gf::Vector2f(20.0f, 20.0f), Role::DEFENDER, color, M_PI*3/2));
    players.push_back(new Player(90.0f, PLAYERSIZE, gf::Vector2f(30.0f, 20.0f), Role::DEFENDER, color, M_PI*3/2));
    players.push_back(new Player(90.0f, PLAYERSIZE, gf::Vector2f(40.0f, 20.0f), Role::DEFENDER, color, M_PI*3/2));
    players.push_back(new Player(85.0f, PLAYERSIZE, gf::Vector2f(50.0f, 30.0f), Role::DEFENDER, color, M_PI*3/2));
    players.push_back(new Player(85.0f, PLAYERSIZE, gf::Vector2f(60.0f, 30.0f), Role::MIDFIELDER, color, M_PI*3/2));
    players.push_back(new Player(85.0f, PLAYERSIZE, gf::Vector2f(70.0f, 30.0f), Role::MIDFIELDER, color, M_PI*3/2));
    players.push_back(new Player(85.0f, PLAYERSIZE, gf::Vector2f(80.0f, 30.0f), Role::MIDFIELDER, color, M_PI*3/2));
    players.push_back(new Player(80.0f, PLAYERSIZE, gf::Vector2f(90.0f, 40.0f), Role::ATTACKER, color, M_PI*3/2));
    players.push_back(new Player(80.0f, PLAYERSIZE, gf::Vector2f(100.0f, 40.0f), Role::ATTACKER, color, M_PI*3/2));
}

void Team::setupPlayers(float fieldHeight, float fieldWidth) {
    float xOffset = fieldWidth / 2.0f;
    float yOffset = up ? 0.1f * fieldHeight : fieldHeight - 0.1f * fieldHeight;
    float playerSpacing = fieldWidth / 10.0f;

    players[0]->setVelocity({0.0f, 0.0f});
    players[0]->setPosition({xOffset, yOffset});

    float defenderYOffset = yOffset + (up ? 1 : -1) * (fieldHeight / 8.0f);
    players[1]->setVelocity({0.0f, 0.0f});
    players[1]->setPosition({xOffset - playerSpacing, defenderYOffset});
    players[2]->setVelocity({0.0f, 0.0f});
    players[2]->setPosition({xOffset, defenderYOffset});
    players[3]->setVelocity({0.0f, 0.0f});
    players[3]->setPosition({xOffset + playerSpacing, defenderYOffset});

    float midfielderYOffset = defenderYOffset + (up ? 1 : -1) * (fieldHeight / 8.0f);
    players[4]->setVelocity({0.0f, 0.0f});
    players[4]->setPosition({xOffset - playerSpacing, midfielderYOffset});
    players[5]->setVelocity({0.0f, 0.0f});
    players[5]->setPosition({xOffset, midfielderYOffset});
    players[6]->setVelocity({0.0f, 0.0f});
    players[6]->setPosition({xOffset + playerSpacing, midfielderYOffset});

    float strikerYOffset = midfielderYOffset + (up ? 1 : -1) * (fieldHeight / 8.0f);
    players[7]->setVelocity({0.0f, 0.0f});
    players[7]->setPosition({xOffset - playerSpacing, strikerYOffset});
    players[8]->setVelocity({0.0f, 0.0f});
    players[8]->setPosition({xOffset, strikerYOffset});
    players[9]->setVelocity({0.0f, 0.0f});
    players[9]->setPosition({xOffset + playerSpacing, strikerYOffset});

    players[10]->setVelocity({0.0f, 0.0f});
    players[10]->setPosition({xOffset, strikerYOffset + (up ? 1 : -1) * (fieldHeight / 8.0f)});
}







