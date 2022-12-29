#include "SandboxGeometry.h"
#include <algorithm>
#include <tuple>

std::ostream& operator<<(std::ostream& os, const vector2& v)
{
	os << v.x << ' ' << v.y;
	return os;
}
std::istream& operator>>(std::istream& is, vector2& v)
{
	is >> v.x;
	is >> v.y;

	return is;
}

void vector2::normalize()
{
	const float len = sqrtf(x * x + y * y);
	x /= len, y /= len;
}

float vector2::length() const
{
	return sqrtf(x * x + y * y);
}

void vector2::rotate(float angle)
{
	point origin(0.f, 0.f);
	point p(x, y);

	const float cosA = cos(angle);
	const float sinA = sin(angle);

	x = origin.x + (p.x - origin.x) * cosA - (p.y - origin.y) * sinA;
	y = origin.y + (p.x - origin.x) * sinA + (p.y - origin.y) * cosA;
}

float vectorLength(const vector2& v1)
{
	return sqrtf(v1.x * v1.x + v1.y * v1.y);
}

float angleOfVectors(const vector2& v1, const vector2& v2)
{
	return acosf(dotProduct(v1, v2) / (vectorLength(v1) * vectorLength(v2)));
}

vector2 normalized(const vector2& v1)
{
	const float len = sqrtf(v1.x * v1.x + v1.y * v1.y);
	return vector2(v1.x / len, v1.y / len);
}

void RotatePoint(const point& origin, point& pointToRotate, const float angle)
{
	const float cosA = cos(angle);
	const float sinA = sin(angle);
	float xx, yy;
	xx = origin.x + (pointToRotate.x - origin.x) * cosA - (pointToRotate.y - origin.y) * sinA;
	yy = origin.y + (pointToRotate.x - origin.x) * sinA + (pointToRotate.y - origin.y) * cosA;

	pointToRotate.x = xx, pointToRotate.y = yy;
}

float getDistance(const point& p1, const point& p2)
{
	return sqrtf((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));
}

std::pair<point, bool> getLineIntersection(const point& line1_1, const point& line1_2, const point& line2_1, const point& line2_2)
{
	float x1 = line1_1.x, x2 = line1_2.x, x3 = line2_1.x, x4 = line2_2.x;
	float y1 = line1_1.y, y2 = line1_2.y, y3 = line2_1.y, y4 = line2_2.y;

	float d = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
	// If d is zero, there is no intersection
	if (fabs(d) < 1e-6) return std::pair<point, bool>({ point(0,0), false });

	float temp1 = (x1 * y2 - y1 * x2), temp2 = (x3 * y4 - y3 * x4);
	float x = (temp1 * (x3 - x4) - (x1 - x2) * temp2) / d;
	float y = (temp1 * (y3 - y4) - (y1 - y2) * temp2) / d;

	return std::pair<point, bool>({ point(x, y), true });
}

std::tuple<point, bool, bool> getLineSegmentIntersection(const point& lineStart1, const point& lineEnd1, const point& lineStart2, const point& lineEnd2)
{
	using std::min;
	using std::max;

	float x1 = lineStart1.x, x2 = lineEnd1.x, x3 = lineStart2.x, x4 = lineEnd2.x;
	float y1 = lineStart1.y, y2 = lineEnd1.y, y3 = lineStart2.y, y4 = lineEnd2.y;

	float d = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
	// If d is zero, there is no intersection
	if (fabs(d) < 1e-6) return std::tuple<point, bool, bool>({ point(0,0), false, false });
	
	float temp1 = (x1 * y2 - y1 * x2), temp2 = (x3 * y4 - y3 * x4);
	float x = (temp1 * (x3 - x4) - (x1 - x2) * temp2) / d;
	float y = (temp1 * (y3 - y4) - (y1 - y2) * temp2) / d;

	// Check if the x and y coordinates are within both lines
	if (x < min(x1, x2) || x > max(x1, x2) ||
		x < min(x3, x4) || x > max(x3, x4)) return std::tuple<point, bool, bool>({point(x,y), true, false});
	if (y < min(y1, y2) || y > max(y1, y2) ||
		y < min(y3, y4) || y > max(y3, y4)) return std::tuple<point, bool, bool>({ point(x,y), true, false });

	return std::tuple<point, bool, bool>({ point(x,y), true, true });
}

