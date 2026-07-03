#pragma once
#include <glm/glm.hpp>
#include "3d/mesh.h"
#include "3d/transform3D.h"
#include "Material.h"
struct ConstData {
	glm::mat4 m{};
};
class VObject
{
public:
	Mesh mesh;
	Transform3D transform;
	Material* material;
	
	void draw(CommandBuffer& cmd, int currentFrame);



};

