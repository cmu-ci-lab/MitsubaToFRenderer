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

#include <mitsuba/render/skdtree.h>
#include <mitsuba/core/statistics.h>
#include <array>

#if defined(MTS_SSE)
#include <mitsuba/core/sse.h>
#include <mitsuba/core/aabb_sse.h>
#include <mitsuba/render/triaccel_sse.h>
#endif

MTS_NAMESPACE_BEGIN

ShapeKDTree::ShapeKDTree() {
#if !defined(MTS_KD_CONSERVE_MEMORY)
	m_triAccel = NULL;
#endif
	m_shapeMap.push_back(0);
}

ShapeKDTree::~ShapeKDTree() {
#if !defined(MTS_KD_CONSERVE_MEMORY)
	if (m_triAccel)
		freeAligned(m_triAccel);
#endif
	for (size_t i=0; i<m_shapes.size(); ++i)
		m_shapes[i]->decRef();
}

static StatsCounter raysTraced("General", "Normal rays traced");
static StatsCounter shadowRaysTraced("General", "Shadow rays traced");

void ShapeKDTree::addShape(const Shape *shape) {
	Assert(!isBuilt());
	if (shape->isCompound())
		Log(EError, "Cannot add compound shapes to a kd-tree - expand them first!");
	if (shape->getClass()->derivesFrom(MTS_CLASS(TriMesh))) {
		// Triangle meshes are expanded into individual primitives,
		// which are visible to the tree construction code. Generic
		// primitives are only handled by their AABBs
		m_shapeMap.push_back((SizeType)
			static_cast<const TriMesh *>(shape)->getTriangleCount());
		m_triangleFlag.push_back(true);
	} else {
		m_shapeMap.push_back(1);
		m_triangleFlag.push_back(false);
	}
	shape->incRef();
	m_shapes.push_back(shape);
}

void ShapeKDTree::build() {
	for (size_t i=1; i<m_shapeMap.size(); ++i)
		m_shapeMap[i] += m_shapeMap[i-1];

	SAHKDTree3D<ShapeKDTree>::buildInternal();

#if !defined(MTS_KD_CONSERVE_MEMORY)
	ref<Timer> timer = new Timer();
	SizeType primCount = getPrimitiveCount();
	Log(EDebug, "Precomputing triangle intersection information (%s)",
			memString(sizeof(TriAccel)*primCount).c_str());
	m_triAccel = static_cast<TriAccel *>(allocAligned(primCount * sizeof(TriAccel)));

	IndexType idx = 0;
	for (IndexType i=0; i<m_shapes.size(); ++i) {
		const Shape *shape = m_shapes[i];
		if (m_triangleFlag[i]) {
			const TriMesh *mesh = static_cast<const TriMesh *>(shape);
			const Triangle *triangles = mesh->getTriangles();
			const Point *positions = mesh->getVertexPositions();
			for (IndexType j=0; j<mesh->getTriangleCount(); ++j) {
				const Triangle &tri = triangles[j];
				const Point &v0 = positions[tri.idx[0]];
				const Point &v1 = positions[tri.idx[1]];
				const Point &v2 = positions[tri.idx[2]];
				m_triAccel[idx].load(v0, v1, v2);
				m_triAccel[idx].shapeIndex = i;
				m_triAccel[idx].primIndex = j;
				++idx;
			}
		} else {
			/* Create a 'fake' triangle, which redirects to a Shape */
			memset(&m_triAccel[idx], 0, sizeof(TriAccel));
			m_triAccel[idx].shapeIndex = i;
			m_triAccel[idx].k = KNoTriangleFlag;
			++idx;
		}
	}
	Log(EDebug, "Finished -- took %i ms.", timer->getMilliseconds());
	Log(m_logLevel, "");
	KDAssert(idx == primCount);
#endif
	ref<Timer> timerBB = new Timer();
	cout << "Constructing a Bounding Box Tree\n";
	size_t maxDepth = getMaxDepth();
	m_BBTree = new BBTree(maxDepth, primCount);
	buildBBTree(m_nodes);
	cout << "Finished -- took " << timerBB->getMilliseconds() << " ms.\n";
//	printBBTree(m_nodes, 0);
}

