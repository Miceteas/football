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
    
    float m_freezeTime;
    bool m_isTackling;
    float m_slideDistance;
    float m_tackleSpeed;
    float m_tackleAngle;
    gf::Vector2f m_tackleVelocity;
    bool collidesWith(const Player& other) const;
    void tackle();
    void freeze(float duration);
    bool isTackling() const;
    void setTackleData(float speed, float angle);

    //TEMPORARY
    void changeColor(gf::Color4f color);
};

#endif