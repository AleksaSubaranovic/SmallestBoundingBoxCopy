#pragma once

#include <math.h>
#include <iostream>
#include <vector>
#include <initializer_list>
#include <stdexcept> 

/*
* Vectors 2D
*/

struct vector2
{
	float x, y;
	constexpr vector2() : x(0.f), y(0.f) {}
	constexpr vector2(float x, float y) : x(x), y(y) {}
	vector2(std::initializer_list<float> l) 
	{
		if (l.size() != 2) throw std::length_error("Vector init list is not correct size");
		x = *l.begin();
		y = *(l.begin() + 1);
	}
	constexpr vector2(const vector2& v) : x(v.x), y(v.y) {}
	constexpr vector2& operator = (const vector2& v) { this->x = v.x, this->y = v.y; return *this; }

	//
	constexpr vector2 operator + (float f) const { return vector2(x + f, y + f); }
	constexpr vector2 operator - (float f) const { return vector2(x - f, y - f); }
	constexpr vector2 operator * (float f) const { return vector2(x * f, y * f); }
	constexpr vector2 operator / (float f) const { return vector2(x / f, y / f); }
	//
	constexpr vector2& operator += (float f) { this->x += f, this->y += f; return *this; }
	constexpr vector2& operator -= (float f) { this->x -= f, this->y -= f; return *this; }
	constexpr vector2& operator *= (float f) { this->x *= f, this->y *= f; return *this; }
	constexpr vector2& operator /= (float f) { this->x /= f, this->y /= f; return *this; }
	//
	constexpr vector2 operator + (const vector2& v) const { return vector2(x + v.x, y + v.y); }
	constexpr vector2 operator - (const vector2& v) const { return vector2(x - v.x, y - v.y); }
	constexpr vector2 operator * (const vector2& v) const { return vector2(x * v.x, y * v.y); }
	constexpr vector2 operator / (const vector2& v) const { return vector2(x / v.x, y / v.y); }
	//
	constexpr vector2& operator += (const vector2& v) { this->x += v.x, this->y += v.y; return *this; }
	constexpr vector2& operator -= (const vector2& v) { this->x -= v.x, this->y -= v.y; return *this; }
	constexpr vector2& operator *= (const vector2& v) { this->x *= v.x, this->y *= v.y; return *this; }
	constexpr vector2& operator /= (const vector2& v) { this->x /= v.x, this->y /= v.y; return *this; }
	//
	constexpr vector2 operator - () const { return vector2(x * -1.f, y * -1.f); }
	constexpr bool operator == (const vector2& v) { return this->x == v.x && this->y == v.y; }
	constexpr bool operator != (const vector2& v) { return this->x != v.x && this->y != v.y; }
	//
	friend std::ostream& operator<<(std::ostream& os, const vector2& v);
	friend std::istream& operator>>(std::istream& is, vector2& v);
	//
	void normalize();
	float length() const;
	// rotate vector from 0,0 around origin by angle(rad)
	void rotate(float angle);
};

constexpr float dotProduct(const vector2& v1, const vector2& v2)
{
	return v1.x * v2.x + v1.y * v2.y;
}

float vectorLength(const vector2& v1);

float angleOfVectors(const vector2& v1, const vector2& v2);

vector2 normalized(const vector2& v1);

//

/*
* Point (2d vector)
*/
typedef vector2 point;

// Rotate point around origin by angle(in rad)
void RotatePoint(const point& origin, point& pointToRotate, const float angle);

float getDistance(const point& p1, const point& p2);

// determinant of 2x2 matrix
constexpr float det(float a, float b, float c, float d) { return a * b - c * d; }

// Get Line intersection point, lines are defined by 2 points on those lines
// @return Returns Intersection point and wheter there is instersection (if result.second == true) => intersection found
std::pair<point, bool> getLineIntersection(const point& line1_1, const point& line1_2, const point& line2_1, const point& line2_2);

// Get Line Segment intersection point, lines are defined by start point and end point
// @return Returns Intersection point, wheter there is intersection and if intersection is in segments
std::tuple<point, bool, bool> getLineSegmentIntersection(const point& lineStart1, const point& lineEnd1, const point& lineStart2, const point& lineEnd2);

// Get Line Segment intersection point, lines are defined by start point and end point
// @param intersectionPoint*, NULL if there is no intersection within segment
void getLineSegmentIntersection(const point& lineStart1, const point& lineEnd1, const point& lineStart2, const point& lineEnd2, point* intersectionPoint);


/**
*  Is point p left of line
*/
constexpr bool isLeft(const point& lineStart, const point& lineEnd, const point& p)
{
	return ((lineEnd.x - lineStart.x) * (p.y - lineStart.y) - (lineEnd.y - lineStart.y) * (p.x - lineStart.x)) > 0.f;
}

/*
* Polygons
* xy cord
* first and last point are connected
* counter clock wise
*/
struct polygon
{
	std::vector<point> points; // vectors as location in xy space
	// todo
	polygon();
	polygon(std::initializer_list<point> points);
	polygon(const polygon& poly);
	polygon(polygon&& poly) noexcept ;
	polygon& operator = (const polygon& poly);
	polygon& operator = (polygon&& poly) noexcept;
	 
	void pushPoint(const point& p);

	point popPoint();

	std::size_t pointCount() const;
};

/**
*  Get Convex Hull of simple polygon
* 
*  @param poly: Simple polygon to get convex hull
*  @return convex polygon made from input polygon points
*/
polygon convexHull(const polygon& poly);

void getRectangleOfLines(const point& p1, const point& p2, const point& p3, const point& p4,
	const point& p5, const point& p6, const point& p7, const point& p8,
	point& pRes1, point& pRes2, point& pRes3, point& pRes4);

/**
*  Get Smallest Bounding Box of polygon
*  Algorithm O(n)
*	step1: Convert to convex polygon
*	setp2: Rotate Rectangle around polygon(rectangle edge is covering 2 points of polygon, others at leas 1)
*/
void getBoundingBox(const polygon& poly, point& p1, point& p2, point& p3, point& p4);