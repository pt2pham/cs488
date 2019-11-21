// Fall 2019

#pragma once

#include "SceneNode.hpp"
#include "Primitive.hpp"
#include "Material.hpp"
#include "Ray.hpp"

class GeometryNode : public SceneNode {
public:
	GeometryNode( const std::string & name, Primitive *prim, 
		Material *mat = nullptr );

	void setMaterial( Material *material );
	Hit intersect(const Ray & ray);
	// virtual Hit intersect(const Ray & ray, std::list<glm::mat4> transforms);

	Material *m_material;
	Primitive *m_primitive;
};
