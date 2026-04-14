#pragma once
#include"vulkan/vulkan.h"
#include <glm/gtc/matrix_transform.hpp>
#include "Camera2D.h"
#define MAX_FRAMES_IN_FLIGHT  2
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
struct MVP
{
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;

};
struct DataInfo
{
	glm::vec2 viewport;
	float deltaTime;
	float time;
};
extern Camera2D g_Cam2d;
