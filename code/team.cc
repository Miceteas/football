#include "team.h"

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

void Team::setupSide(bool side){
    left = side;
}

void Team::showActivePlayers(gf::RenderTarget& target) {
    for (Player *p : players) {
        p->render(target,false); 
    }
}

std::vector<Player*> Team::getPlayers(){
    return players;
}

void Team::showBenchPlayers(gf::RenderTarget& target) {
    for (Player *p : bench) {
        p->render(target,false);
    }
}

void Team::initPlayers() {
    players.push_back(new Player(100.0f, PLAYERSIZE, gf::Vector2f(0.0f, 0.0f), Role::GOALKEEPER, color, 0));
    players.push_back(new Player(90.0f, PLAYERSIZE, gf::Vector2f(10.0f, 20.0f), Role::DEFENDER, color, 0));
    players.push_back(new Player(90.0f, PLAYERSIZE, gf::Vector2f(20.0f, 20.0f), Role::DEFENDER, color, 0));
    players.push_back(new Player(90.0f, PLAYERSIZE, gf::Vector2f(30.0f, 20.0f), Role::DEFENDER, color, 0));
    players.push_back(new Player(90.0f, PLAYERSIZE, gf::Vector2f(40.0f, 20.0f), Role::DEFENDER, color, 0));
    players.push_back(new Player(85.0f, PLAYERSIZE, gf::Vector2f(50.0f, 30.0f), Role::MIDFIELDER, color, 0));
    players.push_back(new Player(85.0f, PLAYERSIZE, gf::Vector2f(60.0f, 30.0f), Role::MIDFIELDER, color, 0));
    players.push_back(new Player(85.0f, PLAYERSIZE, gf::Vector2f(70.0f, 30.0f), Role::MIDFIELDER, color, 0));
    players.push_back(new Player(85.0f, PLAYERSIZE, gf::Vector2f(80.0f, 30.0f), Role::ATTACKER, color, 0));
    players.push_back(new Player(80.0f, PLAYERSIZE, gf::Vector2f(90.0f, 40.0f), Role::ATTACKER, color, 0));
    players.push_back(new Player(80.0f, PLAYERSIZE, gf::Vector2f(100.0f, 40.0f), Role::ATTACKER, color, 0));
}

// need to have small changes for final product!!! But is good enough
void Team::setupPlayers(float fieldWidth, float fieldHeight) {
    float xOffset = left ? 0.1f * fieldWidth : fieldWidth - 0.1f * fieldWidth;
    float yOffset = fieldHeight / 2.0f;
    float playerSpacingY = fieldHeight / 5.0f;
    float playerSpacingX = fieldWidth / 10.0f;

    // Goalkeeper
    players[0]->setVelocity({0.0f, 0.0f});
    players[0]->setPosition({xOffset, yOffset});

    // Défense (4 joueurs)
    float defenseX = xOffset + (left ? 1 : -1) * playerSpacingX;
    players[1]->setPosition({defenseX, yOffset - 1.5f * playerSpacingY});
    players[2]->setPosition({defenseX, yOffset - 0.5f * playerSpacingY});
    players[3]->setPosition({defenseX, yOffset + 0.5f * playerSpacingY});
    players[4]->setPosition({defenseX, yOffset + 1.5f * playerSpacingY});

    // Milieu de terrain (3 joueurs)
    float midfieldX = defenseX + (left ? 1 : -1) * playerSpacingX;
    players[5]->setPosition({midfieldX, yOffset - playerSpacingY});
    players[6]->setPosition({midfieldX, yOffset});
    players[7]->setPosition({midfieldX, yOffset + playerSpacingY});

    // Attaque (3 joueurs)
    float attackX = midfieldX + (left ? 1 : -1) * playerSpacingX;
    players[8]->setPosition({attackX, yOffset - playerSpacingY});
    players[9]->setPosition({attackX, yOffset});
    players[10]->setPosition({attackX, yOffset + playerSpacingY});
    
    /*
    TO REMOVE BEFORE THE END 
    */

    if (left) {
        players[0]->changeColor(gf::Color::Blue);

        players[1]->changeColor(gf::Color::Azure);
        players[2]->changeColor(gf::Color::Azure);
        players[3]->changeColor(gf::Color::Azure);
        players[4]->changeColor(gf::Color::Azure);

        players[5]->changeColor(gf::Color::Cyan);
        players[6]->changeColor(gf::Color::Cyan);
        players[7]->changeColor(gf::Color::Cyan);

        players[8]->changeColor(gf::Color::Spring);
        players[9]->changeColor(gf::Color::Spring);
        players[10]->changeColor(gf::Color::Spring);
    } else {
        players[0]->changeColor(gf::Color::Red);

        players[1]->changeColor(gf::Color::Orange);
        players[2]->changeColor(gf::Color::Orange);
        players[3]->changeColor(gf::Color::Orange);
        players[4]->changeColor(gf::Color::Orange);

        players[5]->changeColor(gf::Color::Rose);
        players[6]->changeColor(gf::Color::Rose);
        players[7]->changeColor(gf::Color::Rose);

        players[8]->changeColor(gf::Color::Yellow);
        players[9]->changeColor(gf::Color::Yellow);
        players[10]->changeColor(gf::Color::Yellow);
    }
}


Player* Team::getClosestPlayerToBall(const Ball& ball) {
    Player* closestPlayer = nullptr;
    float minDistance = std::numeric_limits<float>::max();

    for (Player* player : players) {
        float distance = std::sqrt(std::pow(player->getPosition().x - ball.getPosition().x, 2) +
                                   std::pow(player->getPosition().y - ball.getPosition().y, 2));

        if (distance < minDistance) {
            minDistance = distance;
            closestPlayer = player;
        }
    }

    return closestPlayer;
}

void Team::moveTeam(Ball& ball, const Player *mainPlayer) {
    Player *lastTouched = ball.getLockedTo();
    bool isBallMemberOfTeam = count(players.begin(), players.end(), lastTouched) > 0;
    for (Player *player : players) {
        if (player != mainPlayer) {
            player->AImove(ball, left, isBallMemberOfTeam, players);
        }
    }
}

void Team::update(float dt) {
    for (Player *player : players) {
        player->update(dt);
    }
}