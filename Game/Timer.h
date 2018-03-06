#pragma once


//keep track of elaped time so we can use it for stuff like physics
class Timer {
public:
	Timer();
	~Timer();

	float tick();
	float getElapsedTimeMS();
	float getElapsedTimeSeconds();
	float getCurrentTime();

private:
	float currentTime, previousTime, elapsedTime;
};