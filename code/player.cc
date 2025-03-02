#include "player.h"

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
, m_isSprinting(false)
, m_reduceSpeed(false)
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
    if ((velocity.x == 0 && velocity.y == 0) || m_isTackling) {
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

void Player::reduceSpeed(){
    m_reduceSpeed = true;
}

void Player::freeze(float duration) {
    m_freezeTime = duration;
}

// Good function only if both players are circles but we might change the shapes to be
bool Player::collidesWith(const Player& other) const {
    float distance = std::sqrt(std::pow(m_position.x - other.m_position.x, 2) +
                               std::pow(m_position.y - other.m_position.y, 2));
    
    if (isTackling()) {
        return distance < (m_size + other.m_size);
    }

    return distance < (m_size + other.m_size) / 2.0f;
}


void Player::handleCollision(Player& other) {
    if (collidesWith(other)) {
        float distance = std::sqrt(std::pow(m_position.x - other.m_position.x, 2) +
                                   std::pow(m_position.y - other.m_position.y, 2));

        if (distance < (m_size + other.m_size) / 2.0f && !isTackling() && !other.isTackling()) {
            float overlap = (m_size + other.m_size) / 2.0f - distance;
            if (overlap > 0) {
                gf::Vector2f direction = other.m_position - m_position;
                m_position -= direction * (overlap / distance);
                other.m_position += direction * (overlap / distance);
            }
        }
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
            if (m_isSprinting) {
                m_velocity *= 1.5f;
            }
            if (m_reduceSpeed){
                m_velocity *= 0.8f;
            }
            m_position += dt * m_velocity;
        } else {
            m_velocity = {0.0f, 0.0f};
        }
    }
}

void Player::render(gf::RenderTarget& target, bool isMainPlayer) {
    gf::RectangleShape shape({m_size, m_size});
    shape.setPosition(m_position);
    shape.setAnchor(gf::Anchor::Center);
    shape.setOutlineThickness(0.5f);
    shape.setColor(m_color);
    shape.setOutlineColor(gf::Color::darker(m_color));
    shape.setRotation(m_angle);
    target.draw(shape);

    if (isMainPlayer) {
        gf::RectangleShape square({m_size * 0.4f, m_size * 0.4f});
        square.setPosition(m_position);
        square.setAnchor(gf::Anchor::Center);
        square.setColor(gf::Color::Green);
        target.draw(square);
    }
}

float Player::getAngle() {
    return m_angle;
}

gf::Vector2f Player::getSelfPassVelocity() {
    return {std::cos(m_angle) * BALL_VELOCITY, std::sin(m_angle) * BALL_VELOCITY};
}


gf::Vector2f Player::getPassVelocity(std::vector<Player *> players) {
    float bestAngle = 0;
    bool aimAssist = false;

    for (const auto& other : players) {
        if (this == other) continue;

        float distance = std::sqrt(std::pow(m_position.x - other->m_position.x, 2) +
                                   std::pow(m_position.y - other->m_position.y, 2));

        if (distance <= MAX_PASS_DISTANCE) {
            float angle = std::atan2(other->m_position.y - m_position.y, other->m_position.x - m_position.x);
            if (std::abs(angle - m_angle) < M_PI / 8 
            || std::abs(angle - m_angle + 2 * M_PI)  < M_PI / 8) {
                if (!aimAssist || std::min(std::abs(bestAngle - m_angle), (float) std::abs(bestAngle - m_angle + 2 * M_PI)) > std::min(std::abs(angle - m_angle), (float) std::abs(angle - m_angle + 2 * M_PI))) {
                    bestAngle = angle;
                    aimAssist = true;
                }
            }
        }
    }

    if (!aimAssist) {
        return {std::cos(m_angle) * PASS_VELOCITY, std::sin(m_angle) * PASS_VELOCITY};
    }else {
        return {std::cos(bestAngle) * PASS_VELOCITY, std::sin(bestAngle) * PASS_VELOCITY};
    }
}

gf::Vector2f Player::getShootVelocity(float ballSize, std::vector<Player *> teamPlayersVec) {
    float best_angle = m_angle;
    bool isMemberOfTeam = std::count(teamPlayersVec.begin(), teamPlayersVec.end(), this) > 0;
    float aimingX = isMemberOfTeam ? FIELDXSIZE : 0;
    float aimingY = 0;
    bool aimAssist = false;
    
    if (m_position.y < TOPPOLE) {
        aimingY = TOPPOLE + ballSize;
        aimAssist = true;
    }else if (m_position.y > BOTTOMPOLE) {
        aimingY = BOTTOMPOLE - ballSize;
        aimAssist = true;
    }

    if (aimAssist) {
        float distance = std::sqrt(std::pow(m_position.x - aimingX, 2) +
                                   std::pow(m_position.y - aimingY, 2));
        if (distance < MAX_SHOOT_DISTANCE) {
            float angle = std::atan2(aimingY - m_position.y, aimingX - m_position.x);
            if (std::abs(angle - m_angle) < M_PI / 8 
            || std::abs(angle - m_angle + 2 * M_PI)  < M_PI / 8) {
                best_angle = angle;
            }
        }
    }

    return {std::cos(best_angle) * SHOOT_VELOCITY, std::sin(best_angle) * SHOOT_VELOCITY};
}

void Player::startSprint() {
    m_isSprinting = true;
}

void Player::stopSprint() {
    m_isSprinting = false;
}

