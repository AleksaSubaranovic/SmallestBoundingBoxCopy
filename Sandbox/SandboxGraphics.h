#pragma once

#include "Generic.h" // has sdl

#define GET_START_X(graphicsObj) (graphicsObj).GetParam1()
#define GET_START_Y(graphicsObj) (graphicsObj).GetParam2()
#define GET_END_X(graphicsObj) (graphicsObj).GetParam3()
#define GET_END_Y(graphicsObj) (graphicsObj).GetParam4()
#define GET_X(graphicsObj) (graphicsObj).GetParam1()
#define GET_Y(graphicsObj) (graphicsObj).GetParam2()
#define GET_WIDTH(graphicsObj) (graphicsObj).GetParam3()
#define GET_HEIGHT(graphicsObj) (graphicsObj).GetParam4()
#define GET_RADIUS(graphicsObj) (graphicsObj).GetParam3()

#define SET_START_X(graphicsObj, x) (graphicsObj).SetParam(x, 0)
#define SET_START_Y(graphicsObj, y) (graphicsObj).SetParam(y, 1)
#define SET_END_X(graphicsObj, x) (graphicsObj).SetParam(x, 2)
#define SET_END_Y(graphicsObj, y) (graphicsObj).SetParam(y, 3)
#define SET_X(graphicsObj, x) (graphicsObj).SetParam(x, 0)
#define SET_Y(graphicsObj, y) (graphicsObj).SetParam(y, 1)
#define SET_WIDTH(graphicsObj, w) (graphicsObj).SetParam(w, 2)
#define SET_HEIGHT(graphicsObj, h) (graphicsObj).SetParam(h, 3)
#define SET_RADIUS(graphicsObj, r) (graphicsObj).SetParam(r, 2)

#define SET_START(graphicsObj, x, y) (graphicsObj).SetParam12(x, y)
#define SET_END(graphicsObj, x, y) (graphicsObj).SetParam34(x, y)

#define SET_POINT(graphicsObj, x, y) (graphicsObj).SetParam12(x, y)
#define SET_WIDTH_HEIGHT(graphicsObj, w, h) (graphicsObj).SetParam34(w, h)

// y1, _y1, usde by sdl...

#define SET_LINE(graphicsObj, _x1, __y1, _x2, _y2) (graphicsObj).SetParams(_x1, __y1, _x2, _y2)
#define SET_RECTANGLE(graphicsObj, _x1, __y1, w, h) (graphicsObj).SetParams(_x1, __y1, w, h)
#define SET_CIRCLE(graphicsObj, _x1, __y1, r) (graphicsObj).SetParams(_x1, __y1, r, 0)

namespace SandboxFramework
{
	// renderer
	extern SDL_Renderer* renderer; // gpu
	extern int _width, _height;
	void DrawGraphicObjects();
}

namespace SandboxGraphics
{
	//
	// Draw Functions
	//

	void DrawLine(int x1, int y1, int x2, int y2, color c);

	void DrawRectangle(int x, int y, int width, int height, color c);

	void DrawRectangle(SDL_Rect* rect, color c);

	void FillRectangle(int x, int y, int width, int height, color c);

	void FillRectangle(SDL_Rect* rect, color c);

	void DrawPoint(int x, int y, color c);

	int __compute_outcode(int x, int y);

	void DrawDottedLine(int x1, int y1, int x2, int y2, color c);

	// Draw Circle and Fill Circle do not check for pixel errors

	void DrawCircle(int cx, int cy, int r, color c);

	void FillCircle(int cx, int cy, int r, color c);

	//
	// Graphics Objects 2D

	// Pointer to Graphic objects
	typedef std::shared_ptr<class GraphicsObject> GraphicsObjectPtr;
	typedef std::weak_ptr<class GraphicsObject> GraphicsObjectPtrWeak;

	// Construct Graphics object (line) and returns Shared refrence to it
	GraphicsObjectPtr MakeLine(int startX, int startY, int endX, int endY, color c = BLACK, bool isDotted = false);
	// Construct Graphics object (rectangle) and returns Shared refrence to it
	GraphicsObjectPtr MakeRectangle(int X, int Y, int width, int height, color c = BLACK, bool isDotted = false);
	// Construct Graphics object (circle) and returns Shared refrence to it
	GraphicsObjectPtr MakeCircle(int X, int Y, int radius, color c = BLACK, bool isDotted = false);

	enum class GraphicsObjectType : uint8_t
	{
		NONE, LINE, RECTANGLE, CIRCLE
	};

	class GraphicsObject
	{
		static std::list<GraphicsObjectPtrWeak> graphicObjects;
		GraphicsObjectType type;
		bool isDotted;
		color c;
		bool isFilled;
		int params[4]; // 4 is enough

		GraphicsObject();
	public:

		// Use macros instead
		inline int GetParam1() const { return params[0]; }
		// Use macros instead
		inline int GetParam2() const { return params[1]; }
		// Use macros instead
		inline int GetParam3() const { return params[2]; }
		// Use macros instead
		inline int GetParam4() const { return params[3]; }

		inline GraphicsObjectType GetType() const { return type; }
		inline color GetColor() const { return c; }
		inline bool IsDotted() const { return isDotted; }
		inline bool IsFilled() const { return isFilled; }

		void SetDotted(bool isDotted);
		void SetColor(color color);
		void SetFill(bool isFilled);
		// Use macros instead
		void SetParam(int value, int ind);
		void SetParam12(int x, int y);
		void SetParam34(int x, int y);
		void SetParams(int x1, int y1, int x2, int y2);

		friend GraphicsObjectPtr MakeLine(int startX, int startY, int endX, int endY, color c, bool isDotted);
		friend GraphicsObjectPtr MakeRectangle(int X, int Y, int width, int height, color c, bool isDotted);
		friend GraphicsObjectPtr MakeCircle(int X, int Y, int radius, color c, bool isDotted);

		friend void SandboxFramework::DrawGraphicObjects();
	};

	bool _TryToDraw(GraphicsObjectPtrWeak gPtrWeak);
}

