/*
    This file is added by Adithya.
    */

#pragma once
#if !defined(__MITSUBA_RENDER_ELLIPSE_H_)
#define __MITSUBA_RENDER_ELLIPSE_H_
#define FLOAT double
#define POINT Point3d
#define VECTOR2 Vector2d
#define VECTOR Vector3d
#define VECTOR4 Vector4d
#define PI M_PI_DBL

#include <algorithm>
#include <mitsuba/mitsuba.h>
#include <mitsuba/core/transform.h>
#include <boost/math/special_functions/ellint_2.hpp>
#include <mitsuba/render/sampler.h>
#include <mitsuba/core/matrix.h>

MTS_NAMESPACE_BEGIN



struct Normal_FLOAT : public TVector3<FLOAT> {
	/** \brief Construct a new normal without initializing it.
	 *
	 * This construtor is useful when the normal will either not
	 * be used at all (it might be part of a larger data structure)
	 * or initialized at a later point in time. Always make sure
	 * that one of the two is the case! Otherwise your program will do
	 * computations involving uninitialized memory, which will probably
	 * lead to a difficult-to-find bug.
	 */
	Normal_FLOAT() { }

	/// Initialize the vector with the specified X and Z components
	Normal_FLOAT(FLOAT x, FLOAT y, FLOAT z) : TVector3<FLOAT>(x, y, z) { }

	/// Initialize all components of the the normal with the specified value
	explicit Normal_FLOAT(FLOAT val) : TVector3<FLOAT>(val) { }

	/// Unserialize a normal from a binary data stream
	Normal_FLOAT(Stream *stream) {
		x = stream->readElement<FLOAT>();
		y = stream->readElement<FLOAT>();
		z = stream->readElement<FLOAT>();
	}

	/// Construct a normal from a vector data structure
	Normal_FLOAT(const TVector3<FLOAT> &v) : TVector3<FLOAT>(v.x, v.y, v.z) { }

	/// Assign a vector to this normal
	void operator=(const TVector3<FLOAT> &v) {
		x = v.x; y = v.y; z = v.z;
	}
};

inline Normal_FLOAT normalize(const Normal_FLOAT &n) {
	return n / n.length();
}


/**
 * \brief Basic 4x4 matrix data type
 * \ingroup libcore
 * \ingroup libpython
 */
struct MTS_EXPORT_CORE Matrix4x4_FLOAT : public Matrix<4, 4, FLOAT> {
	inline Matrix4x4_FLOAT() { }

	/// Initialize the matrix with constant entries
	explicit inline Matrix4x4_FLOAT(FLOAT value) : Matrix<4, 4, FLOAT>(value) { }

	/// Initialize the matrix from a given 4x4 array
	explicit inline Matrix4x4_FLOAT(const FLOAT _m[4][4]) : Matrix<4, 4, FLOAT>(_m) { }

	/// Initialize the matrix from a given (FLOAT) 4x4 array in row-major order
	explicit inline Matrix4x4_FLOAT(const FLOAT _m[16]) : Matrix<4, 4, FLOAT>(_m) { }

	/// Initialize the matrix from four 4D column vectors
	explicit inline Matrix4x4_FLOAT(const VECTOR4 &v1, const VECTOR4 &v2, const VECTOR4 &v3, const VECTOR4 &v4) {
		m[0][0] = v1.x; m[0][1] = v2.x; m[0][2] = v3.x; m[0][3] = v4.x;
		m[1][0] = v1.y; m[1][1] = v2.y; m[1][2] = v3.y; m[1][3] = v4.y;
		m[2][0] = v1.z; m[2][1] = v2.z; m[2][2] = v3.z; m[2][3] = v4.z;
		m[3][0] = v1.w; m[3][1] = v2.w; m[3][2] = v3.w; m[3][3] = v4.w;
	}

	/// Unserialize a matrix from a stream
	explicit inline Matrix4x4_FLOAT(Stream *stream) : Matrix<4, 4, FLOAT>(stream) { }

	/// Copy constructor
	inline Matrix4x4_FLOAT(const Matrix<4, 4, FLOAT> &mtx) : Matrix<4, 4, FLOAT>(mtx) { }

