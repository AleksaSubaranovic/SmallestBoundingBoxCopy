#include <SandboxGeometry.h>
#include <SandboxGraphics.h>
#include <PlayerInput.h>
#include <SandboxFramework.h>
#include <fstream>
#include <thread>
using std::string;

SandboxGraphics::GraphicsObjectPtr inputLines[50];
SandboxGraphics::GraphicsObjectPtr polyLines[200];
SandboxGraphics::GraphicsObjectPtr boxLines[4];
std::vector<struct Step> steps;

int currStep;
int lineInd = 0;
int inputId = 0;
int COUNT_2 = 0;
bool isSafe = false;

// convert to pixel oriented point
int cY(int y)
{
	return SandboxFramework::_height - y;
}
// Spam clicking may break program
polygon readPoly(const char* fileName)
{
	std::ifstream file(fileName);

	polygon p;
	float a, b; int n = 0;
	while (file >> a >> b)
	{
		p.pushPoint(point(a * 100, b * 100)); n++;
	}

	for (int i = 0; i < n - 1; i++)
	{
		inputLines[inputId++] = SandboxGraphics::MakeLine(p.points[i].x, cY(p.points[i].y),
			p.points[i + 1].x, cY(p.points[i + 1].y));
	}
	inputLines[inputId++] = SandboxGraphics::MakeLine(p.points[0].x, cY(p.points[0].y),
		p.points.back().x, cY(p.points.back().y));
	inputId++;
	return p;
}

// Simulation step
struct Step
{
	string stepType; // ADD, REMOVE, EDIT, "PHASE2"

	point pA, pB;
	int ind = 0;
	struct ptrTo4Points
	{
		point a, b, c, d;
		ptrTo4Points(point a, point b, point c, point d) : a(a), b(b), c(c), d(d) {};
	};
	ptrTo4Points* ptr = NULL;

	void Do()
	{
		if (stepType == "ADD")
		{
			polyLines[ind] = SandboxGraphics::MakeLine(pA.x, cY(pA.y), pB.x, cY(pB.y), RED);
		}
		else if (stepType == "REMOVE")
		{
			if (polyLines[ind]) polyLines[ind].reset();
		}
		else if (stepType == "EDIT")
		{
			if (!boxLines[0])
			{
				boxLines[0] = SandboxGraphics::MakeLine(ptr->a.x, cY(ptr->a.y), ptr->b.x, cY(ptr->b.y), BLUE, true);
				boxLines[1] = SandboxGraphics::MakeLine(ptr->b.x, cY(ptr->b.y), ptr->c.x, cY(ptr->c.y), BLUE, true);
				boxLines[2] = SandboxGraphics::MakeLine(ptr->c.x, cY(ptr->c.y), ptr->d.x, cY(ptr->d.y), BLUE, true);
				boxLines[3] = SandboxGraphics::MakeLine(ptr->d.x, cY(ptr->d.y), ptr->a.x, cY(ptr->a.y), BLUE, true);
			}
			else
			{
				SET_START(*boxLines[0], ptr->a.x, cY(ptr->a.y)); SET_END(*boxLines[0], ptr->b.x, cY(ptr->b.y));
				SET_START(*boxLines[1], ptr->b.x, cY(ptr->b.y)); SET_END(*boxLines[1], ptr->c.x, cY(ptr->c.y));
				SET_START(*boxLines[2], ptr->c.x, cY(ptr->c.y)); SET_END(*boxLines[2], ptr->d.x, cY(ptr->d.y));
				SET_START(*boxLines[3], ptr->d.x, cY(ptr->d.y)); SET_END(*boxLines[3], ptr->a.x, cY(ptr->a.y));
			}
		}
		else if (stepType == "PHASE2")
		{

		}
	}

