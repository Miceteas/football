// #include <iostream>
// #include <string>
// #include <cmath>
// #include <vector>
// #include <gf/Direction.h>
// #include <gf/Entity.h>
// #include <gf/Font.h>
// #include <gf/Random.h>
// #include <gf/Rect.h>

// enum class Role {
//     ATTACKER,
//     DEFENDER,
//     GOALKEEPER
// };

// float calculateDistance(float x, float y, float x2, float y2);

// class Player {
// private:
//     float m_stamina; //Pas sûr pour le moment
//     float m_speed;
//     float m_size;
//     gf::Vector2f m_position;
//     Role m_role;
// public:
//     /** @brief Construct a new Player object*/
//     Player(float stamina, float speed, gf::Vector2f position, Role role);

//     /** @brief Getters */
//     // idk if we need all of these
//     float getStamina();
//     float getSpeed();
//     float getX();
//     float getY();
//     Role getRole();

//     /** @brief Setters */
//     // idk if needed
//     void setStamina(float stamina);
//     void setSpeed(float speed);
//     void setX(float x);
//     void setY(float y);
//     void setRole(Role role);



//     void move(float dx, float dy); // maybe add speed here?
//     void shoot(float power, float angle); // idk? maybe have speed of the player impact this? :3

//     void display();

// };

// class Ball {
// private:
//     float x, y;
// public:
//     Ball(float x, float y);

//     float getX();
//     float getY();

//     void move(float dx, float dy);
//     void display();

//     // do we need the speed of the ball? maybe?
// };

// class Field {
// private:
//     float width, height;
//     std::vector<std::pair<float, float>> goals;
// public:
//     Field(float width, float height);
//     bool isGoal(float x, float y);
//     bool isOutOfBounds(float x, float y);
// };

