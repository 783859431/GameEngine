#pragma once
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
class Camera2D
{
	

	glm::mat4 view;
	glm::mat4 ortho;

	bool dirty;
public:	
	float zoom;
	float left, right, bottom, top, near, far;
	glm::vec2 position;
	Camera2D() {
		left = 0.0f;
		right = 800.0f;
		bottom = 0.0f;
		top = 600.0f;
		near = -1.0f;
		far = 1.0f;

		position = glm::vec2(0);
		zoom = 1;
		ortho = glm::ortho(
			left, right,
			bottom, top,
			near, far);
		view = glm::translate(
			glm::mat4(1.0f),
			glm::vec3(-position, 0.0f)
		);
		view = glm::scale(view, glm::vec3(zoom, zoom, 1.0f));
		dirty = false;

	}
	glm::mat4& GetProject() 
	{
		ortho = glm::ortho(
			left, right,
			bottom, top,
			near, far);
		return ortho;
	}
	glm::mat4& GetView()
	{

	    view = glm::translate(
	    	glm::mat4(1.0f),
	    	glm::vec3(-position, 0.0f)
	    );
	    view = glm::scale(view, glm::vec3(zoom, zoom, 1.0f));
	    dirty = false;
		
		return view;
	}
	glm::mat4 getVP() 
	{
		return ortho * view;
	}
};
