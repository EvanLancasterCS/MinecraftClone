#pragma once

#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>

class Object {

public:
	Object();

	glm::vec3 getPosition() const;
	glm::vec3 getRotation() const;

	virtual void setPosition(glm::vec3 pos);
	void setRotation(glm::vec3 rot);
	virtual void draw() = 0;

protected:
	glm::vec3 position;
	glm::vec3 rotation;

	const float* vertices;
	
};