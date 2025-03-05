#include <gf/Entity.h>
#include <gf/Vector.h>
#include <gf/RenderTarget.h>
#include <gf/Color.h>
#include <gf/Shapes.h>
#include <vector>
#include <iosfwd>
#include <iostream>
#include "role.h"
#include "side.h"

#include "player.h"
#include "ball.h"
#include "config.h"
#include "settings.h"

class Team {
    private:
        std::string m_name;
        gf::Color4f m_color;
        std::vector<Player*> m_players;
        std::vector<Player*> m_bench;
        int m_setup;
        bool m_left;
        int m_goals;

    public :
        Team(std::string name, gf::Color4f color);
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
        void setupPlayers(int style);
        Player* getClosestPlayerToBall(const Ball& ball);
        void moveTeam(Ball& ball, const Player *mainPlayer);
        void update(float dt);
        void addGoal();
        int getGoals();
};

