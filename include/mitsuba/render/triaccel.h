/*
    This file is part of Mitsuba, a physically based rendering system.

    Copyright (c) 2007-2014 by Wenzel Jakob and others.

    Mitsuba is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License Version 3
    as published by the Free Software Foundation.

    Mitsuba is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once
#if !defined(__MITSUBA_RENDER_TRIACCEL_H_)
#define __MITSUBA_RENDER_TRIACCEL_H_

#include <mitsuba/render/trimesh.h>
#include <mitsuba/core/ellipse.h>
#include <mitsuba/render/sampler.h>
#include <math.h>

MTS_NAMESPACE_BEGIN

// Used when a fake triangle is used to reference a non-triangle shape instance
static const uint32_t KNoTriangleFlag = 0xFFFFFFFF;

/**
 * \brief Pre-computed triangle representation based on Ingo Wald's TriAccel layout.
 *
 * Fits into three 16-byte cache lines if single precision floats are used.
 * The k parameter is also used for classification during kd-tree construction.
 * \ingroup librender
 */
struct TriAccel {
	uint32_t k;
	Float n_u;
	Float n_v;
	Float n_d;

	Float a_u;
	Float a_v;
	Float b_nu;
	Float b_nv;

	Float c_nu;
	Float c_nv;
	uint32_t shapeIndex;
	uint32_t primIndex;

	//FIXME: The raw Points are needed for transient rendering. Will double the space of the triangle. Need to think of a better alternative. Also, we should store the transform if it is going to speed the computations - Adithya
	Point A;
	Point B;
	Point C;


	/// Construct from vertex data. Returns '1' if there was a failure
	inline int load(const Point &A, const Point &B, const Point &C);

	/// For ellipsoidal intersection with triangle
	FINLINE bool ellipseIntersectTriangle(const Ellipse &e, Float &value, Float &u, Float &v, ref<Sampler> sampler) const;

	FINLINE bool circlePolygonIntersection(const Point Corners[], const Float &r, ref<Sampler> sampler, Float &angle) const;
	FINLINE Float circleLineIntersection(const Point &P1, const Point &P2, const Float &r) const;
	FINLINE void Barycentric(const Point &p, const Point &a, const Point &b, const Point &c, Float &u, Float &v) const;

	/// Fast ray-triangle intersection test
	FINLINE bool rayIntersect(const Ray &ray, Float mint, Float maxt,
		Float &u, Float &v, Float &t) const;
};

inline int TriAccel::load(const Point &A, const Point &B, const Point &C) {
	static const int waldModulo[4] = { 1, 2, 0, 1 };

	//FIXME: The raw Points are needed for transient rendering. Will double the space of the triangle. Need to think of a better alternative. Also, we should store the transform if it is going to speed the computations - Adithya
	this->A = A;
	this->B = B;
	this->C = C;


	Vector b = C-A, c = B-A, N = cross(c, b);

	k = 0;
	/* Determine the largest projection axis */
	for (int j=0; j<3; j++) {
		if (std::abs(N[j]) > std::abs(N[k]))
			k = j;
	}

	uint32_t u = waldModulo[k],
		v = waldModulo[k+1];
	const Float n_k = N[k],
		denom = b[u]*c[v] - b[v]*c[u];

	if (denom == 0) {
		k = 3;
		return 1;
	}

	/* Pre-compute intersection calculation constants */
	n_u   =  N[u] / n_k;
	n_v   =  N[v] / n_k;
	n_d   =  dot(Vector(A), N) / n_k;
	b_nu  =  b[u] / denom;
	b_nv  = -b[v] / denom;
	a_u   =  A[u];
	a_v   =  A[v];
	c_nu  =  c[v] / denom;
	c_nv  = -c[u] / denom;
	return 0;
}


FINLINE bool TriAccel::ellipseIntersectTriangle(const Ellipse &e, Float &value, Float &u, Float &v, ref<Sampler> sampler) const {



	Point SphereA;e.transformToSphere(A, SphereA);
	Point SphereB;e.transformToSphere(B, SphereB);
	Point SphereC;e.transformToSphere(C, SphereC);
	Point Origin(0.0f, 0.0f, 0.0f);

	Vector b = SphereC-SphereA, c = SphereB-SphereA, N = cross(c, b);
	N = normalize(N);

	Vector Center = dot(N,SphereA-Origin)*N;

	Float d = Center.lengthSquared();

	if(d > 1){ // ellipse does not intersecti the plane
		return false;
	}

	float R = sqrt(1-d);

	Vector Z(0.0f, 0.0f, 1.0f);
	Transform T3D2D, T3D2Dinv;
	Transform temp;
	T3D2D = temp.rotateVector2Vector(N, Z)*temp.translate(-Center);

	T3D2Dinv = T3D2D.inverse();

	Point Corners[3];

	Corners[0] = T3D2D(SphereA);
	Corners[1] = T3D2D(SphereB);
	Corners[2] = T3D2D(SphereC);

	Float angle = 0.0f;
	if(circlePolygonIntersection(Corners, R, sampler, angle)){
		Point Projection(R*cos(angle), R*sin(angle), 0.0f), Original;
		Projection = T3D2Dinv(Projection);
		e.transformFromSphere(Projection, Original);

		//Verify that original satisfies the elliptic constraint
//		Float EllipticDistance = e.length(Original);


		//Verify that original is in the plane of Triangle ABC
//		Point Origin(0.0f, 0.0f, 0.0f);
//		Float NonPlaneError  = dot(Original-A, cross(Original-B, Original-C))/(Original-Origin).length();


		//Compute the Barycentric co-ordinates. Return that and save it in the cache.
		Barycentric(Original, A, B, C, u, v);
		value = 1.0f;
//		Point VerifyOriginal =(1-u-v)*A + u*B + v*C;

		return true;
	}

	return false;
}

