#pragma once
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
class Camera 
{

	//view 
	glm::vec3 position = glm::vec3(0,0,2);
	glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 camForward = glm::vec3(0,0,1);
	glm::vec3 cameraUp = glm::vec3(0,1,0); //normal
	glm::vec3 cameraRight = glm::vec3(1,0,0); //normal 
	glm::mat4 lookat ;
	glm::mat4 proj ;
	glm::vec3 eulerAng;//pitch yaw roll

	//perspect
	float fov;
	float aspect;
	float near;
	float far;
	
	bool dirtyPerspect = false;
	bool dirtyLookAt = false;
public:
	void SetEuler(float pitch, float yaw)
	{
		eulerAng.x = pitch;
		eulerAng.y = yaw;
		camForward.x = cos(pitch) * cos(yaw);
		camForward.y = sin(pitch);
		camForward.z = cos(pitch) * sin(yaw);
		camForward =  glm::normalize(camForward);
		cameraRight = glm::cross(camForward, cameraUp);
		cameraRight = glm::normalize(cameraRight);
		dirtyLookAt = true;
	}
	void Move(glm::vec3& dt) {


		position += dt;
		dirtyLookAt = true;
	}
	Camera() {
		position = glm::vec3(0, 0, -2);
		target = glm::vec3(0.0f, 0.0f, 0.0f);
		camForward = glm::vec3(0, 0, 1);
		cameraUp = glm::vec3(0, 1, 0); //normal
		cameraRight = glm::vec3(1, 0, 0); //normal 
		fov = 45.0f;
		aspect = 4.0f / 3.0f;
		near = 0.1f;
		far = 10.0f;
		proj = glm::perspective(fov, aspect, near, far);

		lookat = glm::lookAt(position, target, cameraUp);
	}
	void LookAt(glm::vec3& target) 
	{
		this->target = target;
		camForward = target - position;//目标位置-摄像机位置 = 摄像机朝向的向量
		camForward = glm::normalize(camForward);// 求单位向量
		cameraRight = glm::cross(camForward, cameraUp);// 向量叉积求当前相机的右向量
		cameraRight = glm::normalize(cameraRight);//单位化
		dirtyLookAt = true;
	}

	void SetFov(float fov)
	{
		this->fov = fov;
		dirtyPerspect = true;
	}


	void SetAspect(float aspect) 
	{
		this->aspect = aspect;
		dirtyPerspect = true;
	}


	void SetNearFar(float near,float far) 
	{
		this->near = near;
		this->far = far;
		dirtyPerspect = true;
	}


	glm::vec3& forward() {
	
		return camForward;

	}

	glm::vec3& right() {
		return cameraRight;
	}
	glm::vec3& up()
	{
		return cameraUp;
	}
	glm::vec3& eulerAngle()
	{
		return eulerAng;
	}

	glm::mat4& viewMat()
	{
		if (dirtyLookAt)
		{
			// 位置， target， 
		    lookat = glm::lookAt(position,position+camForward, cameraUp);
			dirtyLookAt = false;
		}
		return lookat;
	}

	glm::mat4& perspectMat() {
		if (dirtyPerspect)
		{
		   proj = glm::perspective(fov,aspect,near,far);
		   dirtyPerspect = false;
		}
		return proj;
	}

};