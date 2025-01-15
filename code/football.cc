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

#include "config.h"
#include "ball.h"
#include "player.h"
#include "role.h"
#include "team.h"

#define FOOTBALL_DATA_DIR "@FOOTBALL_DATA_DIR@"

#define BALLSIZE 10
#define PLAYERSIZE 30
#define FIELDXSIZE 3150
#define FIELDYSIZE 2040
#define LEFTPOLE 910
#define RIGHTPOLE 1130

// constants
static constexpr float SPEED = 120.0f;

void touch(bool isLeft, bool forPlayerTeam) {
  //Do something for a touch
}

void corner(int side, bool forPlayerTeam) {
  //Do something for a corner (0 = topleft, 1 = topright, 2 = bottomleft, 3 = bottomright)
}

void goal(bool isPlayerTeam) {
  //Do something for a goal
}

void checkOut(int isOut, std::vector<Player *> vec, Player *lastTouchedBy) {
  if (isOut != 0) {
      bool isMemberOfTeam = count(vec.begin(), vec.end(), lastTouchedBy) > 0;
      switch (isOut) {
        case 1 : 
          corner(0, isMemberOfTeam);
          break;
        case 2 : 
          goal(true);
          break;
        case 3 : 
          corner(1, isMemberOfTeam);
          break;
        case 4 : 
          touch(true, isMemberOfTeam);
          break;
        case 5 : 
          touch(false, isMemberOfTeam);
          break;
        case 6 :
          corner(2, isMemberOfTeam);
          break;
        case 7 : 
          goal(false);
          break;
        case 8 :
          corner(3, isMemberOfTeam);
          break;
      }
    }
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
  shootAction.addScancodeKeyControl(gf::Scancode::B); 
  actions.addAction(shootAction);

  // add entities
  gf::EntityContainer mainEntities;

  gf::Texture grassTexture("../assets/Tilesheet/groundGrass.png");
  gf::Sprite sprite;

  sprite.setTexture(grassTexture);

  gf::Vector2f scale(
    float(window.getSize().x) / grassTexture.getSize().x,
    float(window.getSize().y) / grassTexture.getSize().y
  );
  
  sprite.setScale(scale);
  sprite.setPosition({0.0f, float(window.getSize().x)});

  //renderer.draw(sprite);

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
  team.setupPlayers(window.getSize().x,window.getSize().y);

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
  
  

  Ball ball(BALLSIZE, {200.0f, 20.0f}, gf::Color::Rose);
  mainEntities.addEntity(ball);
  
  gf::Vector2f velocity(0.0f, 0.0f);

  Player* mainPlayer = team.getPlayers()[10]; 

  // main loop
  bool cam1 = true;
  while (window.isOpen()) {
    // input
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

    if ((leftAction.isActive() && rightAction.isActive()) || (!leftAction.isActive() && !rightAction.isActive())) {
      velocity.x = 0;
    } else {
      if (leftAction.isActive()) {
        velocity.x = -SPEED;
      } else if (rightAction.isActive()) {
        velocity.x = SPEED;
      }
    }

    if ((upAction.isActive() && downAction.isActive()) || (!upAction.isActive() && !downAction.isActive())) {
      velocity.y = 0;
    } else {
      if (upAction.isActive()) {
        velocity.y = -SPEED;
      } else if (downAction.isActive()) {
        velocity.y = SPEED;
      }
    }

    // update
    if (dropAction.isActive()) {
      ball.unlock();
    }

    if (ball.isLockedTo(mainPlayer) && shootAction.isActive()) {
      ball.unlock();
      ball.setVelocity(mainPlayer->getShootVelocity());
    }else if (ball.isLockedTo(mainPlayer) && passAction.isActive()) {
      ball.unlock(); 
      ball.setVelocity(mainPlayer->getPassVelocity());
    }

    if (switchAction.isActive()) {
      // Switch main player between two players => to change
      if (cam1) {
        mainPlayer = team.getPlayers()[0];  
      } else {
        mainPlayer = team.getPlayers()[10]; 
      }
      cam1 = !cam1;
    }

    actions.reset();

    gf::Time dt = clock.restart();
    mainPlayer->setVelocity(velocity);

    mainPlayer->update(dt.asSeconds());

    for (auto& [player, sprite] : playerSprites) {
      sprite.setPosition(player->getPosition());
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

    int out = ball.isOutOfField(FIELDXSIZE, FIELDYSIZE, LEFTPOLE, RIGHTPOLE);

    checkOut(out, team.getPlayers(), ball.getLastTouchedBy());

    view.setCenter(mainPlayer->getPosition());
    
    // render
    renderer.clear();
    //renderer.draw(sprite);
    renderer.setView(view);
    
    for (auto& [player, sprite] : playerSprites) {
      // renderer.draw(sprite);
      player->render(renderer);
    } 

    ball.render(renderer);


    renderer.display();
  }

  return 0;
}
