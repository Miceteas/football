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
#include <gf/Text.h>
#include <gf/Font.h>

#include "config.h"
#include "settings.h"
#include "ball.h"
#include "player.h"
#include "role.h"
#include "team.h"
#include "field.h"
#include "minimap.h"

enum class GameState {
    START,
    PLAYING,
    WAITING,
    END
};

GameState currState = GameState::START;

void touch(bool isTop, Team *teamHand, Team *teamRecieve, Ball *ball) {
    //Do something for a touch
    gf::Vector2f posBall;
    gf::Vector2f posPlayer;
    float angle = 0;
    posBall = ball->getPosition();
    posPlayer = ball->getPosition();

    if (isTop) {
        posBall.y = 10;
        posPlayer.y = 0;
        angle = -M_PI / 2.0f;
    }else {
        posBall.y = FIELDYSIZE - 10;
        posPlayer.y = FIELDYSIZE;
        angle = M_PI / 2.0f;
    }

    ball->moveTo(posBall);
    Player *p = teamRecieve->getPlayers()[10];
    p->moveTo(posPlayer);
    p->setAngle(angle);
}

void corner(int side, Team *teamHand, Team *teamRecieve, Ball *ball) {
    //Do something for a corner (0 = topleft, 1 = bottomright, 2 = topleft, 3 = bottomright)
    gf::Vector2f posBall;
    gf::Vector2f posPlayer;
    float angle = 0;
    switch (side) {
        case 0 : 
            posPlayer = {0, 0};
            posBall = {10, 10};
            angle = -M_PI / 4.0f;
            break;
        case 1 : 
            posPlayer = {0, FIELDYSIZE};
            posBall = {10, FIELDYSIZE - 10};
            angle = M_PI / 4.0f;
            break;
        case 2 : 
            posPlayer = {FIELDXSIZE, 0};
            posBall = {FIELDXSIZE - 10, 0};
            angle = -M_PI * 3.0f / 4.0f;
            break;
        case 3 :
            posPlayer = {FIELDXSIZE, FIELDYSIZE};
            posBall = {FIELDXSIZE - 10, FIELDYSIZE - 10};
            angle = M_PI * 3.0f / 4.0f;
            break;
    }

    ball->moveTo(posBall);
    Player *p = teamRecieve->getPlayers()[10];
    p->moveTo(posPlayer);
    p->setAngle(angle);

}

/**
 * @param Team *team the team that scored the goal
 */
void goal(Team *teamGoal, Team *teamNoGoal, Ball *ball) { 
    teamGoal->addGoal();
    teamGoal->setupPlayers(0);
    teamNoGoal->setupPlayers(1);
    gf::Vector2f middlePos(FIELDXSIZE / 2.0f, FIELDYSIZE / 2.0f);
    ball->moveTo(middlePos);
}

