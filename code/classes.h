#include <iostream>
#include <string>
#include <cmath>
#include <vector>

enum class Role {
    Attacker,
    Defender,
    Goalkeeper
};

float calculateDistance(float x, float y, float x2, float y2);

class Player {
private:
    float stamina;
    float speed;
    float x, y;
    Role role;
public:
    /** @brief Construct a new Player object*/
    Player(float stamina, float speed, float x, float y, Role role);

    /** @brief Getters */
    // idk if we need all of these
    float getStamina();
    float getSpeed();
    float getX();
    float getY();
    Role getRole();

    /** @brief Setters */
    // idk if needed
    void setStamina(float stamina);
    void setSpeed(float speed);
    void setX(float x);
    void setY(float y);
    void setRole(Role role);

    void move(float dx, float dy); // maybe add speed here?
    void shoot(float power, float angle); // idk? maybe have speed of the player impact this? :3

    void display();

};

class Ball {
private:
    float x, y;
public:
    Ball(float x, float y);

    float getX();
    float getY();

    void move(float dx, float dy);
    void display();

    // do we need the speed of the ball? maybe?
};

class Field {
private:
    float width, height;
    std::vector<std::pair<float, float>> goals;
public:
    Field(float width, float height);
    bool isGoal(float x, float y);
    bool isOutOfBounds(float x, float y);
};

