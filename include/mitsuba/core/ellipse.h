/*
    This file is added by Adithya.
    */

#pragma once
#if !defined(__MITSUBA_CORE_ELLIPSE_H_)
#define __MITSUBA_CORE_ELLIPSE_H_


#include <mitsuba/mitsuba.h>
#include <mitsuba/core/transform.h>

MTS_NAMESPACE_BEGIN

template <typename _PointType, typename _LengthType> struct TEllipse{
	typedef _PointType                  PointType;
	typedef _LengthType                 LengthType;
	
	/* Focal points */
	PointType f1; 
	PointType f2;
	
	LengthType Tau;
	
	bool degenerateEllipse;

	Transform T3D3D;
	Transform T3D3Dinv;
	Transform T3D2D;
	
	/// Construct a new ellipse
	inline TEllipse(const PointType p1, const PointType p2, const LengthType tau):
			f1(p1), f2(p2), Tau(tau){
		// Algorithm to compute the T3D3D
		LengthType a = tau/2.0f;
		PointType  C = (f1 + f2) * 0.5f;
		LengthType b = (a*a-distanceSquared(C, f1));
		if(b < 0)
			degenerateEllipse = true;
		else
			degenerateEllipse = false;
		b = sqrt(b);
		TVector3<LengthType> D = f2-f1;
		TVector3<LengthType> S(1/a, 1/b, 1/b);
		TVector3<LengthType> R(1.0f, 0.0f, 0.0f);
		TVector3<LengthType> Cv(-C);
		
		Transform temp;
		T3D3D = temp.scale(S)*temp.rotateVector2Vector(D, R)*temp.translate(Cv);
		T3D3Dinv = T3D3D.inverse();
		
	}
	
	inline bool isDegenerate() const { return degenerateEllipse; }
		
	inline void transformToSphere(const Point &A, Point &B) const{
		B = T3D3D(A);
		return;
	}

	inline void transformFromSphere(const Point &A, Point &B) const{
		B = T3D3Dinv(A);
		return;
	}

	inline bool isInside(float x, float y, float z) const{
		Point P(x, y, z);
		if(lengthSquared(T3D3D(P)) < 1)
			return true;
		return false;
	}

	inline bool isInside(Point P) const{
		if(lengthSquared(T3D3D(P)) < 1)
			return true;
		return false;
	}
	
	inline Float length(Point P) const{
		return (P-f1).length() + (P-f2).length();
	}
//
//	inline bool isEllipticPoint(Point P) const{
//		return ((P-f1).length + (P-f2).length - Tau) < 0.01f);
//	}

	
		
};


MTS_NAMESPACE_END

#endif /* __MITSUBA_CORE_RAY_H_ */
