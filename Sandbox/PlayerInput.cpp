#include "PlayerInput.h"
#include "Generic.h"
#include <SDL.h>

PlayerInput::PlayerInput() {}

queue<SDL_Event> PlayerInput::eventQueue = std::queue<SDL_Event>();

unordered_map<SDL_Keycode, KeyEventFunction> PlayerInput::keyEventMap = unordered_map<SDL_Keycode, KeyEventFunction>();

int PlayerInput::xPos = 0;
int PlayerInput::yPos = 0;

void PlayerInput::_ProcessMouseMotionEvent(SDL_Event& e)
{
	SDL_GetMouseState(&xPos, &yPos);
}

void PlayerInput::_ProccessMouseWheelEvent(SDL_Event& e)
{
}

void PlayerInput::_ProccesMouseButtonEvent(SDL_Event& e)
{
}

void PlayerInput::_ProccessKeyboardEvent(SDL_Event& e)
{
	// Call binded event if there is such
	auto keyMapIt = keyEventMap.find(e.key.keysym.sym);
	if (keyMapIt != keyEventMap.end()) // found event
	{
		keyMapIt->second(e.key.state == SDL_PRESSED, e.key.repeat, e.key.timestamp);
	}
}

// public
void PlayerInput::ProccesEvent(SDL_Event e)
{
	PlayerInput::eventQueue.push(e);

	switch (e.type)
	{
	case SDL_KEYDOWN: case SDL_KEYUP:
		_ProccessKeyboardEvent(e);
		break;
	case SDL_MOUSEBUTTONDOWN: case SDL_MOUSEBUTTONUP:
		_ProccesMouseButtonEvent(e);
		break;
	case SDL_MOUSEWHEEL :
		_ProccessMouseWheelEvent(e);
		break;
	case SDL_MOUSEMOTION:
		_ProcessMouseMotionEvent(e);
		break;
	default:
		break;
	}

}

void PlayerInput::PopEvents()
{
	while (!eventQueue.empty())
		eventQueue.pop();
}

queue<SDL_Event>& PlayerInput::GetAllEvents()
{
	return eventQueue;
}

void PlayerInput::BindKeyEvent(SDL_Keycode keyCode, KeyEventFunction f)
{
	try
	{
		//Returns a pair consisting of an iterator to the inserted element (or to the element that prevented the insertion) 
		// and a bool denoting whether the insertion took place (true if insertion happened, false if it did not). 
		bool isInsertedInKeyMap = keyEventMap.insert({ keyCode, f }).second;
		WARN_IF(not isInsertedInKeyMap);
	}
	catch (std::exception e)
	{
		PRINT_ERROR(e.what());
	}
}

void PlayerInput::UnbindKeyEvent(SDL_Keycode keyCode)
{
	try
	{
		bool isKeyEventBinded = keyEventMap.find(keyCode) == keyEventMap.end();
		WARN_IF(isKeyEventBinded);
	}
	catch (std::exception e)
	{
		PRINT_ERROR(e.what());
	}
}
