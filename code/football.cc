// #include <iostream>
// #include <string>
// #include <cmath>
// #include <vector>
// #include "classes.h"


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

#include "config.h"

#define FOOTBALL_DATA_DIR "@FOOTBALL_DATA_DIR@"

static constexpr float Speed = 60.0f;

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
  leftAction.addScancodeKeyControl(gf::Scancode::Q);
  leftAction.addScancodeKeyControl(gf::Scancode::Left);
  actions.addAction(leftAction);

  gf::Action rightAction("Right");
  rightAction.addScancodeKeyControl(gf::Scancode::D);
  rightAction.addScancodeKeyControl(gf::Scancode::Right);
  actions.addAction(rightAction);

  gf::Action upAction("Up");
  upAction.addScancodeKeyControl(gf::Scancode::W);
  upAction.addScancodeKeyControl(gf::Scancode::Z);
  upAction.addScancodeKeyControl(gf::Scancode::Up);
  actions.addAction(upAction);

  gf::Action downAction("Down");
  downAction.addScancodeKeyControl(gf::Scancode::S);
  downAction.addScancodeKeyControl(gf::Scancode::Down);
  actions.addAction(downAction);

  // add entities

  gf::EntityContainer mainEntities;

  // main loop

  renderer.clear(gf::Color::fromRgba32(0x7CFC00FF));

  gf::Clock clock;
  bool fullscreen = false;


  gf::RectangleShape player({20.0f, 20.0f});
  player.setColor(gf::Color::Azure);
  player.setPosition({ 0.0f, 20.0f });
  player.setOutlineThickness(0.5f);
  player.setOutlineColor(gf::Color::darker(gf::Color::Azure));

  gf::CircleShape ball(10.0f);
  ball.setPosition({ 200.0f, 20.0f });
  ball.setColor(gf::Color::Rose);
  ball.setOutlineThickness(1.0f);
  ball.setOutlineColor(gf::Color::lighter(gf::Color::Rose));

  bool isMoving = false;
  gf::Vector2f velocity(0.0f, 0.0f);

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
      isMoving = false;
      velocity.x = 0;
    }else {
      if (leftAction.isActive()) {
        isMoving = true;
        velocity.x = -Speed;
      } else if (rightAction.isActive()) {
        isMoving = true;
        velocity.x = Speed;
      }
    }

    if ((upAction.isActive() && downAction.isActive()) || (!upAction.isActive() && !downAction.isActive())) {
      isMoving = isMoving;
      velocity.y = 0;
    }else {
      if (upAction.isActive()) {
        isMoving = true;
        velocity.y = -Speed;
      } else if (downAction.isActive()) {
        isMoving = true;
        velocity.y = Speed;
      }
    }

    // update
    gf::Time time = clock.restart();
    gf::Vector2f oldPlayerPosition = player.getPosition();
    if (isMoving) {
      player.setPosition({oldPlayerPosition.x + (velocity.x * time.asSeconds()), oldPlayerPosition.y + (velocity.y * time.asSeconds())});
      
      gf::Vector2f playerPosition = player.getPosition();
      gf::Vector2f ballPosition = ball.getPosition();
      
      float playerLeft = playerPosition.x;
      float playerRight = playerPosition.x + player.getSize().x;
      float playerTop = playerPosition.y;
      float playerBottom = playerPosition.y + player.getSize().y;

      float ballLeft = ballPosition.x;
      float ballRight = ballPosition.x + ball.getRadius() * 2; // Diamètre
      float ballTop = ballPosition.y;
      float ballBottom = ballPosition.y + ball.getRadius() * 2; // Diamètre

      bool isColliding = (playerRight > ballLeft && playerLeft < ballRight &&
                          playerBottom > ballTop && playerTop < ballBottom);

      if (isColliding) {
        ball.setPosition({ballPosition.x + (velocity.x * time.asSeconds()), ballPosition.y + (velocity.y * time.asSeconds())});
      }
    }

    // render
    renderer.clear();
    renderer.draw(player);
    renderer.draw(ball);
    renderer.display();

  }

  return 0;
}


