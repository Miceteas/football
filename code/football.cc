#include <cassert>
#include <cstdio>
#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#include <random>
#include <gf/Action.h>
#include <gf/Clock.h>
#include <gf/Color.h>
#include <gf/Event.h>
#include <gf/Log.h>
#include <gf/RenderWindow.h>
#include <gf/Views.h>
#include <gf/Window.h>
#include <gf/Shapes.h>
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
#include "team.h"
#include "field.h"

enum class GameState {
    START,
    PLAYING,
    WAITING,
    END
};

GameState currState = GameState::START;

void touch(bool isTop, Team *teamHand, Team *teamRecieve, Ball *ball) {
    gf::Vector2f posBall;
    gf::Vector2f posPlayer;
    float angle = 0;
    posBall = ball->getPosition();
    posPlayer = ball->getPosition();

    if (isTop) {
        posBall.y = 10;
        posPlayer.y = 0;
        angle = -M_PI / 2.0f;
    } else {
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
        std::cout << "YOU LOST :( " << playerGoals << " - " << aiGoals << std::flush;
    } else if (playerGoals == aiGoals) {
        std::cout << "TIE ! " << playerGoals << " - " << aiGoals << std::flush;
    } else {
        std::cout << "YOU WIN GOOD JOB ! " << playerGoals << " - " << aiGoals << std::flush;
    }
    currState = GameState::END;
}

bool isInside(const gf::RoundedRectangleShape& button, const gf::Vector2f& mousePos) {
    gf::Vector2f pos = button.getPosition();
    gf::Vector2f size = button.getSize();
    return (mousePos.x >= pos.x && mousePos.x <= pos.x + size.x &&
            mousePos.y >= pos.y && mousePos.y <= pos.y + size.y);
}

bool isTooFarFromBall(const Player& player, const Ball& ball) {
    gf::Vector2f playerPos = player.getPosition();
    gf::Vector2f ballPos = ball.getPosition();
    
    float distance = gf::euclideanDistance(playerPos, ballPos);
    
    return distance > MAX_DISTANCE_FROM_BALL;
}