	/// Initialize with the given values
	inline Matrix4x4_FLOAT(
		FLOAT a00, FLOAT a01, FLOAT a02, FLOAT a03,
		FLOAT a10, FLOAT a11, FLOAT a12, FLOAT a13,
		FLOAT a20, FLOAT a21, FLOAT a22, FLOAT a23,
		FLOAT a30, FLOAT a31, FLOAT a32, FLOAT a33) {
		m[0][0] = a00; m[0][1] = a01; m[0][2] = a02; m[0][3] = a03;
		m[1][0] = a10; m[1][1] = a11; m[1][2] = a12; m[1][3] = a13;
		m[2][0] = a20; m[2][1] = a21; m[2][2] = a22; m[2][3] = a23;
		m[3][0] = a30; m[3][1] = a31; m[3][2] = a32; m[3][3] = a33;
	}

	/// Return the determinant of the upper left 3x3 sub-matrix
	inline FLOAT det3x3() const {
		return ((m[0][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1]))
			  - (m[0][1] * (m[1][0] * m[2][2] - m[1][2] * m[2][0]))
			  + (m[0][2] * (m[1][0] * m[2][1] - m[1][1] * m[2][0])));
	}

	/// Matrix-vector multiplication
	inline VECTOR4 operator*(const VECTOR4 &v) const {
		return VECTOR4(
			m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3] * v.w,
			m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3] * v.w,
			m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3] * v.w,
			m[3][0] * v.x + m[3][1] * v.y + m[3][2] * v.z + m[3][3] * v.w
		);
	}

	/// Scalar multiplication (creates a temporary)
	inline Matrix4x4_FLOAT operator*(FLOAT value) const {
		Matrix4x4_FLOAT result;
		for (int i=0; i<4; ++i)
			for (int j=0; j<4; ++j)
				result.m[i][j] = m[i][j]*value;
		return result;
	}

	/// Assignment operator
	inline Matrix4x4_FLOAT &operator=(const Matrix<4, 4, FLOAT> &mat) {
		for (int i=0; i<4; ++i)
			for (int j=0; j<4; ++j)
				m[i][j] = mat.m[i][j];
		return *this;
	}

	/// Return a row by index
	inline VECTOR4 row(int i) const {
		return VECTOR4(
			m[i][0], m[i][1], m[i][2], m[i][3]
		);
	}

	/// Return a column by index
	inline VECTOR4 col(int i) const {
		return VECTOR4(
			m[0][i], m[1][i], m[2][i], m[3][i]
		);
	}
};


struct MTS_EXPORT_CORE Transform_FLOAT {
public:
	/// Create an identity transformation
	Transform_FLOAT() {
		m_transform.setIdentity();
		m_invTransform.setIdentity();
	}

	/// Unserialize a transformation from a stream
	inline Transform_FLOAT(Stream *stream) {
		m_transform = Matrix4x4_FLOAT(stream);
		m_invTransform = Matrix4x4_FLOAT(stream);
	}

	/** \brief Create a transform from the given matrix
	 * and calculate the inverse
	 */
	Transform_FLOAT(const Matrix4x4_FLOAT &trafo)
		: m_transform(trafo) {
		bool success = m_transform.invert(m_invTransform);
		if (!success)
			SLog(EError, "Unable to invert singular matrix %s", trafo.toString().c_str());
	}

	/// Create a transform from the given matrices
	Transform_FLOAT(const Matrix4x4_FLOAT &trafo, const Matrix4x4_FLOAT &invTrafo)
		: m_transform(trafo), m_invTransform(invTrafo) {
	}

	/// Return the inverse transform
	Transform_FLOAT inverse() const {
		return Transform_FLOAT(m_invTransform, m_transform);
	}

	/// Matrix-matrix multiplication
	Transform_FLOAT operator*(const Transform_FLOAT &t) const{
		return Transform_FLOAT(m_transform * t.m_transform,
			t.m_invTransform * m_invTransform);
	}

	/// Return the determinant of the upper left 3x3 submatrix
	inline FLOAT det3x3() const {
		return m_transform.det3x3();
	}