void Player::AImove(Ball& ball, bool left, bool ballInTeam, std::vector<Player *> teamPlayersVec) {
    /* DECISIONS FOR NOW :

    Defenders should stay in their side's third and follow the ball
    Midfielders should stay in the middle third of the field and follow the ball
    Attackers should stay in the opponent's third of the field and follow the ball
    Goalkeepers should stay in their side's tenth and between the poles and follow the ball

    */
    gf::Vector2f ballPos = ball.getPosition();

    float leftThird = FIELDXSIZE / 3.0f;
    float rightThird = 2.0f * FIELDXSIZE / 3.0f;
    float middleThirdStart = leftThird;
    float middleThirdEnd = rightThird;

    float teamThirdStart = left ? 0.0f : rightThird;
    float teamThirdEnd = left ? leftThird : FIELDXSIZE;
    float enemyThirdStart = left ? rightThird : 0.0f;
    float enemyThirdEnd = left ? FIELDXSIZE : leftThird;

    float goalkeeperXStart = left ? 0 : FIELDXSIZE - FIELDXSIZE / 10.0f;
    float goalkeeperXEnd = left ? FIELDXSIZE / 10.0f : FIELDXSIZE;

    gf::Vector2f targetPosition;

    float aboveY = getClosestUpY(teamPlayersVec);
    float belowY = getClosestDownY(teamPlayersVec);
    float dispertion = 0;

    switch (m_role) {
        case Role::ATTACKER:
            if (ballInTeam) {
                // Move towards enemy goal
                // Attackers should stay around 300 pixels vertical from each other
                dispertion = 300;
                targetPosition.x = enemyThirdStart;
                if (ball.isLockedTo(this)) {
                    targetPosition.y = FIELDYSIZE / 2;
                    if ((m_position.x < enemyThirdStart + FIELDXSIZE / 10 + 200)) {
                        ball.unlock();
                        ball.setVelocity(getShootVelocity(ball.getSize(), teamPlayersVec));
                    }
                }else {                    
                    targetPosition.y = std::clamp(ballPos.y, aboveY + dispertion, belowY - dispertion);
                }
            } else {
                // Move towards the ball but stay in the enemy third
                // Attackers should stay around 50 pixels vertical from each other
                dispertion = 50;
                targetPosition.x = std::clamp(ballPos.x, enemyThirdStart, enemyThirdEnd);
                targetPosition.y = std::clamp(ballPos.y, aboveY + dispertion, belowY - dispertion);
            }
            break;

        case Role::MIDFIELDER:
            if (ballInTeam) {
                // Stay in the middle third
                // Midfielders should stay around 200 pixels vertical from each other
                dispertion = 200;
                targetPosition.x = middleThirdStart;
                if (ball.isLockedTo(this)) {
                    targetPosition.y = FIELDYSIZE / 2;
                    if ((m_position.x < middleThirdStart + 100)) {
                        ball.unlock();
                        ball.setVelocity(getPassVelocity(teamPlayersVec));
                    }
                }else {                    
                    targetPosition.y = std::clamp(ballPos.y, aboveY + dispertion, belowY - dispertion);
                }
            } else {
                // Move towards the ball but stay in the middle third
                // Midfielders should stay around 50 pixels vertical from each other
                dispertion = 50;
                targetPosition.x = std::clamp(ballPos.x, middleThirdStart, middleThirdEnd);
                targetPosition.y = std::clamp(ballPos.y, aboveY + dispertion, belowY - dispertion);
            }
            break;

        case Role::DEFENDER:
            if (ballInTeam) {
                // Stay in the team's third
                // Defenders should stay around 25 pixels vertical from each other
                dispertion = 25;
                targetPosition.x = teamThirdStart;
                if (ball.isLockedTo(this)) {
                    targetPosition.y = FIELDYSIZE / 2;
                    if ((m_position.x < teamThirdStart + 100)) {
                        ball.unlock();
                        ball.setVelocity(getPassVelocity(teamPlayersVec));
                    }
                }else {                    
                    targetPosition.y = std::clamp(ballPos.y, aboveY + dispertion, belowY - dispertion);
                }
            } else {
                // Move towards the ball but stay in the team's third
                // Defenders should stay around 100 pixels vertical from each other
                dispertion = 100;
                targetPosition.x = std::clamp(ballPos.x, teamThirdStart, teamThirdEnd);
                targetPosition.y = std::clamp(ballPos.y, aboveY + dispertion, belowY - dispertion);
            }
            break;

        case Role::GOALKEEPER:
            // Stay between the poles and in the team's tenth
            targetPosition.x = std::clamp(ballPos.x, goalkeeperXStart, goalkeeperXEnd);
            targetPosition.y = std::clamp(ballPos.y, TOPPOLE, BOTTOMPOLE);
            break;
    }

    gf::Vector2f direction = targetPosition - m_position;

    // Normalize the direction vector to ensure consistent speed
    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    if (length > 0) {
        direction /= length;
    }

    setVelocity(direction * SPEED);

}

float Player::getClosestUpY(std::vector<Player *> teamPlayersVec) {
    float y = 0;
    for (Player *p : teamPlayersVec) {
        if (p != this && p->m_role == m_role) {
            gf::Vector2f pos = p->getPosition();
            if (pos.y > y && pos.y < m_position.y) {
                y = pos.y;
            }
        }
    }
    return y;
}

float Player::getClosestDownY(std::vector<Player *> teamPlayersVec) {
    float y = FIELDYSIZE;
    for (Player *p : teamPlayersVec) {
        if (p != this && p->m_role == m_role) {
            gf::Vector2f pos = p->getPosition();
            if (pos.y < y && pos.y > m_position.y) {
                y = pos.y;
            }
        }
    }
    return y;
}

//TEMPORARY 

void Player::changeColor(gf::Color4f color) {
    m_color = color;
}