void ShapeKDTree::buildBBTree(const KDNode* node){
	if(node == NULL){
		return;
	}
	if(node->isLeaf()){
		for (unsigned int entry=node->getPrimStart(),
						last = node->getPrimEnd(); entry != last; entry++) {
			const IndexType primIdx = m_indices[entry];
			const TriAccel &ta = m_triAccel[primIdx];

			const TriMesh *mesh = static_cast<const TriMesh *>(m_shapes[ta.shapeIndex]);
			const Triangle *triangles = mesh->getTriangles();
			const Point *positions = mesh->getVertexPositions();

			const Triangle &tri = triangles[ta.primIndex];
			const Point &A = positions[tri.idx[0]];
			const Point &B = positions[tri.idx[1]];
			const Point &C = positions[tri.idx[2]];

			m_BBTree->expandBy(A);
			m_BBTree->expandBy(B);
			m_BBTree->expandBy(C);
			m_BBTree->m_triangleRepetition[primIdx]++;
		}
	}else{
		m_BBTree->goLeft();
		buildBBTree(node->getLeft());
		m_BBTree->goRightSibling();
		buildBBTree(node->getRight());
		m_BBTree->goParent();
		m_BBTree->expandByChildren();
	}
	return;
}

void ShapeKDTree::printBBTree(const KDNode* node, const size_t& index) const{

	if(node->isLeaf()){
		cout << "\nLeaf Node:\n";
		m_BBTree->print(index);
		for (unsigned int entry=node->getPrimStart(),
				last = node->getPrimEnd(); entry != last; entry++) {
			const IndexType primIdx = m_indices[entry];
			const TriAccel &ta = m_triAccel[primIdx];

			const TriMesh *mesh = static_cast<const TriMesh *>(m_shapes[ta.shapeIndex]);
			const Triangle *triangles = mesh->getTriangles();
			const Point *positions = mesh->getVertexPositions();

			const Triangle &tri = triangles[ta.primIndex];
			const Point &A = positions[tri.idx[0]];
			const Point &B = positions[tri.idx[1]];
			const Point &C = positions[tri.idx[2]];

			cout << "A: (" << A.x << ", " << A.y << ", " << A.z << "); ";
			cout << "B: (" << B.x << ", " << B.y << ", " << B.z << "); ";
			cout << "C: (" << C.x << ", " << C.y << ", " << C.z << ");\n";
		}
		cout << "End Leaf Node; \n";
		return;
	}
	switch(node->getAxis()){
		case 0:
			printBBTree(node->getLeft(), 2*index+1);
			printBBTree(node->getRight(), 2*index+2);
			break;
		case 1:
			printBBTree(node->getLeft(), 2*index+1);
			printBBTree(node->getRight(), 2*index+2);
			break;
		case 2:
			printBBTree(node->getLeft(), 2*index+1);
			printBBTree(node->getRight(), 2*index+2);
			break;
	}
	cout << "\nInner Node: ";
	m_BBTree->print(index);
	cout << "End Inner Node; \n";
}

/* Search the KD tree start from root by randomly choosing one of the child node */
bool ShapeKDTree::ellipsoidIntersect(Ellipsoid* e, Float &value, Ray &ray, Intersection &its, ref<Sampler> sampler) const{
	uint8_t temp[MTS_KD_INTERSECTION_TEMP];

	size_t rootIndex = 0;
	if (ellipsoidParseKDTree(m_nodes, rootIndex, e, value, sampler, temp)) {
		fillEllipticIntersectionRecord<true>(ray, temp, its);
		return true;
	}
	return false;
}

