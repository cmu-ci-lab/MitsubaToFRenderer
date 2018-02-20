#include <mitsuba/render/ellipsoid.h>
#include <mitsuba/core/aabb.h>
//#include <boost/dynamic_bitset.hpp>

using boost::math::policies::policy;
using boost::math::policies::digits10;

typedef policy<digits10<5> > my_pol_5;
typedef policy<digits10<10> > my_pol_10;

#define Eps 1e-7
#define almostEqual(a, b) ( ((a-b) < Eps) && ((b-a) < Eps)) // FIXME: check why abs fuction is misbehaving and refactor
#define epsExclusiveGreater(a, b) (a > (b + Eps))
#define epsExclusiveLesser(a, b) (a < (b - Eps))
#define epsInclusiveGreater(a, b) (a > (b - Eps))
#define epsInclusiveLesser(a, b) (a < (b + Eps))

#define epsExclusiveGreaterF(a, b) (a > (b + Epsilon))
#define epsExclusiveLesserF(a, b) (a < (b - Epsilon))
#define epsInclusiveGreaterF(a, b) (a > (b - Epsilon))
#define epsInclusiveLesserF(a, b) (a < (b + Epsilon))

MTS_NAMESPACE_BEGIN

template <typename PointType, typename LengthType>
bool TEllipsoid<PointType, LengthType>::isBoxValid(const AABB& aabb) const{
	if(!isBoxCuttingEllipsoid(aabb)){
		return false;
	}
	if(isBoxOnNegativeHalfSpace(m_f1, m_f1Normal, aabb) || isBoxOnNegativeHalfSpace(m_f2, m_f2Normal, aabb) ){
		return false;
	}
	if(isBoxInsideEllipsoid(aabb)){
		return false;
	}
	return true;
}

template <typename PointType, typename LengthType>
bool TEllipsoid<PointType, LengthType>::isBoxInsideEllipsoid(const AABB& aabb) const{
	for(size_t i = 0; i < 8; i++){
		const Point& temp = aabb.getCorner(i);
		PointType Pt(temp[0], temp[1], temp[2]);
		PointType spherePt;
		transformToSphere(Pt, spherePt);
		if( epsInclusiveGreater(lengthSquared(spherePt), 1) ){
			return false;
		}
	}
	return true;
}

template <typename PointType, typename LengthType>
bool TEllipsoid<PointType, LengthType>::isBoxOnNegativeHalfSpace(const PointType &PT, const Normal &N, const AABB& aabb) const{
	for(size_t i = 0; i < 8; i++){
		const Point& temp = aabb.getCorner(i);
		Normal N1(temp.x-PT.x, temp.y-PT.y, temp.z-PT.z);
		if(epsInclusiveGreaterF(dot(N1, N), 0)){
			return false;
		}
	}
	return true;
}

template <typename PointType, typename LengthType>
bool TEllipsoid<PointType, LengthType>::isBoxCuttingEllipsoid(const AABB& aabb) const {
// Check if the bounding boxes of the ellipsoid intersects with the bounding box of the triangles
// Bounding box intersection algorithm: http://gamemath.com/2011/09/detecting-whether-two-boxes-overlap/

    if (epsExclusiveLesserF(m_aabb.max.x, aabb.min.x)) return false;
    if (epsExclusiveGreaterF(m_aabb.min.x, aabb.max.x)) return false;

    if (epsExclusiveLesserF(m_aabb.max.y, aabb.min.y)) return false;
    if (epsExclusiveGreaterF(m_aabb.min.y, aabb.max.y)) return false;

    if (epsExclusiveLesserF(m_aabb.max.z, aabb.min.z)) return false;
    if (epsExclusiveGreaterF(m_aabb.min.z, aabb.max.z)) return false;

    return true;
}

template <typename PointType, typename LengthType>
void TEllipsoid<PointType, LengthType>::Barycentric(const PointType &p, const PointType &a, const PointType &b, const PointType &c, Float &u, Float &v) const
{
    TVector3<LengthType> v0 = b - a, v1 = c - a, v2 = p - a;
    FLOAT d00 = dot(v0, v0);
    FLOAT d01 = dot(v0, v1);
    FLOAT d11 = dot(v1, v1);
    FLOAT d20 = dot(v2, v0);
    FLOAT d21 = dot(v2, v1);
    FLOAT denom = d00 * d11 - d01 * d01;
    u = (d11 * d20 - d01 * d21) / denom;
    v = (d00 * d21 - d01 * d20) / denom;
}

