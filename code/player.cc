#include "player.h"

Player::Player(float stamina, float size, gf::Vector2f position, Role role, gf::Color4f color)
: m_stamina(stamina)
, m_velocity({0, 0})
, m_size(size)
, m_position(position)
, m_color(color)
, m_role(role)
{
    
}

float Player::getSize() const {
    return m_size;
}

gf::Vector2f Player::getPosition() const {
    return m_position;
}

void Player::setVelocity(gf::Vector2f velocity) {
    m_velocity = velocity;
}

void Player::setPosition(gf::Vector2f position) {
    m_position = position;
}

void Player::update(float dt) {
    m_position += dt * m_velocity;
}

void Player::render(gf::RenderTarget& target) {
    gf::RectangleShape shape({m_size, m_size});
    shape.setPosition(m_position);
    shape.setAnchor(gf::Anchor::TopLeft);
    shape.setOutlineThickness(0.5f);
    shape.setColor(m_color);
    shape.setOutlineColor(gf::Color::darker(m_color));
    target.draw(shape);
}