/* Code taken from https://gamedev.stackexchange.com/questions/23743/whats-the-most-efficient-way-to-find-barycentric-coordinates */
FINLINE void TriAccel::Barycentric(const Point &p, const Point &a, const Point &b, const Point &c, Float &u, Float &v) const
{
    Vector v0 = b - a, v1 = c - a, v2 = p - a;
    Float d00 = dot(v0, v0);
    Float d01 = dot(v0, v1);
    Float d11 = dot(v1, v1);
    Float d20 = dot(v2, v0);
    Float d21 = dot(v2, v1);
    Float denom = d00 * d11 - d01 * d01;
    u = (d11 * d20 - d01 * d21) / denom;
    v = (d00 * d21 - d01 * d20) / denom;
}

FINLINE bool TriAccel::circlePolygonIntersection(const Point Corners[], const Float &r, ref<Sampler> sampler, Float &angle) const{
	int noOfCorners = 3; // This code can be extended trivially to polygons of arbitrary size other than 3
	Float norm_p[noOfCorners];

	Float theta[noOfCorners*2];
	bool directionOutside[noOfCorners*2];
	int intersections = 0;

	for(int i = 0; i < noOfCorners; i++){
		norm_p[i] = sqrt( Corners[i].x*Corners[i].x + Corners[i].y*Corners[i].y );
	}

	/* Determine the intersection first */
	for(int i = 0; i < noOfCorners; i++){
		int j = i + 1;
		if(j == noOfCorners){
			j=0;
		}
		// both points are inside, do nothing
		if(norm_p[i] < r && norm_p[j] < r)
			continue;
		// One of them is inside
		if(norm_p[i] < r && norm_p[j] > r){
			intersections++;
			directionOutside[intersections - 1] = true;
			theta[intersections - 1] = circleLineIntersection(Corners[i], Corners[j], r);
			continue;
		}
		if(norm_p[i] > r && norm_p[j] < r){
			intersections++;
			directionOutside[intersections - 1] = false;
			theta[intersections - 1] = circleLineIntersection(Corners[i], Corners[j], r);
			continue;
		}
		// both points are outside, can intersect in zero points or 2 points
		if(norm_p[i] > r && norm_p[j] > r){
			Vector2 n(Corners[j].y-Corners[i].y, Corners[i].x-Corners[j].x);
			n = normalize(n);
			Float dotP = (n.x*Corners[i].x + n.y*Corners[i].y);
			Point P_O(dotP*n.x, dotP*n.y, 0.0f);

			// If projection is not in the circle, the line-segment is completely outside circle
			if(P_O.x*P_O.x + P_O.y*P_O.y > r*r){
				continue;
			}

			Float alpha = -1.0f;
	        // Compute where the projection is, with respect to the line joining p[i] and p[j]
			if (Corners[i].x != Corners[j].x)
				alpha = (P_O.x-Corners[j].x)/(Corners[i].x-Corners[j].x);
			else
				alpha = (P_O.y-Corners[j].y)/(Corners[i].y-Corners[j].y);

			if(alpha < 0 || alpha > 1)
				continue;



			intersections++;
			directionOutside[intersections - 1] = false;
			theta[intersections - 1] = circleLineIntersection(Corners[i], P_O, r);

			intersections++;
			directionOutside[intersections - 1] = true;
			theta[intersections - 1] = circleLineIntersection(P_O, Corners[j], r);

			continue;
		}
	}

	if(intersections == 0){
		return false;
	}

	if(intersections%2 == 1){
		SLog(EError,"Odd number of intersection in ellipse triangle intersection");
	}

	Vector N  = cross(Corners[1]-Corners[0], Corners[2]-Corners[0]);

	if(N.z < 0){ // flip the theta and angle arrays
		Float ftemp;
		bool btemp;
		for(int i = 0; i < intersections/2; i++){
			ftemp = theta[i];
			theta[i] = theta[intersections-1-i];
			theta[intersections-1-i] = ftemp;

			btemp = directionOutside[i];
			directionOutside[i] = directionOutside[intersections-1-i];
			directionOutside[intersections-1-i] = btemp;
		}
	}

	Float InsideAngles[intersections/2] = {0.0f};
	Float thetaDiff = 0.0f;


	if(directionOutside[0]){
		thetaDiff = theta[1] - theta[0];
		if(thetaDiff < 0){
			thetaDiff += 2*M_PI;
		}
		InsideAngles[0] = thetaDiff;

		for(int i = 1; i < intersections/2; i++){
			thetaDiff = theta[2*i+1] - theta[2*i];
			if(thetaDiff < 0){
				thetaDiff += 2*M_PI;
			}
			InsideAngles[i] = InsideAngles[i - 1] + thetaDiff;
		}

		Float r = InsideAngles[intersections/2-1]*sampler->nextFloat();
		for(int i = 0; i < intersections/2; i++){
			if(r < InsideAngles[i]){
				angle = theta[2*i] + InsideAngles[i] - r;
				return true;
			}
		}
	}
	else{
		thetaDiff = theta[0] - theta[intersections-1];
		if(thetaDiff < 0){
			thetaDiff += 2*M_PI;
		}
		InsideAngles[0] = thetaDiff;

		for(int i = 1; i < intersections/2; i++){
			thetaDiff = theta[2*i] - theta[2*i-1];
			if(thetaDiff < 0){
				thetaDiff += 2*M_PI;
			}
			InsideAngles[i] = InsideAngles[i - 1] + thetaDiff;
		}

		Float r = InsideAngles[intersections/2-1]*sampler->nextFloat();
		for(int i = 0; i < intersections/2; i++){
			if(r < InsideAngles[i]){
				angle = theta[2*i - 1] + InsideAngles[i] - r;
				return true;
			}
		}
	}

	return false;
}