void getLineSegmentIntersection(const point& lineStart1, const point& lineEnd1, const point& lineStart2, const point& lineEnd2, point* intersectionPoint)
{
	auto res = getLineSegmentIntersection(lineStart1, lineEnd1, lineStart2, lineEnd2);

	if (std::get<1>(res) && std::get<2>(res)) // intersection happend and it is within segments
	{
		*intersectionPoint = point(std::get<0>(res));
	}
	else intersectionPoint = NULL;
}

//
// Polygon
//

polygon::polygon() {}
polygon::polygon(std::initializer_list<point> points) : points(points) {}
polygon::polygon(const polygon& poly) : points(poly.points) {}
polygon::polygon(polygon&& poly) noexcept : points(std::move(poly.points)) {}
polygon& polygon::operator = (const polygon& poly) { points = poly.points; return *this; }
polygon& polygon::operator = (polygon&& poly) noexcept { this->points = std::move(poly.points); return *this; }

void polygon::pushPoint(const point& p)
{
	points.push_back(p);
}

point polygon::popPoint()
{
	point p = points.back();
	points.pop_back();
	return p;
}

std::size_t polygon::pointCount() const
{
	return points.size();
}

polygon convexHull(const polygon& poly)
{
	polygon convexPoly;

	// find point with min X value, that point is always convex hull point
	auto minIt = std::min_element(poly.points.cbegin(), poly.points.cend(),
		[](const point& p1, const point& p2) {return p1.x < p2.x; });
	
	convexPoly.points.push_back(*minIt);

	auto maxIt = std::min_element(poly.points.cbegin(), poly.points.cend(),
		[](const point& p1, const point& p2) {return p1.x > p2.x; });

	bool dir = true; // true for minX to maxX, false for opposite

	// build chain counter clock wise, points added must be to the right of current edge
	// erasing last item in std::vector is O(1)

	auto& cpoints = convexPoly.points;
	// first build chain using points right of minIt in points array, they need to keep convexity of chains
	for (auto it = minIt + 1; it < poly.points.cend(); it++)
	{
		while (cpoints.size() > 1 && !isLeft(cpoints[cpoints.size() - 2], cpoints[cpoints.size() - 1], *it))
		{
			cpoints.pop_back();
		}
		
		if ((dir && cpoints.back().x <= it->x) || (!dir && cpoints.back().x >= it->x))
			cpoints.push_back(*it);
		if (it->x == maxIt->x) dir = false;
	}
	// rest or points
	for (auto it = poly.points.cbegin(); it < minIt; it++)
	{
		while (cpoints.size() > 1 && !isLeft(cpoints[cpoints.size() - 2], cpoints[cpoints.size() - 1], *it))
		{
			cpoints.pop_back();
		}

		if ((dir && cpoints.back().x <= it->x) || (!dir && cpoints.back().x >= it->x))
			cpoints.push_back(*it);
		if (it->x == maxIt->x) dir = false;
	}

	return convexPoly;
}

void getRectangleOfLines(const point& p1, const point& p2, const point& p3, const point& p4,
	const point& p5, const point& p6, const point& p7, const point& p8,
	point& pRes1, point& pRes2, point& pRes3, point& pRes4)
{
	// points p1 ... p8 are for lines
	// pRes for results
	// doesn't check if lines intersect
	pRes1 = getLineIntersection(p1, p2, p3, p4).first;
	pRes2 = getLineIntersection(p3, p4, p5, p6).first;
	pRes3 = getLineIntersection(p5, p6, p7, p8).first;
	pRes4 = getLineIntersection(p7, p8, p1, p2).first;
}

void getBoundingBox(const polygon& poly, point& p1, point& p2, point& p3, point& p4)
{
	// STEP 1
	polygon convexPoly = convexHull(poly); // O(n)

	// STEP 2

	// indexes for critical points
	int minX = 0, maxX = 0, minY = 0, maxY = 0;
	int n = convexPoly.pointCount();
	auto& points = convexPoly.points; // refrence

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

#define NEXTP(P) ((P + 1) % n)
	
	vector2 v0, v1, v2, v3, v4; // v0 is prev direction of P1, v1 = P1 to NEXT(P1)
	v0 = vector2(0.f, -1.f); // first vector from minX to tempP1
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

		v0 = v1; // last direction
		P1 = NEXTP(P1); // move to next point
	}

	p1 = bestP1, p2 = bestP2, p3 = bestP3, p4 = bestP4;
}