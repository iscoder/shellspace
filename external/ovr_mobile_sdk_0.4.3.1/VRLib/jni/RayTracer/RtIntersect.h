/************************************************************************************

Filename    :   RtIntersect.h
Content     :   Basic intersection routines.
Created     :   May, 2014
Authors     :   J.M.P. van Waveren

Copyright   :   Copyright 2014 Oculus VR, LLC. All Rights reserved.

*************************************************************************************/
#ifndef __RTINTERSECT_H__
#define __RTINTERSECT_H__

#include "Kernel/OVR_Math.h"

namespace RtIntersect
{
	/*
		An Efficient and Robust Ray�Box Intersection Algorithm
		Amy Williams, Steve Barrus, R. Keith Morley, Peter Shirley
		Journal of Graphics Tools, Issue 10, Pages 49-54, June 2005

		Returns true if the ray intersects the bounds.
		't0' and 't1' are the distances along the ray where the intersections occurs.
		1st intersection = rayStart + t0 * rayDir
		2nd intersection = rayStart + t1 * rayDir
	*/
	bool RayBounds( const OVR::Vector3f & rayStart, const OVR::Vector3f & rayDir,
					const OVR::Vector3f & mins, const OVR::Vector3f & maxs,
					float & t0, float & t1 );

	/*
		Fast, Minimum Storage Ray/Triangle Intersection
		Tomas M�ller, Ben Trumbore
		Journal of Graphics Tools, 1997

		Triangles are back-face culled.
		Returns true if the ray intersects the triangle.
		't0' is the distance along the ray where the intersection occurs.
		intersection = rayStart + t0 * rayDir;
		'u' and 'v' are the barycentric coordinates.
		intersection = ( 1 - u - v ) * v0 + u * v1 + v * v2
	*/
	bool RayTriangle( const OVR::Vector3f & rayStart, const OVR::Vector3f & rayDir,
					const OVR::Vector3f & v0, const OVR::Vector3f & v1, const OVR::Vector3f & v2,
					float & t0, float & u, float & v );
}

#endif // !__RTINTERSECT_H__
