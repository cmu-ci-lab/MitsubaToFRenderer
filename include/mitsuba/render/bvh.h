/* 
* This code is written by Adithya Pediredla using Adrian Jarabo's code as a refernce
 */

#pragma once

#if !defined(__MITSUBA_RENDER_BVH_H_)
#define __MITSUBA_RENDER_BVH_H_

#include <mitsuba/core/aabb.h>
#include <mitsuba/render/shape.h>
#include <mitsuba/render/triaccel.h>
#include <vector>
#include <algorithm>


MTS_NAMESPACE_BEGIN

template<class T>
class Aggregate
{
public:
	std::vector<const Shape *> m_shapes;
	std::vector<T> m_triaccels; //triaccels contain shape index from which the mesh (triangles i.e three indices and positions) can be accessed
protected:
	AABB m_bb;
	bool m_frozen;
public:
	Aggregate():m_frozen(false){}
	Aggregate(std::vector<const Shape *> shapes, std::vector<T> &triaccels):m_frozen(false)
	{
		m_shapes = shapes;
		m_triaccels = triaccels;
		for( size_t i=0; i< triaccels.size(); ++i){
			expandBBox(m_bb, triaccels[i]);
		}
	}
	~Aggregate(){clear();}


	virtual void add_primitive( const T &p)
	{	
		m_triaccels.push_back(p);
		expandBBox(m_bb, p);
		m_frozen = false;
	}
	virtual void add_primitives( const std::vector<T> &ps)
	{
		for( unsigned int i=0; i<ps.size(); ++i)
		{
			expandBBox(m_bb, ps[i]);
			m_triaccels.push_back(ps[i]);
		}
		m_frozen = false;
	}
	virtual void freeze(){}

	virtual void clear(){ m_triaccels.clear(); m_bb = AABB(); }
	
	int nb_primitives() const{ return m_triaccels.size(); }
	const T &get_primitive(int i)const{ return m_triaccels[i]; }
	T &get_primitive(int i){ return  m_triaccels[i]; }

	// Information about the volume of the aggregate 
	AABB get_bounding_box()const
	{
		return m_bb;
	}
	Point get_center() const
	{
		return m_bb.getCenter();
	}
	void expandBBox(AABB &bb, const T &ta){
		const TriMesh *mesh = static_cast<const TriMesh *>(m_shapes[ta.shapeIndex]);
		const Triangle *triangles = mesh->getTriangles();
		const Point *positions = mesh->getVertexPositions();

		const Triangle &tri = triangles[ta.primIndex];
		bb.expandBy(positions[tri.idx[0]]);
		bb.expandBy(positions[tri.idx[1]]);
		bb.expandBy(positions[tri.idx[2]]);
	}
	Point getCenter(const T &ta) const{
		const TriMesh *mesh = static_cast<const TriMesh *>(m_shapes[ta.shapeIndex]);
		const Triangle *triangles = mesh->getTriangles();
		const Point *positions = mesh->getVertexPositions();

		const Triangle &tri = triangles[ta.primIndex];
		return 0.33f * (positions[tri.idx[0]] +
						positions[tri.idx[1]] +
						positions[tri.idx[2]]);
	}

};

/** Class modelling the nodes in the hierarchy, including leaves */
struct Node
{
	std::vector<int>::iterator begin, end;
	AABB bb;
	bool AABBsameAsParent; // mark if the  parent has same AABB as the child, so that the BBox test can be avoided.
	Node *child1, *child2;

	Node(): AABBsameAsParent(false), child1(0), child2(0){}
};

template<class T>
class BVH: public Aggregate<T>
{
public:
	// Although Wald [2007] suggests that using just 8 bins is enough for
	// BVH construction, we opt for a more conservative approach, since we
	// do not aim for super-efficient construction.
	static int nb_bins_construction;
	// This is a rather standard number of primitives.
	static int min_nb_primitives_leaf;
	static int max_nb_primitives_leaf;

	std::vector<Node> nodes;
	std::vector<int> indices;
private:


