#include "Input.h"
using namespace Input;
float Mouse::lastX = 0;
float Mouse::lastY = 0;
Button Mouse::mouseButton[MAX_MOUSE_BUTTONS];
double Mouse::x = 0;
double Mouse::y = 0;
float Mouse::deltaX = 0;
float Mouse::deltaY = 0;
float Mouse::scrollX = 0;
float Mouse::scrollY = 0;
Key Keyboard::keys[MAX_KEYS];