FINLINE Float TriAccel::circleLineIntersection(const Point &P1, const Point &P2, const Float &r) const{
	Float x1 = P1.x;
	Float y1 = P1.y;
	Float x2 = P2.x;
	Float y2 = P2.y;

	Float dx = x1 - x2;
	Float dy = y1 - y2;
	Float a  = dx*dx + dy*dy;
	Float b  = 2*(dx*x2 + dy*y2);
	Float c  = x2*x2 + y2*y2 - r*r;


	Float det = sqrt(b*b-4*a*c);
	Float alpha = (-b+det)/(2*a);

	Float x = 0;
	Float y = std::numeric_limits<Float>::min();

	if(alpha > 0 && alpha <1){
		x = alpha * x1 + (1-alpha) * x2;
		y = alpha * y1 + (1-alpha) * y2;
	}else{
		alpha = (-b-det)/(2*a);
		if(alpha > 0 && alpha <1){
			x = alpha * x1 + (1-alpha) * x2;
			y = alpha * y1 + (1-alpha) * y2;
		}else{
			SLog(EError,"Circle-Line intersection called with out an actual intersection");
		}
	}

	return (Float)atan2(y, x);
}


FINLINE bool TriAccel::rayIntersect(const Ray &ray, Float mint, Float maxt,
	Float &u, Float &v, Float &t) const {

#if 0
	static const MM_ALIGN16 int waldModulo[4] = { 1, 2, 0, 1 };
	const int ku = waldModulo[k], kv = waldModulo[k+1];
	/* Get the u and v components */
	const Float o_u = ray.o[ku], o_v = ray.o[kv], o_k = ray.o[k],
				d_u = ray.d[ku], d_v = ray.d[kv], d_k = ray.d[k];
#else
	Float o_u, o_v, o_k, d_u, d_v, d_k;
	switch (k) {
		case 0:
			o_u = ray.o[1];
			o_v = ray.o[2];
			o_k = ray.o[0];
			d_u = ray.d[1];
			d_v = ray.d[2];
			d_k = ray.d[0];
			break;
		case 1:
			o_u = ray.o[2];
			o_v = ray.o[0];
			o_k = ray.o[1];
			d_u = ray.d[2];
			d_v = ray.d[0];
			d_k = ray.d[1];
			break;
		case 2:
			o_u = ray.o[0];
			o_v = ray.o[1];
			o_k = ray.o[2];
			d_u = ray.d[0];
			d_v = ray.d[1];
			d_k = ray.d[2];
			break;
		default:
			return false;
	}
#endif


#if defined(MTS_DEBUG_FP)
	if (d_u * n_u + d_v * n_v + d_k == 0)
		return false;
#endif

	/* Calculate the plane intersection (Typo in the thesis?) */
	t = (n_d - o_u*n_u - o_v*n_v - o_k) /
		(d_u * n_u + d_v * n_v + d_k);

	if (t < mint || t > maxt)
		return false;

	/* Calculate the projected plane intersection point */
	const Float hu = o_u + t * d_u - a_u;
	const Float hv = o_v + t * d_v - a_v;

	/* In barycentric coordinates */
	u = hv * b_nu + hu * b_nv;
	v = hu * c_nu + hv * c_nv;
	return u >= 0 && v >= 0 && u+v <= 1.0f;
}

MTS_NAMESPACE_END

#endif /* __MITSUBA_RENDER_TRIACCEL_H_ */
