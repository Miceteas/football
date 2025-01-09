#include "ball.h"

Ball::Ball(float size, gf::Vector2f position, gf::Color4f color)
: m_velocity({0, 0})
, m_size(size)
, m_position(position)
, m_color(color)
, belongsTo(nullptr) 
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
    m_position += dt * m_velocity;
}

void Ball::lockTo(Player *p) {
    belongsTo = p;
}

void Ball::unlock() {
    belongsTo = nullptr;
}

bool Ball::isLockedTo(Player *p) const {
    return belongsTo == p;
}

void Ball::render(gf::RenderTarget& target) {
    gf::CircleShape shape(m_size);
    shape.setPosition(m_position);
    shape.setColor(m_color);
    shape.setAnchor(gf::Anchor::TopLeft);
    shape.setOutlineThickness(0.5f);
    shape.setOutlineColor(gf::Color::lighter(m_color));
    target.draw(shape);
}
