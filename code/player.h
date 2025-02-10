#ifndef PLAYER_H
#define PLAYER_H

#include <gf/Entity.h>
#include <gf/Vector.h>
#include <gf/RenderTarget.h>
#include <gf/Color.h>
#include <gf/Shapes.h>
#include "role.h"
#include "ball.h"
#include "config.h"

class Ball;

class Player : public gf::Entity {
  private:
    float m_stamina;
    gf::Vector2f m_velocity;
    float m_size;
    gf::Vector2f m_position;
    Role m_role;
    gf::Color4f m_color;
    float m_angle;
    float m_freezeTime;
    bool m_isTackling;
    float m_slideDistance;
    float m_tackleSpeed;
    float m_tackleAngle;
    gf::Vector2f m_tackleVelocity;
    bool m_isSprinting;

    float calcAngle(gf::Vector2f velocity);

  public:
    Player(float stamina, float size, gf::Vector2f position, Role role, gf::Color4f color, float angle);

    float getSize() const;
    gf::Vector2f getPosition() const;
    gf::Vector2f getVelocity() const;

    void setPosition(gf::Vector2f position);
    void setVelocity(gf::Vector2f velocity);
    void update(float dt);
    void render(gf::RenderTarget& target, bool isMainPlayer);
    float getAngle();
    gf::Vector2f getPassVelocity(std::vector<Player *> players);
    gf::Vector2f getShootVelocity(float ballSize, std::vector<Player *> teamPlayersVec);

    void setTackleData(float speed, float angle);
    bool isTackling() const;
    bool collidesWith(const Player& other) const;
    void freeze(float duration);
    void handleCollision(Player& other);
    void startSprint();
    void stopSprint();
    void AImove(const Ball& ball, bool left, bool isBallMemberOfTeam);
  
    //TEMPORARY 
    void changeColor(gf::Color4f color);
};

#endif
