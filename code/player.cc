#include "player.h"

#define PASSVELOCITY 300
#define SHOOTVELOCITY 600

Player::Player(float stamina, float size, gf::Vector2f position, Role role, gf::Color4f color, float angle)
: m_stamina(stamina)
, m_velocity({0, 0})
, m_size(size)
, m_position(position)
, m_color(color)
, m_role(role)
, m_angle(angle)
, m_isTackling(false)
, m_freezeTime(0.0f)
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

void Player::setTackleData(float speed, float angle) {
    m_tackleSpeed = speed;
    m_tackleAngle = angle;
    m_tackleVelocity = {speed * std::cos(angle), speed * std::sin(angle)};
    m_isTackling = true;
    m_slideDistance = 150.0f;
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

// Jamais utilisée ?
void Player::tackle() {
    if (!m_isTackling) {
        m_isTackling = true;
        m_slideDistance = 200.0f; 
        // Pas m_tackleSpeed ?
        float tackleSpeed = 300.0f;
        m_velocity = {tackleSpeed * std::cos(m_angle), tackleSpeed * std::sin(m_angle)};
    }
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
        static constexpr float tackleFriction = 150.0f; 
        static constexpr float minSlideSpeed = 5.0f; 
        float speed = std::sqrt(m_tackleVelocity.x * m_tackleVelocity.x + m_tackleVelocity.y * m_tackleVelocity.y);

        if (speed > 0.0f && m_slideDistance > 0.0f) {
            float deceleration = tackleFriction * dt;
            speed = std::max(speed - deceleration, 0.0f);
            m_slideDistance -= speed * dt;

            if (speed > 0.0f) {
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

float Player::getAngle() {
    return m_angle;
}

gf::Vector2f Player::getPassVelocity() {
    return {(float)cos(m_angle) * PASSVELOCITY, (float)sin(m_angle) * PASSVELOCITY};
}

gf::Vector2f Player::getShootVelocity() {
    return {(float)cos(m_angle) * SHOOTVELOCITY, (float)sin(m_angle) * SHOOTVELOCITY};
}

//TEMPORARY 

void Player::changeColor(gf::Color4f color) {
    m_color = color;
}