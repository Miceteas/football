#ifndef BALL_H
#define BALL_H

#include <gf/Entity.h>
#include <gf/Vector.h>
#include <gf/RenderTarget.h>
#include <gf/Color.h>
#include <gf/Shapes.h>
#include "player.h"
#include "config.h"
#include "settings.h"

class Player;

class Ball : public gf::Entity {
  private:
    gf::Vector2f m_velocity;
    float m_size;
    gf::Vector2f m_position;
    gf::Color4f m_color;
    Player *m_belongsTo;
    Player *m_lastTouchedBy;
    bool m_isLocked; 
    float m_cooldown;

  public:
    Ball(float size, gf::Vector2f position, gf::Color4f color);

    gf::Vector2f getPosition() const;
    gf::Vector2f getVelocity() const; // Déplacé ici pour être public
    float getSize() const;

    void setVelocity(gf::Vector2f velocity);
    void update(float dt);
    void lockTo(Player *p);
    void unlock();
    bool isLockedTo(Player *p) const;
    void render(gf::RenderTarget& target);
    int isOutOfField(int tileSize);
    Player *getLastTouchedBy();
    Player *getLockedTo();

    void moveTo(gf::Vector2f position);
    bool isTouchingPlayer(const Player& player) const;
};

#endif