	// Class Comp, used for comparison when partitioning prim1itives in
	// the building of the tree.
	template<int axis>
	struct Comp
	{
		Comp(BVH<T> *bvh): b_bvh(bvh){}
		BVH<T> *b_bvh;
		bool operator ()(const int obj1, const int obj2)const
		{
			if( b_bvh-> getCenter(b_bvh->m_triaccels[obj1])[axis] <= b_bvh-> getCenter(b_bvh->m_triaccels[obj2])[axis])
				return false;

			return true;
		}

	};
	friend struct Comp<0>; friend struct Comp<1>; friend struct Comp<2>;
	Comp<0> comp0; Comp<1> comp1; Comp<2> comp2;

	void sort_objects(const std::vector<int>::iterator &begin, const std::vector<int>::iterator &end, const int axis);

	void build_bounding_box(const std::vector<int>::iterator &begin, const std::vector<int>::iterator &end);

	/** Divide the primitives contained in 'objects' into two
		groups, using a SAH. The binning technique used cames from:
		Wald, I. 2007. "On fast Construction of SAH-based Bounding Volume
		Hierarchies". In Symposium on Interactive Ray Tracing 2007.
		http://www.sci.utah.edu/~wald/Publications/2007/FastBuild/download/fastbuild.pdf */
	void split_objects(const Node &node, Node **node1, Node **node2);
	void build_tree(Node &node);

	void build_BBoxRelationShip(const Node* node) const{
		if(node->child1 != 0){
			if(node->bb == node->child1->bb)
				node->child1->AABBsameAsParent = true;
			build_BBoxRelationShip(node->child1);
		}
		if(node->child2 != 0){
			if(node->bb == node->child2->bb)
				node->child2->AABBsameAsParent = true;
			build_BBoxRelationShip(node->child2);
		}
	}

	void print_tree(const Node* node) const{
		std::cout << "BBox:" << node->bb.toString() << std::endl;
		if(node->AABBsameAsParent)
			std::cout << "BBoxRelationShip: same as parent" << std::endl;
		else
			std::cout << "BBoxRelationShip: different" << std::endl;

		if(node->child1 == 0 && node->child2 == 0){ //leaf node
			for(std::vector<int>::iterator it = node->begin; it != node->end; it++){
				std::cout << *it << " ";
			}
			std::cout << std::endl;
		}
		if(node->child1 != 0){
			std::cout << "Going to left: \n";
			print_tree(node->child1);
		}
		if(node->child2 != 0){
			std::cout << "Going to Right: \n";
			print_tree(node->child2);
		}
	}


public:
	BVH();
	BVH(std::vector<const Shape *> shapes, std::vector<T> &triaccels);
	~BVH();

	void freeze();
	void clear();

	Float get_intersection_cost()const{return 0.;}
}; //BVH

//-----------------------------------------------------------------------
// Constructors & Destructor
template<class T>
BVH<T>::BVH():Aggregate<T>(),comp0(this),comp1(this),comp2(this){}

template<class T>
BVH<T>::BVH(std::vector<const Shape *> shapes, std::vector<T> &triaccels):Aggregate<T>(shapes, triaccels),comp0(this),comp1(this),comp2(this)
{
	freeze();
	build_BBoxRelationShip(&this->nodes[0]);
//	print_tree(&this->nodes[0]);
}

template<class T>
BVH<T>::~BVH()
{	clear();	}


//------------------------------------------------------------------------
// Functions to build the tree...
template<class T>
void BVH<T>::freeze()
{
	if( this->m_frozen )
		return;

	// Clear tree data to rebuild it
	indices.clear();
	nodes.clear();

	// Create primitives index
	indices.reserve(this->m_triaccels.size());
	for(int i=0; i<this->m_triaccels.size(); ++i)
		indices.push_back(i);

	// Since each primitive is referenced only one time, the number of nodes
	// is bounded by 2N-1, with N the number of primitives.
	nodes.reserve(2*this->m_triaccels.size()-1);

	// Create root node
	Node n;
	nodes.push_back(n);
	nodes.back().begin = indices.begin(); nodes.back().end = indices.end();

	nodes.back().bb = this->m_bb;

	// Recursively create BVH based on root node
	build_tree(nodes.back());

//	// Pack together the primitives that belong to the same leaf
//	std::vector<T> aux_primitives = this->m_triaccels;
//	for(int i=0; i<this->m_triaccels.size(); ++i)
//	{
//		this->m_triaccels[i] = aux_primitives[indices[i]];
//		indices[i] = i;
//	}

	this->m_frozen = true;
}