	/// Test for a scale component
	inline bool hasScale() const {
		for (int i=0; i<3; ++i) {
			for (int j=i; j<3; ++j) {
				FLOAT sum = 0;
				for (int k=0; k<3; ++k)
					sum += m_transform.m[i][k] * m_transform.m[j][k];

				if (i == j && std::abs(sum-1) > 1e-3f)
					return true;
				else if (i != j && std::abs(sum) > 1e-3f)
					return true;
			}
		}
		return false;
	}

	/// Test if this is the identity transformation
	inline bool isIdentity() const {
		return m_transform.isIdentity();
	}

	/// Test if this is the zero transformation
	inline bool isZero() const {
		return m_transform.isZero();
	}

	/**
	 * \brief Matrix-vector multiplication for points in 3d space
	 *
	 * \remark In the Python bindings, this is function implemented as
	 * the multiplication operator (\c __mul__).
	 */
	inline POINT operator()(const POINT &p) const {
		FLOAT x = m_transform.m[0][0] * p.x + m_transform.m[0][1] * p.y
		        + m_transform.m[0][2] * p.z + m_transform.m[0][3];
		FLOAT y = m_transform.m[1][0] * p.x + m_transform.m[1][1] * p.y
		        + m_transform.m[1][2] * p.z + m_transform.m[1][3];
		FLOAT z = m_transform.m[2][0] * p.x + m_transform.m[2][1] * p.y
		        + m_transform.m[2][2] * p.z + m_transform.m[2][3];
		FLOAT w = m_transform.m[3][0] * p.x + m_transform.m[3][1] * p.y
		        + m_transform.m[3][2] * p.z + m_transform.m[3][3];
#ifdef MTS_DEBUG
		if (w == 0)
			SLog(EWarn, "w==0 in Transform::operator(Point &)");
#endif
		if (w == 1.0f)
			return POINT(x, y, z);
		else
			return POINT(x, y, z) / w;
	}

	/// Transform a point by an affine / non-projective matrix
	inline POINT transformAffine(const POINT &p) const {
		FLOAT x = m_transform.m[0][0] * p.x + m_transform.m[0][1] * p.y
		        + m_transform.m[0][2] * p.z + m_transform.m[0][3];
		FLOAT y = m_transform.m[1][0] * p.x + m_transform.m[1][1] * p.y
		        + m_transform.m[1][2] * p.z + m_transform.m[1][3];
		FLOAT z = m_transform.m[2][0] * p.x + m_transform.m[2][1] * p.y
		        + m_transform.m[2][2] * p.z + m_transform.m[2][3];
		return POINT(x,y,z);
	}

	/// Transform a point by a affine / non-projective matrix (no temporaries)
	inline void transformAffine(const POINT &p, POINT &dest) const {
		dest.x = m_transform.m[0][0] * p.x + m_transform.m[0][1] * p.y
		       + m_transform.m[0][2] * p.z + m_transform.m[0][3];
		dest.y = m_transform.m[1][0] * p.x + m_transform.m[1][1] * p.y
		       + m_transform.m[1][2] * p.z + m_transform.m[1][3];
		dest.z = m_transform.m[2][0] * p.x + m_transform.m[2][1] * p.y
		       + m_transform.m[2][2] * p.z + m_transform.m[2][3];
	}

	/**
	 * \brief Matrix-vector multiplication for points in 3d space (no temporaries)
	 * \remark This function is not available in the Python bindings
	 */
    inline void operator()(const POINT &p, POINT &dest) const {
		dest.x = m_transform.m[0][0] * p.x + m_transform.m[0][1] * p.y
		       + m_transform.m[0][2] * p.z + m_transform.m[0][3];
		dest.y = m_transform.m[1][0] * p.x + m_transform.m[1][1] * p.y
		       + m_transform.m[1][2] * p.z + m_transform.m[1][3];
		dest.z = m_transform.m[2][0] * p.x + m_transform.m[2][1] * p.y
		       + m_transform.m[2][2] * p.z + m_transform.m[2][3];
		FLOAT w = m_transform.m[3][0] * p.x + m_transform.m[3][1] * p.y
		        + m_transform.m[3][2] * p.z + m_transform.m[3][3];

#ifdef MTS_DEBUG
		if (w == 0)
			SLog(EWarn, "w==0 in Transform::operator(Point &, Point &)");
#endif
		if (w != 1.0f)
			dest /= w;
	}

