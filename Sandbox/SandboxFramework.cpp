#include "SandboxFramework.h"
#include <SDL.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <chrono>
#include "PlayerInput.h"
#include "SandboxGraphics.h"

SDL_Window* SandboxFramework::window = NULL;
SDL_Surface* SandboxFramework::screenSurface = NULL; // cpu rasters
SDL_Renderer* SandboxFramework::renderer = NULL; // gpu
int SandboxFramework::_width = 0, SandboxFramework::_height = 0;
std::string SandboxFramework::name("-1");
Clock::time_point SandboxFramework::lastTimePoint = Clock::now();

initType SandboxFramework::initEvent = NULL; // post init
updateType SandboxFramework::updateEvent = NULL;
cleanupType SandboxFramework::cleanupEvent = NULL; // pre cleanup

bool SandboxFramework::shouldClearRenderer = true;

bool SandboxFramework::shouldDrawCordinateLines = false;

bool SandboxFramework::sandboxInit(const int w, const int h, std::string _name)
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		return false;
	}
	// SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
	window = SDL_CreateWindow(_name.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, SDL_WINDOW_SHOWN);
	if (window == nullptr)
	{
		printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
		return false;
	}
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	// SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	screenSurface = SDL_GetWindowSurface(window); // not needed ?

	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0xFF, 0xFF, 0xFF)); // fill screen with white
	SDL_UpdateWindowSurface(window);

	_width = w;
	_height = h;
	name = _name;
	lastTimePoint = Clock::now();

	if (initEvent) initEvent();

	return true;
}

void SandboxFramework::sandboxCleanup()
{
	if (cleanupEvent) cleanupEvent();

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window); // also for screen surface
	SDL_Quit();
}

bool SandboxFramework::sandboxUpdate()
{
	// Clean screen
	if (shouldClearRenderer)
	{
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderClear(renderer);
		if (shouldDrawCordinateLines)
		{
			for (int x = 0; x < _width; x += 50)
				SandboxGraphics::DrawDottedLine(x, 0, x, _height - 1, GRAY);
			for (int y = 0; y < _height; y += 50)
				SandboxGraphics::DrawDottedLine(0, y, _width - 1, y, GRAY);
		}
	}

	bool b = true;

	// Handle events
	SDL_Event e;
	while (SDL_PollEvent(&e))
	{
		if (e.type == SDL_QUIT)
		{
			b = false;
			break;
		}
		PlayerInput::ProccesEvent(e);
	}
	// calc delta time
	auto time = Clock::now() - lastTimePoint;
	float deltaTime = std::chrono::duration<float, std::ratio<1, 1>>(time).count();

	DrawGraphicObjects();

	// call user defined update function
	if (updateEvent) updateEvent(deltaTime);

	// Update Screen
	SDL_RenderPresent(renderer);

	PlayerInput::PopEvents();
	lastTimePoint = Clock::now();

	return b;
}

void SandboxFramework::DrawGraphicObjects()
{
	auto& objs = SandboxGraphics::GraphicsObject::graphicObjects;
	if (objs.empty()) return;

	for (auto it = objs.begin(); it != objs.end();)
	{
		bool res = SandboxGraphics::_TryToDraw(*it);
		if (res)
			++it;
		else
			objs.erase(it++);
	}
}

void SandboxFramework::UpdateFlag(const char* flagName, bool value)
{
	std::string s(flagName);
	if (s == "DRAW_CORDINATE")
		shouldDrawCordinateLines = value;
	else if (s == "STOP_CLEAR")
		shouldClearRenderer = value;
}

void SandboxFramework::BindEvents(initType init, updateType update, cleanupType cleanup)
{
	initEvent = init;
	updateEvent = update;
	cleanupEvent = cleanup;
}