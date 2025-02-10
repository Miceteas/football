#include <gf/Entity.h>
#include <gf/Vector.h>
#include <gf/RenderTarget.h>
#include <gf/Color.h>
#include <gf/Shapes.h>
#include <vector>
#include <iosfwd>
#include <iostream>
#include "role.h"

#include "player.h"
#include "ball.h"
#include "config.h"

class Team {
    private:
        std::string name;
        gf::Color4f color;
        std::vector<Player*> players;
        std::vector<Player*> bench;
        int setup;
        bool left;

    public :
        Team(std::string name, gf::Color4f color) : name(name), color(color), setup(0), left(true) {}
        std::string getName();
        gf::Color4f getColor();
        void changeSetup(int setup);
        
        void showActivePlayers(gf::RenderTarget& target); // idk... 
        void showBenchPlayers(gf::RenderTarget& target);

        void setupSide(bool side);
        void addPlayer(Player *p);
        void putPlayerBench(Player *p);
        std::vector<Player*> getPlayers();

        void initPlayers();
        void setupPlayers(float fieldWidth, float fieldHeight);
        Player* getClosestPlayerToBall(const Ball& ball);
        void moveTeam(Ball& ball, const Player *mainPlayer);
        void update(float dt);
};

