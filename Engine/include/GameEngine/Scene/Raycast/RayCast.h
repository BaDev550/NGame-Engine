#ifndef RAYCAST_H
#define RAYCAST_H
#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class RayCast
{
public:
	glm::vec3 getRayFromMouse(
		const glm::vec2& mousePos,
		const glm::mat4& view,
		const glm::mat4& projection,
		const glm::vec3& cameraPos);

	unsigned int windowWidth = 1000, windowHeight = 1000;
};
#endif // !RAYCAST_H