	void Undo()
	{
		if (stepType == "ADD")
		{
			if (polyLines[ind]) polyLines[ind].reset();
		}
		else if (stepType == "REMOVE")
		{
			polyLines[ind] = SandboxGraphics::MakeLine(pA.x, cY(pA.y), pB.x, cY(pB.y), RED);
		}
		else if (stepType == "EDIT")
		{
			if (currStep == COUNT_2)
			{
				boxLines[0].reset(); boxLines[1].reset(); boxLines[2].reset(); boxLines[3].reset();
			}
			else
			{
				ptrTo4Points* temp = steps[currStep].ptr;
				SET_START(*boxLines[0], temp->a.x, cY(temp->a.y)); SET_END(*boxLines[0], temp->b.x, cY(temp->b.y));
				SET_START(*boxLines[1], temp->b.x, cY(temp->b.y)); SET_END(*boxLines[1], temp->c.x, cY(temp->c.y));
				SET_START(*boxLines[2], temp->c.x, cY(temp->c.y)); SET_END(*boxLines[2], temp->d.x, cY(temp->d.y));
				SET_START(*boxLines[3], temp->d.x, cY(temp->d.y)); SET_END(*boxLines[3], temp->a.x, cY(temp->a.y));
			}
		}
	}
};

// TRAVEL FROM MinX to MaxX
polygon convexHullS(const polygon& poly)
{
	polygon convexPoly;

	// find point with min X value, that point is always convex hull point
	auto minIt = std::min_element(poly.points.cbegin(), poly.points.cend(),
		[](const point& p1, const point& p2) {return p1.x < p2.x; });

	convexPoly.points.push_back(*minIt);

	// find point with max X value, that point is always convex hull point and end of chain
	auto maxIt = std::min_element(poly.points.cbegin(), poly.points.cend(),
		[](const point& p1, const point& p2) {return p1.x > p2.x; });

	// travek direction
	bool dir = true; // true for xMin to xMax, false for opposite

	// build chain counter clock wise
	// erasing last item in std::vector is O(1)

	// FOR SIMULATION ONLY
	std::vector<int> lineIndexes;

	auto& cpoints = convexPoly.points;

	// first build chain using points right of minIt in points array
	for (auto it = minIt + 1; it < poly.points.cend(); it++)
	{
		while (cpoints.size() > 1 && !isLeft(cpoints[cpoints.size() - 2], cpoints[cpoints.size() - 1], *it))
		{
			// step : REMOVE
			Step s;
			s.stepType = "REMOVE";
			s.pA = cpoints[cpoints.size() - 2];
			s.pB = cpoints[cpoints.size() - 1];
			s.ind = lineIndexes.back();
			lineIndexes.pop_back();
			steps.push_back(s);

			cpoints.pop_back();
		}

		if (dir)
		{
			if (cpoints.back().x > it->x) continue;
		}
		else if (cpoints.back().x < it->x) continue;

		cpoints.push_back(*it);
		
		if (it->x == maxIt->x) dir = false;

		// step : ADDP
		Step s;
		s.stepType = "ADD";
		lineIndexes.push_back(lineInd);
		s.ind = lineInd++;
		s.pA = cpoints[cpoints.size() - 2];
		s.pB = *it;
		steps.push_back(s);
	}

	// rest of points
	for (auto it = poly.points.cbegin(); it < minIt; it++)
	{
		while (cpoints.size() > 1 && !isLeft(cpoints[cpoints.size() - 2], cpoints[cpoints.size() - 1], *it))
		{	
			// step : REMOVE
			Step s;
			s.stepType = "REMOVE";
			s.pA = cpoints[cpoints.size() - 2];
			s.pB = cpoints[cpoints.size() - 1];
			s.ind = lineIndexes.back();
			lineIndexes.pop_back();
			steps.push_back(s);

			cpoints.pop_back();
		}

		if (dir)
		{
			if (cpoints.back().x > it->x) continue;
		}
		else if (cpoints.back().x < it->x) continue;

		cpoints.push_back(*it);

		if (it->x == maxIt->x) dir = false;

		// step : ADDP
		Step s;
		s.stepType = "ADD";
		lineIndexes.push_back(lineInd);
		s.ind = lineInd++;
		s.pA = cpoints[cpoints.size() - 2];
		s.pB = *it;
		steps.push_back(s);
	}

	// step : ADDP
	Step s;
	s.stepType = "ADD";
	lineIndexes.push_back(lineInd);
	s.ind = lineInd++;
	s.pA = cpoints[cpoints.size() - 1];
	s.pB = cpoints[0];
	steps.push_back(s);

	return convexPoly;
}

