#include "ball.h"
#include "player.h"

Ball::Ball(float size, gf::Vector2f position, gf::Color4f color)
: m_velocity({0, 0})
, m_size(size)
, m_position(position)
, m_color(color)
, m_belongsTo(nullptr)
, m_lastTouchedBy(nullptr)
, m_cooldown(0.0f)
{
    
}

gf::Vector2f Ball::getPosition() const {
    return m_position;
}

float Ball::getSize() const {
    return m_size;
}

void Ball::setVelocity(gf::Vector2f velocity) {
    m_velocity = velocity;
}

void Ball::update(float dt) {
    if (m_cooldown > 0.0f) {
        m_cooldown -= dt;
    }
    static constexpr float friction = 150.0f;
    float speed = std::sqrt(m_velocity.x * m_velocity.x + m_velocity.y * m_velocity.y);

    if (m_belongsTo != nullptr) {
        gf::Vector2f playerVelocity = m_belongsTo->getVelocity();
        gf::Vector2f playerPosition = m_belongsTo->getPosition();
        gf::Vector2f offset(cos(m_belongsTo->getAngle()) * m_belongsTo->getSize(), sin(m_belongsTo->getAngle()) * m_belongsTo->getSize());

        m_position = playerPosition + offset;
        m_velocity = playerVelocity;
        return;
    }

    if (speed > 0.001f) {
        float deceleration = m_belongsTo == nullptr ? (friction * dt) : 0;
        speed = std::max(speed - deceleration, 0.0f);

        if (speed > 0.0f) {
            m_velocity = m_velocity / std::sqrt(m_velocity.x * m_velocity.x + m_velocity.y * m_velocity.y) * speed;
        } else {
            m_velocity = {0.0f, 0.0f};
        }
    } else {
        m_velocity = {0.0f, 0.0f};
    }

    m_position += m_velocity * dt;
}

void Ball::lockTo(Player *p) {
    if (m_cooldown <= 0) {
        m_belongsTo = p;
        m_lastTouchedBy = p;
        m_cooldown = BASECOOLDOWN;
    } 
}

void Ball::unlock() {
    m_belongsTo = nullptr;
    m_cooldown = BASECOOLDOWN;
}

bool Ball::isLockedTo(Player *p) const {
    return m_belongsTo == p;
}

gf::Vector2f Ball::getVelocity() const {
    return m_velocity;
}

Player *Ball::getLastTouchedBy() {
    return m_lastTouchedBy;
}

Player *Ball::getLockedTo() {
    return m_belongsTo;
}

int Ball::isOutOfField(int tileSize) {
    //Because there is a little offset on the sides of the field of 8 pixels / 64 pixels in the tilesheet
    int offset = tileSize / 8;
    if (m_position.x + m_size < offset) {
        if (m_position.y + m_size < TOPPOLE) {
            //Corner on topleft
            return 1;
        }else if (m_position.y - m_size > BOTTOMPOLE) {
            //Corner on bottomleft
            return 3;
        }else {
            //Goal against left team
            return 2;
        }
    }else if (m_position.x - m_size > FIELDXSIZE - offset) {
        if (m_position.y + m_size < TOPPOLE) {
            //Corner on topright
            return 6;
        }else if (m_position.y - m_size > BOTTOMPOLE) {
            //Corner on bottomright
            return 8;
        }else {
            //Goal against right team
            return 7;
        }
    }else {
        if (m_position.y + m_size < offset) {
            //Touch on top side
            return 4;
        }else if (m_position.y - m_size > FIELDYSIZE - offset) {
            //Touch on bottom side
            return 5;
        }else {
            //Not out of field
            return 0;
        }
    }
}

bool Ball::isTouchingPlayer(const Player& player) const {
    float distance = gf::euclideanDistance(m_position, player.getPosition());
    return distance < (m_size + player.getSize()) / 2.0f;
}

void Ball::moveTo(gf::Vector2f position) {
    m_position = position;
    m_velocity = gf::Vector2f(0.0f, 0.0f);
}