template<class T>
void BVH<T>::build_tree( Node &node )
{
	Node *n1 = 0, *n2 = 0;
	split_objects(node, &n1, &n2);
	if( n1 && n2 ) //Note that it's actually not needed comparing both, but that way the code is more cute.
	{
		node.child1 = n1;
		node.child2 = n2;

		build_tree(*n1);
		build_tree(*n2);
	}
}
//
template<class T>
void BVH<T>::build_bounding_box(const std::vector<int>::iterator &begin, const std::vector<int>::iterator &end)
{
	this->m_bb = AABB();
	for(std::vector<int>::iterator i=begin; i!=end; ++i)
		expandBBox(this->m_bb, this->m_triaccels[*i]);
}

template<class T>
void BVH<T>::sort_objects(const std::vector<int>::iterator &begin, const std::vector<int>::iterator &end, const int axis)
{
	switch(axis)
	{
	case 0:
		std::stable_sort(begin, end, comp0);
		break;
	case 1:
		std::stable_sort(begin, end, comp1);
		break;
	case 2:
		std::stable_sort(begin, end, comp2);
		break;
	}
}
//
template<class T>
void BVH<T>::split_objects(const Node &node, Node **node1, Node **node2)
{
	if( (node.end-node.begin) <= min_nb_primitives_leaf )
	{	(*node1) = (*node2) = 0; return;	}

	int axis = node.bb.getLargestAxis();
	int dominant_axis = 0;
	int dim = node.bb.getCenter()[axis];

	// We first compute the cost of not-traversing the node
	Float C_nt = 0;
	for( std::vector<int>::iterator i = node.begin; i != node.end; ++i )
		C_nt += this->m_triaccels[*i].getIntersectionCost();

	// We compute the individual intersection cost as the average cost;
	Float C_i = 8.;//C_nt / static_cast<Float>(node.end-node.begin);

	// Sort the primitives
	sort_objects(node.begin, node.end, axis);

	// We now compute the SAH heuristic for the defined bins, so we keep the
	// best possible partition.
	Float delta_position = node.bb.getExtents()[axis]/static_cast<Float>(nb_bins_construction+1);
	Float current_position = delta_position+node.bb.min[axis];

	// Precompute all fixed values to be used
	Float C_t = 1.;//11471.59804;//2*bb.get_intersection_cost();
	Float iSAV = 1./node.bb.getSurfaceArea();

	// ... and use as best current case the end of traversal
	Float best_SA = (C_t > C_nt)?C_nt:std::numeric_limits<Float>::infinity();
	std::vector<int>::iterator best_switchpoint = node.end;
	AABB best_bb[2];
	int N_c[2]; N_c[0]=N_c[1]=0;
	int runningBest=0;
	while(1)
	{
		int min_nb_bin=0;
		Float min_nb_position;
		bool to_switch_nb_bin = true;

		// ... then compute SAH of each bin
		for( int b=0; b<nb_bins_construction; ++b)
		{
			AABB bb_c[2]; int c=0;
			N_c[0]=N_c[1]=0;
			bool toswitch = true; std::vector<int>::iterator switchpoint = node.end;

			// For each primitive in the node evaluate at wich side of the partition
			// it is (its centroid), and add it to each respective bounding box
			for( std::vector<int>::iterator i = node.begin; i != node.end; ++i )
			{
				//printf("%f vs %f: %s\n",primitives[*i].get_center()[axis],current_position,toswitch?"true":"false");
				if( toswitch && (this->getCenter(this->m_triaccels[*i])[axis] < current_position) )
				{	c=1; toswitch=false; switchpoint=i; }

				// expand by primitive
				this->expandBBox(bb_c[c], this->m_triaccels[*i]);
				++N_c[c];
			}

			Float p1 = bb_c[0].getSurfaceArea()*iSAV, p2 = bb_c[1].getSurfaceArea()*iSAV;
			Float sum_p = p1+p2;

			// Then, compute this partition's cost...
			Float SA = C_t + ((N_c[0])?(bb_c[0].getSurfaceArea()*N_c[0]):0. + (N_c[1])?(bb_c[1].getSurfaceArea()*N_c[1]):0.) * iSAV*C_i;

			// Set if a change between the number of primitives on one side or in the other, to
			// keep track the minimum in case the sampling is too coarse to spot it.
			if(to_switch_nb_bin && (N_c[0] < N_c[1]))
			{	to_switch_nb_bin = false; min_nb_bin = b-1;
				min_nb_position=current_position-delta_position;	}

			// ... and if it's lower than the best current partition, keep it!
			// Note that, for each leaf, we continue until the nb_primitives_leaf is reached
			if( SA < best_SA && !(N_c[0]<min_nb_primitives_leaf || N_c[1]<min_nb_primitives_leaf ) )
			{
				best_SA = SA;
				best_switchpoint = switchpoint;
				best_bb[0] = bb_c[0];
				best_bb[1] = bb_c[1];
			}

			// ... finally, update the partition position.
			current_position += delta_position;
		}

		// If found a good partition, end recursion
		if( best_switchpoint != node.end )
			break;

		// If no good partition reached, and there's a small enough number
		// of primitives, just end and store the current node as a leaf
//  	Adithya: This appears wrong to me. One of them will be zero if the partition does not happen at all.
//		if( N_c[0]<max_nb_primitives_leaf || N_c[1]<max_nb_primitives_leaf )
//		{	(*node1) = (*node2) = 0; return;	}
		// This is my code
		if( (N_c[0]+N_c[1])<max_nb_primitives_leaf )
				{	(*node1) = (*node2) = 0; return;	}

		// Else, iterate in the change interval, looking at it
		// with finer sampling
		current_position = min_nb_position;
		delta_position /= static_cast<Float>(nb_bins_construction+1);
		// ...or, if delta is too small, change the partition axis

		// Adithya: This part of the code is commented out. I have enabled it and tried to fix it a little.
		if( delta_position < 1e-5 )
		{
			if(runningBest == 0)
				{axis = node.bb.getSecondLargestAxis();runningBest++;}
			else if(runningBest == 1)
				{axis = node.bb.getThirdLargestAxis();runningBest++;}
			else
				{	(*node1) = (*node2) = 0; return;	}

			sort_objects(node.begin, node.end, axis);

			// We now compute the SAH heuristic for the defined bins, so we keep the
			// best possible partition.
			delta_position = node.bb.getExtents()[axis]/static_cast<Float>(nb_bins_construction+1);
			current_position = delta_position+node.bb.min[axis];
		}
//		{	(*node1) = (*node2) = 0; return;	}
	}

	// ... and return the partitioned nodes
	Node n_1;
	n_1.begin = node.begin;
	n_1.end = best_switchpoint;
	n_1.bb = best_bb[0];
  	nodes.push_back(n_1); *node1 = &(nodes.back());

	Node n_2;
	n_2.begin = best_switchpoint;
	n_2.end = node.end;
	n_2.bb = best_bb[1];
  	nodes.push_back(n_2); *node2 = &nodes.back();
}


