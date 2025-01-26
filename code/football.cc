#include <cassert>
#include <cstdio>
#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#include "classes.h"
#include <gf/Action.h>
#include <gf/Clock.h>
#include <gf/Color.h>
#include <gf/EntityContainer.h>
#include <gf/Event.h>
#include <gf/Log.h>
#include <gf/RenderWindow.h>
#include <gf/ResourceManager.h>
#include <gf/ViewContainer.h>
#include <gf/Views.h>
#include <gf/Window.h>
#include <gf/Random.h>
#include <gf/Shapes.h>
#include <gf/Collision.h>
#include <gf/Entity.h>
#include <gf/SpriteBatch.h>
#include <gf/Sprite.h>
#include <gf/Texture.h>
#include <gf/Image.h>
#include <cmath>
#include <unordered_map>
#include <gf/TileLayer.h>

#include "config.h"
#include "ball.h"
#include "player.h"
#include "role.h"
#include "team.h"
#include "field.h"

#define FOOTBALL_DATA_DIR "@FOOTBALL_DATA_DIR@"

#define BALLSIZE 10
#define PLAYERSIZE 30
#define FIELDXSIZE 3200
#define FIELDYSIZE 1664
#define TOPPOLE 640
#define BOTTOMPOLE 1024
#define FIELD_X_TILES 25
#define FIELD_Y_TILES 13
#define TILESIZE 128

// constants
static constexpr float SPEED = 240.0f;

void touch(bool isTop, bool forPlayerTeam) {
  //Do something for a touch
}

void corner(int side, bool forPlayerTeam) {
  //Do something for a corner (0 = topleft, 1 = bottomright, 2 = topleft, 3 = bottomright)
}

/**
 * @param bool isPlayerTeam true if the goal was made against the team of the player (left)
 */
void goal(bool isPlayerTeam) {
  //Do something for a goal
}

void checkOut(int isOut, std::vector<Player *> teamPlayersVec, Player *lastTouchedBy) {
    if (isOut != 0) {
        bool isMemberOfTeam = count(teamPlayersVec.begin(), teamPlayersVec.end(), lastTouchedBy) > 0;
        switch (isOut) {
            case 1 : 
                corner(0, isMemberOfTeam);
                lastTouchedBy->changeColor(gf::Color::Spring);
                break;
            case 2 : 
                goal(true);
                lastTouchedBy->changeColor(gf::Color::Red);
                break;
            case 3 : 
                corner(1, isMemberOfTeam);
                lastTouchedBy->changeColor(gf::Color::Spring);
                break;
            case 4 : 
                touch(true, isMemberOfTeam);
                lastTouchedBy->changeColor(gf::Color::Violet);
                break;
            case 5 : 
                touch(false, isMemberOfTeam);
                lastTouchedBy->changeColor(gf::Color::Violet);
                break;
            case 6 :
                corner(2, isMemberOfTeam);
                lastTouchedBy->changeColor(gf::Color::Spring);
                break;
            case 7 : 
                goal(false);
                lastTouchedBy->changeColor(gf::Color::Red);
                break;
            case 8 :
                corner(3, isMemberOfTeam);
                lastTouchedBy->changeColor(gf::Color::Spring);
                break;
        }
    }else {
        if (lastTouchedBy != nullptr) {
            lastTouchedBy->changeColor(gf::Color::Azure);
        }
    }
}

gf::Vector2f normalizeVelocity(gf::Vector2f velocity) {
    gf::Vector2f ret = velocity;
    if (ret.x != 0 && ret.y != 0) {
        ret /= sqrt(2);
    }
    return ret;
}