	/**
	 * \brief Matrix-vector multiplication for vectors in 3d space
	 * \remark In the Python bindings, this is function implemented as
	 * the multiplication operator (\c __mul__).
	 */
    inline VECTOR operator()(const VECTOR &v) const {
		FLOAT x = m_transform.m[0][0] * v.x + m_transform.m[0][1] * v.y
		        + m_transform.m[0][2] * v.z;
		FLOAT y = m_transform.m[1][0] * v.x + m_transform.m[1][1] * v.y
		        + m_transform.m[1][2] * v.z;
		FLOAT z = m_transform.m[2][0] * v.x + m_transform.m[2][1] * v.y
		        + m_transform.m[2][2] * v.z;
		return VECTOR(x, y, z);
	}

	/**
	 * \brief Matrix-vector multiplication for vectors in 3d space (no temporaries)
	 * \remark This function is not available in the Python bindings
	 */
    inline void operator()(const VECTOR &v, VECTOR &dest) const {
		dest.x = m_transform.m[0][0] * v.x + m_transform.m[0][1] * v.y
		       + m_transform.m[0][2] * v.z;
		dest.y = m_transform.m[1][0] * v.x + m_transform.m[1][1] * v.y
		       + m_transform.m[1][2] * v.z;
		dest.z = m_transform.m[2][0] * v.x + m_transform.m[2][1] * v.y
		       + m_transform.m[2][2] * v.z;
	}

	/**
	 * \brief Matrix-normal multiplication
	 * \remark In the Python bindings, this is function implemented as
	 * the multiplication operator (\c __mul__).
	 */
    inline Normal_FLOAT operator()(const Normal_FLOAT &v) const {
		FLOAT x = m_invTransform.m[0][0] * v.x + m_invTransform.m[1][0] * v.y
		        + m_invTransform.m[2][0] * v.z;
		FLOAT y = m_invTransform.m[0][1] * v.x + m_invTransform.m[1][1] * v.y
		        + m_invTransform.m[2][1] * v.z;
		FLOAT z = m_invTransform.m[0][2] * v.x + m_invTransform.m[1][2] * v.y
		        + m_invTransform.m[2][2] * v.z;
		return Normal_FLOAT(x, y, z);
	}

	/**
	 * \brief Matrix-normal multiplication (no temporaries)
	 * \remark This function is not available in the Python bindings
	 */
    inline void operator()(const Normal_FLOAT &v, Normal_FLOAT &dest) const {
		dest.x = m_invTransform.m[0][0] * v.x + m_invTransform.m[1][0] * v.y
		       + m_invTransform.m[2][0] * v.z;
		dest.y = m_invTransform.m[0][1] * v.x + m_invTransform.m[1][1] * v.y
		       + m_invTransform.m[2][1] * v.z;
		dest.z = m_invTransform.m[0][2] * v.x + m_invTransform.m[1][2] * v.y
		       + m_invTransform.m[2][2] * v.z;
	}

	/**
	 * \brief 4D matrix-vector multiplication
	 * \remark In the Python bindings, this is function implemented as
	 * the multiplication operator (\c __mul__).
	 */
	inline VECTOR4 operator()(const VECTOR4 &v) const {
		FLOAT x = m_transform.m[0][0] * v.x + m_transform.m[0][1] * v.y
		        + m_transform.m[0][2] * v.z + m_transform.m[0][3] * v.w;
		FLOAT y = m_transform.m[1][0] * v.x + m_transform.m[1][1] * v.y
		        + m_transform.m[1][2] * v.z + m_transform.m[1][3] * v.w;
		FLOAT z = m_transform.m[2][0] * v.x + m_transform.m[2][1] * v.y
		        + m_transform.m[2][2] * v.z + m_transform.m[2][3] * v.w;
		FLOAT w = m_transform.m[3][0] * v.x + m_transform.m[3][1] * v.y
		        + m_transform.m[3][2] * v.z + m_transform.m[3][3] * v.w;
		return VECTOR4(x,y,z,w);
	}

