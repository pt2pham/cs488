// Fall 2019

#include "GeometryNode.hpp"
#include "Debug.hpp"

//---------------------------------------------------------------------------------------
GeometryNode::GeometryNode(
	const std::string & name, Primitive *prim, Material *mat )
	: SceneNode( name )
	, m_material( mat )
	, m_primitive( prim )
{
	m_nodeType = NodeType::GeometryNode;
}

void GeometryNode::setMaterial( Material *mat )
{
	// Obviously, there's a potential memory leak here.  A good solution
	// would be to use some kind of reference counting, as in the 
	// C++ shared_ptr.  But I'm going to punt on that problem here.
	// Why?  Two reasons:
	// (a) In practice we expect the scene to be constructed exactly
	//     once.  There's no reason to believe that materials will be
	//     repeatedly overwritten in a GeometryNode.
	// (b) A ray tracer is a program in which you compute once, and 
	//     throw away all your data.  A memory leak won't build up and
	//     crash the program.

	m_material = mat;
}

// Invert transformations before calculating intersection
Hit GeometryNode::intersect(const Ray & ray) {
	Ray invRay = Ray::rayCopy(invtrans * ray.origin, invtrans * ray.direction);
	Hit h = m_primitive->intersect(invRay);

	// Traverse children
	for (auto child : children) {
		Hit hit = child->intersect(invRay);
		if (hit.hit && (!h.hit || hit.t < h.t))
			h = hit;
	}

	if (h.hit) {
		h.normal = glm::transpose(invtrans) * h.normal;
		h.mat = (PhongMaterial *) m_material;
	}

	return h;
}
