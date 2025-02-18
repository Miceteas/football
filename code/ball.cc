#include "ball.h"

#define BASECOOLDOWN 0.2

Ball::Ball(float size, gf::Vector2f position, gf::Color4f color)
: m_velocity({0, 0})
, m_size(size)
, m_position(position)
, m_color(color)
, belongsTo(nullptr)
, m_cooldown(0.0f)
, lastTouchedBy(nullptr)
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

    if (belongsTo != nullptr) {
        gf::Vector2f playerVelocity = belongsTo->getVelocity();
        gf::Vector2f playerPosition = belongsTo->getPosition();
        gf::Vector2f offset(cos(belongsTo->getAngle()) * belongsTo->getSize(), sin(belongsTo->getAngle()) * belongsTo->getSize());

        m_position = playerPosition + offset;
        m_velocity = playerVelocity;
        return;
    }

    if (speed > 0.001f) {
        float deceleration = belongsTo == nullptr ? (friction * dt) : 0;
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
        belongsTo = p;
        lastTouchedBy = p;
        m_cooldown = BASECOOLDOWN;
    } 
}

void Ball::unlock() {
    belongsTo = nullptr;
    m_cooldown = BASECOOLDOWN;
}

bool Ball::isLockedTo(Player *p) const {
    return belongsTo == p;
}

gf::Vector2f Ball::getVelocity() const {
    return m_velocity;
}

void Ball::render(gf::RenderTarget& target) {
    gf::CircleShape shape(m_size);
    shape.setPosition(m_position);
    shape.setColor(m_color);
    shape.setAnchor(gf::Anchor::Center);
    shape.setOutlineThickness(0.5f);
    shape.setOutlineColor(gf::Color::lighter(m_color));
    target.draw(shape);
}

Player *Ball::getLastTouchedBy() {
    return lastTouchedBy;
}

Player *Ball::getLockedTo() {
    return belongsTo;
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
    float distance = std::sqrt(std::pow(m_position.x - player.getPosition().x, 2) +
                               std::pow(m_position.y - player.getPosition().y, 2));
    return distance < (m_size + player.getSize()) / 2.0f;
}