	/**
	 * \brief 4D matrix-vector multiplication (no temporaries)
	 * \remark This function is not available in the Python bindings
	 */
	inline void operator()(const VECTOR4 &v, VECTOR4 &dest) const {
		dest.x = m_transform.m[0][0] * v.x + m_transform.m[0][1] * v.y
		       + m_transform.m[0][2] * v.z + m_transform.m[0][3] * v.w;
		dest.y = m_transform.m[1][0] * v.x + m_transform.m[1][1] * v.y
		       + m_transform.m[1][2] * v.z + m_transform.m[1][3] * v.w;
		dest.z = m_transform.m[2][0] * v.x + m_transform.m[2][1] * v.y
		       + m_transform.m[2][2] * v.z + m_transform.m[2][3] * v.w;
		dest.w = m_transform.m[3][0] * v.x + m_transform.m[3][1] * v.y
		       + m_transform.m[3][2] * v.z + m_transform.m[3][3] * v.w;
	}

	/// Return the underlying matrix
	inline const Matrix4x4_FLOAT &getMatrix() const { return m_transform; }

	/// Return the underlying inverse matrix (const version)
	inline const Matrix4x4_FLOAT &getInverseMatrix() const { return m_invTransform; }

	/// Create a translation transformation
	static Transform_FLOAT translate(const VECTOR &v){
		Matrix4x4_FLOAT trafo(
			1, 0, 0, v.x,
			0, 1, 0, v.y,
			0, 0, 1, v.z,
			0, 0, 0, 1
		);
		Matrix4x4_FLOAT invTrafo(
			1, 0, 0, -v.x,
			0, 1, 0, -v.y,
			0, 0, 1, -v.z,
			0, 0, 0, 1
		);
		return Transform_FLOAT(trafo, invTrafo);
	}

	/// Create a rotation transformation around an arbitrary axis. The angle is specified in degrees
	static Transform_FLOAT rotate(const VECTOR &axis, FLOAT angle){
		FLOAT sinTheta, cosTheta;

		/* Make sure that the axis is normalized */
		VECTOR naxis = normalize(axis);
		math::sincos(degToRad(angle), &sinTheta, &cosTheta);

		Matrix4x4_FLOAT result;
		result(0, 0) = naxis.x * naxis.x + (1.0f - naxis.x * naxis.x) * cosTheta;
		result(0, 1) = naxis.x * naxis.y * (1.0f - cosTheta) - naxis.z * sinTheta;
		result(0, 2) = naxis.x * naxis.z * (1.0f - cosTheta) + naxis.y * sinTheta;
		result(0, 3) = 0;

		result(1, 0) = naxis.x * naxis.y * (1.0f - cosTheta) + naxis.z * sinTheta;
		result(1, 1) = naxis.y * naxis.y + (1.0f - naxis.y * naxis.y) * cosTheta;
		result(1, 2) = naxis.y * naxis.z * (1.0f - cosTheta) - naxis.x * sinTheta;
		result(1, 3) = 0;

		result(2, 0) = naxis.x * naxis.z * (1.0f - cosTheta) - naxis.y * sinTheta;
		result(2, 1) = naxis.y * naxis.z * (1.0f - cosTheta) + naxis.x * sinTheta;
		result(2, 2) = naxis.z * naxis.z + (1.0f - naxis.z * naxis.z) * cosTheta;
		result(2, 3) = 0;

		result(3, 0) = 0;
		result(3, 1) = 0;
		result(3, 2) = 0;
		result(3, 3) = 1;

		/* The matrix is orthonormal */
		Matrix4x4_FLOAT transp;
		result.transpose(transp);
		return Transform_FLOAT(result, transp);
	}

