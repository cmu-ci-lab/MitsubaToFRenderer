/*
    This file is added by Adithya.
    */

#pragma once
#if !defined(__MITSUBA_CORE_ELLIPSE_H_)
#define __MITSUBA_CORE_ELLIPSE_H_


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

	inline TEllipsoid(const PointType p1, const PointType p2, const LengthType tau);

	inline bool isDegenerate() const { return degenerateEllipsoid; }

	/* Intersect the triangle formed by triA, triB, triC with the current Ellipsoid to create a sample whose barycentric co-ordinates are in u, v.
	 * Value is probability of the sample ( = inverse of the length of the ellipsoid-triangle intersection) */
	bool ellipsoidIntersectTriangle(const Point &triA, const Point &triB, const Point &triC, Float &value, Float &u, Float &v, ref<Sampler> sampler) const;

	/* Transforms a point from 3D space to Ellipsoid space*/
	inline void transformToEllipsoid(const Point &A, Point &B) const;

	/* Transforms a point from Ellipsoid space to 3D space*/
	inline void transformFromEllipsoid(const Point &A, Point &B) const;

	/* Transforms a point from 3D space to Sphere space*/
	inline void transformToSphere(const Point &A, Point &B) const;

	/* Transforms a point from Sphere space to 3D space*/
	inline void transformFromSphere(const Point &A, Point &B) const;

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

/// Construct a new ellipsoid
template <typename PointType, typename LengthType>
inline TEllipsoid<PointType, LengthType>::TEllipsoid(const PointType p1, const PointType p2, const LengthType tau):
		f1(p1), f2(p2), Tau(tau){
	// Algorithm to compute the T3D3D
	a = tau/2.0f;
	C = (f1 + f2) * 0.5f;
	b = (a*a-distanceSquared(C, f1));
	if(b < 0)
		degenerateEllipsoid = true;
	else
		degenerateEllipsoid = false;
	b = sqrt(b);
	TVector3<LengthType> D = f2-f1;
	TVector3<LengthType> Scale(1/a, 1/b, 1/b);
	TVector3<LengthType> Rot(1.0f, 0.0f, 0.0f);
	TVector3<LengthType> Cv(-C);

	T3D2Ellipsoid 	 = Transform::rotateVector2Vector(D, Rot)*Transform::translate(Cv);
	invT3D2Ellipsoid = T3D2Ellipsoid.inverse();

	T3D2Sphere 	  = Transform::scale(Scale)*T3D2Ellipsoid;
	invT3D2Sphere = T3D2Sphere.inverse();

	/*compute bounding box for the arbitrary oriented ellipse*/
	Matrix4x4 S(TVector4<LengthType>(1.0f, 0.0f, 0.0f, 0.0f),
				TVector4<LengthType>(0.0f, 1.0f, 0.0f, 0.0f),
				TVector4<LengthType>(0.0f, 0.0f, 1.0f, 0.0f),
				TVector4<LengthType>(0.0f, 0.0f, 0.0f,-1.0f)
							);
	Matrix4x4 MT, M = invT3D2Sphere.getMatrix();
	M.transpose(MT);
	Matrix4x4 R = M*S*MT;

	Float temp1, temp2;
	if(R.m[3][3] < 0){
		temp1 = 1/R.m[3][3];
		temp2 = sqrt(R.m[3][2]*R.m[3][2] - (R.m[3][3]*R.m[2][2]));
		m_aabb.max.z = temp1*(R.m[3][2] - temp2);
		m_aabb.min.z = temp1*(R.m[3][2] + temp2);

		temp2 = sqrt(R.m[3][1]*R.m[3][1] - (R.m[3][3]*R.m[1][1]));
		m_aabb.max.y = temp1*(R.m[3][1] - temp2);
		m_aabb.min.y = temp1*(R.m[3][1] + temp2);

		temp2 = sqrt(R.m[3][0]*R.m[3][0] - (R.m[3][3]*R.m[0][0]));
		m_aabb.max.x = temp1*(R.m[3][0] - temp2);
		m_aabb.min.x = temp1*(R.m[3][0] + temp2);
	}else{
		temp1 = 1/R.m[3][3];
		temp2 = sqrt(R.m[3][2]*R.m[3][2] - (R.m[3][3]*R.m[2][2]));
		m_aabb.max.z = temp1*(R.m[3][2] + temp2);
		m_aabb.min.z = temp1*(R.m[3][2] - temp2);

		temp2 = sqrt(R.m[3][1]*R.m[3][1] - (R.m[3][3]*R.m[1][1]));
		m_aabb.max.y = temp1*(R.m[3][1] + temp2);
		m_aabb.min.y = temp1*(R.m[3][1] - temp2);

		temp2 = sqrt(R.m[3][0]*R.m[3][0] - (R.m[3][3]*R.m[0][0]));
		m_aabb.max.x = temp1*(R.m[3][0] + temp2);
		m_aabb.min.x = temp1*(R.m[3][0] - temp2);
	}

}