void RotateCalippersS(polygon& cPoly)
{
	// indexes for critical points
	int minX = 0, maxX = 0, minY = 0, maxY = 0;
	int n = cPoly.pointCount();
	auto& points = cPoly.points; // refrence

	for (int i = 0; i < n; i++) // get critical points
	{
		if (points[i].x < points[minX].x) minX = i;
		if (points[i].x > points[maxX].x) maxX = i;
		if (points[i].y < points[minY].y) minY = i;
		if (points[i].y > points[maxY].y) maxY = i;
	}

	int P1, P2, P3, P4; // indexes of points of rectangle edges
	P1 = minX, P2 = minY, P3 = maxX, P4 = maxY;
	point tempP1, tempP2, tempP3, tempP4; // dots used to calculate lines
	tempP1 = point(points[P1].x, points[P1].y - 1);
	tempP2 = point(points[P2].x + 1, points[P2].y);
	tempP3 = point(points[P3].x, points[P3].y + 1);
	tempP4 = point(points[P4].x - 1, points[P4].y);

	point bestP1, bestP2, bestP3, bestP4; // will store best solution
	float minArea; // best area

	// construct first rectangle (not optimal)
	getRectangleOfLines(points[P1], tempP1, points[P2], tempP2, points[P3], tempP3, points[P4], tempP4,
		bestP1, bestP2, bestP3, bestP4);
	minArea = getDistance(bestP1, bestP2) * getDistance(bestP2, bestP3);

	Step s;
	s.stepType = "EDIT";
	s.ptr = new Step::ptrTo4Points(bestP1, bestP2, bestP3, bestP4);
	steps.push_back(s);

#define NEXTP(P) ((P + 1) % n)

	vector2 v0, v1, v2, v3, v4; // v0 is prev direction of P1, v1 = P1 to NEXT(P1)
	v0 = vector2(0.f, -1.f); // first vector from minX to tempP1

	point p1, p2, p3, p4;

	for (int i = 0; i < n; i++) // rotate around convex polygon
	{
		v1 = points[NEXTP(P1)] - points[P1];
		float angle = angleOfVectors(v0, v1);
		tempP1 = points[NEXTP(P1)]; // rotated line

		RotatePoint(points[P2], tempP2, angle);
		v2 = tempP2 - points[P2];
		while (isLeft(points[P2], points[NEXTP(P2)], tempP2))
		{
			P2 = NEXTP(P2);
			tempP2 = points[P2] + v2;
		}

		v3 = -v1; // opposite direction
		tempP3 = points[P3] + v3;
		while (isLeft(points[P3], points[NEXTP(P3)], tempP3))
		{
			P3 = NEXTP(P3);
			tempP3 = points[P3] + v3;
		}

		v4 = -v2;  // opposite direction
		tempP4 = points[P4] + v4;
		while (isLeft(points[P4], points[NEXTP(P4)], tempP4))
		{
			P4 = NEXTP(P4);
			tempP4 = points[P4] + v4;
		}

		getRectangleOfLines(points[P1], tempP1, points[P2], tempP2, points[P3], tempP3, points[P4], tempP4,
			p1, p2, p3, p4);
		float area = getDistance(p1, p2) * getDistance(p2, p3);
		if (area < minArea)
			bestP1 = p1, bestP2 = p2, bestP3 = p3, bestP4 = p4, minArea = area;

		Step ss;
		ss.stepType = "EDIT";
		ss.ptr = new Step::ptrTo4Points(p1, p2, p3, p4);
		steps.push_back(ss);

		v0 = v1; // last direction
		P1 = NEXTP(P1); // move to next point
	}

	Step sss;
	sss.stepType = "EDIT";
	sss.ptr = new Step::ptrTo4Points(bestP1, bestP2, bestP3, bestP4);
	steps.push_back(sss);

	std::cout << "Min Area: " << minArea << "Box points: " << p1 << ' ' << p2 << ' ' << p3 << ' ' << p4 << '\n';
}

