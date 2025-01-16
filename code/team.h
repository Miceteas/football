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

class Team {
    private:
        std::string name;
        gf::Color4f color;
        std::vector<Player*> players;
        std::vector<Player*> bench;
        int setup;
        bool up;

    public :
        Team(std::string name, gf::Color4f color) : name(name), color(color), setup(0), up(true) {}
        std::string getName();
        gf::Color4f getColor();
        void changeSetup(int setup);
        
        void showActivePlayers(gf::RenderTarget& target);
        void showBenchPlayers(gf::RenderTarget& target);

        void addPlayer(Player *p);
        void putPlayerBench(Player *p);
        std::vector<Player*> getPlayers();

        void initPlayers();
        void setupPlayers(float fieldWidth, float fieldHeight);
};

