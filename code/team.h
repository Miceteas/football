#include "player.h"

class Team {
    private:
        std::string name;
        gf::Color4f color;
        std::vector<Player*> players;
        std::vector<Player*> bench;

    public :
        Team(std::string name, gf::Color4f color) : name(name), color(color) {}
        std::string getName();
        gf::Color4f getColor();
        
        void showActivePlayers();
        void showBenchPlayers();
        void addPlayer(Player *p);
        void putPlayerBench(Player *p);
};