template <typename PointType, typename LengthType>
inline void TEllipsoid<PointType, LengthType>::transformToEllipsoid(const Point &A, Point &B) const{
	B = T3D2Ellipsoid(A);
	return;
}

template <typename PointType, typename LengthType>
inline void TEllipsoid<PointType, LengthType>::transformFromEllipsoid(const Point &A, Point &B) const{
	B = invT3D2Ellipsoid(A);
	return;
}

template <typename PointType, typename LengthType>
inline void TEllipsoid<PointType, LengthType>::transformToSphere(const Point &A, Point &B) const{
	B = T3D2Sphere(A);
	return;
}

template <typename PointType, typename LengthType>
inline void TEllipsoid<PointType, LengthType>::transformFromSphere(const Point &A, Point &B) const{
	B = invT3D2Sphere(A);
	return;
}

template <typename PointType, typename LengthType>
void TEllipsoid<PointType, LengthType>::Barycentric(const Point &p, const Point &a, const Point &b, const Point &c, Float &u, Float &v) const
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

template <typename PointType, typename LengthType>
bool TEllipsoid<PointType, LengthType>::circlePolygonIntersectionAngles(Float thetaMin[], Float thetaMax[], size_t &indices, const Point Corners[], const Float &r) const{
	int noOfCorners = 3; // This code can be extended trivially to polygons of arbitrary size other than 3
	Float norm_p[noOfCorners];
	indices = 0;

	Float temp;
	struct IntersectionRecord{
		Float theta;
		bool directionOutside;
		IntersectionRecord(Float theta, bool directionOutside){
			this->theta = theta;
			this->directionOutside = directionOutside;
		}
		static bool compare(IntersectionRecord I1, IntersectionRecord I2){
			return (I1.theta < I2.theta);
		}
	};
	std::vector<IntersectionRecord> intersectionRecord;
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
			temp = circleLineIntersection(Corners[i], Corners[j], r);
			intersectionRecord.push_back(IntersectionRecord(temp,true));
			continue;
		}
		if(norm_p[i] < r && norm_p[j] == r){
			intersections++;
			temp = atan2(Corners[j].y, Corners[j].x);
			intersectionRecord.push_back(IntersectionRecord(temp,true));
			continue;
		}
		if(norm_p[i] > r && norm_p[j] < r){
			intersections++;
			temp = circleLineIntersection(Corners[i], Corners[j], r);
			intersectionRecord.push_back(IntersectionRecord(temp,false));
			continue;
		}
		if(norm_p[i] == r && norm_p[j] < r){
			intersections++;
			temp = atan2(Corners[i].y, Corners[i].x);
			intersectionRecord.push_back(IntersectionRecord(temp,false));
			continue;
		}
		if(norm_p[i] == r && norm_p[j] > r){
			if(numberOfCircleLineIntersections(Corners[i], Corners[j], r) == 2){
				intersections++;
				temp = atan2(Corners[i].y, Corners[i].x);
				intersectionRecord.push_back(IntersectionRecord(temp,false));

				intersections++;
				temp = circleLineIntersection(Corners[i], Corners[j], r);
				intersectionRecord.push_back(IntersectionRecord(temp,true));
			}else{
				intersections++;
				temp = atan2(Corners[i].y, Corners[i].x);
				intersectionRecord.push_back(IntersectionRecord(temp,true));
			}
			continue;
		}
		if(norm_p[i] > r && norm_p[j] == r){
			if(numberOfCircleLineIntersections(Corners[i], Corners[j], r) == 2){
				intersections++;
				temp = circleLineIntersection(Corners[i], Corners[j], r);
				intersectionRecord.push_back(IntersectionRecord(temp,false));

				intersections++;
				temp = atan2(Corners[j].y, Corners[j].x);
				intersectionRecord.push_back(IntersectionRecord(temp,true));
			}else{
				intersections++;
				temp = atan2(Corners[j].y, Corners[j].x);
				intersectionRecord.push_back(IntersectionRecord(temp,false));
			}
			continue;
		}

		// both points are on the circle
		if(norm_p[i] == r && norm_p[j] == r){
			intersections++;
			temp = atan2(Corners[i].y, Corners[i].x);
			intersectionRecord.push_back(IntersectionRecord(temp,false));

			intersections++;
			temp = atan2(Corners[j].y, Corners[j].x);
			intersectionRecord.push_back(IntersectionRecord(temp,true));
			continue;
		}

		// both points are outside, can intersect in zero points or 2 points
		if(norm_p[i] > r && norm_p[j] > r){
			Vector2 n(Corners[j].y-Corners[i].y, Corners[i].x-Corners[j].x);
			n = normalize(n);
			Float dotP = (n.x*Corners[i].x + n.y*Corners[i].y);
			Point P_O(dotP*n.x, dotP*n.y, 0.0f);

			// If projection is not in the circle, the line-segment is completely outside circle
			if(P_O.x*P_O.x + P_O.y*P_O.y >= r*r){
				continue;
			}

			Float alpha = -1.0f;
	        // Compute where the projection is, with respect to the line joining p[i] and p[j]
			if (Corners[i].x != Corners[j].x)
				alpha = (P_O.x-Corners[j].x)/(Corners[i].x-Corners[j].x);
			else
				alpha = (P_O.y-Corners[j].y)/(Corners[i].y-Corners[j].y);

			if(alpha <= 0 || alpha >= 1)
				continue;

			intersections++;
			temp = circleLineIntersection(Corners[i], P_O, r);
			intersectionRecord.push_back(IntersectionRecord(temp,false));

			intersections++;
			temp = circleLineIntersection(P_O, Corners[j], r);
			intersectionRecord.push_back(IntersectionRecord(temp,true));
			continue;
		}
	}

	if(intersections == 0){
		Vector2 v0(Corners[0].x, Corners[0].y);
		Vector2 v1(Corners[1].x-Corners[0].x, Corners[1].y-Corners[0].y);
		Vector2 v2(Corners[2].x-Corners[0].x, Corners[2].y-Corners[0].y);

	    Float a = -det(v0, v2)/det(v1, v2);
	    Float b =  det(v0, v1)/det(v1, v2);
		if(a>0 && b>0 && (a+b)<1){
			thetaMin[0]=0;
			thetaMax[0]=2*M_PI;
			indices = 1;
			return true;
		}
		return false;
	}

	// Consolidate non-unique thetas by merging the repeats (for points lying on the circle)
	std::vector<int> deleteIndices;
	size_t size = intersectionRecord.size();
	if(size == 2)
		size = 1; // For size=2, checking the last value of theta is redundancy
	for(size_t i = 0; i < size; i++){
		size_t j = (i+1)%intersectionRecord.size();
		if(intersectionRecord[i].theta == intersectionRecord[j].theta){
			if(intersectionRecord[i].directionOutside == intersectionRecord[j].directionOutside)
				deleteIndices.push_back(i);
			else{
				deleteIndices.push_back(i);
				if(j != 0) // To maintain deleteIndices as ascending array
					deleteIndices.push_back(j);
				else
					deleteIndices.insert(deleteIndices.begin(), j);
			}
			i++;
		}
	}
	for(auto rit = deleteIndices.rbegin();rit != deleteIndices.rend(); rit++)
		intersectionRecord.erase(intersectionRecord.begin() + *rit);

	std::for_each(intersectionRecord.begin(), intersectionRecord.end(), [](IntersectionRecord &x){ x.theta = fmod(x.theta, 2*M_PI);
																	if (x.theta < 0)
																		x.theta += 2*M_PI;
																	return x; });

	std::sort(intersectionRecord.begin(), intersectionRecord.end(), IntersectionRecord::compare);


	// Remaining intersections must be even
	if(intersectionRecord.size()%2 == 1){
		SLog(EError,"Odd number of intersection in ellipse triangle intersection: radius: %f, corner-1 (%f, %f, %f), corner-2 (%f, %f, %f), corner-3 (%f, %f, %f)", r, Corners[0].x, Corners[0].y, Corners[0].z, Corners[1].x, Corners[1].y, Corners[1].z, Corners[2].x, Corners[2].y, Corners[2].z);
	}


	Vector N  = cross(Corners[1]-Corners[0], Corners[2]-Corners[0]);

	size_t start = 0;
	if( intersectionRecord[0].directionOutside ^ (N.z > 0) )
		start = 1;

	size_t j;
	for(size_t i = start; i < intersectionRecord.size(); i+=2){
		thetaMin[indices] = intersectionRecord[i].theta;
		j = i + 1;
		if(j == intersectionRecord.size())
			j = 1;
		thetaMax[indices] = intersectionRecord[j].theta;
		if(thetaMax[indices] < thetaMin[indices]){
			thetaMax[indices + 1] = thetaMax[indices];
			thetaMax[indices] = 2*M_PI;
			thetaMin[indices + 1] = 0;
			indices++;
		}
		indices++;
	}
	return true;
}