void algorithm(const polygon& p) // bounding box of polygon p, 4 temena boxa u O(n)
{
	// verzija bez simulacije u Sandbox/SandboxGeometry.cpp
	// convex hull poligona O(n)
	polygon cPoly = convexHullS(p);
	
	Step s;
	s.stepType = "PHASE2";
	steps.push_back(s);
	COUNT_2 = steps.size() - 1;
	RotateCalippersS(cPoly);

	currStep = -1;
}

// User input:
// 1,2,3 read file with polygon data
// space => next, space + shift => skips faster
// backspace => prev step, + shift for faster skip
//
void onSpace(bool isPressed, bool isRepeated, Uint32 timestamp)
{
	if (!isPressed || (isRepeated && !IS_SHIFT())) return;
	if (!isSafe) return;
	
	currStep++;
	if (currStep >= steps.size()) currStep--;
	else steps[currStep].Do();
}

void onBackspace(bool isPressed, bool isRepeated, Uint32 timestamp)
{
	if (!isPressed || (isRepeated && !IS_SHIFT())) return;
	if (!isSafe) return;

	if (currStep == 0) return;
	steps[currStep--].Undo();
}

void onOne(bool isPressed, bool isRepeated, Uint32 timestamp)
{
	if (isRepeated || !isPressed) return;

	for (int i = 0; i < inputId; i++)
		inputLines[i].reset();

	for (auto s : steps)
	{
		if (s.ptr != NULL)
			delete s.ptr;
	}
	if (steps.size()) steps.erase(steps.begin(), steps.end());

	for (int i = 0; i < lineInd; i++)
		if (polyLines[i])
			polyLines[i].reset();

	for (int i = 0; i < 4; i++)
		if (boxLines[i])
			boxLines[i].reset();

	polygon p = readPoly("FirstPoly.txt");
	algorithm(p);
	isSafe = true;
}

void onTwo(bool isPressed, bool isRepeated, Uint32 timestamp)
{
	if (isRepeated || !isPressed) return;

	for (int i = 0; i < inputId; i++)
		inputLines[i].reset();

	for (auto s : steps)
	{
		if (s.ptr != NULL)
			delete s.ptr;
	}
	if (steps.size()) steps.erase(steps.begin(), steps.end());

	for (int i = 0; i < lineInd + 1; i++)
		if (polyLines[i])
			polyLines[i].reset();
	for (int i = 0; i < 4; i++)
		if (boxLines[i])
			boxLines[i].reset();

	polygon p = readPoly("SecondPoly.txt");
	algorithm(p);
	isSafe = true;
}

void onThree(bool isPressed, bool isRepeated, Uint32 timestamp)
{
	if (isRepeated || !isPressed) return;

	for (int i = 0; i < inputId; i++)
		inputLines[i].reset();

	for (auto s : steps)
	{
		if (s.ptr != NULL)
			delete s.ptr;
	}
	if (steps.size()) steps.erase(steps.begin(), steps.end());

	for (int i = 0; i < lineInd + 1; i++)
		if (polyLines[i])
			polyLines[i].reset();
	for (int i = 0; i < 4; i++)
		if (boxLines[i])
			boxLines[i].reset();

	polygon p = readPoly("ThirdPoly.txt");
	algorithm(p);
	isSafe = true;
}
//
// User Input

void SimAlgorithm()
{
	// bind keyboard events, nebitno za algoritam
	PlayerInput::BindKeyEvent(KEY_(SPACE), onSpace);
	PlayerInput::BindKeyEvent(KEY_(BACKSPACE), onBackspace);
	PlayerInput::BindKeyEvent(KEY_(1), onOne);
	PlayerInput::BindKeyEvent(KEY_(2), onTwo);
	PlayerInput::BindKeyEvent(KEY_(3), onThree);
}

void CleanAll()
{
	for (int i = 0; i < inputId; i++)
		inputLines[i].reset();

	for (auto s : steps)
	{
		if (s.ptr != NULL)
			delete s.ptr;
	}
	if (steps.size()) steps.erase(steps.begin(), steps.end());

	for (int i = 0; i < lineInd + 1; i++)
		if (polyLines[i])
			polyLines[i].reset();
	for (int i = 0; i < 4; i++)
		if (boxLines[i])
			boxLines[i].reset();
}