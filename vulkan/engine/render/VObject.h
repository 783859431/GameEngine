#pragma once
#include <glm/glm.hpp>
#include "3d/mesh.h"
#include "3d/transform.h"
#include "Material.h"
struct ConstData {
	glm::mat4 m{};
};
class VObject
{
public:
	Mesh mesh;
	TransformComponent transform;
	Material* material;
	
	void draw(CommandBuffer& cmd, int currentFrame);



};

