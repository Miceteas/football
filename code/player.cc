#include "player.h"
#include "ball.h"

Player::Player(float stamina, float size, gf::Vector2f position, Role role, Side side, gf::Color4f color, float angle)
: m_stamina(stamina)
, m_velocity({0, 0})
, m_size(size)
, m_position(position)
, m_role(role)
, m_side(side)
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

void Player::setAngle(float angle) {
    m_angle = angle;
}

void Player::setAim(std::vector<Player *> teamPlayersVec, bool left) {

    gf::Vector2f targetPosition;

    float currMaxDist = std::numeric_limits<float>::max();
    Role aimedRole;

    if (m_role != Role::ATTACKER) {
        switch (m_role) {
            case Role::GOALKEEPER : 
                aimedRole = Role::DEFENDER;
                break;
            case Role::DEFENDER :
                aimedRole = Role::MIDFIELDER;
                break;
            case Role::MIDFIELDER : 
                aimedRole = Role::ATTACKER;
                break;
            case Role::ATTACKER :
                aimedRole = Role::ATTACKER;
                break;
        }
    
        for (const auto& other : teamPlayersVec) {
            if (this == other) continue;
            if (other->m_role != aimedRole) continue;

            float distance = gf::squareDistance(m_position, other->m_position);
    
            if (distance <= currMaxDist && other->m_position.x < m_position.x) {
                currMaxDist = distance;
                targetPosition = other->m_position;
            }
        }
    }else {
        targetPosition.x = left ? FIELDXSIZE : 0.0f;
        targetPosition.y = FIELDYSIZE / 2.0f;
    }

    gf::Vector2f direction = targetPosition - m_position;

    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    if (length > 0) {
        direction /= length;
    }

    setVelocity(direction * SPEED);
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
    float distance = gf::euclideanDistance(m_position, other.m_position);
    
    if (isTackling()) {
        return distance < (m_size + other.m_size);
    }

    return distance < (m_size + other.m_size) / 2.0f;
}


void Player::handleCollision(Player& other) {
    if (collidesWith(other)) {
        float distance = gf::euclideanDistance(m_position, other.m_position);

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
                m_reduceSpeed = false;
                m_velocity *= 0.8f;
            }
            m_position += dt * m_velocity;
        } else {
            m_velocity = {0.0f, 0.0f};
        }
    }
}

void Player::renderMain(gf::RenderTarget& target) {
    gf::RectangleShape square({m_size * 0.4f, m_size * 0.4f});
    square.setPosition(m_position);
    square.setAnchor(gf::Anchor::Center);
    square.setColor(gf::Color::Green);
    target.draw(square);
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

        float distance = gf::euclideanDistance(m_position, other->m_position);

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

    gf::Vector2f aimingPos(aimingX, aimingY);

    if (aimAssist) {
        float distance = gf::euclideanDistance(m_position, aimingPos);
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
    gf::Vector2f ballPos = ball.getPosition();

    float XleftFull = 0;
    float XleftQuarter = FIELDXSIZE / 4.0f;
    float Xmiddle = FIELDXSIZE / 2.0f;
    float XrightQuarter = FIELDXSIZE * 3.0f / 4.0f;
    float XrightFull = FIELDXSIZE;

    float YtopFull = 0;
    float YtopQuarter = FIELDYSIZE / 4.0f;
    float Ymiddle = FIELDYSIZE / 2.0f;
    float YbottomQuarter = FIELDYSIZE * 3.0f / 4.0f;
    float YbottomFull = FIELDYSIZE;

    float teamXStart = left ? XleftFull : Xmiddle;
    float teamXEnd = left ? Xmiddle : XrightFull;
    float middleStart = XleftQuarter;
    float middleEnd = XrightQuarter;
    float enemyXStart = left ? Xmiddle : XleftFull;
    float enemyXEnd = left ? XrightFull : Xmiddle;
    float goalkeeperXStart = left ? XleftFull : FIELDXSIZE * 9 / 10;
    float goalkeeperXEnd = left ? FIELDXSIZE / 10 : XrightFull;
    float enemyGoalX = left ? XrightFull : XleftFull;

    gf::Vector2f targetPosition;

    switch (m_side) {
        case Side::TOP : 
            targetPosition.y = std::clamp(ballPos.y, YtopFull, Ymiddle);
            break;
        case Side::MIDDLE : 
            targetPosition.y = std::clamp(ballPos.y, YtopQuarter, YbottomQuarter);
            break;
        case Side::BOTTOM : 
            targetPosition.y = std::clamp(ballPos.y, Ymiddle, YbottomFull);
            break;
    }

    switch (m_role) {
        case Role::ATTACKER:
            if (ballInTeam) {
                if (ball.isLockedTo(this)) {
                    targetPosition.y = FIELDYSIZE / 2;
                    targetPosition.x = enemyGoalX;
                    if ((m_position.x < enemyGoalX + FIELDXSIZE / 5)) {
                        setAim(teamPlayersVec, left);
                        ball.unlock();
                        ball.setVelocity(getShootVelocity(ball.getSize(), teamPlayersVec));
                    }
                }else {
                    targetPosition.x = ballPos.x + (left ? 200 : -200);
                }
            } else {
                targetPosition.x = std::clamp(ballPos.x, enemyXStart, enemyXEnd);
            }
            break;

        case Role::MIDFIELDER:
            if (ballInTeam) {
                if (ball.isLockedTo(this)) {
                    targetPosition.x = middleStart;
                    if ((m_position.x < middleStart + FIELDXSIZE / 10)) {
                        setAim(teamPlayersVec, left);
                        ball.unlock();
                        ball.setVelocity(getPassVelocity(teamPlayersVec));
                    }
                }else {
                    targetPosition.x = ballPos.x + (left ? 200 : -200);
                }    
            } else {
                targetPosition.x = std::clamp(ballPos.x, middleStart, middleEnd);
            }
            break;

        case Role::DEFENDER:
            if (ballInTeam) {
                if (ball.isLockedTo(this)) {
                    targetPosition.x = teamXStart;
                    if ((m_position.x < teamXStart + FIELDXSIZE / 10)) {
                        setAim(teamPlayersVec, left);
                        ball.unlock();
                        ball.setVelocity(getPassVelocity(teamPlayersVec));
                    }
                } else {
                    targetPosition.x = ballPos.x + (left ? 200 : -200);
                }
            } else {
                targetPosition.x = std::clamp(ballPos.x, teamXStart, teamXEnd);
            }
            break;

        case Role::GOALKEEPER:
            // Stay between the poles and in the team's tenth
            if (ball.isLockedTo(this)) {
                if (m_position.x < goalkeeperXStart) {
                    setAim(teamPlayersVec, left);
                    ball.unlock();
                    ball.setVelocity(getPassVelocity(teamPlayersVec));
                }
            }
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

void Player::moveTo(gf::Vector2f position) {
    m_position = position;
    m_velocity = {0.0f, 0.0f};
}