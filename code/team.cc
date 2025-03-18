#include "team.h"

Team::Team(std::string name, gf::Color4f color)
: m_name(name)
, m_color(color)
, m_setup(0)
, m_left(true) 
, m_goals(0)
{
    
} 

std::string Team::getName() {
    return m_name;
}

gf::Color4f Team::getColor() {
    return m_color;
}

int Team::getGoals() {
    return m_goals;
}

void Team::addGoal() {
    ++m_goals;
}

void Team::changeSetup(int setup) {
    m_setup = setup;
}

void Team::addPlayer(Player *p) {
    m_players.push_back(p);
}

void Team::putPlayerBench(Player *p) {
    m_bench.push_back(p);
}

void Team::setupSide(bool side){
    m_left = side;
}

std::vector<Player*> Team::getPlayers(){
    return m_players;
}

void Team::initPlayers() {
    m_players.push_back(new Player(100.0f, PLAYERSIZE, gf::Vector2f(0.0f, 0.0f), Role::GOALKEEPER, Side::MIDDLE, m_color, 0));
    m_players.push_back(new Player(90.0f, PLAYERSIZE, gf::Vector2f(10.0f, 20.0f), Role::DEFENDER, Side::TOP,  m_color, 0));
    m_players.push_back(new Player(90.0f, PLAYERSIZE, gf::Vector2f(20.0f, 20.0f), Role::DEFENDER, Side::MIDDLE, m_color, 0));
    m_players.push_back(new Player(90.0f, PLAYERSIZE, gf::Vector2f(30.0f, 20.0f), Role::DEFENDER, Side::MIDDLE, m_color, 0));
    m_players.push_back(new Player(90.0f, PLAYERSIZE, gf::Vector2f(40.0f, 20.0f), Role::DEFENDER, Side::BOTTOM, m_color, 0));
    m_players.push_back(new Player(85.0f, PLAYERSIZE, gf::Vector2f(50.0f, 30.0f), Role::MIDFIELDER, Side::TOP, m_color, 0));
    m_players.push_back(new Player(85.0f, PLAYERSIZE, gf::Vector2f(60.0f, 30.0f), Role::MIDFIELDER, Side::MIDDLE, m_color, 0));
    m_players.push_back(new Player(85.0f, PLAYERSIZE, gf::Vector2f(70.0f, 30.0f), Role::MIDFIELDER, Side::BOTTOM, m_color, 0));
    m_players.push_back(new Player(85.0f, PLAYERSIZE, gf::Vector2f(80.0f, 30.0f), Role::ATTACKER, Side::TOP, m_color, 0));
    m_players.push_back(new Player(80.0f, PLAYERSIZE, gf::Vector2f(90.0f, 40.0f), Role::ATTACKER, Side::MIDDLE, m_color, 0));
    m_players.push_back(new Player(80.0f, PLAYERSIZE, gf::Vector2f(100.0f, 40.0f), Role::ATTACKER, Side::BOTTOM, m_color, 0));
}

void Team::setupPlayers(int style) {
    float xOffset = m_left ? 0.1f * FIELDXSIZE : FIELDXSIZE - 0.1f * FIELDXSIZE;
    float yOffset = FIELDYSIZE / 2.0f;
    float playerSpacingY = FIELDYSIZE / 5.0f;
    float playerSpacingX = FIELDXSIZE / 10.0f;

    for (Player *p : m_players) {
        p->setVelocity({0.0f, 0.0f});
    }
    
    // Goalkeeper
    m_players[0]->setPosition({xOffset, yOffset});

    // Défense (4 joueurs)
    float defenseX = xOffset + (m_left ? 1 : -1) * playerSpacingX;
    m_players[1]->setPosition({defenseX, yOffset - 1.5f * playerSpacingY});
    m_players[2]->setPosition({defenseX, yOffset - 0.5f * playerSpacingY});
    m_players[3]->setPosition({defenseX, yOffset + 0.5f * playerSpacingY});
    m_players[4]->setPosition({defenseX, yOffset + 1.5f * playerSpacingY});

    // Milieu de terrain (3 joueurs)
    float midfieldX = defenseX + (m_left ? 1 : -1) * playerSpacingX;
    m_players[5]->setPosition({midfieldX, yOffset - playerSpacingY});
    m_players[6]->setPosition({midfieldX, yOffset});
    m_players[7]->setPosition({midfieldX, yOffset + playerSpacingY});

    float attackX = 0;

    if (style == 1) {
        // Attaque (3 joueurs) position avancée,
        attackX = midfieldX + (m_left ? 2.0f : -2.0f) * playerSpacingX;
        playerSpacingY /= 2;
    }else {
        // Attaque (3 joueurs) position normale,
        attackX = midfieldX + (m_left ? 1 : -1) * playerSpacingX;
    }
        
    m_players[8]->setPosition({attackX, yOffset - playerSpacingY});
    m_players[9]->setPosition({attackX, yOffset});
    m_players[10]->setPosition({attackX, yOffset + playerSpacingY});
}

Player* Team::getClosestPlayerToBall(const Ball& ball) {
    Player* closestPlayer = nullptr;
    float minDistance = std::numeric_limits<float>::max();

    for (Player* player : m_players) {
        float distance = gf::squareDistance(player->getPosition(), ball.getPosition());

        if (distance < minDistance) {
            minDistance = distance;
            closestPlayer = player;
        }
    }

    return closestPlayer;
}

void Team::moveTeam(Ball& ball, const Player *mainPlayer) {
    Player *lastTouched = ball.getLockedTo();
    bool isBallMemberOfTeam = count(m_players.begin(), m_players.end(), lastTouched) > 0;
    for (Player *player : m_players) {
        if (player != mainPlayer) {
            player->AImove(ball, m_left, isBallMemberOfTeam, m_players);
        }
    }
}

void Team::update(float dt) {
    for (Player *player : m_players) {
        player->update(dt);
    }
}