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
	
	/* center of the ellipse */
	PointType  C;

	LengthType Tau;
	
	bool degenerateEllipse;

	Transform T3D3D;
	Transform T3D3Dinv;
	Transform T3D2D;
	
	struct BoundingBox{
		PointType min;
		PointType max;
	}m_aabb;

	/// Construct a new ellipse
	inline TEllipse(const PointType p1, const PointType p2, const LengthType tau):
			f1(p1), f2(p2), Tau(tau){
		// Algorithm to compute the T3D3D
		LengthType a = tau/2.0f;
		C = (f1 + f2) * 0.5f;
		LengthType b = (a*a-distanceSquared(C, f1));
		if(b < 0)
			degenerateEllipse = true;
		else
			degenerateEllipse = false;
		b = sqrt(b);
		TVector3<LengthType> D = f2-f1;
		TVector3<LengthType> Scale(1/a, 1/b, 1/b);
		TVector3<LengthType> Rot(1.0f, 0.0f, 0.0f);
		TVector3<LengthType> Cv(-C);
		
		T3D3D = Transform::scale(Scale)*Transform::rotateVector2Vector(D, Rot)*Transform::translate(Cv);
		T3D3Dinv = T3D3D.inverse();
		
		/*compute bounding box for the arbitrary oriented ellipse*/
		Matrix4x4 S(TVector4<LengthType>(1.0f, 0.0f, 0.0f, 0.0f),
					TVector4<LengthType>(0.0f, 1.0f, 0.0f, 0.0f),
					TVector4<LengthType>(0.0f, 0.0f, 1.0f, 0.0f),
					TVector4<LengthType>(0.0f, 0.0f, 0.0f,-1.0f)
								);
		Matrix4x4 MT, M = T3D3Dinv.getMatrix();
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