void checkOut(int isOut, std::vector<Player *> teamPlayersVec, Player *lastTouchedBy, Team *team1, Team *team2, Ball *ball) {
    bool isMemberOfTeam = count(teamPlayersVec.begin(), teamPlayersVec.end(), lastTouchedBy) > 0;
    Team *teamHand = team2;
    Team *teamRecieve = team1;
    
    if (isMemberOfTeam) {
        teamHand = team1;
        teamRecieve = team2;
    }

    if (isOut != 0) {
        switch (isOut) {
            case 1 : 
                corner(0, teamHand, teamRecieve, ball);
                break;
            case 2 : 
                goal(team2, team1, ball);
                break;
            case 3 : 
                corner(1, teamHand, teamRecieve, ball);
                break;
            case 4 : 
                touch(true, teamHand, teamRecieve, ball);
                break;
            case 5 : 
                touch(false, teamHand, teamRecieve, ball);
                break;
            case 6 :
                corner(2, teamHand, teamRecieve, ball);
                break;
            case 7 : 
                goal(team1, team2, ball);
                break;
            case 8 :
                corner(3, teamHand, teamRecieve, ball);
                break;
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

void endMatch(int playerGoals, int aiGoals) {
    if (playerGoals < aiGoals) {
        std::cout << "YOU LOST :(" << playerGoals << "-" << aiGoals << std::flush;
    } else if (playerGoals == aiGoals) {
        std::cout << "TIE !" << playerGoals << "-" << aiGoals << std::flush;
    } else {
        std::cout << "YOU WIN GOOD JOB !" << playerGoals << "-" << aiGoals << std::flush;
    }
}

bool isInside(const gf::RoundedRectangleShape& button, const gf::Vector2f& mousePos) {
    gf::Vector2f pos = button.getPosition();
    gf::Vector2f size = button.getSize();
    return (mousePos.x >= pos.x && mousePos.x <= pos.x + size.x &&
            mousePos.y >= pos.y && mousePos.y <= pos.y + size.y);
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
    sprintAction.setContinuous();
    actions.addAction(sprintAction);

    gf::Action menu("menu");
    menu.addScancodeKeyControl(gf::Scancode::P); 
    actions.addAction(menu);

    // add entities
    gf::EntityContainer mainEntities;

    gf::Clock clock;
    bool fullscreen = false;

    // Initialize a team with 11 players
    Team team("Team A", gf::Color::Azure);
    team.initPlayers();  // Initialize the players

    Team team2("Team B", gf::Color::Chartreuse);
    team2.initPlayers();
    team2.setupSide(false);

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
    team.setupPlayers(1);
    team2.setupPlayers(0);

    std::unordered_map<Player*, gf::Sprite> playerSprites;
    std::unordered_map<Player*, gf::Sprite> playerSprites2;

    for (size_t i = 0; i < team.getPlayers().size(); ++i) {
        Player* player = team.getPlayers()[i];
        gf::Sprite sprite(playerTextures[i]);
        sprite.setScale({player->getSize()/ playerTextures[i].getSize().x, player->getSize() / playerTextures[i].getSize().y});
        sprite.setPosition(player->getPosition());
        sprite.setRotation(player->getAngle());
        playerSprites[player] = sprite;
    }

    for (size_t i = 0; i < team2.getPlayers().size(); ++i) {
        Player* player = team2.getPlayers()[i];
        gf::Sprite sprite(playerTextures[i]);
        sprite.setScale({player->getSize()/ playerTextures[i].getSize().x, player->getSize() / playerTextures[i].getSize().y});
        sprite.setPosition(player->getPosition());
        sprite.setRotation(player->getAngle());
        playerSprites2[player] = sprite;
    }

    // Add team players to the mainEntities
    
    
    float x = (13 % FIELD_X_TILES) * TILESIZE - TILESIZE/2; 
    float y = (FIELD_Y_TILES*TILESIZE)/2;
    Ball ball(TILESIZE/10, {x, y}, gf::Color::Rose);
    mainEntities.addEntity(ball);
    
    gf::Vector2f velocity(0.0f, 0.0f);

    Player* mainPlayer = team.getPlayers()[10]; 

    float currSeconds = 0;
    float currMinutes = 0;
    
    // main loop
    
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

    bool isInMenu = true;

    gf::RoundedRectangleShape playButton({ 200.0f, 50.0f }, 10.0f); // Coins arrondis de 10px
    playButton.setPosition({ 300.0f, 325.0f });
    playButton.setColor(gf::Color::White);
    playButton.setOutlineThickness(3.0f); // Contour noir pour améliorer la visibilité
    playButton.setOutlineColor(gf::Color::Black);
    
    gf::RoundedRectangleShape quitButton({ 200.0f, 50.0f }, 10.0f);
    quitButton.setPosition({ 300.0f, 440.0f });
    quitButton.setColor(gf::Color::White);
    quitButton.setOutlineThickness(3.0f);
    quitButton.setOutlineColor(gf::Color::Black);
    
    gf::Font font("../data/ClearSans-Bold.ttf");
    
    gf::Text playText("Play", font, 50);
    playText.setPosition({ 340.0f, 365.0f });
    
    gf::Text quitText("Quit", font, 50);
    quitText.setPosition({ 340.0f, 480.0f });
    
    gf::Texture backgroundTexture("../assets/startmenu.png");
    gf::Sprite backgroundSprite(backgroundTexture);
    gf::Vector2f textureSize = backgroundTexture.getSize();
    
    float scaleX = windowSize.x / textureSize.x;
    float scaleY = windowSize.y / textureSize.y;
    float scale = std::max(scaleX, scaleY);
    backgroundSprite.setScale({ scale, scale });
    
    backgroundSprite.setPosition({ (windowSize.x - textureSize.x * scale) / 2,
        (windowSize.y - textureSize.y * scale) / 2 });
    

    while (window.isOpen()) {
        gf::Event event;
        while (window.pollEvent(event)) {
            actions.processEvent(event);
            views.processEvent(event);
    
            if (closeWindowAction.isActive()) {
                window.close();
            }
    
            if (fullscreenAction.isActive()) {
                fullscreen = !fullscreen;
                window.setFullscreen(fullscreen);
            }
    
            if (event.type == gf::EventType::MouseButtonPressed) {
                gf::Vector2f mousePos = {static_cast<float>(event.mouseButton.coords.x),
                                         static_cast<float>(event.mouseButton.coords.y)};
    
                if (isInside(playButton, mousePos)) {
                    isInMenu = false;
                }
    
                if (isInside(quitButton, mousePos) && isInMenu == true) {
                    window.close();
                }
            }
    
            if (event.type == gf::EventType::MouseMoved) {
                gf::Vector2f mousePos = {static_cast<float>(event.mouseCursor.coords.x),
                                         static_cast<float>(event.mouseCursor.coords.y)};
                
                if (isInside(playButton, mousePos)) {
                    playButton.setColor(gf::v1::Color4f(0.5f, 0.5f, 0.5f, 1.0f)); 
                } else {
                    playButton.setColor(gf::v1::Color4f(1.0f, 1.0f, 1.0f, 1.0f)); 
                }
            
                if (isInside(quitButton, mousePos)) {
                    quitButton.setColor(gf::v1::Color4f(0.5f, 0.5f, 0.5f, 1.0f)); 
                } else {
                    quitButton.setColor(gf::v1::Color4f(1.0f, 1.0f, 1.0f, 1.0f)); 
                }
            }            
        }
    
        if (isInMenu) {
            renderer.clear();
            renderer.draw(backgroundSprite); 
            renderer.draw(playButton);
            renderer.draw(quitButton);
            renderer.draw(playText);
            renderer.draw(quitText);
            renderer.display();
            continue;
        } else {
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

                for (Player* other : team2.getPlayers()) {
                    if (player != other) {
                        player->handleCollision(*other);
                    }
                }
            }
            for (Player* player : team2.getPlayers()) {
                if (ball.isTouchingPlayer(*player)) {
                    ball.lockTo(player);
                }

                for (Player* other : team.getPlayers()) {
                    if (player != other) {
                        player->handleCollision(*other);
                    }
                }

                for (Player* other : team2.getPlayers()) {
                    if (player != other) {
                        player->handleCollision(*other);
                    }
                }
            }

            if (ball.isLockedTo(mainPlayer)) {
                mainPlayer->reduceSpeed();
                if (sprintAction.isActive()) {
                    ball.unlock();
                    ball.setVelocity(mainPlayer->getSelfPassVelocity());
                }
                if (passAction.isActive()) {
                    ball.unlock();
                    ball.setVelocity(mainPlayer->getPassVelocity(team.getPlayers()));
                }

                if (shootAction.isActive()) {
                    ball.unlock();
                    ball.setVelocity(mainPlayer->getShootVelocity(ball.getSize(), team.getPlayers()));
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
                for (Player* player : team2.getPlayers()) {
                    if (player != mainPlayer && mainPlayer->collidesWith(*player)) {
                        player->freeze(1.0f);
                    }
                }
            }

            if (sprintAction.isActive()) {
                mainPlayer->startSprint();
            } else {
                mainPlayer->stopSprint();
            }

            if (switchAction.isActive() && !mainPlayer->isTackling() && !ball.isLockedTo(mainPlayer)) {
                Player* closestPlayer = team.getClosestPlayerToBall(ball);
                if (closestPlayer) {
                    mainPlayer = closestPlayer;
                }
            }

            actions.reset();

            gf::Time dt = clock.restart();
            mainPlayer->setVelocity(velocity);

            for (auto& [player, sprite] : playerSprites) {
                sprite.setPosition(player->getPosition());
                sprite.setRotation(player->getAngle());
            }

            for (auto& [player, sprite] : playerSprites2) {
                sprite.setPosition(player->getPosition());
                sprite.setRotation(player->getAngle());
            }

            gf::Vector2f mainPlayerPosition = mainPlayer->getPosition();
            gf::Vector2f ballPosition = ball.getPosition();

            if (ball.isLockedTo(mainPlayer)) {
                ball.setVelocity(velocity);
            } else {
                ball.setVelocity(ball.getVelocity());
            }

            // team.moveTeam(ball, mainPlayer);
            team2.moveTeam(ball, mainPlayer);

            ball.update(dt.asSeconds());
            team.update(dt.asSeconds());
            team2.update(dt.asSeconds());

            int out = ball.isOutOfField(TILESIZE);

            checkOut(out, team.getPlayers(), ball.getLastTouchedBy(), &team, &team2, &ball);

            if (ball.isLockedTo(mainPlayer) || ball.getLastTouchedBy() == nullptr) {
                view.setCenter(mainPlayer->getPosition());
            }else {
                view.setCenter(ball.getPosition());
            }

            if (!out) {
                currSeconds += dt.asSeconds();
        
                if (currSeconds >= BASETIMEOFMINUTE) {
                    ++currMinutes;
                    currSeconds = 0;
                }

                if (currMinutes >= 90) {
                    endMatch(team.getGoals(), team2.getGoals());
                }
            }

            //Minimap minimap(field, 0.2f); // 0.2f is the scale for the minimap
            //minimap.setPosition(10.0f, 10.0f); // Position in the top-left corner
            
            // Main loop (rendering section)
            renderer.clear();

            // Set the main view for the field and game entities
            renderer.setView(view);

            // Render the field
            renderer.draw(field.getTileLayer(), gf::RenderStates());

            // Render the players
            for (auto& [player, sprite] : playerSprites) {
                player->render(renderer, player == mainPlayer);
            }

            for (auto& [player, sprite] : playerSprites2) {
                player->render(renderer, false);
            }

            // Render the ball
            ball.render(renderer);


            // Render the minimap
            //minimap.render(renderer);

            // Return to the main game view
            renderer.setView(view);

            // Display everything
            renderer.display();
        }
    }

    return 0;
}
