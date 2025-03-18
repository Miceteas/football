#ifndef PLAYER_H
#define PLAYER_H

#include <gf/Entity.h>
#include <gf/Vector.h>
#include <gf/RenderTarget.h>
#include <gf/Color.h>
#include <gf/Shapes.h>
#include "role.h"
#include "side.h"
#include "settings.h"

class Ball;

class Player : public gf::Entity {
  private:
    float m_stamina;
    gf::Vector2f m_velocity;
    float m_size;
    gf::Vector2f m_position;
    Role m_role;
    Side m_side;
    gf::Color4f m_color;
    float m_angle;
    float m_freezeTime;
    bool m_isTackling;
    float m_slideDistance;
    float m_tackleSpeed;
    float m_tackleAngle;
    gf::Vector2f m_tackleVelocity;
    bool m_isSprinting;
    bool m_reduceSpeed;

    void setAim(std::vector<Player *> teamPlayersVec, bool left);
    float calcAngle(gf::Vector2f velocity);

  public:
    Player(float stamina, float size, gf::Vector2f position, Role role, Side side, gf::Color4f color, float angle);

    float getSize() const;
    gf::Vector2f getPosition() const;
    gf::Vector2f getVelocity() const;

    void setPosition(gf::Vector2f position);
    void setVelocity(gf::Vector2f velocity);
    void update(float dt);
    void renderMain(gf::RenderTarget& target);
    float getAngle();
    void setAngle(float angle);
    gf::Vector2f getSelfPassVelocity();
    gf::Vector2f getPassVelocity(std::vector<Player *> players);
    gf::Vector2f getShootVelocity(float ballSize, std::vector<Player *> teamPlayersVec);

    void setTackleData(float speed, float angle);
    bool isTackling() const;
    bool collidesWith(const Player& other) const;
    void freeze(float duration);
    void handleCollision(Player& other);
    void startSprint();
    void stopSprint();
    void AImove(Ball& ball, bool left, bool isBallMemberOfTeam, std::vector<Player *> teamPlayersVec);
    void reduceSpeed();
    void moveTo(gf::Vector2f position);
  
};

#endif
