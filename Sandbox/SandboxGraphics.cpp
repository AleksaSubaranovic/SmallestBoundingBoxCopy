#include "SandboxGraphics.h"
#include <algorithm>
#include <utility>
#include "Generic.h"

using namespace SandboxFramework;

void SandboxGraphics::DrawLine(int x1, int y1, int x2, int y2, color c)
{
	if (SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a) ||
		SDL_RenderDrawLine(renderer, x1, y1, x2, y2))
		PRINT_SDL_ERROR();
}

void SandboxGraphics::DrawRectangle(int x, int y, int width, int height, color c)
{
	SDL_Rect rect = { x, y, width, height };

	if (SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a) ||
		SDL_RenderDrawRect(renderer, &rect))
		PRINT_SDL_ERROR();
}

void SandboxGraphics::DrawRectangle(SDL_Rect* rect, color c)
{
	if (SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a) ||
		SDL_RenderDrawRect(renderer, rect))
		PRINT_SDL_ERROR();
}

void SandboxGraphics::FillRectangle(int x, int y, int width, int height, color c)
{
	SDL_Rect rect = { x, y, width, height };

	if (SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a) ||
		SDL_RenderFillRect(renderer, &rect))
		PRINT_SDL_ERROR();
}

void SandboxGraphics::FillRectangle(SDL_Rect* rect, color c)
{
	if (SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a) ||
		SDL_RenderFillRect(renderer, rect))
		PRINT_SDL_ERROR();
}

void SandboxGraphics::DrawPoint(int x, int y, color c)
{
	if (SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a) ||
		SDL_RenderDrawPoint(renderer, x, y))
		PRINT_SDL_ERROR();
}

/* Cohen-Sutherland line clipping algorithm constants */
#define __cohsuth_INSIDE 0     /* 0000 */
#define __cohsuth_LEFT 1   /* 0001 */
#define __cohsuth_RIGHT 2     /* 0010 */
#define __cohsuth_BOTTOM 4     /* 0100 */
#define __cohsuth_TOP 8     /* 1000 */

int SandboxGraphics::__compute_outcode(int x, int y)
{
	int xmin = 0, ymin = 0, xmax = _width - 1, ymax = _height - 1;
	int code = __cohsuth_INSIDE;
	if (x < xmin)
	{
		code |= __cohsuth_LEFT;
	}
	else if (x > xmax)
	{
		code |= __cohsuth_RIGHT;
	}

	if (y < ymin)
	{
		code |= __cohsuth_BOTTOM;
	}
	else if (y > ymax)
	{
		code |= __cohsuth_TOP;
	}

	return code;
}

void SandboxGraphics::DrawDottedLine(int x1, int y1, int x2, int y2, color c)
{
	if (SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a))
	{
		PRINT_SDL_ERROR();
		return;
	}
	int counter = 0;
	{
		int xmin = 0, ymin = 0, xmax = _width, ymax = _height - 1;
		int outcode0 = __compute_outcode(x1, y1);
		int outcode1 = __compute_outcode(x2, y2);
		int accept = 0;
		int xnew = 0, ynew = 0;
		int outside_outcode;

		while (1)
		{
			if (!(outcode0 | outcode1))
			{
				/* trivial */
				accept = 1;
				break;
			}
			else if (outcode0 & outcode1)
			{
				/* trivial */
				break;
			}
			else
			{
				outside_outcode = outcode0 ? outcode0 : outcode1;
				if (outside_outcode & __cohsuth_TOP)
				{
					xnew = x1 + (x2 - x1) * (ymax - y1) / (y2 - y1);
					ynew = ymax;
				}
				else if (outside_outcode & __cohsuth_BOTTOM)
				{
					xnew = x1 + (x2 - x1) * (ymin - y1) / (y2 - y1);
					ynew = ymin;
				}
				else if (outside_outcode & __cohsuth_RIGHT)
				{
					ynew = y1 + (y2 - y1) * (xmax - x1) / (x2 - x1);
					xnew = xmax;
				}
				else if (outside_outcode & __cohsuth_LEFT)
				{
					ynew = y1 + (y2 - y1) * (xmin - x1) / (x2 - x1);
					xnew = xmin;
				}

				if (outside_outcode == outcode0)
				{
					x1 = xnew;
					y1 = ynew;
					outcode0 = __compute_outcode(x1, y1);
				}
				else
				{
					x2 = xnew;
					y2 = ynew;
					outcode1 = __compute_outcode(x2, y2);
				}
			}
		}

		if (!accept)
			return;
		x1 = SDL_clamp(x1, 0, xmax);
		y1 = SDL_clamp(y1, 0, ymax);
		x2 = SDL_clamp(x2, 0, xmax);
		y2 = SDL_clamp(y2, 0, ymax);

		/* printf("---\nx0: %d\ny0: %d\nx1: %d\ny1: %d\n", x1, y1, x2, y2); */

		int dx = SDL_abs((x2 - x1)), sx = x1 < x2 ? 1 : -1;
		int dy = -SDL_abs((y2 - y1)), sy = y1 < y2 ? 1 : -1;
		int err = dx + dy, e2; /* error value e_xy */

		while (1)
		{
			counter = (counter + 1) % 10;
			if (counter < 5)
			{
				int res = SDL_RenderDrawPoint(renderer, x1, y1); // color pixel
				if (res)
				{
					PRINT_SDL_ERROR();
					return;
				}
			}
			if (x1 == x2 && y1 == y2) break;
			e2 = 2 * err;
			if (e2 >= dy) { err += dy; x1 += sx; } /* e_xy+e_x > 0 */
			if (e2 <= dx) { err += dx; y1 += sy; } /* e_xy+e_y < 0 */
		}

	}
}

