#pragma once

#include <SDL.h>
#include <stdio.h>
#include <iostream>
#include <memory> // std
#include <vector> // std
#include <list> // std

#define TO_STRING(text) #text
// X TO STRING for macro expansion
#define X_TO_STRING(X) TO_STRING(X)
#define WARN_IF(condition) {if (condition) fprintf(stderr, "Warning: " #condition "\n");}
#define CONCAT_TO_STRING(x, y) #x #y
#define CONCAT(x, y) x##y

#define PRINT_SDL_ERROR() fprintf(stderr, "SDL Error: %s\n", SDL_GetError())
#define PRINT_ERROR(s) fprintf(stderr, "%s \n", s)

/**
*  Get Timestamp of event e in milliseconds
*/
inline Uint32 GetEventTimestamp(const SDL_Event& e)
{
	switch (e.type)
	{
	case SDL_DISPLAYEVENT:
		return e.display.timestamp;
	case SDL_WINDOWEVENT:
		return e.window.timestamp;
	case SDL_KEYDOWN: case SDL_KEYUP:
		return e.key.timestamp;
	case SDL_TEXTEDITING:
		return e.edit.timestamp;
	case SDL_TEXTINPUT:
		return e.text.timestamp;
	case SDL_MOUSEMOTION:
		return e.motion.timestamp;
	case SDL_MOUSEWHEEL:
		return e.wheel.timestamp;
	case SDL_MOUSEBUTTONDOWN : case SDL_MOUSEBUTTONUP:
		return e.button.timestamp;
	case SDL_JOYAXISMOTION:
		return e.jaxis.timestamp;
	case SDL_JOYBALLMOTION:
		return e.jball.timestamp;
	case SDL_JOYHATMOTION:
		return e.jhat.timestamp;
	case SDL_JOYBUTTONDOWN : case SDL_JOYBUTTONUP:
		return e.jbutton.timestamp;
	case SDL_JOYDEVICEADDED : case SDL_JOYDEVICEREMOVED: 
		return e.jdevice.timestamp;
	case SDL_JOYBATTERYUPDATED :
		return e.jbattery.timestamp;
	case SDL_CONTROLLERAXISMOTION:
		return e.caxis.timestamp;
	case SDL_CONTROLLERBUTTONDOWN : case SDL_CONTROLLERBUTTONUP:
		return e.cbutton.timestamp;
	case SDL_CONTROLLERDEVICEADDED: case SDL_CONTROLLERDEVICEREMOVED: case SDL_CONTROLLERDEVICEREMAPPED:
		return e.cdevice.timestamp;
	case SDL_CONTROLLERSENSORUPDATE:
		return e.csensor.timestamp;
	case SDL_CONTROLLERTOUCHPADDOWN: case SDL_CONTROLLERTOUCHPADMOTION: // ? all events ?
		return e.ctouchpad.timestamp;
	case SDL_AUDIODEVICEADDED : case SDL_AUDIODEVICEREMOVED:
		return e.adevice.timestamp;
	case SDL_QUIT:
		return e.quit.timestamp;
	case SDL_USEREVENT:
		return e.user.timestamp;
	case SDL_SYSWMEVENT:
		return e.syswm.timestamp;
	case SDL_FINGERMOTION: case SDL_FINGERDOWN : case SDL_FINGERUP :
		return e.tfinger.timestamp;
	case SDL_MULTIGESTURE:
		return e.mgesture.timestamp;
	case SDL_DOLLARGESTURE : case SDL_DOLLARRECORD:
		return e.dgesture.timestamp;
	case SDL_DROPFILE : case SDL_DROPTEXT : case SDL_DROPBEGIN : case SDL_DROPCOMPLETE:
		return e.drop.timestamp;
	//defualt:
	//	return e.common.timestamp;
	}
}
#define IS_KEYDOWN_EVENT(e) e.type == SDL_KEYDOWN
#define IS_KEYUP_EVENT(e) e.type == SDL_KEYUP
#define KEY_SYMBOL_EVENT(e) e.key.keysym.sym
#define KEY_(code) SDLK_ ## code

#define IS_LSHIFT() (SDL_GetModState() & SDL_Keymod::KMOD_LSHIFT)
#define IS_RSHIFT() (SDL_GetModState() & SDL_Keymod::KMOD_RSHIFT)
#define IS_LCTRL() (SDL_GetModState() & SDL_Keymod::KMOD_LCTRL)
#define IS_RCTRL() (SDL_GetModState() & SDL_Keymod::KMOD_RCTRL)
#define IS_LALT() (SDL_GetModState() & SDL_Keymod::KMOD_LALT)
#define IS_RALT() (SDL_GetModState() & SDL_Keymod::KMOD_RALT)
#define IS_CAPS() (SDL_GetModState() & SDL_Keymod::KMOD_CAPS)
#define IS_SHIFT() (SDL_GetModState() & SDL_Keymod::KMOD_SHIFT)
#define IS_CTRL() (SDL_GetModState() & SDL_Keymod::KMOD_CTRL)
#define IS_ALT() (SDL_GetModState() & SDL_Keymod::KMOD_ALT)

// Less useful
#define IS_MB_UP_EVENT(e) e.type == SDL_MOUSEBUTTONUP
#define IS_MB_DOWN_EVENT(e) e.type == SDL_MOUSEBUTTONDOWN
#define MB_BUTTON_EVENT(e) e.button

#define IS_MOUSE_MOTION_EVENT(e) e.type == SDL_MOUSEMOTION
#define MOUSE_MOTION_EVENT(e) e.motion

#define IS_MOUSE_WHEEL_EVENT(e) e.type == SDL_MOUSEWHEEL
#define MOUSE_WHEEL_EVENT(e) e.wheel
//

/*
* TODO: Window Events
*/


typedef SDL_Color color;
// Colors // const => internal linkage, but inline
const color RED{ 255,0,0,255 };
const color GREEN{ 0,255,0,255 };
const color BLUE{ 0,0,255,255 };
const color BLACK{ 0,0,0,255 };
const color WHITE{ 255,255,255,255 };
const color YELLOW{ 255,255,0,255 };
const color MAGENTA{ 255,0,255,255 };
const color CYAN{ 0,255,255,255 };
const color GRAY{ 128,128,128,255 };

inline color RGB(Uint8 r, Uint8 g, Uint8 b) { color c{ r,g,b,255 }; return c; }
