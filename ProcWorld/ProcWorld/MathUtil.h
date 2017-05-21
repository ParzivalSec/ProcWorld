#pragma once
#include "BoundingBox.h"
#include "Ray.h"
#include "KDTreeController.h"
#include <iostream>

template <typename T>
T Lerp(T start, T end, float interpolationValue)
{
	return start + interpolationValue * (end - start);
}

inline glm::vec3 KochanekBartels(glm::vec3 P0, glm::vec3 P1, glm::vec3 P2, glm::vec3 P3,
	float tension, float continuity, float bias, float t)
{
	//	 Hermite Matrix
	//	 2.0f, -2.0f,  1.0f,  1.0f,
	//	-3.0f,  3.0f, -2.0f, -1.0f,
	//	 0.0f,  0.0f,  1.0f,  0.0f,
	//	 1.0f,  0.0f,  0.0f,  0.0f

	// Vector of power (just holds powers up to power of three from t)
	glm::vec4 S(t * t * t, t * t, t, 1.0f);

	// Compute incomiming tangent vector 
	glm::vec3 tangentVectorS =
		(((1 - tension) * (1 - continuity) * (1 + bias)) / 2) * (P2 - P1) +
		(((1 - tension) * (1 + continuity) * (1 - bias)) / 2) * (P3 - P2);

	// Compute outgoing tangent vector
	glm::vec3 tangentVectorD =
		(((1 - tension) * (1 + continuity) * (1 + bias)) / 2) * (P1 - P0) +
		(((1 - tension) * (1 - continuity) * (1 - bias)) / 2) * (P2 - P1);

	// Fill in values for the hermite matrix
	float h1 = 2 * S[0] - 3 * S[1] + 1;
	float h2 = -2 * S[0] + 3 * S[1];
	float h3 = S[0] - 2 * S[1] + S[2];
	float h4 = S[0] - S[1];

	return (h1 * P1 + h2 * P2 + h3 * tangentVectorD + h4 * tangentVectorS);
}

//bool intersect(const Ray &r) const
//{
//	float tmin, tmax, tymin, tymax, tzmin, tzmax;
//
//	tmin = (bounds[r.sign[0]].x - r.orig.x) * r.invdir.x;
//	tmax = (bounds[1 - r.sign[0]].x - r.orig.x) * r.invdir.x;
//	tymin = (bounds[r.sign[1]].y - r.orig.y) * r.invdir.y;
//	tymax = (bounds[1 - r.sign[1]].y - r.orig.y) * r.invdir.y;
//
//	if ((tmin > tymax) || (tymin > tmax))
//		return false;
//	if (tymin > tmin)
//		tmin = tymin;
//	if (tymax < tmax)
//		tmax = tymax;
//
//	tzmin = (bounds[r.sign[2]].z - r.orig.z) * r.invdir.z;
//	tzmax = (bounds[1 - r.sign[2]].z - r.orig.z) * r.invdir.z;
//
//	if ((tmin > tzmax) || (tzmin > tmax))
//		return false;
//	if (tzmin > tmin)
//		tmin = tzmin;
//	if (tzmax < tmax)
//		tmax = tzmax;
//
//	return true;
//}

inline bool RayBoxIntersection(Ray& ray, BoundingBox& aabb)
{
	float tmin, tmax, tymin, tymax, tzmin, tzmax;

	int bound = ray.mSign[0];

	if (bound == 0)
	{
		tmin = (aabb.GetMin().x - ray.mOrigin.x) * ray.mInvDir.x;
		tmax = (aabb.GetMax().x - ray.mOrigin.x) * ray.mInvDir.x;
	}
	else
	{
		tmin = (aabb.GetMax().x - ray.mOrigin.x) * ray.mInvDir.x;
		tmax = (aabb.GetMin().x - ray.mOrigin.x) * ray.mInvDir.x;
	}

	bound = ray.mSign[1];

	if (bound == 0)
	{
		tymin = (aabb.GetMin().y - ray.mOrigin.y) * ray.mInvDir.y;
		tymax = (aabb.GetMax().y - ray.mOrigin.y) * ray.mInvDir.y;
	}
	else
	{
		tymin = (aabb.GetMax().y - ray.mOrigin.y) * ray.mInvDir.y;
		tymax = (aabb.GetMin().y - ray.mOrigin.y) * ray.mInvDir.y;
	}

	if ((tmin > tymax) || (tymin > tmax))
		return false;
	if (tymin > tmin)
		tmin = tymin;
	if (tymax < tmax)
		tmax = tymax;

	bound = ray.mSign[2];

	if (bound == 0)
	{
		tzmin = (aabb.GetMin().z - ray.mOrigin.z) * ray.mInvDir.z;
		tzmax = (aabb.GetMax().z - ray.mOrigin.z) * ray.mInvDir.z;
	}
	else
	{
		tzmin = (aabb.GetMax().z - ray.mOrigin.z) * ray.mInvDir.z;
		tzmax = (aabb.GetMin().z - ray.mOrigin.z) * ray.mInvDir.z;
	}

	if ((tmin > tzmax) || (tzmin > tmax))
		return false;
	if (tzmin > tmin)
		tmin = tzmin;
	if (tzmax < tmax)
		tmax = tzmax;

	return true;
}

#define EPSILON 0.000001

inline bool RayTriangleIntersection(Ray& ray, Triangle& tri, float& hitDist)
{
	glm::vec3 e1, e2;  //Edge1, Edge2
	glm::vec3 P, Q, T;
	float det, inv_det, u, v;
	float t;

	//Find vectors for two edges sharing V1
	e1 = tri.B - tri.A;
	e2 = tri.C - tri.A;
	//Begin calculating determinant - also used to calculate u parameter
	P = glm::cross(ray.mDir, e2);
	//if determinant is near zero, ray lies in plane of triangle or ray is parallel to plane of triangle
	det = glm::dot(e1, P);
	//NOT CULLING
	if (det > -EPSILON && det < EPSILON) return false;
	inv_det = 1.f / det;

	//calculate distance from V1 to ray origin
	T = ray.mOrigin - tri.A;

	//Calculate u parameter and test bound
	u = glm::dot(T, P) * inv_det;
	//The intersection lies outside of the triangle
	if (u < 0.f || u > 1.f) return false;

	//Prepare to test v parameter
	Q = glm::cross(T, e1);

	//Calculate V parameter and test bound
	v = glm::dot(ray.mDir, Q) * inv_det;
	//The intersection lies outside of the triangle
	if (v < 0.f || u + v  > 1.f) return false;

	t = glm::dot(e2, Q) * inv_det;

	if (t > EPSILON) { //ray intersection
		hitDist = t;
		return true;
	}

	// No hit, no win
	return false;
}