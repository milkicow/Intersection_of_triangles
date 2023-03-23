#pragma once
#include <iostream>
#include <array>

class Input {
private:
    static constexpr int NUMBER_OF_KEYBOARD_KEYS = 348; 
    static constexpr int NUMBER_OF_MOUSE_BUTTONS = 7;



public:

    static std::array<bool, NUMBER_OF_KEYBOARD_KEYS> keyboard_keys;
    static std::array<bool, NUMBER_OF_MOUSE_BUTTONS> mouse_buttons;

    static bool is_keyboard_key(int key) { return keyboard_keys[key]; }
    static void press_keyboard_key(int key) { keyboard_keys[key] = true; }
    static void release_keyboard_key(int key) { keyboard_keys[key] = false; }

    static bool is_mouse_button(int key) { return mouse_buttons[key]; }
    static void press_mouse_button(int key) { mouse_buttons[key] = true; } 
    static void release_mouse_button(int key) { mouse_buttons[key] = false; }

    // static constexpr int get_NUMBER_OF_KEYBOARD_KEYS() { return NUMBER_OF_KEYBOARD_KEYS; }
    // static constexpr int get_NUMBER_OF_MOUSE_BUTTONS() { return NUMBER_OF_MOUSE_BUTTONS; }
};
