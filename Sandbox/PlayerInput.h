#pragma once

#include <queue>
#include <unordered_map>
#include <functional>
#include <SDL_events.h>

using std::queue;
using std::unordered_map;

// friend function, Framework can call private functions and only Framework
namespace SandboxFramework { bool sandboxUpdate(); };

/**
  Function is of type : void function_name (bool isPressed, bool isRepeat, Uint32 timestamp)
*/
typedef std::function<void(bool, bool, Uint32)> KeyEventFunction;

class PlayerInput
{
	PlayerInput();// private constructor, can't create instance
	
	static queue<SDL_Event> eventQueue;
	/*
	  Map of events, keys are keyboard Keycodes
	*/
	static unordered_map<SDL_Keycode, KeyEventFunction> keyEventMap;
	//
	// no joystick, controller or text input/edit implemented yet
	//

	static void _ProcessMouseMotionEvent(SDL_Event& e);

	static void _ProccessMouseWheelEvent(SDL_Event& e);

	static void _ProccesMouseButtonEvent(SDL_Event& e);

	static void _ProccessKeyboardEvent(SDL_Event& e);

	static void ProccesEvent(SDL_Event e); // called by Framework (friend class)

	static void PopEvents(); // called by Framework

	static int xPos, yPos;

public:
	
	static queue<SDL_Event>& GetAllEvents();
	
	// Bind event to key
	// Keycode is Keyboard key
	// Function is of type : void function_name (bool isPressed, bool isRepeat, Uint32 timestamp)
	static void BindKeyEvent(SDL_Keycode keyCode, KeyEventFunction f);

	// Unbind event from key keyCode
	static void UnbindKeyEvent(SDL_Keycode keyCode);

	// Get Mouse X cordinate
	inline static int GetMouseX() { return xPos; }
	// Get Mouse Y cordinate
	inline static int GetMouseY() { return yPos; }

	friend bool SandboxFramework::sandboxUpdate();
};