bool ShapeKDTree::ellipsoidParseKDTree(const KDNode* node, size_t& index, Ellipsoid* e, Float &value, ref<Sampler> sampler, void *temp) const{
	IntersectionCache *cache =
			static_cast<IntersectionCache *>(temp);
	int multiplier = 1;

	//Check root node
	Cache::STATE state = e->cacheCheck(index);

	if(state == Cache::STATE::EFails)
			return false;

	if(state == Cache::STATE::ETBD){
		if(!e->isBoxValid(m_BBTree->getAABB(index))){
			e->updateCache(index, Cache::STATE::EFails);
			return false;
		}else{
			e->updateCache(index, Cache::STATE::EIntersects);
		}
	}

	bool leftNodeIntersects  = false;
	bool rightNodeIntersects = false;

	while(!node->isLeaf()){
		// check if left node intersects
		state = e->cacheCheck(2*index + 1);
		if(state == Cache::STATE::EFails){
			leftNodeIntersects = false;
		}else if(state == Cache::STATE::EIntersects){
			leftNodeIntersects = true;
		}else{
			if(!e->isBoxValid(m_BBTree->getAABB(2*index + 1))){
				e->updateCache(index, Cache::STATE::EFails);
				leftNodeIntersects = false;
			}else{
				e->updateCache(index, Cache::STATE::EIntersects);
				leftNodeIntersects = true;
			}
		}

		// check if right node intersects
		state = e->cacheCheck(2*index + 2);
		if(state == Cache::STATE::EFails){
			rightNodeIntersects = false;
		}else if(state == Cache::STATE::EIntersects){
			rightNodeIntersects = true;
		}else{
			if(!e->isBoxValid(m_BBTree->getAABB(2*index + 2))){
				e->updateCache(index, Cache::STATE::EFails);
				rightNodeIntersects = false;
			}else{
				e->updateCache(index, Cache::STATE::EIntersects);
				rightNodeIntersects = true;
			}
		}

		if(leftNodeIntersects){
			if(rightNodeIntersects){
				//both nodes intersect, so sample a node randomly
				if(sampler->nextFloat() < 0.5f){ // go left
					node = node->getLeft();
					index  = 2*index + 1;
				}else{ // go right
					node = node->getRight();
					index  = 2*index + 2;
				}
				multiplier *= 2;
			}else{
				//Only leftNode intersects
				node = node->getLeft();
				index  = 2*index + 1;
			}
		}else if(rightNodeIntersects){
			//Only rightNode intersects
			node = node->getRight();
			index  = 2*index + 2;
		}else{
			// no intersection
			return false;
		}
	}

	// Old algorithm for tree parsing
//	while(!node->isLeaf()){
//		// Check BBox is valid
//		Cache::STATE state = e->cacheCheck(index);
//		if(state == Cache::STATE::EFails)
//				return false;
//
//		if(state == Cache::STATE::ETBD){
//			if(!e->isBoxValid(m_BBTree->getAABB(index))){
//				e->updateCache(index, Cache::STATE::EFails);
//				return false;
//			}else{
//				e->updateCache(index, Cache::STATE::EIntersects);
//			}
//		}
//		//Travel through the tree
//		if(sampler->nextFloat() < 0.5f){ // go left
//			node = node->getLeft();
//			index  = 2*index + 1;
//		}else{ // go right;
//			node = node->getRight();
//			index  = 2*index + 2;
//		}
//		multiplier *= 2;
//		if(node == NULL)
//			SLog(EError,"KD Node cannot become NULL");
//	}
	//leaf code

	int l = (int)(node->getPrimStart());
	int u = (int)(node->getPrimEnd());

	if(!(l < u))
		return false;

	// Find all intersecting triangles
	int intersectingTriangles[u-l];
	int countIntersectingTriangles = 0;
	for(int x = l; x < u; x++){
		const IndexType &primIdx = m_indices[x];
		const Cache::STATE state = e->cacheCheck(index);
		const TriAccel &ta = m_triAccel[primIdx];

		if(state == Cache::EFails || ta.k == KNoTriangleFlag){
			//continue
		}else if(state == Cache::EIntersects){
			intersectingTriangles[countIntersectingTriangles] = x;
			countIntersectingTriangles ++;
		}else{
			//gather the required data structures
			const TriMesh *mesh = static_cast<const TriMesh *>(m_shapes[ta.shapeIndex]);
			const Triangle *triangles = mesh->getTriangles();
			const Point *positions = mesh->getVertexPositions();
			const Normal *normals = mesh->getVertexNormals();

			const Triangle &tri = triangles[ta.primIndex];
			const Point &A = positions[tri.idx[0]];
			const Point &B = positions[tri.idx[1]];
			const Point &C = positions[tri.idx[2]];
			Normal N = cross(B-A, C-A);
			if(normals != NULL){
				if(dot(normals[tri.idx[0]], N) < 0)
					N = -N;
			}
			if(e->earlyTriangleReject(A, B, C, N)){
				e->cacheSetTriState(primIdx,Cache::EFails);
			}else{
				// The statement below in not exactly correct, but then even if we sample this triangle in the future, the full-ellipsoid intersection will change this state to false. Till then, we can sample this triangle and additionally, we don't have to unnecessarily do the early test for this triangle again and again.
				e->cacheSetTriState(primIdx,Cache::EIntersects);
				intersectingTriangles[countIntersectingTriangles] = x;
				countIntersectingTriangles ++;
			}
		}
	}
	if(countIntersectingTriangles == 0)
		return false;

	//sample a triangle from the intersecting triangles
	int x = intersectingTriangles[sampler->nextSize(countIntersectingTriangles)];
	const IndexType &primIdx = m_indices[x];
	const TriAccel &ta = m_triAccel[primIdx];

	const TriMesh *mesh = static_cast<const TriMesh *>(m_shapes[ta.shapeIndex]);
	const Triangle *triangles = mesh->getTriangles();
	const Point *positions = mesh->getVertexPositions();

	const Triangle &tri = triangles[ta.primIndex];
	const Point &A = positions[tri.idx[0]];
	const Point &B = positions[tri.idx[1]];
	const Point &C = positions[tri.idx[2]];

// Old code that sample a triangle randomly
//	int x = l+sampler->nextSize(u-l);
//	const IndexType &primIdx = m_indices[x];
//	const TriAccel &ta = m_triAccel[primIdx];

//	const TriMesh *mesh = static_cast<const TriMesh *>(m_shapes[ta.shapeIndex]);
//	const Triangle *triangles = mesh->getTriangles();
//	const Point *positions = mesh->getVertexPositions();
//	const Normal *normals = mesh->getVertexNormals();

//	const Triangle &tri = triangles[ta.primIndex];
//	const Point &A = positions[tri.idx[0]];
//	const Point &B = positions[tri.idx[1]];
//	const Point &C = positions[tri.idx[2]];
//	if(e->cacheGetTriState(primIdx) == Cache::ETBD){

//		Normal N = cross(B-A, C-A);
//		if(normals != NULL){
//			if(dot(normals[tri.idx[0]], N) < 0)
//				N = -N;
//		}
//		// Do early rejection tests only once
//		if(e->earlyTriangleReject(A, B, C, N)){
//			e->cacheSetTriState(primIdx,Cache::EFails);
//			return false;
//		}
//	}


	Float tempU;
	Float tempV;

	if(e->ellipsoidIntersectTriangle(A, B, C, value, tempU, tempV, sampler)){
		e->cacheSetTriState(primIdx,Cache::EIntersects);
		cache->shapeIndex = ta.shapeIndex;
		cache->primIndex = ta.primIndex;
		cache->u = tempU;
		cache->v = tempV;
		if(m_BBTree->m_triangleRepetition[primIdx] <= 0){
			SLog(EError, "Triangle repetition of %i th triangle is not properly measured", primIdx);
		}
		value = value*(u-l)*multiplier/(double)m_BBTree->m_triangleRepetition[primIdx];
		return true;
	}
	e->cacheSetTriState(primIdx,Cache::EFails);
	return false;
}

