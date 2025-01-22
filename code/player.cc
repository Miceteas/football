#include "player.h"

#define PASS_VELOCITY 300
#define SHOOT_VELOCITY 600
#define SLIDE_DISTANCE 150.0f
#define TACKLE_FRICTION 150.0f
#define MIN_SLIDE_SPEED 5.0f

Player::Player(float stamina, float size, gf::Vector2f position, Role role, gf::Color4f color, float angle)
: m_stamina(stamina)
, m_velocity({0, 0})
, m_size(size)
, m_position(position)
, m_role(role)
, m_color(color)
, m_angle(angle)
, m_freezeTime(0.0f)
, m_isTackling(false)
, m_slideDistance(SLIDE_DISTANCE)
, m_tackleSpeed(0.0f)
, m_tackleAngle(0.0f)
, m_tackleVelocity({0, 0}) 
{
    
}

float Player::getSize() const {
    return m_size;
}

gf::Vector2f Player::getPosition() const {
    return m_position;
}

gf::Vector2f Player::getVelocity() const {
    return m_velocity;
}

float Player::calcAngle(gf::Vector2f velocity) {
    if (velocity.x == 0 && velocity.y == 0) {
        return m_angle;
    }
    return std::atan2(velocity.y, velocity.x);
}

void Player::setVelocity(gf::Vector2f velocity) {
    m_velocity = velocity;
    m_angle = calcAngle(velocity);
}

void Player::setPosition(gf::Vector2f position) {
    m_position = position;
}

void Player::setTackleData(float speed, float angle) {
    m_tackleSpeed = speed;
    m_tackleAngle = angle;
    m_tackleVelocity = {speed * std::cos(angle), speed * std::sin(angle)};
    m_isTackling = true;
    m_slideDistance = SLIDE_DISTANCE;
}

bool Player::isTackling() const {
    return m_isTackling;
}

void Player::freeze(float duration) {
    m_freezeTime = duration;
}

// Good function only if both players are circles but we might change the shapes to be
bool Player::collidesWith(const Player& other) const {
    float distance = std::sqrt(std::pow(m_position.x - other.m_position.x, 2) +
                               std::pow(m_position.y - other.m_position.y, 2));
    return distance < (m_size + other.m_size) / 2.0f;
}

void Player::update(float dt) {
    if (m_freezeTime > 0.0f) {
        m_freezeTime -= dt;
        if (m_freezeTime <= 0.0f) {
            m_freezeTime = 0.0f;
        }
        m_velocity = {0.0f, 0.0f};
        return;
    }

    m_color = gf::Color::Red;

    if (m_isTackling) {
        float speed = std::sqrt(m_tackleVelocity.x * m_tackleVelocity.x + m_tackleVelocity.y * m_tackleVelocity.y);

        if (speed > 0.0f && m_slideDistance > 0.0f) {
            float deceleration = TACKLE_FRICTION * dt;
            speed = std::max(speed - deceleration, 0.0f);
            m_slideDistance -= speed * dt;

            if (speed > MIN_SLIDE_SPEED) {
                m_velocity = m_tackleVelocity / std::sqrt(m_tackleVelocity.x * m_tackleVelocity.x + m_tackleVelocity.y * m_tackleVelocity.y) * speed;
                m_position += dt * m_velocity;
            } else {
                m_isTackling = false;
                m_velocity = {0.0f, 0.0f};
            }
        } else {
            m_isTackling = false;
            m_velocity = {0.0f, 0.0f};
        }
    } else {
        if (std::sqrt(m_velocity.x * m_velocity.x + m_velocity.y * m_velocity.y) > 0.0f) {
            m_position += dt * m_velocity;
        } else {
            m_velocity = {0.0f, 0.0f};
        }
    }
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

float Player::getAngle() {
    return m_angle;
}

gf::Vector2f Player::getPassVelocity() {
    return {std::cos(m_angle) * PASS_VELOCITY, std::sin(m_angle) * PASS_VELOCITY};
}

gf::Vector2f Player::getShootVelocity() {
    return {std::cos(m_angle) * SHOOT_VELOCITY, std::sin(m_angle) * SHOOT_VELOCITY};
}

//TEMPORARY 

void Player::changeColor(gf::Color4f color) {
    m_color = color;
}