void SandboxGraphics::DrawCircle(int cx, int cy, int r, color c)
{
	if (SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a))
	{
		PRINT_SDL_ERROR();
		return;
	}

	int x = r - 1;
	int y = 0;
	int dx = 1;
	int dy = 1;
	int err = dx - (r << 1);
	int res = 0;
	while (x >= y)
	{

		SDL_RenderDrawPoint(renderer, cx + x, cy + y);
		SDL_RenderDrawPoint(renderer, cx + y, cy + x);
		SDL_RenderDrawPoint(renderer, cx - y, cy + x);
		SDL_RenderDrawPoint(renderer, cx - x, cy + y);
		SDL_RenderDrawPoint(renderer, cx - x, cy - y);
		SDL_RenderDrawPoint(renderer, cx - y, cy - x);
		SDL_RenderDrawPoint(renderer, cx + y, cy - x);
		SDL_RenderDrawPoint(renderer, cx + x, cy - y);

		if (err <= 0)
		{
			y++;
			err += dy;
			dy += 2;
		}
		if (err > 0)
		{
			x--;
			dx += 2;
			err += dx - (r << 1);
		}
	}
}

void SandboxGraphics::FillCircle(int cx, int cy, int r, color c)
{
	if (SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a))
	{
		PRINT_SDL_ERROR();
		return;
	}

	const int n = 2 * r;
	std::pair<Uint16, Uint16>* arr = new std::pair<Uint16, Uint16>[n];

	const int offset = cy - r;
	int x = r - 1;
	int y = 0;
	int dx = 1;
	int dy = 1;
	int err = dx - (r << 1);

	while (x >= y)
	{

		SDL_RenderDrawPoint(renderer, cx + x, cy + y);
		SDL_RenderDrawPoint(renderer, cx + y, cy + x);
		SDL_RenderDrawPoint(renderer, cx - y, cy + x);
		SDL_RenderDrawPoint(renderer, cx - x, cy + y);
		SDL_RenderDrawPoint(renderer, cx - x, cy - y);
		SDL_RenderDrawPoint(renderer, cx - y, cy - x);
		SDL_RenderDrawPoint(renderer, cx + y, cy - x);
		SDL_RenderDrawPoint(renderer, cx + x, cy - y);

		arr[cy + y - offset].first = cx - x;
		arr[cy + y - offset].second = cx + x;

		arr[cy + x - offset].first = cx - y;
		arr[cy + x - offset].second = cx + y;

		arr[cy - y - offset].first = cx - x;
		arr[cy - y - offset].second = cx + x;

		arr[cy - x - offset].first = cx - y;
		arr[cy - x - offset].second = cx + y;

		if (err <= 0)
		{
			y++;
			err += dy;
			dy += 2;
		}
		if (err > 0)
		{
			x--;
			dx += 2;
			err += dx - (r << 1);
		}
	}


	for (int i = 0; i < n; i++)
		for (int j = arr[i].first; j <= arr[i].second; j++)
			SDL_RenderDrawPoint(renderer, j, i + offset);

	delete[] arr;
}

// GO_PTR => GRAPHICS OBJECT PTR (std::shared_ptr)
using GO_PTR = SandboxGraphics::GraphicsObjectPtr;
// GO_PTR_WEAK => GRAPHICS OBJECT PTR WEAK (std::weak_ptr)
using GO_PTR_WEAK = SandboxGraphics::GraphicsObjectPtrWeak;

std::list< GO_PTR_WEAK> SandboxGraphics::GraphicsObject::graphicObjects = std::list< GO_PTR_WEAK>();