bool ShapeKDTree::rayIntersect(const Ray &ray, Intersection &its) const {
	uint8_t temp[MTS_KD_INTERSECTION_TEMP];
	its.t = std::numeric_limits<Float>::infinity();
	Float mint, maxt;

	#if defined(MTS_FP_DEBUG_STRICT)
		Assert(
			std::isfinite(ray.o.x) && std::isfinite(ray.o.y) && std::isfinite(ray.o.z) &&
			std::isfinite(ray.d.x) && std::isfinite(ray.d.y) && std::isfinite(ray.d.z));
	#endif

	++raysTraced;
	if (m_aabb.rayIntersect(ray, mint, maxt)) {
		/* Use an adaptive ray epsilon */
		Float rayMinT = ray.mint;
		if (rayMinT == Epsilon)
			rayMinT *= std::max(std::max(std::max(std::abs(ray.o.x),
				std::abs(ray.o.y)), std::abs(ray.o.z)), Epsilon);

		if (rayMinT > mint) mint = rayMinT;
		if (ray.maxt < maxt) maxt = ray.maxt;

		if (EXPECT_TAKEN(maxt > mint)) {
			if (rayIntersectHavran<false>(ray, mint, maxt, its.t, temp)) {
				fillIntersectionRecord<true>(ray, temp, its);
				return true;
			}
		}
	}
	return false;
}

