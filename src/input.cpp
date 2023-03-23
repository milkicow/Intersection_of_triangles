#include "input.hpp"

// constexpr int Input::NUMBER_OF_KEYBOARD_KEYS = 348; 
// constexpr int Input::NUMBER_OF_MOUSE_BUTTONS = 7;

std::array<bool, Input::NUMBER_OF_KEYBOARD_KEYS> Input::keyboard_keys{};
std::array<bool, Input::NUMBER_OF_MOUSE_BUTTONS> Input::mouse_buttons{};