GO_PTR SandboxGraphics::MakeLine(int startX, int startY, int endX, int endY, color c, bool isDotted)
{
	GO_PTR gPtr = GO_PTR(new GraphicsObject());
	gPtr->type = SandboxGraphics::GraphicsObjectType::LINE;
	gPtr->SetParams(startX, startY, endX, endY);
	gPtr->SetDotted(isDotted);
	gPtr->SetColor(c);

	SandboxGraphics::GraphicsObject::graphicObjects.push_back(gPtr); // gPtr is copied into weak refrence
	return gPtr;
}

GO_PTR SandboxGraphics::MakeRectangle(int X, int Y, int width, int height, color c, bool isDotted)
{
	GO_PTR gPtr = GO_PTR(new GraphicsObject());
	gPtr->type = SandboxGraphics::GraphicsObjectType::RECTANGLE;
	gPtr->SetParams(X, Y, width, height);
	gPtr->SetDotted(isDotted);
	gPtr->SetColor(c);

	SandboxGraphics::GraphicsObject::graphicObjects.push_back(gPtr); // gPtr is copied into weak refrence
	return gPtr;
}

GO_PTR SandboxGraphics::MakeCircle(int X, int Y, int radius, color c, bool isDotted)
{
	GO_PTR gPtr = GO_PTR(new GraphicsObject());
	gPtr->type = SandboxGraphics::GraphicsObjectType::CIRCLE;
	gPtr->SetParams(X, Y, radius, 0);
	gPtr->SetDotted(isDotted);
	gPtr->SetColor(c);

	SandboxGraphics::GraphicsObject::graphicObjects.push_back(gPtr); // gPtr is copied into weak refrence
	return gPtr;
}

bool SandboxGraphics::_TryToDraw(GraphicsObjectPtrWeak gPtrWeak)
{
	if (GO_PTR gPtr = gPtrWeak.lock())
	{
		switch (gPtr->GetType())
		{
		case GraphicsObjectType::LINE:
			if (gPtr->IsDotted())
				DrawDottedLine(gPtr->GetParam1(), gPtr->GetParam2(), gPtr->GetParam3(), gPtr->GetParam4(), gPtr->GetColor());
			else
				DrawLine(gPtr->GetParam1(), gPtr->GetParam2(), gPtr->GetParam3(), gPtr->GetParam4(), gPtr->GetColor());
			break;
		case GraphicsObjectType::RECTANGLE:
			if (gPtr->IsDotted())
			{} // to do
			else if (gPtr->IsFilled())
				FillRectangle(gPtr->GetParam1(), gPtr->GetParam2(), gPtr->GetParam3(), gPtr->GetParam4(), gPtr->GetColor());
			else
				DrawRectangle(gPtr->GetParam1(), gPtr->GetParam2(), gPtr->GetParam3(), gPtr->GetParam4(), gPtr->GetColor());
			break;
		case GraphicsObjectType::CIRCLE:
			if (gPtr->IsDotted())
			{
			} // to do
			else if (gPtr->IsDotted())
				FillCircle(gPtr->GetParam1(), gPtr->GetParam2(), gPtr->GetParam3(), gPtr->GetColor());
			else
				DrawCircle(gPtr->GetParam1(), gPtr->GetParam2(), gPtr->GetParam3(), gPtr->GetColor());
		}
		return true;
	}
	else return false;
}

SandboxGraphics::GraphicsObject::GraphicsObject()
{
	c = WHITE;
	type = GraphicsObjectType::NONE;
	isDotted = false;
	isFilled = false;
	params[0] = params[1] = params[2] = params[3] = 0;
}

void SandboxGraphics::GraphicsObject::SetDotted(bool isDotted)
{
	this->isDotted = isDotted;
}

void SandboxGraphics::GraphicsObject::SetColor(color color)
{
	this->c = color;
}

void SandboxGraphics::GraphicsObject::SetFill(bool isFilled)
{
	this->isFilled = isFilled;
}

void SandboxGraphics::GraphicsObject::SetParam(int value, int ind)
{
	this->params[SDL_clamp(ind, 0, 3)] = value;
}

void SandboxGraphics::GraphicsObject::SetParam12(int x, int y)
{
	this->params[0] = x; this->params[1] = y;
}

void SandboxGraphics::GraphicsObject::SetParam34(int x, int y)
{
	this->params[2] = x; this->params[3] = y;
}

void SandboxGraphics::GraphicsObject::SetParams(int x1, int y1, int x2, int y2)
{
	this->params[0] = x1, this->params[1] = y1, this->params[2] = x2, this->params[3] = y2;
}
