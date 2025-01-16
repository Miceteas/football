#ifndef PLAYER_H
#define PLAYER_H

#include <gf/Entity.h>
#include <gf/Vector.h>
#include <gf/RenderTarget.h>
#include <gf/Color.h>
#include <gf/Shapes.h>
#include "role.h"


class Player : public gf::Entity {
  private:
    float m_stamina;
    gf::Vector2f m_velocity;
    float m_size;
    gf::Vector2f m_position;
    Role m_role;
    gf::Color4f m_color;
    float m_angle;
    float calcAngle(gf::Vector2f velocity);
    bool isEqual(float a, float b);
    
  public:
    Player(float stamina, float size, gf::Vector2f position, Role role, gf::Color4f color, float angle);

    float getSize() const;
    gf::Vector2f getPosition() const;
    gf::Vector2f getVelocity() const;

    void setPosition(gf::Vector2f position);
    void setVelocity(gf::Vector2f velocity);
    void update(float dt);
    void render(gf::RenderTarget& target);
    float getAngle();
    gf::Vector2f getPassVelocity();
    gf::Vector2f getShootVelocity();
    /*---------------------------------*/
    void makePass(gf::Vector2f direction);

};

#endif