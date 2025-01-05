#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#include "classes.h"


float calculateDistance(float x, float y, float x2, float y2){
    return sqrt((x2 - x) * (x2 - x) + (y2 - y) * (y2 - y));
}

Player::Player(float stamina, float speed, float x, float y, Role role){
    this->stamina = stamina;
    this->speed = speed;
    this->x = x;
    this->y = y;
    this->role = role;
}

float Player::getStamina(){
    return this->stamina;
}

float Player::getSpeed(){
    return this->speed;
}

float Player::getX(){
    return this->x;
}

float Player::getY(){
    return this->y;
}

Role Player::getRole(){
    return this->role;
}

void Player::setStamina(float stamina){
    this->stamina = stamina;
}

void Player::setSpeed(float speed){
    this->speed = speed;
}

void Player::setX(float x){
    this->x = x;
}

void Player::setY(float y){
    this->y = y;
}

void Player::setRole(Role role){
    this->role = role;
}

void Player::move(float dx, float dy){
    this->x += dx;
    this->y += dy;
}

void Player::display(){}

