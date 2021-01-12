#pragma once
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/glm/glm.hpp>

#include "Time.h"

class Camera
{
public:
	Camera();

	glm::mat4 getView();
	
	static glm::mat4 getProjection();
	static glm::vec3 getPosition();

	void processInput(GLFWwindow* window);

	void mouse_callback(GLFWwindow* window, double xPos, double yPos);

	void updateFrustrum();

	bool chunkWithinFrustrum(glm::vec2 pt);

private:
	glm::mat4 getRotationMatrix();
	

	glm::vec3 frustrumVerts[6];

	static glm::vec3 position;
	static glm::mat4 projection;
	glm::mat4 view;
	glm::vec3 rotation; // pitch, yaw, roll
	glm::vec3 front;
	glm::vec3 up;

	bool firstMouse;
	float lastX;
	float lastY;

	float sensitivity = 0.1f;
	float speed = 10.0f;
};