	static Transform_FLOAT scale(const VECTOR &v){
		Matrix4x4_FLOAT trafo(
			v.x, 0,   0,   0,
			0,   v.y, 0,   0,
			0,   0,   v.z, 0,
			0,   0,   0,   1
		);
		Matrix4x4_FLOAT invTrafo(
			1.0f/v.x, 0,        0,        0,
			0,        1.0f/v.y, 0,        0,
			0,        0,        1.0f/v.z, 0,
			0,        0,        0,        1
		);
		return Transform_FLOAT(trafo, invTrafo);
	}

	/// Create a rotation transformation that rotates the vector start to final. The rotation matrix is not unique
	static Transform_FLOAT rotateVector2Vector(const VECTOR &start, const VECTOR &final){
		Vector3d tempStart(start[0], start[1], start[2]);
		Vector3d tempFinal(final[0], final[1], final[2]) ;

		Transform_FLOAT R;
		FLOAT startLength = tempStart.length();
		FLOAT finalLength = tempFinal.length();

		if(startLength == 0 || finalLength == 0)
			return R;
		tempStart/= startLength;
		tempFinal/= finalLength;

		Vector3d v = cross(tempStart,tempFinal);
		FLOAT s = v.length();
		FLOAT c = dot(tempStart,tempFinal);
		if(s == 0)
			return R;

		Matrix4x4_FLOAT vX(0, 	-v.z, 	v.y, 	0,
					 v.z, 	   0,  -v.x,	0,
					-v.y, 	 v.x, 	  0, 	0,
					   0, 	   0, 	  0, 	1.0);

		R.m_transform = R.m_transform + vX + ((1-c)/(s*s)) * vX * vX;
		R.m_transform.m[3][3] = 1;

		Transform_FLOAT T(R.m_transform);

		return T;
	}

	/// Serialize a transformation to a stream
	inline void serialize(Stream *stream) const {
		m_transform.serialize(stream);
		m_invTransform.serialize(stream);
	}

	/// Equality comparison operator
	inline bool operator==(const Transform_FLOAT &trafo) const {
		return m_transform == trafo.m_transform;
	}

	/// Inequality comparison operator
	inline bool operator!=(const Transform_FLOAT &trafo) const {
		return m_transform != trafo.m_transform;
	}

	/// Return a string representation
	std::string toString() const;
private:
	Matrix4x4_FLOAT m_transform;
	Matrix4x4_FLOAT m_invTransform;
};



/* For ellipsoid intersections */
struct Cache{
	enum STATE : char{
		// To be determined if the ellipsoid intersects (either Bounding box or triangle)
		ETBD = 0x00,

		// Intersects (either Bounding box or triangle)
		EIntersects = 0x01,

		// Does not intersect (either Bounding box or triangle)
		EFails = 0x02,
	};

private:
	std::vector<bool> m_isTriangleStateValid;
	std::vector<bool> m_TriangleState;

	std::vector<bool> m_isNodeStateValid;
	std::vector<bool> m_NodeState;

	// FIXME: These data structures are not needed with good coding optimization
	STATE *m_triangleState;
	STATE *m_nodeState;

	size_t m_currentNode;
	size_t m_triangleSize;
	size_t m_nodeSize;

public:

	Cache(size_t maxDepth, size_t primCount){
		m_nodeSize = pow(2, maxDepth)-1;
		m_triangleSize = primCount;
		m_currentNode = 0;
		m_triangleState = new STATE[m_triangleSize];
		m_nodeState = new STATE[m_nodeSize];

		memset(m_triangleState, STATE::ETBD, m_triangleSize);
		memset(m_nodeState, STATE::ETBD, m_nodeSize);

//		m_isTriangleStateValid.reserve(m_triangleSize);
//		m_TriangleState.reserve(m_triangleSize);
//		m_isNodeStateValid.reserve(m_nodeSize);
//		m_NodeState.reserve(m_nodeSize);
		m_isTriangleStateValid.assign(m_triangleSize, false);
		m_TriangleState.assign(m_triangleSize, false);
		m_isNodeStateValid.assign(m_nodeSize, false);
		m_NodeState.assign(m_nodeSize, false);
	}

	STATE getState();

	void setState(const STATE &state);

	STATE getTriState(const size_t &index);