template <typename PointType, typename LengthType>
int TEllipsoid<PointType, LengthType>::numberOfCircleLineIntersections(const Point &P1, const Point &P2, const Float &r) const{
	Float x1 = P1.x;
	Float y1 = P1.y;
	Float x2 = P2.x;
	Float y2 = P2.y;

	Float dx = x1 - x2;
	Float dy = y1 - y2;
	Float a  = dx*dx + dy*dy;
	Float b  = 2*(dx*x2 + dy*y2);
	Float c  = x2*x2 + y2*y2 - r*r;

	Float det = (b*b-4*a*c);
	Float alpha = (-b+det)/(2*a);
	int count = 0;
	if(alpha >= 0 && alpha <= 1)
		count++;
	alpha = (-b-det)/(2*a);
	if(alpha >= 0 && alpha <= 1)
		count++;
	return count;
}

template <typename PointType, typename LengthType>
Float TEllipsoid<PointType, LengthType>::circleLineIntersection(const Point &P1, const Point &P2, const Float &r) const{
	double x1 = (double)P1.x;
	double y1 = (double)P1.y;
	double x2 = (double)P2.x;
	double y2 = (double)P2.y;

	double dx = x1 - x2;
	double dy = y1 - y2;
	double a  = dx*dx + dy*dy;
	double b  = 2*(dx*x2 + dy*y2);
	double c  = x2*x2 + y2*y2 - r*r;

	double det = sqrt(b*b-4*a*c);
	if(det < 0){ // To compensate for Float precision errors
		SLog(EError,"Circle-Line intersection resulted in a possible float precision error or called without an intersection -- Debug values: P1(%lf, %lf, %lf), P2(%lf, %lf, %lf), r(%lf),"
				"																					  x1(%lf), y1(%lf), x2(%lf), y2(%lf),"
				"																					  dx(%lf), dy(%lf), a(%lf), b(%lf), c(%lf), det(%lf);",
																										P1.x, P1.y, P1.z, P2.x, P2.y, P2.z, r,
																										x1, y1, x2, y2,
																										dx, dy, a, b, c, det);
	}

	double alpha = (-b+det)/(2*a);

	double x = 0;
	double y = std::numeric_limits<Float>::min();

	if(alpha >= 0 && alpha <= 1){
		x = alpha * x1 + (1-alpha) * x2;
		y = alpha * y1 + (1-alpha) * y2;
	}else{
		alpha = (-b-det)/(2*a);
		if(alpha >= 0 && alpha <= 1){
			x = alpha * x1 + (1-alpha) * x2;
			y = alpha * y1 + (1-alpha) * y2;
		}else{
			SLog(EError,"Circle-Line intersection called with out an actual intersection -- Debug values: P1(%lf, %lf, %lf), P2(%lf, %lf, %lf), r(%lf),"
					"																					  x1(%lf), y1(%lf), x2(%lf), y2(%lf),"
					"																					  dx(%lf), dy(%lf), a(%lf), b(%lf), c(%lf), det(%lf);",
																											P1.x, P1.y, P1.z, P2.x, P2.y, P2.z, r,
																											x1, y1, x2, y2,
																											dx, dy, a, b, c, det);
		}
	}

	return (Float)atan2(y, x);
}