//------------------------------------------------------------------------
// Clearing...
template<class T>
void BVH<T>::clear()
{
	Aggregate<T>::clear();
	nodes.clear();
	indices.clear();
}

//------------------------------------------------------------------------
// Static data initialization...
template<class T>
int BVH<T>::nb_bins_construction = 6;
template<class T>
int BVH<T>::min_nb_primitives_leaf = 4;
template<class T>
int BVH<T>::max_nb_primitives_leaf = 8;

//
//template BVH<Triangle>::sort_objects(const std::vector<int>::iterator &begin, const std::vector<int>::iterator &end, const int axis);
//
//template BVH<Triangle>::build_bounding_box(const std::vector<int>::iterator &begin, const std::vector<int>::iterator &end);
//
//template BVH<Triangle>::split_objects(const Node &node, Node **node1, Node **node2);
//
//template BVH<Triangle>::build_tree(Node &node);
//
//template BVH<Triangle>::BVH();
//template BVH<Triangle>::BVH(std::vector<T> &objects, Point* &positions);
//template BVH<Triangle>::~BVH();
//
//template BVH<Triangle>::freeze();
//template BVH<Triangle>::clear();
//
//template BVH<Triangle>::get_intersection_cost()const;
//
//template class MTS_EXPORT_RENDER BVH<Triangle>;

MTS_NAMESPACE_END

#endif //_BVH_H_