template <typename PointType, typename LengthType>
bool TEllipsoid<PointType, LengthType>::circlePolygonIntersectionAngles(FLOAT thetaMin[], FLOAT thetaMax[], size_t &indices, const PointType Corners[], const FLOAT &r) const{
	int noOfCorners = 3; // This code can be extended trivially to polygons of arbitrary size other than 3
	FLOAT norm_p[noOfCorners];
	int locations_p[noOfCorners];

	indices = 0;

	FLOAT temp;
	std::vector<IntersectionRecord> intersectionRecord;

	for(int i = 0; i < noOfCorners; i++){
		norm_p[i] = sqrt( Corners[i].x*Corners[i].x + Corners[i].y*Corners[i].y );
	}

	for(int i = 0; i < noOfCorners; i++){
	    int checks = 0;
	    if(almostEqual(norm_p[i], r)){
	    	locations_p[i] =  0;
			checks = checks + 1;
	    }

	    if(epsExclusiveLesser(norm_p[i], r)){
	    	locations_p[i] = -1;
			checks = checks + 1;
	    }

	    if(epsExclusiveGreater(norm_p[i], r)){
	    	locations_p[i] =  1;
			checks = checks + 1;
	    }

	    if(std::isnan(locations_p[i]))
	        SLog(EError, "Point location in circle-poly intersection is not determined");

	    if(checks != 1)
	        SLog(EError, "Point location in circle-poly intersection is not clear");
	}

	for(int i = 0; i < noOfCorners; i++){
		int j = i + 1;
		if(j == noOfCorners){
			j=0;
		}

		// both points are inside, do nothing
		if(locations_p[i] == -1 && locations_p[j] == -1)
			continue;

		// Staring inside, ending outside
		if(locations_p[i] == -1 && locations_p[j] ==  1){
			temp = circleLineIntersection(Corners[i], Corners[j], r);
			intersectionRecord.push_back(IntersectionRecord(temp,true));
			continue;
		}

		// Staring inside, ending on the circle
		if(locations_p[i] == -1 && locations_p[j] ==  0){
			temp = atan2(Corners[j].y, Corners[j].x);
			intersectionRecord.push_back(IntersectionRecord(temp,true));
			continue;
		}

		// Staring outside, ending inside
		if(locations_p[i] ==  1 && locations_p[j] == -1){
			temp = circleLineIntersection(Corners[i], Corners[j], r);
			intersectionRecord.push_back(IntersectionRecord(temp,false));
			continue;
		}

		// Staring on the circle, ending inside
		if(locations_p[i] ==  0 && locations_p[j] == -1){
			temp = atan2(Corners[i].y, Corners[i].x);
			intersectionRecord.push_back(IntersectionRecord(temp,false));
			continue;
		}

		// Staring and ending on the circle
		if(locations_p[i] ==  0 && locations_p[j] == 0){
			temp = atan2(Corners[i].y, Corners[i].x);
			intersectionRecord.push_back(IntersectionRecord(temp,false));

			temp = atan2(Corners[j].y, Corners[j].x);
			intersectionRecord.push_back(IntersectionRecord(temp,true));
			continue;
		}

		// Complex case - 1: Starting on the circle and ending outside
		if(locations_p[i] ==  0 && locations_p[j] ==  1){
			FLOAT temp2;
			if(specialCircleLineIntersection(Corners[i], Corners[j], r, 2, temp2) == 2){
				temp = atan2(Corners[i].y, Corners[i].x);
				intersectionRecord.push_back(IntersectionRecord(temp,false));

				intersectionRecord.push_back(IntersectionRecord(temp2,true));
			}else{
				temp = atan2(Corners[i].y, Corners[i].x);
				intersectionRecord.push_back(IntersectionRecord(temp,true));
			}
			continue;
		}

		// Complex case - 2: Starting outside and ending on the circle
		if(locations_p[i] ==  1 && locations_p[j] ==  0){
			FLOAT temp2;
			if(specialCircleLineIntersection(Corners[i], Corners[j], r, 1, temp2) == 2){
				intersectionRecord.push_back(IntersectionRecord(temp2,false));

				temp = atan2(Corners[j].y, Corners[j].x);
				intersectionRecord.push_back(IntersectionRecord(temp,true));
			}else{
				temp = atan2(Corners[j].y, Corners[j].x);
				intersectionRecord.push_back(IntersectionRecord(temp,false));
			}
			continue;
		}

		// Complex case - 3: both points are outside, can intersect in zero points or 2 points
		if(locations_p[i] ==  1 && locations_p[j] ==  1){
			VECTOR2 n(Corners[j].y-Corners[i].y, Corners[i].x-Corners[j].x);
			n = normalize(n);
			FLOAT dotP = (n.x*Corners[i].x + n.y*Corners[i].y);
			PointType P_O(dotP*n.x, dotP*n.y, 0.0);

			// If projection is not in the circle, the line-segment is completely outside circle
			if(epsInclusiveGreater(P_O.x*P_O.x + P_O.y*P_O.y, r*r)){
				continue;
			}


			FLOAT alpha = -1.0;
	        // Compute where the projection is, with respect to the line joining p[i] and p[j]
			if (!almostEqual(Corners[i].x, Corners[j].x))
				alpha = (P_O.x-Corners[j].x)/(Corners[i].x-Corners[j].x);
			else if(!almostEqual(Corners[i].y, Corners[j].y))
				alpha = (P_O.y-Corners[j].y)/(Corners[i].y-Corners[j].y);
			else{
				SLog(EWarn, "Two corners of a triangle inputted to circle-poly intersection code are almost same \n");
				continue;
			}

			if(alpha <= 0 || alpha >= 1)
				continue;

			temp = circleLineIntersection(Corners[i], P_O, r);
			intersectionRecord.push_back(IntersectionRecord(temp,false));

			temp = circleLineIntersection(P_O, Corners[j], r);
			intersectionRecord.push_back(IntersectionRecord(temp,true));
			continue;
		}
		SLog(EError,"Circle-triangle intersection reached an impossible location. Cannot execute");
	}

	// wrap angles to [0,2*pi]
	std::for_each(intersectionRecord.begin(), intersectionRecord.end(), [](IntersectionRecord &x){ x.theta = fmod(x.theta, 2*PI);
																	if (x.theta < 0)
																		x.theta += 2*PI;
																	return x; });

	//sort the thetas
	std::sort(intersectionRecord.begin(), intersectionRecord.end(), IntersectionRecord::compare);



	// Consolidate non-unique thetas by merging the repeats (for points lying on the circle)
	std::vector<int> deleteIndices;
	size_t size = intersectionRecord.size();
	if(size == 2)
		size = 1; // For size=2, checking the last value of theta is redundancy

	for(size_t i = 0; i < size; i++){
		size_t j = (i+1)%intersectionRecord.size();
		//FIXME: This almost equal can result in errors. Need to find a way to avoid it
		if(almostEqual(intersectionRecord[i].theta, intersectionRecord[j].theta)){
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

	// Remaining intersections must be even
	if(intersectionRecord.size()%2 == 1){
		SLog(EError,"Odd number of intersection in ellipse triangle intersection: radius: %f, corner-1 (%f, %f, %f), corner-2 (%f, %f, %f), corner-3 (%f, %f, %f)", r, Corners[0].x, Corners[0].y, Corners[0].z, Corners[1].x, Corners[1].y, Corners[1].z, Corners[2].x, Corners[2].y, Corners[2].z);
	}

	/* special case when the circle is completely inside the triangle*/
	if(intersectionRecord.size() == 0){
		/* check that all the points are outside the circle */
		if(norm_p[0] < r || norm_p[1] < r || norm_p[2] < r)
				return false;

		/* check that a center of circle is inside the triangle */
		VECTOR2 v0(Corners[0].x, Corners[0].y);
		VECTOR2 v1(Corners[1].x-Corners[0].x, Corners[1].y-Corners[0].y);
		VECTOR2 v2(Corners[2].x-Corners[0].x, Corners[2].y-Corners[0].y);

	    FLOAT a = -det(v0, v2)/det(v1, v2);
	    FLOAT b =  det(v0, v1)/det(v1, v2);
		if(a>0 && b>0 && (a+b)<1){
			thetaMin[0]=0;
			thetaMax[0]=2*PI;
			indices = 1;
			//Sanitycheck --> All the triangle vertices must be outside the circle
			if(!(norm_p[0] > r && norm_p[1] > r && norm_p[2] > r)){
				for(size_t j = 0; j < 3; j++)
					std::cout << "Corners[" << j << "];" << Corners[j].x << "," << Corners[j].y << "," << Corners[j].z << std::endl;
				std::cout << "radius:" << r << std::endl;
				SLog(EError, "Circle-Triangle intersection is returning illegally that the circle is completely inside triangle. intersectionRecord size: %d; deleteIndices size: %d\n", intersectionRecord.size(), deleteIndices.size());
			}
			return true;
		}
		return false;
	}

	TVector3<LengthType> N  = cross(Corners[1]-Corners[0], Corners[2]-Corners[0]);

	size_t start = 0;
	if( intersectionRecord[0].directionOutside ^ (N.z > 0) )
		start = 1;

	size_t j;
	for(size_t i = start; i < intersectionRecord.size(); i+=2){
		thetaMin[indices] = intersectionRecord[i].theta;
		j = i + 1;
		if(j == intersectionRecord.size())
			j = 0;
		thetaMax[indices] = intersectionRecord[j].theta;
		if(thetaMax[indices] < thetaMin[indices]){
			thetaMax[indices + 1] = thetaMax[indices];
			thetaMax[indices] = 2*PI;
			thetaMin[indices + 1] = 0;
			indices++;
		}
		indices++;
	}
	if(indices == 0)
		SLog(EError, "Circle triangle intersection returning true with out any intersections \n");
	if(indices > 4)
		SLog(EError, "Circle triangle intersection has more sets of angles (%d) than max permissive 4\n",indices);
	for(size_t i= 0;i<indices;i++){
		if(thetaMin[i] == thetaMax[i] && thetaMax[i]!=2*PI && thetaMax[i]!=0){
			SLog(EError, "Circle triangle intersection has duplicates that are not eliminated properly \n");
		}
	}
	return true;
}

template <typename PointType, typename LengthType>
FLOAT TEllipsoid<PointType, LengthType>::circleLineIntersection(const PointType &P1, const PointType &P2, const FLOAT &r) const{
	FLOAT x1 = P1.x;
	FLOAT y1 = P1.y;
	FLOAT x2 = P2.x;
	FLOAT y2 = P2.y;

	FLOAT dx = x1 - x2;
	FLOAT dy = y1 - y2;
	FLOAT a  = dx*dx + dy*dy;
	FLOAT b  = 2*(dx*x2 + dy*y2);
	FLOAT c  = x2*x2 + y2*y2 - r*r;

	FLOAT det = (b*b-4*a*c);
	if(det < 0){ // To compensate for Float precision errors
		SLog(EWarn,"Circle-Line intersection resulted in a possible float precision error or called without an intersection -- Debug values: P1(%lf, %lf, %lf), P2(%lf, %lf, %lf), r(%lf),"
				"																					  x1(%lf), y1(%lf), x2(%lf), y2(%lf),"
				"																					  dx(%lf), dy(%lf), a(%lf), b(%lf), c(%lf), det(%lf);",
																										P1.x, P1.y, P1.z, P2.x, P2.y, P2.z, r,
																										x1, y1, x2, y2,
																										dx, dy, a, b, c, det);
		det = 0;
	}
	det = sqrt(det);
	FLOAT alpha;

	FLOAT x = 0;
	FLOAT y = std::numeric_limits<FLOAT>::min();

	if(b >= 0){
		alpha = (-b-det)/(2*a);
		if(alpha >=0 && alpha <= 1){
			x = alpha * x1 + (1-alpha) * x2;
			y = alpha * y1 + (1-alpha) * y2;
		}else{
			alpha = (2*c)/(-b-det);
			if(alpha >= 0 && alpha <= 1){
				x = alpha * x1 + (1-alpha) * x2;
				y = alpha * y1 + (1-alpha) * y2;
			}else{
				SLog(EWarn,"Circle-Line intersection called with out an actual intersection -- Debug values: P1(%lf, %lf, %lf), P2(%lf, %lf, %lf), r(%lf),"
						"																					  x1(%lf), y1(%lf), x2(%lf), y2(%lf),"
						"																					  dx(%lf), dy(%lf), a(%lf), b(%lf), c(%lf), det(%lf);",
																												P1.x, P1.y, P1.z, P2.x, P2.y, P2.z, r,
																												x1, y1, x2, y2,
																												dx, dy, a, b, c, det);
			}
		}
	}else{
		alpha = (2*c)/(-b+det);
		if(alpha >=0 && alpha <= 1){
			x = alpha * x1 + (1-alpha) * x2;
			y = alpha * y1 + (1-alpha) * y2;
		}else{
			alpha = (-b+det)/(2*a);
			if(alpha >= 0 && alpha <= 1){
				x = alpha * x1 + (1-alpha) * x2;
				y = alpha * y1 + (1-alpha) * y2;
			}else{
				SLog(EWarn,"Circle-Line intersection called with out an actual intersection -- Debug values: P1(%lf, %lf, %lf), P2(%lf, %lf, %lf), r(%lf),"
						"																					  x1(%lf), y1(%lf), x2(%lf), y2(%lf),"
						"																					  dx(%lf), dy(%lf), a(%lf), b(%lf), c(%lf), det(%lf);",
																												P1.x, P1.y, P1.z, P2.x, P2.y, P2.z, r,
																												x1, y1, x2, y2,
																												dx, dy, a, b, c, det);
			}
		}
	}

	return (FLOAT)atan2(y, x);
}

template <typename PointType, typename LengthType>
int TEllipsoid<PointType, LengthType>::specialCircleLineIntersection(const PointType &P1, const PointType &P2, const FLOAT &r, const int &specialCase, FLOAT &angle) const{
	FLOAT x1 = (FLOAT)P1.x;
	FLOAT y1 = (FLOAT)P1.y;
	FLOAT x2 = (FLOAT)P2.x;
	FLOAT y2 = (FLOAT)P2.y;

	FLOAT dx;
	FLOAT dy;
	FLOAT a;
	FLOAT b;
	FLOAT alpha;

	int noOfSols = 1;
	if(specialCase == 1){
	    dx = x1 - x2;
	    dy = y1 - y2;
	    a = dx*dx + dy*dy;
	    b = 2*(dx*x2+dy*y2);

	    alpha = -b/a;
	    if(alpha >= 0 && alpha <= 1)
	        noOfSols++;
	}else{
	    dx = x2 - x1;
	    dy = y2 - y1;
	    a = dx*dx + dy*dy;
	    b = 2*(dx*x1+dy*y1);

	    alpha = 1+b/a; //(1-alpha)
	    if(alpha >= 0 && alpha <= 1)
	        noOfSols = noOfSols + 1;
	}

	angle = atan2(alpha*y1+(1-alpha)*y2, alpha*x1+(1-alpha)*x2);
	return noOfSols;
}



template <typename PointType, typename LengthType>
FLOAT TEllipsoid<PointType, LengthType>::ellipticSampleWeight(const FLOAT k, const FLOAT thetaMin[], const FLOAT thetaMax[],const size_t &indices, const FLOAT &m1) const{
	FLOAT arcLength = 0;
	for(size_t i = 0; i < indices; i++){
		FLOAT max = boost::math::ellint_2(k, thetaMax[i], my_pol_10());
		FLOAT min = boost::math::ellint_2(k, thetaMin[i], my_pol_10());
		if(max < min)
			SLog(EError, "Elliptic integrals are not computed accurately");
		arcLength += max - min;
	}
	arcLength = arcLength * m1;
	if(arcLength == 0)
		SLog(EError, "Arc length of the ellipse is zero; Total indices: %d, k: %f, first angle range: (%f, %f)", indices, k, thetaMin[0], thetaMax[0]);
	return (1/arcLength);
}

template <typename PointType, typename LengthType>
FLOAT TEllipsoid<PointType, LengthType>::uniformAngleSampling(const FLOAT thetaMin[], const FLOAT thetaMax[], const size_t &indices, ref<Sampler> sampler, FLOAT &thetaRange) const{
	FLOAT cumsum[4];
	cumsum[0] = thetaMax[0] - thetaMin[0];

	for(size_t i=1; i < indices; i++){
		cumsum[i] = cumsum[i - 1] + (thetaMax[i] - thetaMin[i]);
	}

	thetaRange = cumsum[indices-1];
	FLOAT theta_s = thetaRange * sampler->nextFloat(), theta;

	if(theta_s < cumsum[0])
		theta = theta_s + thetaMin[0];
	else
		for(size_t i = 1;i < indices; i++){
			if(theta_s < cumsum[i]){
				theta = theta_s -cumsum[i-1] + thetaMin[i];
				break;
			}
		}
	if(std::isnan(theta) || theta < 0 || theta > 2*PI)
		SLog(EError,"theta not calculated in uniformAngleSampling");
	return theta;
}

template <typename PointType, typename LengthType>
FLOAT TEllipsoid<PointType, LengthType>::ellipticCurveSampling(const FLOAT k, const FLOAT thetaMin[], const FLOAT thetaMax[], const size_t &indices, ref<Sampler> sampler) const{
	FLOAT cumsum[4];
	cumsum[0] = thetaMax[0] - thetaMin[0];

	for(size_t i=1; i < indices; i++){
		cumsum[i] = cumsum[i - 1] + (thetaMax[i] - thetaMin[i]);
	}

	while(1){
		FLOAT rand = sampler->nextFloat();
		if(rand < 0 || rand > 1)
			SLog(EError, "Sampler is generating wrong rand");
		FLOAT theta_s = cumsum[indices-1] * rand, theta = NAN;
		if(theta_s < cumsum[0])
			theta = theta_s + thetaMin[0];
		else
			for(size_t i = 1;i < indices; i++){
				if(theta_s < cumsum[i]){
					theta = theta_s -cumsum[i-1] + thetaMin[i];
					break;
				}
			}
		if(std::isnan(theta))
			SLog(EError,"theta not calculated in elliptic sampling");
		FLOAT r = sampler->nextFloat();
		if(r < sqrt(1 - pow(k*cos(theta),2))){
			return theta;
		}
	}
}

template <typename PointType, typename LengthType>
bool TEllipsoid<PointType, LengthType>::earlyTriangleReject(const Point &a, const Point &b, const Point &c, const Normal &N) const{

	Point f1_Float(m_f1.x, m_f1.y, m_f1.z);
	Point f2_Float(m_f2.x, m_f2.y, m_f2.z);
	if(epsExclusiveLesserF(dot(N, f1_Float - a), 0) || epsExclusiveLesserF(dot(N, f2_Float - a), 0) )
		return true;
	if(epsExclusiveLesserF(dot(m_f1Normal, a - f1_Float), 0) && epsExclusiveLesserF(dot(m_f1Normal, b - f1_Float), 0) && epsExclusiveLesserF(dot(m_f1Normal, c - f1_Float), 0))
		return true;
	if(epsExclusiveLesserF(dot(m_f2Normal, a - f2_Float), 0) && epsExclusiveLesserF(dot(m_f2Normal, b - f2_Float), 0) && epsExclusiveLesserF(dot(m_f2Normal, c - f2_Float), 0))
		return true;


	PointType triA(a.x, a.y, a.z);
	PointType triB(b.x, b.y, b.z);
	PointType triC(c.x, c.y, c.z);

	PointType spherePtA, spherePtB, spherePtC;
	transformToSphere(triA, spherePtA);
	transformToSphere(triB, spherePtB);
	transformToSphere(triC, spherePtC);
	if( epsExclusiveLesser(lengthSquared(spherePtA), 1) && epsExclusiveLesser(lengthSquared(spherePtB), 1) && epsExclusiveLesser(lengthSquared(spherePtC), 1)){
		return true;
	}

	PointType Origin(0.0, 0.0, 0.0);

	TVector3<LengthType> Nd = cross(spherePtB - spherePtA, spherePtC - spherePtA);
	Nd = normalize(Nd);

	TVector3<LengthType> Center = dot(Nd,spherePtA-Origin)*Nd;

	FLOAT d = Center.lengthSquared();

	if(d > 1){ // ellipsoid does not intersect the plane
		return true;
	}

	return false;
}


template <typename PointType, typename LengthType>
bool TEllipsoid<PointType, LengthType>::ellipsoidIntersectTriangle(const Point &temp_triA, const Point &temp_triB, const Point &temp_triC, Float &value, Float &u, Float &v, ref<Sampler> sampler) const {

	PointType triA(temp_triA.x, temp_triA.y, temp_triA.z);
	PointType triB(temp_triB.x, temp_triB.y, temp_triB.z);
	PointType triC(temp_triC.x, temp_triC.y, temp_triC.z);

	//Compute the center of the ellipse (resulting from ellipsoid-plane intersection)
	PointType SphereA;transformToSphere(triA, SphereA);
	PointType SphereB;transformToSphere(triB, SphereB);
	PointType SphereC;transformToSphere(triC, SphereC);
	PointType Origin(0.0, 0.0, 0.0);

	if(epsExclusiveLesser(lengthSquared(SphereA), 1) &&
	   epsExclusiveLesser(lengthSquared(SphereB), 1) &&
	   epsExclusiveLesser(lengthSquared(SphereC), 1)){
		return false;
	}

	TVector3<LengthType> b = SphereC-SphereA, c = SphereB-SphereA, N = cross(c, b);
	N = normalize(N);

	TVector3<LengthType> Center = dot(N,SphereA-Origin)*N;

	TVector3<LengthType> O(Center[0]*this->m_majorAxis, Center[1]*this->m_minorAxis, Center[2]*this->m_minorAxis); // Note that O is position vector of the center of the ellipse

	FLOAT d = Center.lengthSquared();

	if(d > 1){ // ellipsoid does not intersect the plane
		return false;
	}

	//Compute the angle of the ellipse with T
	PointType EllipsoidA;transformToEllipsoid(triA, EllipsoidA);
	PointType EllipsoidB;transformToEllipsoid(triB, EllipsoidB);
	PointType EllipsoidC;transformToEllipsoid(triC, EllipsoidC);

	TVector3<LengthType> T = EllipsoidB - EllipsoidA;T = normalize(T);
	N = cross(T, EllipsoidC-EllipsoidA); N = normalize(N);
	TVector3<LengthType> U = cross(N, T);

	FLOAT TTD = weightedIP(T, T);
	FLOAT TUD = weightedIP(T, U);
	FLOAT UUD = weightedIP(U, U);
	FLOAT OOD = weightedIP(O, O);

	FLOAT theta = 0.5 * atan2(2*TUD, UUD-TTD);
	if(std::isnan(theta) || std::isinf(theta)){
		SLog(EError,"Theta is not valid; TUD: %f, UUD: %f, TTD: %f; T: (%f, %f, %f); a:%f, b:%f\n", TUD, UUD, TTD, T[0], T[1], T[2], m_majorAxis, b);
	}

	TVector3<LengthType> NewX = T*cos(theta) - U*sin(theta);
	TVector3<LengthType> NewY = T*sin(theta) + U*cos(theta);

	if(std::isnan(NewX[0]) || std::isnan(NewX[1]) || std::isnan(NewX[2]) || std::isinf(NewX[0]) || std::isinf(NewX[1]) || std::isinf(NewX[2]) ||
		std::isnan(NewY[0]) || std::isnan(NewY[1]) || std::isnan(NewY[2]) || std::isinf(NewY[0]) || std::isinf(NewY[1]) || std::isinf(NewY[2]))
		SLog(EError,"One of the Ellipse axis is not valid:");

	// Compute the transform to shift axis of ellipsoid to ellipse
	Transform_FLOAT invEllipsoid2Ellipse(Matrix4x4_FLOAT({NewX[0], NewY[0], N[0], 0.0,
												 NewX[1], NewY[1], N[1], 0.0,
												 NewX[2], NewY[2], N[2], 0.0,
												 0.0, 0.0, 0.0, 1.0
												}));
	Transform_FLOAT Ellipsoid2Ellipse = invEllipsoid2Ellipse.inverse()*Transform_FLOAT::translate(-O);
	invEllipsoid2Ellipse = Ellipsoid2Ellipse.inverse();

	// Compute major and minor axis
	FLOAT Delta 	= sqrt(4*TUD*TUD+(TTD-UUD)*(TTD-UUD));
	FLOAT DR1  = TTD + UUD - Delta;
	FLOAT DR2  = TTD + UUD + Delta;
	FLOAT m1 	= sqrt(2 * (1-OOD)/DR1); // Major axis
	FLOAT m2 	= sqrt(2 * (1-OOD)/DR2); // Minor axis
//	FLOAT k  	= sqrt(1-m2*m2/(m1*m1)); // eccentricity

	// Compute transform to scale ellipse to circle

	Transform_FLOAT Ellipse2Circle = Transform_FLOAT::scale(TVector3<LengthType>(1, m1/m2, 1));
	Transform_FLOAT Ellipsoid2Circle = Ellipse2Circle*Ellipsoid2Ellipse;

	PointType Corners[3];
	Corners[0] = Ellipsoid2Circle(EllipsoidA);
	Corners[1] = Ellipsoid2Circle(EllipsoidB);
	Corners[2] = Ellipsoid2Circle(EllipsoidC);

	// Circle Triangle Intersection to get all the angles
	FLOAT thetaMin[4];
	FLOAT thetaMax[4];
	size_t indices;
	value = 0;

	if(circlePolygonIntersectionAngles(thetaMin, thetaMax, indices, Corners, m1)){
		// Sample an angle
		if(indices == 0)
			SLog(EError, "Circle polygon intersection returned true without any intersection");
		FLOAT thetaRange;
		FLOAT angle = uniformAngleSampling(thetaMin, thetaMax, indices, sampler, thetaRange);
		PointType Projection(m1*cos(angle), m2*sin(angle), 0.0), Original;
		Projection = invEllipsoid2Ellipse(Projection);
		transformFromEllipsoid(Projection, Original);

		FLOAT TTE = weightedIPd(T, T);
		FLOAT TUE = weightedIPd(T, U);
		FLOAT UUE = weightedIPd(U, U);
		FLOAT OOE = weightedIPd(O, O);
		FLOAT dDelta = (1/Delta)*(4*TUD*TUE + (TTE-UUE)*(TTD-UUD));
		FLOAT dDR1, dDR2;
		if(Delta < Eps){
			dDR1 = TTE + UUE;
			dDR2 = dDR1;
		}else{
			FLOAT temp1 = TTE + UUE;
			dDR1 = temp1 - dDelta;
			dDR2 = temp1 + dDelta;
		}

		Transform_FLOAT dOM(Matrix4x4_FLOAT({T.x/(m_majorAxis*m_majorAxis), T.y/(m_minorAxis*m_minorAxis), T.z/(m_minorAxis*m_minorAxis), 0.0,
											 U.x/(m_majorAxis*m_majorAxis), U.y/(m_minorAxis*m_minorAxis), U.z/(m_minorAxis*m_minorAxis), 0.0,
											 N.x, 							N.y, 						   N.z, 						  0.0,
											 0.0, 							0.0, 						   0.0,							  1.0
													}));

		TVector3<LengthType> dOV(T.x*O.x/pow(m_majorAxis,4) + T.y*O.y/pow(m_minorAxis,4) + T.z*O.z/pow(m_minorAxis,4), U.x*O.x/pow(m_majorAxis,4) + U.y*O.y/pow(m_minorAxis,4) + U.z*O.z/pow(m_minorAxis,4),  0.0);
		dOV *= m_tau/2;
		TVector3<LengthType> dO = dOM.inverse()(dOV);
		FLOAT OdOD= weightedIP(O, dO);

		FLOAT NR  = (1-OOD);
		FLOAT dNR = - OOE - OdOD;
		FLOAT cn  = cos(angle), sn = sin(angle);
		FLOAT cn2 = cn*cn;
		FLOAT sn2 = sn*sn;

		FLOAT Delta2 = Delta*Delta;

		FLOAT msnthetadtheta 	  = (Delta*(UUE-TTE)-dDelta*(UUD-TTD))/Delta2;
		FLOAT cnthetadtheta  	  = 2 * (Delta*TUE-dDelta*TUD)/Delta2;
		TVector3<LengthType> dTN  = T*msnthetadtheta - U*cnthetadtheta;
		TVector3<LengthType> dUN  = T*cnthetadtheta  + U*msnthetadtheta ;
		FLOAT dAxis 		 	  = dot(O, dTN*m2*cn + dUN*m1*sn);

		value = ( (DR1*dNR-NR*dDR1)/(DR1*DR1)*m2/m1*cn2 + (DR2*dNR-NR*dDR2)/(DR2*DR2)*m1/m2*sn2 + dot(m2*cn*NewX + m1*sn*NewY, dO) + dAxis)*thetaRange;

		//Compute the Barycentric co-ordinates. Return that and save it in the cache to be compatible with mitsuba
		Barycentric(Original, triA, triB, triC, u, v);
		// Adjust corner misses. Note that this biases the measurements slightly.
		if(u < 0 && u > -Eps){u = 0;}
		if(v < 0 && v > -Eps){v = 0;}
		if(u > 1 && u < (1+Eps)){u = 1;}
		if(v < 0 && v > (1+Eps)){v = 1;}

		if(u < 0 || u > 1 || v < 0 || v > 1){

			cout << "angle Found:" << angle << "\n";
			cout << "Indices:" << indices << "\n";
			for(size_t i=0; i < indices ; i++)
				cout  << thetaMin[i] << " - " << thetaMax[i]  <<  "\n";
			cout << "angle Found:" << angle << "\n";
			cout << "Ellipsoid: f1(" << this->m_f1.x << "," << this->m_f1.y << "," << this->m_f1.z << "); f2(" << this->m_f2.x << "," << this->m_f2.y << "," << this->m_f2.z << "); Tau:"  <<  this->m_tau  <<  "\n";
			cout << "triA:(" << triA.x << "," << triA.y << "," << triA.z << ");\n";
			cout << "triB:(" << triB.x << "," << triB.y << "," << triB.z << ");\n";
			cout << "triC:(" << triC.x << "," << triC.y << "," << triC.z << ");\n";
			cout << "Original Point:(" << Original.x << "," << Original.y << "," << Original.z << ");\n";
			cout << "On Origin centered Ellipsoid Point:(" << Projection.x << "," << Projection.y << "," << Projection.z << ");\n";

			SLog(EWarn,"wrong intersection found by elliptic algorithm; Not counting; u:%f, v:%f", u, v);
			return false;
		}
		return true;
	}

	return false;
}

template bool TEllipsoid<Point3d, double>::isBoxValid(const AABB& aabb) const;

template bool TEllipsoid<Point3d, double>::isBoxInsideEllipsoid(const AABB& aabb) const;

template bool TEllipsoid<Point3d, double>::isBoxOnNegativeHalfSpace(const PointType &PT, const Normal &N, const AABB& aabb) const;

template bool TEllipsoid<Point3d, double>::isBoxCuttingEllipsoid(const AABB& aabb) const;

template bool TEllipsoid<Point3d, double>::earlyTriangleReject(const Point &a, const Point &b, const Point &c, const Normal &N) const;

template void TEllipsoid<Point3d, double>::Barycentric(const PointType &p, const PointType &a, const PointType &b, const PointType &c, Float &u, Float &v) const;

template bool TEllipsoid<Point3d, double>::circlePolygonIntersectionAngles(FLOAT thetaMin[], FLOAT thetaMax[], size_t &indices, const PointType Corners[], const FLOAT &r) const;

template FLOAT TEllipsoid<Point3d, double>::circleLineIntersection(const PointType &P1, const PointType &P2, const FLOAT &r) const;

template int TEllipsoid<Point3d, double>::specialCircleLineIntersection(const PointType &P1, const PointType &P2, const FLOAT &r, const int &specialCase, FLOAT &angle) const;

template FLOAT TEllipsoid<Point3d, double>::ellipticSampleWeight(const FLOAT k, const FLOAT thetaMin[], const FLOAT thetaMax[],const size_t &indices, const FLOAT &m1) const;

template FLOAT TEllipsoid<Point3d, double>::ellipticCurveSampling(const FLOAT k, const FLOAT thetaMin[], const FLOAT thetaMax[], const size_t &indices, ref<Sampler> sampler) const;

template bool TEllipsoid<Point3d, double>::ellipsoidIntersectTriangle(const Point &triA, const Point &triB, const Point &triC, Float &value, Float &u, Float &v, ref<Sampler> sampler) const;

template class MTS_EXPORT_RENDER TEllipsoid<Point3d, double>;

MTS_NAMESPACE_END
