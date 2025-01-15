#include "ball.h"

Ball::Ball(float size, gf::Vector2f position, gf::Color4f color)
: m_velocity({0, 0})
, m_size(size)
, m_position(position)
, m_color(color)
, belongsTo(nullptr)
, m_cooldown(0)
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
/*
void Ball::update(float dt) {
    m_position += m_velocity * dt;

}*/


void Ball::update(float dt) {
    --m_cooldown;
    static constexpr float friction = 150.0f;
    float speed = std::sqrt(m_velocity.x * m_velocity.x + m_velocity.y * m_velocity.y);

    if (std::sqrt(m_velocity.x * m_velocity.x + m_velocity.y * m_velocity.y) < 0.1f) {
        m_velocity = {0.0f, 0.0f};
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
    } 
}

void Ball::unlock() {
    belongsTo = nullptr;
    m_cooldown = 5;
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

int Ball::isOutOfField(int xsize, int ysize, int leftPole, int rightPole) {
    if (m_position.y + m_size < 0) {
        if (m_position.x + m_size < leftPole) {
            return 1;
        }else if (m_position.x - m_size > rightPole) {
            return 3;
        }else {
            return 2;
        }
    }else if (m_position.y - m_size > ysize) {
        if (m_position.x + m_size < leftPole) {
            return 6;
        }else if (m_position.x - m_size > rightPole) {
            return 8;
        }else {
            return 7;
        }
    }else {
        if (m_position.x + m_size < 0) {
            return 4;
        }else if (m_position.x - m_size > xsize) {
            return 5;
        }else {
            return 0;
        }
    }
}