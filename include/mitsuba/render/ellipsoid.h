/*
    This file is added by Adithya.
    */

#pragma once
#if !defined(__MITSUBA_RENDER_ELLIPSE_H_)
#define __MITSUBA_RENDER_ELLIPSE_H_


#include <mitsuba/mitsuba.h>
#include <mitsuba/core/transform.h>
#include <boost/math/special_functions/ellint_2.hpp>
#include <mitsuba/render/sampler.h>

MTS_NAMESPACE_BEGIN

template <typename _PointType, typename _LengthType> struct TEllipsoid{
	typedef _PointType                  PointType;
	typedef _LengthType                 LengthType;
	
	/* Focal points */
	PointType f1; 
	PointType f2;
	
	/* center of the ellipse */
	PointType  C;

	LengthType Tau;
	
	/* Major and minor axis lengths */
	LengthType a, b;

	bool degenerateEllipsoid;

	Transform T3D2Ellipsoid;
	Transform invT3D2Ellipsoid;
	Transform T3D2Sphere;
	Transform invT3D2Sphere;
	
	struct BoundingBox{
		PointType min;
		PointType max;
	}m_aabb;

	TEllipsoid(const PointType p1, const PointType p2, const LengthType tau);

	inline bool isDegenerate() const { return degenerateEllipsoid; }

	/* Intersect the triangle formed by triA, triB, triC with the current Ellipsoid to create a sample whose barycentric co-ordinates are in u, v.
	 * Value is probability of the sample ( = inverse of the length of the ellipsoid-triangle intersection) */
	bool ellipsoidIntersectTriangle(const PointType &triA, const PointType &triB, const PointType &triC, Float &value, Float &u, Float &v, ref<Sampler> sampler) const;

	/* Transforms a point from 3D space to Ellipsoid space*/
	inline void transformToEllipsoid(const Point &A, Point &B) const{
		B = T3D2Ellipsoid(A);
		return;
	}

	/* Transforms a point from Ellipsoid space to 3D space*/
	inline void transformFromEllipsoid(const Point &A, Point &B) const{
		B = invT3D2Ellipsoid(A);
		return;
	}

	/* Transforms a point from 3D space to Sphere space*/
	inline void transformToSphere(const Point &A, Point &B) const{
		B = T3D2Sphere(A);
		return;
	}

	/* Transforms a point from Sphere space to 3D space*/
	inline void transformFromSphere(const Point &A, Point &B) const{
		B = invT3D2Sphere(A);
		return;
	}

	/* Compute Weighted Inner Product on the Ellipsoid */
	Float weightedIP(Vector &A, Vector &B) const{
		return (A[0]*B[0]/(a*a) + A[1]*B[1]/(b*b) + A[2]*B[2]/(b*b));
	}

	/* Code taken from https://gamedev.stackexchange.com/questions/23743/whats-the-most-efficient-way-to-find-barycentric-coordinates */
	void Barycentric(const Point &p, const Point &a, const Point &b, const Point &c, Float &u, Float &v) const;

	bool circlePolygonIntersectionAngles(Float thetaMin[], Float thetaMax[], size_t &indices, const Point Corners[], const Float &r) const;

	int numberOfCircleLineIntersections(const Point &P1, const Point &P2, const Float &r) const;

	Float circleLineIntersection(const Point &P1, const Point &P2, const Float &r) const;

	Float ellipticSampleWeight(Float k, Float thetaMin[], Float thetaMax[],size_t &indices) const;

	Float ellipticCurveSampling(Float k, Float thetaMin[], Float thetaMax[], size_t &indices, ref<Sampler> sampler) const;

	//	inline bool isInside(float x, float y, float z) const{
	//		Point P(x, y, z);
	//		if(lengthSquared(T3D2Sphere(P)) < 1)
	//			return true;
	//		return false;
	//	}
	//
	//	inline bool isInside(Point P) const{
	//		if(lengthSquared(T3D2Sphere(P)) < 1)
	//			return true;
	//		return false;
	//	}

	//	inline Float length(Point P) const{
	//		return (P-f1).length() + (P-f2).length();
	//	}
	//
	//	inline bool isEllipticPoint(Point P) const{
	//		return ((P-f1).length + (P-f2).length - Tau) < 0.01f);
	//	}
};

MTS_NAMESPACE_END

#endif /* __MITSUBA_CORE_RAY_H_ */