template <typename PointType, typename LengthType>
Float TEllipsoid<PointType, LengthType>::ellipticSampleWeight(Float k, Float thetaMin[], Float thetaMax[],size_t &indices) const{
	Float arcLength = 0;
	for(size_t i = 0; i < indices; i++){
		arcLength += boost::math::ellint_2(k, thetaMax[i]) - boost::math::ellint_2(k, thetaMin[i]);
	}
	return arcLength;
}

template <typename PointType, typename LengthType>
Float TEllipsoid<PointType, LengthType>::ellipticCurveSampling(Float k, Float thetaMin[], Float thetaMax[], size_t &indices, ref<Sampler> sampler) const{
	Float cumsum[4];
	cumsum[0] = thetaMax[0] - thetaMin[0];

	for(size_t i=2; i < indices; i++){
		cumsum[i] = cumsum[i - 1] + (thetaMax[i] - thetaMin[i]);
	}

	while(1){
		Float theta_s = cumsum[indices-1] * sampler->nextFloat(), theta;
		if(theta_s < cumsum[0])
			theta = theta_s + thetaMin[0];
		for(size_t i = 2;i < indices; i++){
			if(theta_s < cumsum[i]){
				theta = theta_s -cumsum[i-1] + thetaMin[i];
			}
		}
		Float r = sampler->nextFloat();
		if(r < sqrt(1 - pow(k*cos(theta),2))){
			return theta;
		}
	}
}
template <typename PointType, typename LengthType>
bool TEllipsoid<PointType, LengthType>::ellipsoidIntersectTriangle(const Point &triA, const Point &triB, const Point &triC, Float &value, Float &u, Float &v, ref<Sampler> sampler) const {
	//Compute the center of the ellipse (resulting from ellipsoid-plane intersection)
	Point SphereA;transformToSphere(triA, SphereA);
	Point SphereB;transformToSphere(triB, SphereB);
	Point SphereC;transformToSphere(triC, SphereC);
	Point Origin(0.0f, 0.0f, 0.0f);

	Vector b = SphereC-SphereA, c = SphereB-SphereA, N = cross(c, b);
	N = normalize(N);

	Vector Center = dot(N,SphereA-Origin)*N;

	Vector O(Center[0]*this->a, Center[1]*this->b, Center[2]*this->b); // Note that O is position vector of the center of the ellipse

	Float d = Center.lengthSquared();

	if(d > 1){ // ellipsoid does not intersect the plane
		return false;
	}

	//Compute the angle of the ellipse with T
	Point EllipsoidA;transformToEllipsoid(triA, EllipsoidA);
	Point EllipsoidB;transformToEllipsoid(triB, EllipsoidB);
	Point EllipsoidC;transformToEllipsoid(triC, EllipsoidC);

	Vector T = EllipsoidB - EllipsoidA;T = normalize(T);
	N = cross(T, EllipsoidC-EllipsoidA); N = normalize(N);
	Vector U = cross(N, T);

	Float TTD = weightedIP(T, T);
	Float TUD = weightedIP(T, U);
	Float UUD = weightedIP(U, U);
	Float OOD = weightedIP(O, O);

	Float theta = 0.5 * atan2(2*TUD, UUD-TTD);
	if(std::isnan(theta) || std::isinf(theta)){
		SLog(EError,"Theta is not valid; TUD: %f, UUD: %f, TTD: %f; \n", TUD, UUD, TTD);
	}

	Vector NewX = T*cos(theta) - U*sin(theta);
	Vector NewY = T*sin(theta) + U*cos(theta);

	if(std::isnan(NewX[0]) || std::isnan(NewX[1]) || std::isnan(NewX[2]) || std::isinf(NewX[0]) || std::isinf(NewX[1]) || std::isinf(NewX[2]) ||
		std::isnan(NewY[0]) || std::isnan(NewY[1]) || std::isnan(NewY[2]) || std::isinf(NewY[0]) || std::isinf(NewY[1]) || std::isinf(NewY[2]))
		SLog(EError,"One of the Ellipse axis is not valid:");

	// Compute the transform to shift axis of ellipsoid to ellipse
	Transform invEllipsoid2Ellipse(Matrix4x4({NewX[0], NewY[0], N[0], 0.0f,
												 NewX[1], NewY[1], N[1], 0.0f,
												 NewX[2], NewY[2], N[2], 0.0f,
												 0.0f, 0.0f, 0.0f, 1.0f
												}));
	Transform Ellipsoid2Ellipse = invEllipsoid2Ellipse.inverse()*Transform::translate(-O);
	invEllipsoid2Ellipse = Ellipsoid2Ellipse.inverse();

	// Compute major and minor axis
	Float det 	= sqrt(4*TUD*TUD+(TTD-UUD)*(TTD-UUD));
	Float m1 	= sqrt(2 * (1-OOD)/(TTD+UUD-det)); // Major axis
	Float m2 	= sqrt(2 * (1-OOD)/(TTD+UUD+det)); // Minor axis
	Float k  	= sqrt(1-m2*m2/(m1*m1)); // eccentricity

	// Compute transform to scale ellipse to circle

	Transform Ellipse2Circle = Transform::scale(Vector(1, m1/m2, 1));
	Transform Ellipsoid2Circle = Ellipse2Circle*Ellipsoid2Ellipse;

	Point Corners[3];
	Corners[0] = Ellipsoid2Circle(EllipsoidA);
	Corners[1] = Ellipsoid2Circle(EllipsoidB);
	Corners[2] = Ellipsoid2Circle(EllipsoidC);

	// Circle Triangle Intersection to get all the angles
	Float thetaMin[4];
	Float thetaMax[4];
	size_t indices;
	value = 0;
	if(circlePolygonIntersectionAngles(thetaMin, thetaMax, indices, Corners, m1)){
		// Sample an angle using elliptic sampling algorithm
		Float angle = ellipticCurveSampling(k, thetaMin, thetaMax, indices, sampler);
		Point Projection(m1*cos(angle), m2*sin(angle), 0.0f), Original;
		Projection = invEllipsoid2Ellipse(Projection);
		transformFromEllipsoid(Projection, Original);
		value = ellipticSampleWeight(k, thetaMin, thetaMax, indices)/m1;
		//Compute the Barycentric co-ordinates. Return that and save it in the cache.
		Barycentric(Original, triA, triB, triC, u, v);
		return true;
	}
//Old algorithm that is no longer relevant
//	float R = sqrt(1-d);
//
//	Vector Z(0.0f, 0.0f, 1.0f);
//	Transform T3D2D, T3D2Dinv;
//	Transform temp;
//	T3D2D = temp.rotateVector2Vector(N, Z)*temp.translate(-Center);
//
//	T3D2Dinv = T3D2D.inverse();
//
//	Point Corners[3];
//
//	Corners[0] = T3D2D(SphereA);
//	Corners[1] = T3D2D(SphereB);
//	Corners[2] = T3D2D(SphereC);
//
//	Float angle = 0.0f;
//	if(circlePolygonIntersection(Corners, R, sampler, angle, value)){
//		Point Projection(R*cos(angle), R*sin(angle), 0.0f), Original;
//		Projection = T3D2Dinv(Projection);
//		e.transformFromSphere(Projection, Original);
//		//Compute the Barycentric co-ordinates. Return that and save it in the cache.
//		Barycentric(Original, A, B, C, u, v);
//
//		return true;
//	}

	return false;
}


MTS_NAMESPACE_END

#endif /* __MITSUBA_CORE_RAY_H_ */
