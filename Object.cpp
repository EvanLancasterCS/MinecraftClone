#include "Object.h"


Object::Object()
{
	position = glm::vec3(0, 0, 0);
	rotation = glm::vec3(0, 0, 0);
}

glm::vec3 Object::getPosition() const { return position; }
glm::vec3 Object::getRotation() const { return rotation; }

void Object::setPosition(glm::vec3 pos) { position = pos; }
void Object::setRotation(glm::vec3 rot) { rotation = rot; }