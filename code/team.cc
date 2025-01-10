#include "team.h"
#include "player.h"


std::string Team::getName() {
    return name;
}

gf::Color4f Team::getColor() {
    return color;
}

void Team::addPlayer(Player *p) {
    players.push_back(p);
}

void Team::putPlayerBench(Player *p) {
    bench.push_back(p);
}

void Team::showActivePlayers() {
    for (Player *p : players) {
        //p->render(target); sorry idk how to do it...
    }
}

void Team::showBenchPlayers() {
    for (Player *p : bench) {
        //p->render(target); sorry idk how to do it...
    }
}


