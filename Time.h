#pragma once

class Time
{
public:
	
	static void processInput();

	static float deltaTime();

private:
	static float deltaT;
	static float lastFrame;
	
	static float timeFPSTracker;
	static int countFPSTracker;
	static int FPS;
};