bool ShapeKDTree::rayIntersect(const Ray &ray, Float &t, ConstShapePtr &shape,
		Normal &n, Point2 &uv) const {
	uint8_t temp[MTS_KD_INTERSECTION_TEMP];
	Float mint, maxt;

	t = std::numeric_limits<Float>::infinity();

	++shadowRaysTraced;
	if (m_aabb.rayIntersect(ray, mint, maxt)) {
		/* Use an adaptive ray epsilon */
		Float rayMinT = ray.mint;
		if (rayMinT == Epsilon)
			rayMinT *= std::max(std::max(std::abs(ray.o.x),
				std::abs(ray.o.y)), std::abs(ray.o.z));

		if (rayMinT > mint) mint = rayMinT;
		if (ray.maxt < maxt) maxt = ray.maxt;

		if (EXPECT_TAKEN(maxt > mint)) {
			if (rayIntersectHavran<false>(ray, mint, maxt, t, temp)) {
				const IntersectionCache *cache = reinterpret_cast<const IntersectionCache *>(temp);
				shape = m_shapes[cache->shapeIndex];

				if (m_triangleFlag[cache->shapeIndex]) {
					const TriMesh *trimesh = static_cast<const TriMesh *>(shape);
					const Triangle &tri = trimesh->getTriangles()[cache->primIndex];
					const Point *vertexPositions = trimesh->getVertexPositions();
					const Point2 *vertexTexcoords = trimesh->getVertexTexcoords();
					const uint32_t idx0 = tri.idx[0], idx1 = tri.idx[1], idx2 = tri.idx[2];
					const Point &p0 = vertexPositions[idx0];
					const Point &p1 = vertexPositions[idx1];
					const Point &p2 = vertexPositions[idx2];
					n = normalize(cross(p1-p0, p2-p0));

					if (EXPECT_TAKEN(vertexTexcoords)) {
						const Vector b(1 - cache->u - cache->v, cache->u, cache->v);
						const Point2 &t0 = vertexTexcoords[idx0];
						const Point2 &t1 = vertexTexcoords[idx1];
						const Point2 &t2 = vertexTexcoords[idx2];
						uv = t0 * b.x + t1 * b.y + t2 * b.z;
					} else {
						uv = Point2(0.0f);
					}
				} else {
					/// Uh oh... -- much unnecessary work is done here
					Intersection its;
					its.t = t;
					shape->fillIntersectionRecord(ray,
						reinterpret_cast<const uint8_t*>(temp) + 2*sizeof(IndexType), its);
					n = its.geoFrame.n;
					uv = its.uv;
					if (its.shape)
						shape = its.shape;
				}

				return true;
			}
		}
	}
	return false;
}


