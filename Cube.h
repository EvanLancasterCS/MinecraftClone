#pragma once

#include "Object.h"
#include "TextureLoader.h"

class Cube : public Object
{

public:
	Cube();

	virtual void setPosition(glm::vec3 pos);
	
	void draw();

	unsigned int getBlockID() const;
	void setBlockID(unsigned int id);

	unsigned int blockID;

private:
	static unsigned int vbo;
	static unsigned int vao;
	static unsigned int ebo;
	
	glm::mat4 translationMatrix;
	
};