int main() {
    static constexpr gf::Vector2u ScreenSize(800, 800);

    gf::Log::setLevel(gf::Log::Info);

    // setup resource directory
    gf::ResourceManager resourceManager;
    resourceManager.addSearchDir(FOOTBALL_DATA_DIR);
    //resourceManager.addSearchDir("./assets");

    gf::Random random;

    // initialize window
    gf::Window window("gf football", ScreenSize, ~gf::WindowHints::Resizable);
    window.setVerticalSyncEnabled(true);
    window.setFramerateLimit(60);

    gf::RenderWindow renderer(window);

    gf::Vector2u windowSize = window.getSize();

    // add cameras
    gf::ViewContainer views;
    gf::ExtendView view({0.0f, 20.0f}, {float(window.getSize().x), float(window.getSize().y)});
    views.addView(view);
    views.setInitialFramebufferSize(window.getSize());

    // add actions
    gf::ActionContainer actions;

    gf::Action closeWindowAction("Close window");
    closeWindowAction.addCloseControl();
    closeWindowAction.addKeycodeKeyControl(gf::Keycode::Escape);
    actions.addAction(closeWindowAction);

    gf::Action fullscreenAction("Fullscreen");
    fullscreenAction.addKeycodeKeyControl(gf::Keycode::F);
    actions.addAction(fullscreenAction);

    gf::Action leftAction("Left");
    leftAction.addScancodeKeyControl(gf::Scancode::A);
    leftAction.addScancodeKeyControl(gf::Scancode::Q);
    leftAction.addScancodeKeyControl(gf::Scancode::Left);
    leftAction.setContinuous();
    actions.addAction(leftAction);

    gf::Action rightAction("Right");
    rightAction.addScancodeKeyControl(gf::Scancode::D);
    rightAction.addScancodeKeyControl(gf::Scancode::Right);
    rightAction.setContinuous();
    actions.addAction(rightAction);

    gf::Action upAction("Up");
    upAction.addScancodeKeyControl(gf::Scancode::W);
    upAction.addScancodeKeyControl(gf::Scancode::Z);
    upAction.addScancodeKeyControl(gf::Scancode::Up);
    upAction.setContinuous();
    actions.addAction(upAction);

    gf::Action downAction("Down");
    downAction.addScancodeKeyControl(gf::Scancode::S);
    downAction.addScancodeKeyControl(gf::Scancode::Down);
    downAction.setContinuous();
    actions.addAction(downAction);

    gf::Action dropAction("Drop");
    dropAction.addScancodeKeyControl(gf::Scancode::X);
    actions.addAction(dropAction);

    gf::Action switchAction("Switch");
    switchAction.addScancodeKeyControl(gf::Scancode::C);
    actions.addAction(switchAction);

    gf::Action passAction("Pass");
    passAction.addScancodeKeyControl(gf::Scancode::V); 
    actions.addAction(passAction);

    gf::Action shootAction("Shoot");
    shootAction.addScancodeKeyControl(gf::Scancode::Space); 
    actions.addAction(shootAction);

    gf::Action tackleAction("Tackle");
    tackleAction.addScancodeKeyControl(gf::Scancode::E); 
    actions.addAction(tackleAction);

    gf::Action sprintAction("Sprint");
    sprintAction.addScancodeKeyControl(gf::Scancode::B); 
    actions.addAction(sprintAction);

    // Temporary
    gf::Action stopSprintAction("StopSprint");
    stopSprintAction.addScancodeKeyControl(gf::Scancode::N); 
    actions.addAction(stopSprintAction);

    // add entities
    gf::EntityContainer mainEntities;

    gf::Clock clock;
    bool fullscreen = false;

    // Initialize a team with 11 players
    Team team("Team A", gf::Color::Azure);
    team.initPlayers();  // Initialize the players

    // Texture players
    /*gf::Texture texture("../assets/PNG/Blue/characterBlue(1).png") ;

    gf::Sprite playerSprite;
    playerSprite.setTexture(texture);*/

    const std::string playerTexturesBasePath = "../assets/PNG/Blue/characterBlue (";
    std::vector<gf::Texture> playerTextures;

    for (int i = 1; i <= 10; ++i) {
        gf::Texture texture(playerTexturesBasePath + std::to_string(i) + ").png");
        playerTextures.push_back(std::move(texture));
    }

    gf::Texture playerTexture("../assets/PNG/Blue/characterBlue (1).png");
    playerTextures.push_back(std::move(playerTexture));

    
    
    // default setup
    team.setupPlayers(FIELD_X_TILES*TILESIZE,FIELD_Y_TILES*TILESIZE);

    std::unordered_map<Player*, gf::Sprite> playerSprites;

    for (size_t i = 0; i < team.getPlayers().size(); ++i) {
        Player* player = team.getPlayers()[i];
        gf::Sprite sprite(playerTextures[i]);
        sprite.setScale({player->getSize()/ playerTextures[i].getSize().x, player->getSize() / playerTextures[i].getSize().y});
        sprite.setPosition(player->getPosition());
        sprite.setRotation(player->getAngle());
        playerSprites[player] = sprite;
    }

    // Add team players to the mainEntities
    
    
    float x = (13 % FIELD_X_TILES) * TILESIZE - TILESIZE/2; 
    float y = (FIELD_Y_TILES*TILESIZE)/2;
    Ball ball(TILESIZE/10, {x, y}, gf::Color::Rose);
    mainEntities.addEntity(ball);
    
    gf::Vector2f velocity(0.0f, 0.0f);

    Player* mainPlayer = team.getPlayers()[10]; 

    // main loop
    bool cam1 = true;


    while (window.isOpen()) {
        // inputs
        gf::Event event;

        while (window.pollEvent(event)) {
            actions.processEvent(event);
            views.processEvent(event);
        }

        if (closeWindowAction.isActive()) {
            window.close();
        }

        if (fullscreenAction.isActive()) {
            fullscreen = !fullscreen;
            window.setFullscreen(fullscreen);
        }

        velocity = {0.0f, 0.0f};
        if (leftAction.isActive()) {
            velocity.x += -SPEED;
        }
        if (rightAction.isActive()) {
            velocity.x += SPEED;
        }

        if (upAction.isActive()) {
            velocity.y += -SPEED;
        }
        if (downAction.isActive()) {
            velocity.y += SPEED;
        }
        velocity = normalizeVelocity(velocity);

        // update

        if (dropAction.isActive() && ball.isLockedTo(mainPlayer)) {
            ball.unlock();
        }

        for (Player* player : team.getPlayers()) {
            if (ball.isTouchingPlayer(*player)) {
                ball.lockTo(player);
            }
            if (ball.isLockedTo(player)) {
                mainPlayer = player;
            }

            for (Player* other : team.getPlayers()) {
                if (player != other) {
                    player->handleCollision(*other);
                }
            }
        }

        if (ball.isLockedTo(mainPlayer)) {
            if (passAction.isActive()) {
                ball.unlock();
                ball.setVelocity(mainPlayer->getPassVelocity());
            }

            if (shootAction.isActive()) {
                ball.unlock();
                ball.setVelocity(mainPlayer->getShootVelocity());
            }
        }

        if (tackleAction.isActive() && !mainPlayer->isTackling() &&  !ball.isLockedTo(mainPlayer)) {
        mainPlayer->setTackleData(400.0f, mainPlayer->getAngle()); 
        }

        if (mainPlayer->isTackling()) {
            for (Player* player : team.getPlayers()) {
                if (player != mainPlayer && mainPlayer->collidesWith(*player)) {
                    player->freeze(1.0f);
                }
            }
        }

       if (sprintAction.isActive()) {
            mainPlayer->startSprint();
       }
       if (stopSprintAction.isActive()){
            mainPlayer->stopSprint();
       }

        if (switchAction.isActive() && !mainPlayer->isTackling() && !ball.isLockedTo(mainPlayer)) {
            if (cam1) {
                Player* closestPlayer = team.getClosestPlayerToBall(ball);
                if (closestPlayer) {
                    mainPlayer = closestPlayer;
                }
            }
            cam1 = !cam1;
        }

        actions.reset();

        gf::Time dt = clock.restart();
        mainPlayer->setVelocity(velocity);

        mainPlayer->update(dt.asSeconds());

        for (auto& [player, sprite] : playerSprites) {
            sprite.setPosition(player->getPosition());
            sprite.setRotation(player->getAngle());
        }

        gf::Vector2f mainPlayerPosition = mainPlayer->getPosition();
        gf::Vector2f ballPosition = ball.getPosition();

        float mainPlayerSize = mainPlayer->getSize();
        float ballSize = mainPlayerSize;

        float mainPlayerLeft = mainPlayerPosition.x;
        float mainPlayerRight = mainPlayerPosition.x + mainPlayerSize;
        float mainPlayerTop = mainPlayerPosition.y;
        float mainPlayerBottom = mainPlayerPosition.y + mainPlayerSize;

        float ballLeft = ballPosition.x;
        float ballRight = ballPosition.x + ballSize;
        float ballTop = ballPosition.y;
        float ballBottom = ballPosition.y + ballSize;

        bool isColliding = (mainPlayerRight > ballLeft && mainPlayerLeft < ballRight &&
                            mainPlayerBottom > ballTop && mainPlayerTop < ballBottom);

        if (isColliding) {
            ball.lockTo(mainPlayer);
        }

        if (ball.isLockedTo(mainPlayer)) {
            ball.setVelocity(velocity);
        } else {
            ball.setVelocity(ball.getVelocity());
        }

        ball.update(dt.asSeconds());

        int out = ball.isOutOfField(FIELDXSIZE, FIELDYSIZE, TOPPOLE, BOTTOMPOLE, TILESIZE);

        checkOut(out, team.getPlayers(), ball.getLastTouchedBy());

        if (ball.isLockedTo(mainPlayer) || ball.getLastTouchedBy() == nullptr) {
            view.setCenter(mainPlayer->getPosition());
        }else {
            view.setCenter(ball.getPosition());
        }

        std::vector<int> tileOrder = {
            // Row 1
            5, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 6,
            // Row 2
            14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 17,
            // Row 3
            14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 17,
            // Row 4
            27, 29, 29, 37, 0, 0, 0, 0, 0, 0, 0, 0, 16, 0, 0, 0, 0, 0, 0, 0, 0, 36, 29, 29, 30,
            // Row 5
            14, 0, 0, 17, 99, 0, 0, 0, 0, 0, 0, 0, 16, 0, 0, 0, 0, 0, 0, 0, 95, 14, 0, 0, 17,
            // Row 6
            14, 0, 0, 17, 112, 0, 0, 0, 0, 0, 0, 7, 46, 9, 0, 0, 0, 0, 0, 0, 108, 14, 0, 0, 17,
            // Row 7
            14, 0, 13, 17, 125, 0, 0, 0, 0, 0, 0, 20, 15, 22, 0, 0, 0, 0, 0, 0, 121, 14, 0, 13, 17,
            // Row 8
            14, 0, 0, 17, 138, 0, 0, 0, 0, 0, 0, 33, 48, 35, 0, 0, 0, 0, 0, 0, 134, 14, 0, 0, 17,
            // Row 9
            14, 0, 0, 17, 151, 0, 0, 0, 0, 0, 0, 0, 16, 0, 0, 0, 0, 0, 0, 0, 147, 14, 0, 0, 17,
            // Row 10
            27, 29, 29, 50, 0, 0, 0, 0, 0, 0, 0, 0, 16, 0, 0, 0, 0, 0, 0, 0, 0, 49, 29, 29, 30,
            // Row 11
            14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 17,
            // Row 12
            14,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 17,
            // Row 13
            18, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 42, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 19
        };  

        FootballField field("../assets/Tilesheet/groundGrass.png", tileOrder, FIELD_X_TILES, FIELD_Y_TILES, TILESIZE);
        
        // render
        renderer.clear();

        renderer.setView(view);

        renderer.draw(field.getTileLayer(), gf::RenderStates());


        for(auto& [player, sprite] : playerSprites) {
            player->render(renderer, player == mainPlayer);
        }

        ball.render(renderer);


        renderer.display();
    }

    return 0;
}