bool ShapeKDTree::rayIntersect(const Ray &ray) const {
	Float mint, maxt, t = std::numeric_limits<Float>::infinity();

	++shadowRaysTraced;
	if (m_aabb.rayIntersect(ray, mint, maxt)) {
		/* Use an adaptive ray epsilon */
		Float rayMinT = ray.mint;
		if (rayMinT == Epsilon)
			rayMinT *= std::max(std::max(std::abs(ray.o.x),
				std::abs(ray.o.y)), std::abs(ray.o.z));

		if (rayMinT > mint) mint = rayMinT;
		if (ray.maxt < maxt) maxt = ray.maxt;

		if (EXPECT_TAKEN(maxt > mint))
			if (rayIntersectHavran<true>(ray, mint, maxt, t, NULL))
				return true;
	}
	return false;
}

#if defined(MTS_HAS_COHERENT_RT)

/// Ray traversal stack entry for uncoherent ray tracing
struct CoherentKDStackEntry {
	/* Current ray interval */
	RayInterval4 MM_ALIGN16 interval;
	/* Pointer to the far child */
	const ShapeKDTree::KDNode * __restrict node;
};

static StatsCounter coherentPackets("General", "Coherent ray packets");
static StatsCounter incoherentPackets("General", "Incoherent ray packets");

void ShapeKDTree::rayIntersectPacket(const RayPacket4 &packet,
		const RayInterval4 &rayInterval, Intersection4 &its, void *temp) const {
	CoherentKDStackEntry MM_ALIGN16 stack[MTS_KD_MAXDEPTH];
	RayInterval4 MM_ALIGN16 interval;

	const KDNode * __restrict currNode = m_nodes;
	int stackIndex = 0;

	++coherentPackets;

	/* First, intersect with the kd-tree AABB to determine
	   the intersection search intervals */
	if (!m_aabb.rayIntersectPacket(packet, interval))
		return;

	interval.mint.ps = _mm_max_ps(interval.mint.ps, rayInterval.mint.ps);
	interval.maxt.ps = _mm_min_ps(interval.maxt.ps, rayInterval.maxt.ps);

	SSEVector itsFound( _mm_cmpgt_ps(interval.mint.ps, interval.maxt.ps));
	SSEVector masked(itsFound);
	if (_mm_movemask_ps(itsFound.ps) == 0xF)
		return;

	while (currNode != NULL) {
		while (EXPECT_TAKEN(!currNode->isLeaf())) {
			const uint8_t axis = currNode->getAxis();

			/* Calculate the plane intersection */
			const __m128
				splitVal = _mm_set1_ps(currNode->getSplit()),
				t = _mm_mul_ps(_mm_sub_ps(splitVal, packet.o[axis].ps),
					packet.dRcp[axis].ps);

			const __m128
				startsAfterSplit = _mm_or_ps(masked.ps,
					_mm_cmplt_ps(t, interval.mint.ps)),
				endsBeforeSplit = _mm_or_ps(masked.ps,
					_mm_cmpgt_ps(t, interval.maxt.ps));

			currNode = currNode->getLeft() + packet.signs[axis][0];

			/* The interval completely completely lies on one side
			   of the split plane */
			if (EXPECT_TAKEN(_mm_movemask_ps(startsAfterSplit) == 15)) {
				currNode = currNode->getSibling();
				continue;
			}

			if (EXPECT_TAKEN(_mm_movemask_ps(endsBeforeSplit) == 15))
				continue;

			stack[stackIndex].node = currNode->getSibling();
			stack[stackIndex].interval.maxt =    interval.maxt;
			stack[stackIndex].interval.mint.ps = _mm_max_ps(t, interval.mint.ps);
			interval.maxt.ps =                   _mm_min_ps(t, interval.maxt.ps);
			masked.ps = _mm_or_ps(masked.ps,
					_mm_cmpgt_ps(interval.mint.ps, interval.maxt.ps));
			stackIndex++;
		}

		/* Arrived at a leaf node - intersect against primitives */
		const IndexType primStart = currNode->getPrimStart();
		const IndexType primEnd = currNode->getPrimEnd();

		if (EXPECT_NOT_TAKEN(primStart != primEnd)) {
			SSEVector
				searchStart(_mm_max_ps(rayInterval.mint.ps,
					_mm_mul_ps(interval.mint.ps, SSEConstants::om_eps.ps))),
				searchEnd(_mm_min_ps(rayInterval.maxt.ps,
					_mm_mul_ps(interval.maxt.ps, SSEConstants::op_eps.ps)));

			for (IndexType entry=primStart; entry != primEnd; entry++) {
				const TriAccel &kdTri = m_triAccel[m_indices[entry]];
				if (EXPECT_TAKEN(kdTri.k != KNoTriangleFlag)) {
					itsFound.ps = _mm_or_ps(itsFound.ps,
						mitsuba::rayIntersectPacket(kdTri, packet, searchStart.ps, searchEnd.ps, masked.ps, its));
				} else {
					const Shape *shape = m_shapes[kdTri.shapeIndex];

					for (int i=0; i<4; ++i) {
						if (masked.i[i])
							continue;
						Ray ray;
						for (int axis=0; axis<3; axis++) {
							ray.o[axis] = packet.o[axis].f[i];
							ray.d[axis] = packet.d[axis].f[i];
							ray.dRcp[axis] = packet.dRcp[axis].f[i];
						}
						Float t;

						if (shape->rayIntersect(ray, searchStart.f[i], searchEnd.f[i], t,
								reinterpret_cast<uint8_t *>(temp)
								+ i * MTS_KD_INTERSECTION_TEMP + 2*sizeof(IndexType))) {
							its.t.f[i] = t;
							its.shapeIndex.i[i] = kdTri.shapeIndex;
							its.primIndex.i[i] = KNoTriangleFlag;
							itsFound.i[i] = 0xFFFFFFFF;
						}
					}
				}
				searchEnd.ps = _mm_min_ps(searchEnd.ps, its.t.ps);
			}
		}

		/* Abort if the tree has been traversed or if
		   intersections have been found for all four rays */
		if (_mm_movemask_ps(itsFound.ps) == 0xF || --stackIndex < 0)
			break;

		/* Pop from the stack */
		currNode = stack[stackIndex].node;
		interval = stack[stackIndex].interval;
		masked.ps = _mm_or_ps(itsFound.ps,
			_mm_cmpgt_ps(interval.mint.ps, interval.maxt.ps));
	}
}

