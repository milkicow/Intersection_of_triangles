#pragma once
#include <iostream>

const int NUMBER_OF_BUTTONS = 348; 
static std::array<bool, NUMBER_OF_BUTTONS> buttons{};

class Camera {

private:
    glm::vec3 viewer_position = glm::vec3(2.0f, 2.0f, 2.0f);
    glm::vec3 view_position =  glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 camera_direction = glm::normalize (glm::vec3 {-2.0f, -2.0f, -2.0f});
    glm::vec3 camera_up (0.0f, 0.0f, 1.0f);

    float speed = 1.0;

public:
    glm::vec3 determine_move(const std::array<bool, NUMBER_OF_BUTTONS>& buttons);

};

glm::vec3 Camera::determine_move(const std::array<bool, NUMBER_OF_BUTTONS>& buttons) {

    glm::vec3 movement;

    if (buttons[GLFW_KEY_W]) {
        movement += glm::normalize (camera_direction) * speed;
    }
    else if (buttons[GLFW_KEY_S]) {
        movement -= glm::normalize (camera_direction) * speed;
    }
    else if (buttons[GLFW_KEY_A]) {
        movement -= glm::normalize (glm::cross (camera_direction, camera_up)) * speed;
    }
    else if (buttons[GLFW_KEY_D]) {
        movement += glm::normalize (glm::cross (camera_direction, camera_up)) * speed;
    }
    else if (buttons[GLFW_KEY_UP]) {
        
    }
    else if (buttons[GLFW_KEY_DOWN]) {

    }
    else if (buttons[GLFW_KEY_LEFT]) {

    }
    else if (buttons[GLFW_KEY_RIGHT]) {

    }

    return movement;
}

