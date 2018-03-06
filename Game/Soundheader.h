#pragma once
#pragma once
#pragma comment (lib, "fmod_vc.lib") // includes the library through code (compiler/OS specific, not part of standard c++ library)

#include <Windows.h>
#include"FMOD\fmod.hpp"
#include "FMOD\fmod_errors.h"
#include <conio.h> // keyboard input
#include <iostream>

void FmodErrorCheck(FMOD_RESULT result);

class Listener
{
public:
	FMOD_VECTOR listenerpos = { 0.0f, 0.0f, -1.0f };
	FMOD_VECTOR forward = { 0.0f, 0.0f, 1.0f };
	FMOD_VECTOR up = { 0.0f, 1.0f, 0.0f };
	FMOD_VECTOR vel = { 0.0f, 0.0f, 0.0f };
	FMOD_VECTOR pos = { -10.0f, 0.0f, 0.0f };
};

class SoundEngine
{
private:
	bool init = false;
	FMOD_RESULT result;

public:
	SoundEngine();
	~SoundEngine();

	bool Init();
	void CleanUp();

	// Add Update

	FMOD::System    *system;
	void            *extradriverdata = 0;
	unsigned int     version;

	Listener listener;
};