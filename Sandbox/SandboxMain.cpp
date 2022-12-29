//Using SDL and standard IO
#include <SDL.h>
#include "SandboxFramework.h"
#include <stdio.h>
#include <iostream>
#include "Generic.h"
#include "PlayerInput.h"
#include "SandboxGraphics.h"
#include <thread>
#include "SandboxGeometry.h"

//Screen dimension constants
const int SCREEN_WIDTH = 720;
const int SCREEN_HEIGHT = 720;

using namespace SandboxFramework;

int main(int argc, char* args[])
{
	sandboxInit(SCREEN_WIDTH, SCREEN_HEIGHT);

	while (sandboxUpdate())
	{
	}

	sandboxCleanup();

	return 0;
}

