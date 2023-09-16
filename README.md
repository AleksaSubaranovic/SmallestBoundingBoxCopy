
# SmallestBoundingBox

Showcase of Algorithm for finding SmallestBoundingBox of Polygon.

Code is in `SandboxGeometry.cpp`.

Visual Implementation is in `SmallestBoundingBox/SmallestBoundingBoxSimulation.cpp`.

## Function Body

```cpp

void getBoundingBox(const polygon& poly, point& p1, point& p2, point& p3, point& p4)
{
    // Step 1
    polygon convexPoly = convexHull(poly); // O(n)

    // Step 2 
    FindSmallest(convexPoly, p1, p2, p3, p4); // O(n)
}
```

### Step 1
Convert Simple Polygon to convex

```cpp
polygon convexHull(const polygon& poly)
{
	polygon convexPoly;

	// find point with min X value, that point is always convex hull point
	auto minIt = std::min_element(poly.points.cbegin(), poly.points.cend(),
		[](const point& p1, const point& p2) {return p1.x < p2.x; });
	
	convexPoly.points.push_back(*minIt);

	auto maxIt = std::max_element(poly.points.cbegin(), poly.points.cend(),
		[](const point& p1, const point& p2) {return p1.x < p2.x; });

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
```
### Step 2
Generate Bounding Box and rotate it until there all boxes are checked. Return smallest.

`getRectangleOfLines` is helper function that returns intersections.

```cpp
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
```


## Screenshots of Algorithm Steps
![Bounding Box 9_16_2023 3_50_49 PM](https://github.com/AleksaSubaranovic/SmallestBoundingBoxCopy/assets/66173682/3b8ce542-f3ee-4b77-9a23-642f55b8ab52)
![Bounding Box 9_16_2023 3_51_22 PM](https://github.com/AleksaSubaranovic/SmallestBoundingBoxCopy/assets/66173682/dc216800-f01a-45b9-9c23-5e3cb750962d)

![Bounding Box 9_16_2023 3_51_31 PM](https://github.com/AleksaSubaranovic/SmallestBoundingBoxCopy/assets/66173682/0917f828-a7e5-46bd-852e-e71ca72a282e)
![Bounding Box 9_16_2023 3_51_42 PM](https://github.com/AleksaSubaranovic/SmallestBoundingBoxCopy/assets/66173682/1e61d3b7-30f0-4e02-8d35-9ac571fd1d66)
![Bounding Box 9_16_2023 3_51_48 PM](https://github.com/AleksaSubaranovic/SmallestBoundingBoxCopy/assets/66173682/e4861b3f-f083-4257-aef8-536669e2c18d)
![Bounding Box 9_16_2023 3_51_52 PM](https://github.com/AleksaSubaranovic/SmallestBoundingBoxCopy/assets/66173682/4875ae04-690a-4cf6-ae16-e4becccb39d8)
![Bounding Box 9_16_2023 3_52_06 PM](https://github.com/AleksaSubaranovic/SmallestBoundingBoxCopy/assets/66173682/e5342225-8156-4e6d-ae57-cbfef5c4fc11)