void ShapeKDTree::rayIntersectPacketIncoherent(const RayPacket4 &packet,
		const RayInterval4 &rayInterval, Intersection4 &its4, void *temp) const {

	++incoherentPackets;
	for (int i=0; i<4; i++) {
		Ray ray;
		Float t;
		for (int axis=0; axis<3; axis++) {
			ray.o[axis] = packet.o[axis].f[i];
			ray.d[axis] = packet.d[axis].f[i];
			ray.dRcp[axis] = packet.dRcp[axis].f[i];
		}
		ray.mint = rayInterval.mint.f[i];
		ray.maxt = rayInterval.maxt.f[i];
		uint8_t *rayTemp = reinterpret_cast<uint8_t *>(temp) + i * MTS_KD_INTERSECTION_TEMP;
		if (ray.mint < ray.maxt && rayIntersectHavran<false>(ray, ray.mint, ray.maxt, t, rayTemp)) {
			const IntersectionCache *cache = reinterpret_cast<const IntersectionCache *>(rayTemp);
			its4.t.f[i] = t;
			its4.shapeIndex.i[i] = cache->shapeIndex;
			its4.primIndex.i[i] = cache->primIndex;
			its4.u.f[i] = cache->u;
			its4.v.f[i] = cache->v;
		}
	}
}

#endif

MTS_IMPLEMENT_CLASS(ShapeKDTree, false, KDTreeBase)
MTS_NAMESPACE_END
