#ifndef _BOX_H_
#define _BOX_H_

#include <assert.h>
#include "vector3.h"
#include "ray.h"

/*
 * Axis-aligned bounding box class, for use with the optimized ray-box
 * intersection test described in:
 *
 *      Amy Williams, Steve Barrus, R. Keith Morley, and Peter Shirley
 *      "An Efficient and Robust Ray-Box Intersection Algorithm"
 *      Journal of graphics tools, 10(1):49-54, 2005
 *
 */

class Box {
  public:
    Box() { }
    Box(const Vector3 &min, const Vector3 &max) {
 //     assert(min < max);
      parameters[0] = min;
      parameters[1] = max;
    }
    // (t0, t1) is the interval for valid hits
    bool intersect(const Ray &, float t0, float t1) const;

	void setOrigin(Vector3 &v)
	{
		Vector3 difference = parameters[1] - parameters[0];
		parameters[0] = v;
		parameters[1] = parameters[0] + difference;
	}

    // corners
    Vector3 parameters[2];

	Vector3 min() { return parameters[0]; }
	Vector3 max() { return parameters[1]; }
	const bool inside(const Vector3 &p) {
		return ((p.x() >= parameters[0].x() && p.x() <= parameters[1].x()) &&
		     	(p.y() >= parameters[0].y() && p.y() <= parameters[1].y()) &&
			    (p.z() >= parameters[0].z() && p.z() <= parameters[1].z()));
	}
	const bool inside(Vector3 *points, int size) {
		bool allInside = true;
		for (int i = 0; i < size; i++) {
			if (!inside(points[i])) allInside = false;
			break;
		}
		return allInside;
	}

	// implement for Homework Project
	//
	 bool overlap(const Box &box) {
		return (box.parameters[0].x() <= parameters[1].x() && box.parameters[1].x() >= parameters[0].x()) &&
			   (box.parameters[0].y() <= parameters[1].y() && box.parameters[1].y() >= parameters[0].y()) && 
			   (box.parameters[0].z() <= parameters[1].z() && box.parameters[1].z() >= parameters[0].z());
	}

	Vector3 center() {
		return ((max() - min()) / 2 + min());
	}
};

#endif // _BOX_H_