	void setTriState(const size_t &index, const STATE &state);



	inline void goLeft(){
		m_currentNode = 2*m_currentNode+1;
	}

	inline void goRight(){
		m_currentNode = 2*m_currentNode+2;
	}

	inline void reset(){
		m_currentNode = 0;
	}

	~Cache(){
		delete [] m_triangleState;
		delete [] m_nodeState;
	}

};


template <typename _PointType, typename _LengthType> struct TEllipsoid{
	typedef _PointType                  PointType;
	typedef _LengthType                 LengthType;

	TEllipsoid(const Point p1, const Point p2, const Normal p1_normal, const Normal p2_normal, const LengthType tau, const size_t maxDepth, const size_t primCount):
			f1(p1), f2(p2), f1_normal(p1_normal), f2_normal(p2_normal), Tau(tau), ellipsoidCache(maxDepth, primCount){
		// Algorithm to compute the T3D3D

		a = tau/2.0;
		C = (f1 + f2) * 0.5;
		b = (a*a-distanceSquared(C, f1));
		if(b < 1e-3) // Very thin ellipsoid will cause low value paths only with shadow vertex. FIXME: Biases measurements
			degenerateEllipsoid = true;
		else
			degenerateEllipsoid = false;
		b = sqrt(b);
		TVector3<LengthType> D = f2-f1;
		TVector3<LengthType> Scale(1/a, 1/b, 1/b);
		TVector3<LengthType> Rot(1.0, 0.0, 0.0);
		TVector3<LengthType> Cv(-C);

		T3D2Ellipsoid 	 = Transform_FLOAT::rotateVector2Vector(D, Rot)*Transform_FLOAT::translate(Cv);
		invT3D2Ellipsoid = T3D2Ellipsoid.inverse();

		T3D2Sphere 	  = Transform_FLOAT::scale(Scale)*T3D2Ellipsoid;
		invT3D2Sphere = T3D2Sphere.inverse();

		/*compute bounding box for the arbitrary oriented ellipse*/
		/*Algo from http://blog.yiningkarlli.com/2013/02/bounding-boxes-for-ellipsoids.html*/
		Matrix4x4_FLOAT S(TVector4<LengthType>(1.0, 0.0, 0.0, 0.0),
					TVector4<LengthType>(0.0, 1.0, 0.0, 0.0),
					TVector4<LengthType>(0.0, 0.0, 1.0, 0.0),
					TVector4<LengthType>(0.0, 0.0, 0.0,-1.0)
								);
		Matrix4x4_FLOAT MT, M = invT3D2Sphere.getMatrix();
		M.transpose(MT);
		Matrix4x4_FLOAT R = M*S*MT;

		FLOAT temp1, temp2;
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


	inline bool isDegenerate() const { return degenerateEllipsoid; }

	/* Intersect the triangle formed by triA, triB, triC with the current Ellipsoid to create a sample whose barycentric co-ordinates are in u, v.
	 * Value is probability of the sample ( = inverse of the length of the ellipsoid-triangle intersection) */
	bool ellipsoidIntersectTriangle(const Point &triA, const Point &triB, const Point &triC, Float &value, Float &u, Float &v, ref<Sampler> sampler) const;

	/* Transforms a point from 3D space to Ellipsoid space*/
	inline void transformToEllipsoid(const PointType &A, PointType &B) const{
		B = T3D2Ellipsoid(A);
		return;
	}

	/* Transforms a point from Ellipsoid space to 3D space*/
	inline void transformFromEllipsoid(const PointType &A, PointType &B) const{
		B = invT3D2Ellipsoid(A);
		return;
	}

	/* Transforms a point from 3D space to unit sphere space*/
	inline void transformToSphere(const PointType &A, PointType &B) const{
		B = T3D2Sphere(A);
		return;
	}

	/* Transforms a point from unit sphere space to 3D space*/
	inline void transformFromSphere(const PointType &A, PointType &B) const{
		B = invT3D2Sphere(A);
		return;
	}

	/* Compute Weighted Inner Product on the Ellipsoid */
	inline FLOAT weightedIP(TVector3<LengthType> &A, TVector3<LengthType> &B) const{
		return (A[0]*B[0]/(a*a) + A[1]*B[1]/(b*b) + A[2]*B[2]/(b*b));
	}

	/* Compute Weighted Inner Product on the Ellipsoid differential */
	inline FLOAT weightedIPd(TVector3<LengthType> &A, TVector3<LengthType> &B) const{
		return -(A[0]*B[0]/(a*a*a) + A[1]*B[1]*a/(b*b*b*b) + A[2]*B[2]*a/(b*b*b*b));
	}

	/* Early rejection of the triangle if the triangle is not in the positive hyperspace of either of the focal points or if the focal points are not in the positive hyperspace of the triangle*/
	bool earlyTriangleReject(const Point &a, const Point &b, const Point &c) const;

	/*Convert intersections found by ellipsoid intersection algorithm into barycentric co-ordinates for the rest of mitsuba code to work*/
	void Barycentric(const PointType &p, const PointType &a, const PointType &b, const PointType &c, Float &u, Float &v) const;

	bool circlePolygonIntersectionAngles(FLOAT thetaMin[], FLOAT thetaMax[], size_t &indices, const PointType Corners[], const FLOAT &r) const;

	FLOAT circleLineIntersection(const PointType &P1, const PointType &P2, const FLOAT &r) const;

	int specialCircleLineIntersection(const PointType &P1, const PointType &P2, const FLOAT &r, const int &specialCase, FLOAT &angle) const;

	FLOAT ellipticSampleWeight(const FLOAT k, const FLOAT thetaMin[], const FLOAT thetaMax[],const size_t &indices, const FLOAT &m1) const;

	FLOAT ellipticCurveSampling(const FLOAT k, const FLOAT thetaMin[], const FLOAT thetaMax[], const size_t &indices, ref<Sampler> sampler) const;

	FLOAT uniformAngleSampling(const FLOAT thetaMin[], const FLOAT thetaMax[], const size_t &indices, ref<Sampler> sampler, FLOAT &thetaRange) const;

	bool isBoxValid(const AABB& aabb) const;

	bool isBoxInsideEllipsoid(const AABB& aabb) const;

	bool isBoxCuttingEllipsoid(const AABB& aabb) const;

	bool isBoxOnNegativeHalfSpace(const PointType &PT, const Normal &N, const AABB& aabb) const;

	inline Cache::STATE cacheCheck(){
		return ellipsoidCache.getState();
	}

	inline void updateCache(const Cache::STATE &state){
		return ellipsoidCache.setState(state);
	}

	inline void cacheLeft(){
		ellipsoidCache.goLeft();
	}

	inline void cacheRight(){
		ellipsoidCache.goRight();
	}

	inline void cacheReset(){
		ellipsoidCache.reset();
	}

	inline Cache::STATE cacheGetTriState(const size_t &index){
		return ellipsoidCache.getTriState(index);
	}

	inline void cacheSetTriState(const size_t &index, const Cache::STATE &state){
		ellipsoidCache.setTriState(index, state);
	}


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

	/* Focal points */
	PointType f1;
	PointType f2;

	/* Normals of the triangle containing f1 and f2 */
	Normal f1_normal;
	Normal f2_normal;

	/* center of the ellipse */
	PointType  C;

	LengthType Tau;

	/* Major and minor axis lengths */
	LengthType a, b;

	bool degenerateEllipsoid;

	Transform_FLOAT T3D2Ellipsoid;
	Transform_FLOAT invT3D2Ellipsoid;
	Transform_FLOAT T3D2Sphere;
	Transform_FLOAT invT3D2Sphere;

	Cache ellipsoidCache;

	struct BoundingBox{
		PointType min;
		PointType max;
	}m_aabb;

};
struct IntersectionRecord{
	FLOAT theta;
	bool directionOutside;
	IntersectionRecord(FLOAT theta, bool directionOutside){
		this->theta = theta;
		this->directionOutside = directionOutside;
	}
	static bool compare(IntersectionRecord I1, IntersectionRecord I2){
		return (I1.theta < I2.theta);
	}
};

MTS_NAMESPACE_END

#endif
