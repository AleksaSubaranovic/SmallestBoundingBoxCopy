#pragma once
#include <SDL.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <chrono>
#include "Generic.h"

using namespace std::chrono;
typedef high_resolution_clock Clock;

typedef void (*initType)();
typedef void (*cleanupType)();
typedef void (*updateType)(float);

// typedef SDL_Color color; (also in Generic.h)

namespace SandboxFramework
{
	// global (SandboxFramework namespace) variables, external linkage, definitions in cpp

	extern SDL_Window* window;
	extern SDL_Surface* screenSurface; // cpu 
	extern SDL_Renderer* renderer; // gpu
	extern int _width , _height;
	extern std::string name;
	extern Clock::time_point lastTimePoint;

	extern initType initEvent; // post init
	extern updateType updateEvent;
	extern cleanupType cleanupEvent; // pre cleanup

	// If changed to false then user must clear renderer by hand
	extern bool shouldClearRenderer;

	extern bool shouldDrawCordinateLines;

	// Init Sandbox
	bool sandboxInit(const int w, const int h, std::string _name = "Sandbox");

	// Close Sanbox
	void sandboxCleanup();
	
	// Called every frame (tick)
	bool sandboxUpdate();

	// Bind event that are called on init(once after init phase), update(every frame) and cleanup(once before cleanup phase)
	void BindEvents(initType init, updateType update, cleanupType cleanup);

	void DrawGraphicObjects();

	void UpdateFlag(const char* flagName, bool value);

	//bool 
};