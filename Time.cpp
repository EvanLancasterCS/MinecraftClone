#include "Time.h"
#include <GLFW/glfw3.h>
#include <iostream>


float Time::deltaT = 0.0f;
float Time::lastFrame = 0.0f;
float Time::timeFPSTracker = 0.0f;
int Time::countFPSTracker = 0;
int Time::FPS = 0;

void Time::processInput()
{
	float currFrame = glfwGetTime();
	deltaT = currFrame - lastFrame;
	lastFrame = currFrame;

	timeFPSTracker += deltaT;
	countFPSTracker++;
	if (timeFPSTracker >= 1)
	{
		FPS = countFPSTracker;
		timeFPSTracker = 0;
		countFPSTracker = 0;
	}
	std::cout << FPS << std::endl;
}

float Time::deltaTime() {
	return Time::deltaT;
}