int main() {
    static constexpr gf::Vector2u ScreenSize(800, 800);

    gf::Log::setLevel(gf::Log::Info);

    gf::Window window("gf football", ScreenSize, ~gf::WindowHints::Resizable);
    window.setVerticalSyncEnabled(true);
    window.setFramerateLimit(60);

    gf::RenderWindow renderer(window);

    gf::ExtendView view({0.0f, 20.0f}, {float(window.getSize().x), float(window.getSize().y)});

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

    gf::Action pauseAction("Pause");
    pauseAction.addScancodeKeyControl(gf::Scancode::P); 
    actions.addAction(pauseAction);

    gf::Clock clock;

    float x = (13 % FIELD_X_TILES) * TILESIZE - TILESIZE/2; 
    float y = (FIELD_Y_TILES*TILESIZE)/2;
    Ball ball(TILESIZE/10, {x, y}, gf::Color::Rose);    

    Team team("Team A", gf::Color::Azure);
    team.initPlayers();

    Team team2("Team B", gf::Color::Chartreuse);
    team2.initPlayers();
    team2.setupSide(false);

    gf::Time dt;

    std::vector<gf::Texture> teamTextures;
    std::vector<gf::Texture> enemyTextures;
    gf::Texture ballTexture("../assets/PNG/Equipment/ball_soccer2.png");
    gf::Sprite ballSprite (ballTexture);
    ballSprite.setAnchor(gf::Anchor::Center);
    ballSprite.setScale({ball.getSize() / ballTexture.getSize().x, ball.getSize() / ballTexture.getSize().y});
    srand(time(NULL));

    for (int i = 1; i <= 11; ++i) {
        int randN = (rand() % 5) + 1;
        gf::Texture teamTexture("../assets/PNG/Blue/characterBlue (" + std::to_string(randN) + ").png");
        teamTextures.push_back(std::move(teamTexture));
        gf::Texture enemyTexture("../assets/PNG/Green/characterGreen (" + std::to_string(randN) + ").png");
        enemyTextures.push_back(std::move(enemyTexture));
    }

    // default setup
    team.setupPlayers(1);
    team2.setupPlayers(0);

    std::unordered_map<Player*, gf::Sprite> playerSprites;
    std::unordered_map<Player*, gf::Sprite> playerSprites2;

    for (size_t i = 0; i < team.getPlayers().size(); ++i) {
        Player* player = team.getPlayers()[i];
        gf::Sprite sprite(teamTextures[i]);
        sprite.setAnchor(gf::Anchor::Center);
        sprite.setScale({player->getSize() / teamTextures[i].getSize().x, player->getSize() / teamTextures[i].getSize().y});
        playerSprites[player] = sprite;
    }

    for (size_t i = 0; i < team2.getPlayers().size(); ++i) {
        Player* player = team2.getPlayers()[i];
        gf::Sprite sprite(enemyTextures[i]);
        sprite.setAnchor(gf::Anchor::Center);
        sprite.setScale({player->getSize()/ enemyTextures[i].getSize().x, player->getSize() / enemyTextures[i].getSize().y});
        playerSprites2[player] = sprite;
    }    
    
    gf::Vector2f velocity(0.0f, 0.0f);

    Player* mainPlayer = team.getPlayers()[10]; 

    float currSeconds = 0;
    int currMinutes = 0;
    
    
    std::vector<int> tileOrder = {
        5,  2,  2,  2,   2,  2,  2,  2,  2,  2,  2,  2,  3,  2,  2,  2,  2,  2,  2,  2,   2,  2,  2,  2,  6,
        14,  0,  0,  0,   0,  0,  0,  0,  0,  0,  0,  0, 16,  0,  0,  0,  0,  0,  0,  0,   0,  0,  0,  0, 17,
        14,  0,  0,  0,   0,  0,  0,  0,  0,  0,  0,  0, 16,  0,  0,  0,  0,  0,  0,  0,   0,  0,  0,  0, 17,
        27, 29, 29, 37,   0,  0,  0,  0,  0,  0,  0,  0, 16,  0,  0,  0,  0,  0,  0,  0,   0, 36, 29, 29, 30,
        14,  0,  0, 17,  99,  0,  0,  0,  0,  0,  0,  0, 16,  0,  0,  0,  0,  0,  0,  0,  95, 14,  0,  0, 17,
        14,  0,  0, 17, 112,  0,  0,  0,  0,  0,  0,  7, 46,  9,  0,  0,  0,  0,  0,  0, 108, 14,  0,  0, 17,
        14,  0, 13, 17, 125,  0,  0,  0,  0,  0,  0, 20, 15, 22,  0,  0,  0,  0,  0,  0, 121, 14,  0, 13, 17,
        14,  0,  0, 17, 138,  0,  0,  0,  0,  0,  0, 33, 48, 35,  0,  0,  0,  0,  0,  0, 134, 14,  0,  0, 17,
        14,  0,  0, 17, 151,  0,  0,  0,  0,  0,  0,  0, 16,  0,  0,  0,  0,  0,  0,  0, 147, 14,  0,  0, 17,
        27, 29, 29, 50,   0,  0,  0,  0,  0,  0,  0,  0, 16,  0,  0,  0,  0,  0,  0,  0,   0, 49, 29, 29, 30,
        14,  0,  0,  0,   0,  0,  0,  0,  0,  0,  0,  0, 16,  0,  0,  0,  0,  0,  0,  0,   0,  0,  0,  0, 17,
        14,  0,  0,  0,   0,  0,  0,  0,  0,  0,  0,  0, 16,  0,  0,  0,  0,  0,  0,  0,   0,  0,  0,  0, 17,
        18, 41, 41, 41,  41, 41, 41, 41, 41, 41, 41, 41, 42, 41, 41, 41, 41, 41, 41, 41,  41, 41, 41, 41, 19
    };  
    
    FootballField field("../assets/Tilesheet/groundGrass.png", tileOrder, FIELD_X_TILES, FIELD_Y_TILES, TILESIZE);
    
    gf::RoundedRectangleShape playButton({ 200.0f, 50.0f }, 10.0f);
    playButton.setPosition({ 300.0f, 325.0f });
    playButton.setColor(gf::Color::White);
    playButton.setOutlineThickness(3.0f);
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

    gf::Text scoreText("0 - 0", font, 50);
    scoreText.setAnchor(gf::Anchor::TopCenter);
    scoreText.setColor(gf::Color::White);

    gf::Text timeText("00:00", font, 50);
    timeText.setAnchor(gf::Anchor::BottomCenter);
    timeText.setColor(gf::Color::White);

    
    gf::Texture backgroundTexture("../assets/startmenu.png");
    gf::Sprite backgroundSprite(backgroundTexture);
    gf::Vector2f textureSize = backgroundTexture.getSize();
    
    float scaleX = ScreenSize.x / textureSize.x;
    float scaleY = ScreenSize.y / textureSize.y;
    float scale = std::max(scaleX, scaleY);
    backgroundSprite.setScale({ scale, scale });
    
    backgroundSprite.setPosition({ (ScreenSize.x - textureSize.x * scale) / 2,
        (ScreenSize.y - textureSize.y * scale) / 2 });
        
    // main loop
    
    while (window.isOpen()) {
        gf::Event event;
        while (window.pollEvent(event)) {
            actions.processEvent(event);
    
            if (event.type == gf::EventType::MouseButtonPressed) {
                gf::Vector2f mousePos = {static_cast<float>(event.mouseButton.coords.x),
                                         static_cast<float>(event.mouseButton.coords.y)};
    
                if (currState == GameState::START) {
                    if (isInside(playButton, mousePos)) {
                        currState = GameState::PLAYING;
                    }
        
                    if (isInside(quitButton, mousePos)) {
                        window.close();
                    }
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

        if (closeWindowAction.isActive()) {
            window.close();
        }

        if (fullscreenAction.isActive()) {
            window.toggleFullscreen();
        }

        if (pauseAction.isActive()) {
            if (currState == GameState::PLAYING) {
                currState = GameState::WAITING;
            } else if (currState == GameState::WAITING) {
                currState = GameState::PLAYING;
            }
        }
    
        if (currState == GameState::START) {
            renderer.clear();
            renderer.draw(backgroundSprite); 
            renderer.draw(playButton);
            renderer.draw(quitButton);
            renderer.draw(playText);
            renderer.draw(quitText);
            renderer.display();
            dt = clock.restart();
            actions.reset();
            continue;
        } else if (currState == GameState::PLAYING) {
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

            if (ball.getLockedTo()) {
                ball.getLockedTo()->reduceSpeed();
            }

            if (ball.isLockedTo(mainPlayer)) {
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
    
            if (isTooFarFromBall(*mainPlayer, ball)) {
                mainPlayer = team.getClosestPlayerToBall(ball);
            }

            actions.reset();

            dt = clock.restart();
            mainPlayer->setVelocity(velocity);

            for (auto& [player, sprite] : playerSprites) {
                sprite.setPosition(player->getPosition());
                sprite.setRotation(player->getAngle());
            }

            for (auto& [player, sprite] : playerSprites2) {
                sprite.setPosition(player->getPosition());
                sprite.setRotation(player->getAngle());
            }

            ballSprite.setPosition(ball.getPosition());

            if (ball.isLockedTo(mainPlayer)) {
                ball.setVelocity(velocity);
            } else {
                ball.setVelocity(ball.getVelocity());
            }

            team.moveTeam(ball, mainPlayer);
            team2.moveTeam(ball, mainPlayer);

            ball.update(dt.asSeconds());
            team.update(dt.asSeconds());
            team2.update(dt.asSeconds());

            int out = ball.isOutOfField(TILESIZE);

            checkOut(out, team.getPlayers(), ball.getLastTouchedBy(), &team, &team2, &ball);

            if (ball.isLockedTo(mainPlayer) || !ball.getLastTouchedBy()) {
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

            scoreText.setString(std::to_string(team.getGoals()) + " - " + std::to_string(team2.getGoals()));
            scoreText.setPosition({view.getCenter().x, view.getCenter().y - 400.0f});

            timeText.setString(std::to_string(currMinutes) + ":" + std::to_string(int(currSeconds * 60 / BASETIMEOFMINUTE)));
            timeText.setPosition({view.getCenter().x, view.getCenter().y + 400.0f});
            
            // Main loop (rendering section)
            renderer.clear();

            renderer.setView(view);

            renderer.draw(field.getTileLayer(), gf::RenderStates());

            for (auto& [player, sprite] : playerSprites) {
                renderer.draw(sprite);
                if (player == mainPlayer) player->renderMain(renderer);
            }

            for (auto& [player, sprite] : playerSprites2) {
                renderer.draw(sprite);
            }

            renderer.draw(ballSprite);

            renderer.draw(scoreText);
            renderer.draw(timeText);

            renderer.display();
        }else {
            dt = clock.restart();
            actions.reset();
        }
    }

    return 0;
}
