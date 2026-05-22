#pragma once
#include "GLFW/glfw3.h"
#include <iostream>
namespace Input
{
	typedef enum ButtonType {
		LeftButton = GLFW_MOUSE_BUTTON_LEFT,
		RightButton = GLFW_MOUSE_BUTTON_RIGHT,
		MiddleButton = GLFW_MOUSE_BUTTON_MIDDLE
	};		
	
	struct Button
	{
			int state;
	};

	class Mouse
	{
#define MAX_MOUSE_BUTTONS 3


		typedef enum ButtonState {
			BUTTON_RELEASED = 0,
			BUTTON_PRESSED,
			BUTTON_JUST_PRESSED,
			BUTTON_JUST_RELEASED
		} KeyState;
		static Button mouseButton[MAX_MOUSE_BUTTONS];
	public:
		static void Mouse_ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
		{
		    scrollX = xoffset;
			scrollY = yoffset;

		}

		static double x, y;      // 当前鼠标位置
		static float lastX, lastY; // 上一帧位置（可用来算 delta）
		static float deltaX, deltaY;
		static float scrollX, scrollY; // 滚轮偏移量

		static inline int MouseButtonDown(int button) {
			return mouseButton[button].state == BUTTON_PRESSED ||
				mouseButton[button].state == BUTTON_JUST_PRESSED;
		}

		static inline int MouseButtonPressed(int button) {
			return mouseButton[button].state == BUTTON_JUST_PRESSED;
		}

		static inline int MouseButtonReleased(int button) {
			return mouseButton[button].state == BUTTON_JUST_RELEASED;
		}

		static inline double MouseDeltaX() { return x - lastX; }
		static inline double MouseDeltaY() { return y - lastY; }
		static  void Update(GLFWwindow* window)
		{
			static bool first = true;
			glfwGetCursorPos(window, &x, &y);
			if (first)
			{
				lastX = x;
				lastY = y;
				first = false;

			}
			// 获取当前位置
			deltaX = x - lastX;
			deltaY = y - lastY;
			lastX = x;
			lastY = y;
			// 更新按键状态
			for (int i = 0; i < MAX_MOUSE_BUTTONS; ++i) {
				int glfwState = glfwGetMouseButton(window, i);
				int prevState = mouseButton[i].state;
				if (glfwState == GLFW_PRESS) {
					if (prevState == BUTTON_RELEASED || prevState == BUTTON_JUST_RELEASED)
						mouseButton[i].state = BUTTON_JUST_PRESSED;
					else
						mouseButton[i].state = BUTTON_PRESSED;
				}
				else { // GLFW_RELEASE
					if (prevState == BUTTON_PRESSED || prevState == BUTTON_JUST_PRESSED)
						mouseButton[i].state = BUTTON_JUST_RELEASED;
					else
						mouseButton[i].state = BUTTON_RELEASED;
				}
			}

		}


	};



	typedef struct Key {
			int state;
		} Key;
	class Keyboard
	{

        #define MAX_KEYS GLFW_KEY_LAST + 1

		typedef enum KeyState {
			KEY_RELEASED = 0,
			KEY_PRESSED,
			KEY_JUST_PRESSED,
			KEY_JUST_RELEASED
		} KeyState;

	
	public:
		static Key keys[MAX_KEYS];

		// 便捷查询
		static inline bool KeyHold(int key) {
			return keys[key].state == KEY_PRESSED ||
				keys[key].state == KEY_JUST_PRESSED;
		}

		static inline bool KeyPress(int key) {
			return keys[key].state == KEY_JUST_PRESSED;
		}

		static inline bool KeyReleased(int key) {
			return keys[key].state == KEY_JUST_RELEASED;
		}
		static void Update(GLFWwindow* window)
		{
			for (int key = 0; key < MAX_KEYS; ++key) {
				int glfwState = glfwGetKey(window, key);
				int prevState = keys[key].state;

				if (glfwState == GLFW_PRESS) {
					if (prevState == KEY_RELEASED ||
						prevState == KEY_JUST_RELEASED) {
						keys[key].state = KEY_JUST_PRESSED;
					}
					else {
						keys[key].state = KEY_PRESSED;
					}
				}
				else { // GLFW_RELEASE
					if (prevState == KEY_PRESSED ||
						prevState == KEY_JUST_PRESSED) {
						keys[key].state = KEY_JUST_RELEASED;
					}
					else {
						keys[key].state = KEY_RELEASED;
					}
				}
			}
		}

	};




};

