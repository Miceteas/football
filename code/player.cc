#include "player.h"

Player::Player(float stamina, float size, gf::Vector2f position, Role role, gf::Color4f color, float angle)
: m_stamina(stamina)
, m_velocity({0, 0})
, m_size(size)
, m_position(position)
, m_color(color)
, m_role(role)
, m_angle(angle)
{
    
}

float Player::getSize() const {
    return m_size;
}

gf::Vector2f Player::getPosition() const {
    return m_position;
}

float Player::calcAngle(gf::Vector2f velocity) {
    float ang = m_angle;
    if (velocity.x > 0 && velocity.y > 0) {
        ang = M_PI / 4;
    } else if (velocity.x > 0 && velocity.y < 0) {
        ang = M_PI * 7 / 4;
    } else if (velocity.x < 0 && velocity.y > 0) {
        ang = M_PI * 3 / 4;
    } else if (velocity.x < 0 && velocity.y < 0) {
        ang = M_PI * 5 / 4;
    } else if (velocity.x > 0 && velocity.y == 0) {
        ang = 0;
    } else if (velocity.x < 0 && velocity.y == 0) {
        ang = M_PI;
    } else if (velocity.x == 0 && velocity.y > 0) {
        ang = M_PI / 2;
    } else if (velocity.x == 0 && velocity.y < 0) {
        ang = M_PI * 3 / 2;
    } else {
        ang = m_angle;
    }
    return ang;
}

void Player::setVelocity(gf::Vector2f velocity) {
    m_velocity = velocity;
    m_angle = calcAngle(velocity);
}

void Player::setPosition(gf::Vector2f position) {
    m_position = position;
}

void Player::update(float dt) {
    m_position += dt * m_velocity;
}

gf::Vector2f Player::getVelocity() const {
    return m_velocity;
}

void Player::render(gf::RenderTarget& target) {
    gf::RectangleShape shape({m_size, m_size});
    shape.setPosition(m_position);
    shape.setAnchor(gf::Anchor::Center);
    shape.setOutlineThickness(0.5f);
    shape.setColor(m_color);
    shape.setOutlineColor(gf::Color::darker(m_color));
    shape.setRotation(m_angle);
    target.draw(shape);
}

bool Player::isEqual(float a, float b) {
    return std::fabs(a - b) < 0.01;
}

float Player::getAngle() {
    return m_angle;
}

gf::Vector2f Player::getPassVelocity() {
    if (isEqual(m_angle, M_PI / 4)) {
        return {sqrt(300 * 300 / 2), sqrt(300 * 300 / 2)};
    } else if (isEqual(m_angle, M_PI * 3 / 4)) {
        return {-sqrt(300 * 300 / 2), sqrt(300 * 300 / 2)};
    } else if (isEqual(m_angle, M_PI * 5 / 4)) {
        return {-sqrt(300 * 300 / 2), -sqrt(300 * 300 / 2)};
    } else if (isEqual(m_angle, M_PI * 7 / 4)) {
        return {sqrt(300 * 300 / 2), -sqrt(300 * 300 / 2)};
    } else if (isEqual(m_angle, 0)) {
        return {300, 0};
    } else if (isEqual(m_angle, M_PI / 2)) {
        return {0, 300};
    } else if (isEqual(m_angle, M_PI)) {
        return {-300, 0};
    } else if (isEqual(m_angle, M_PI * 3 / 2)) {
        return {0, -300};
    } else {
        return {0, -1000};
    }
}

gf::Vector2f Player::getShootVelocity() {
    if (isEqual(m_angle, M_PI / 4)) {
        return {sqrt(600 * 600 / 2), sqrt(600 * 600 / 2)};
    } else if (isEqual(m_angle, M_PI * 3 / 4)) {
        return {-sqrt(600 * 600 / 2), sqrt(600 * 600 / 2)};
    } else if (isEqual(m_angle, M_PI * 5 / 4)) {
        return {-sqrt(600 * 600 / 2), -sqrt(600 * 600 / 2)};
    } else if (isEqual(m_angle, M_PI * 7 / 4)) {
        return {sqrt(600 * 600 / 2), -sqrt(600 * 600 / 2)};
    } else if (isEqual(m_angle, 0)) {
        return {600, 0};
    } else if (isEqual(m_angle, M_PI / 2)) {
        return {0, 600};
    } else if (isEqual(m_angle, M_PI)) {
        return {-600, 0};
    } else if (isEqual(m_angle, M_PI * 3 / 2)) {
        return {0, -600};
    } else {
        return {0, -1000};
    }
}