#include <GLFW/glfw3.h>
#include "camera.hpp"
#include "input.hpp"

glm::vec3 Camera::determine_move() {

    glm::vec3 movement{0.0f, 0.0f, 0.0f};

    if (Input::is_keyboard_key(GLFW_KEY_W)) {
        movement += glm::normalize (camera_direction) * speed;
    }
    if (Input::is_keyboard_key(GLFW_KEY_S)) {
        movement -= glm::normalize (camera_direction) * speed;
    }
    if (Input::is_keyboard_key(GLFW_KEY_A)) {
        movement -= glm::normalize (glm::cross (camera_direction, camera_up)) * speed;
    }
    if (Input::is_keyboard_key(GLFW_KEY_D)) {
        movement += glm::normalize (glm::cross (camera_direction, camera_up)) * speed;
    }
    // else if (keyboard_keys[GLFW_KEY_UP]) {

    // }
    // else if (keyboard_keys[GLFW_KEY_DOWN]) {

    // }
    // else if (keyboard_keys[GLFW_KEY_LEFT]) {

    // }
    // else if (keyboard_keys[GLFW_KEY_RIGHT]) {

    // }

    return movement;
}

glm::vec3 Camera::determine_rotate(double xpos, double ypos) {

    glm::vec3 rotation;
    static double prev_x = xpos;
    static double prev_y = ypos;

    static double phi = glm::radians (225.0f), ksi = glm::radians (-35.26f);

    if (Input::is_mouse_button(GLFW_MOUSE_BUTTON_LEFT)) {
        double delta_x = xpos - prev_x;
        double delta_y = ypos - prev_y;

        double sensivity = 0.001;

        phi -= delta_x * sensivity;
        ksi -= delta_y * sensivity;
    }
    prev_x = xpos;
    prev_y = ypos;

    rotation = glm::vec3 (glm::cos(ksi) * glm::cos (phi), glm::cos(ksi) * glm::sin(phi), glm::sin(ksi));

    return rotation;
}
