

// float calculateDistance(float x, float y, float x2, float y2){
//     return sqrt((x2 - x) * (x2 - x) + (y2 - y) * (y2 - y));
// }

// Player::Player(float stamina, float speed, float x, float y, Role role){
//     this->stamina = stamina;
//     this->speed = speed;
//     this->x = x;
//     this->y = y;
//     this->role = role;
// }

// float Player::getStamina(){
//     return this->stamina;
// }

// float Player::getSpeed(){
//     return this->speed;
// }

// float Player::getX(){
//     return this->x;
// }

// float Player::getY(){
//     return this->y;
// }

// Role Player::getRole(){
//     return this->role;
// }

// void Player::setStamina(float stamina){
//     this->stamina = stamina;
// }

// void Player::setSpeed(float speed){
//     this->speed = speed;
// }

// void Player::setX(float x){
//     this->x = x;
// }

// void Player::setY(float y){
//     this->y = y;
// }

// void Player::setRole(Role role){
//     this->role = role;
// }

// void Player::move(float dx, float dy){
//     this->x += dx;
//     this->y += dy;
// }

// void Player::display(){}

/*
 * Gamedev Framework (gf)
 * Copyright (C) 2016-2022 Julien Bernard
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */
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

#include "config.h"

#define FOOTBALL_DATA_DIR "@FOOTBALL_DATA_DIR@"

static constexpr float SPEED = 100.0f;

enum class Role {
    ATTACKER,
    DEFENDER,
    GOALKEEPER
};

class Player : public gf::Entity {
  private:
    float m_stamina; //Pas sûr pour le moment
    gf::Vector2f m_velocity;
    float m_size;
    gf::Vector2f m_position;
    Role m_role;
    gf::Color4f m_color;
  public : 
    Player(float stamina, float size, gf::Vector2f position, Role role, gf::Color4f color) 
      : m_stamina(stamina)
      , m_velocity({0,0})
      , m_size(size)
      , m_position(position)
      , m_color(color)
      , m_role(role)
    {

    }

      // Square(gf::Vector2f position, float size, gf::Color4f color)
      // : m_position(position)
      // , m_velocity(0, 0)
      // , m_size(size)
      // , m_color(color)
      // {
    
      // }

    float getSize() {
      return m_size;
    }

    gf::Vector2f getPosition() {
      return m_position;
    }

    void setVelocity(gf::Vector2f velocity) {
      m_velocity = velocity;
    }

    void update(float dt) {
      m_position += dt * m_velocity;
    }

    void render(gf::RenderTarget& target) {
      gf::RectangleShape shape({m_size, m_size});
      shape.setPosition(m_position);
      shape.setAnchor(gf::Anchor::TopLeft);
      shape.setOutlineThickness(0.5f);
      shape.setColor(m_color);
      shape.setOutlineColor(gf::Color::darker(m_color));
      target.draw(shape);
    }
};

class Ball : public gf::Entity {
  private:
    gf::Vector2f m_velocity;
    float m_size;
    gf::Vector2f m_position;
    gf::Color4f m_color;
    Player *belongsTo;
  public : 
    Ball(float size, gf::Vector2f position, gf::Color4f color)
      : m_velocity({0,0})
      , m_size(size)
      , m_position(position)
      , m_color(color)
      , belongsTo(nullptr)
    {

    }

    gf::Vector2f getPosition() {
      return m_position;
    }

    float getSize() {
      return m_size;
    }

    void setVelocity(gf::Vector2f velocity) {
      m_velocity = velocity;
    }

    void update(float dt) {
      m_position += dt * m_velocity;
    }

    void lockTo(Player *p) {
      belongsTo = p;
    }

    void unlock() {
      belongsTo = nullptr;
    }

    bool isLockedTo(Player *p) {
      return belongsTo == p;
    }

    void render(gf::RenderTarget& target) {
      gf::CircleShape shape(m_size);
      shape.setPosition(m_position);
      shape.setColor(m_color);
      shape.setAnchor(gf::Anchor::TopLeft);
      shape.setOutlineThickness(0.5f);
      shape.setOutlineColor(gf::Color::lighter(m_color));
      target.draw(shape);
    }
};

int main() {
  static constexpr gf::Vector2u ScreenSize(500, 500);

  gf::Log::setLevel(gf::Log::Info);

  // setup resource directory

  gf::ResourceManager resourceManager;
  resourceManager.addSearchDir(FOOTBALL_DATA_DIR);

  gf::Random random;

  // initialize window

  gf::Window window("gf football", ScreenSize, ~gf::WindowHints::Resizable);
  window.setVerticalSyncEnabled(true);
  window.setFramerateLimit(60);

  gf::RenderWindow renderer(window);

  // add cameras

  gf::ViewContainer views;

  gf::ExtendView view({0.0f, 20.0f}, {500.0f, 500.0f});

  views.addView(view);

  views.setInitialFramebufferSize(ScreenSize);

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

  // add entities

  gf::EntityContainer mainEntities;

  renderer.clear(gf::Color::fromRgba32(0x7CFC00FF));
  

  gf::Clock clock;
  bool fullscreen = false;

  Player player1(100.0f, 20.0f, {0.0f, 20.0f}, Role::ATTACKER, gf::Color::Azure);

  mainEntities.addEntity(player1);

  Player player2(100.0f, 20.0f, {400.0f, 20.0f}, Role::ATTACKER, gf::Color::Red);

  mainEntities.addEntity(player2);

  Ball ball(10.0f, {200.0f, 20.0f}, gf::Color::Rose);

  mainEntities.addEntity(ball);
  
  gf::Vector2f velocity(0.0f, 0.0f);

  Player *mainPlayer = &player1;

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
    }else {
      if (leftAction.isActive()) {
        velocity.x = -SPEED;
      } else if (rightAction.isActive()) {
        velocity.x = SPEED;
      }
    }

    if ((upAction.isActive() && downAction.isActive()) || (!upAction.isActive() && !downAction.isActive())) {
      velocity.y = 0;
    }else {
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

    if (switchAction.isActive()) {
      if (cam1) {
        mainPlayer = &player2;
        player1.setVelocity({0.0f, 0.0f});
      }else {
        mainPlayer = &player1;
        player2.setVelocity({0.0f, 0.0f});
      }
      cam1 = !cam1;
    }

    actions.reset();  

    gf::Time dt = clock.restart();
    mainPlayer->setVelocity(velocity);

    mainPlayer->update(dt.asSeconds());
    
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
    }else {
      ball.setVelocity({0.0f, 0.0f});
    }

    // mainEntities.update(dt);
    ball.update(dt.asSeconds());

    view.setCenter(mainPlayer->getPosition());

    // render
    renderer.clear();

    renderer.setView(view);
    ball.render(renderer);
    player1.render(renderer);
    player2.render(renderer);
    // mainEntities.render(renderer);
    renderer.display();

  }

  return 0;
}


//Pas oublier une entité pour tous les joueurs pour les 2 équipes en même temps
//