// Fall 2019

#include <iostream>
#include <fstream>

#include <glm/ext.hpp>

#include "cs488-framework/ObjFileDecoder.hpp"
#include "Mesh.hpp"

using namespace glm;
using namespace std;

Mesh::Mesh( const std::string& fname )
	: m_vertices()
	, m_faces()
{
	std::string code;
	double vx, vy, vz;
	size_t s1, s2, s3;

	std::ifstream ifs( ("Assets/" + fname).c_str() );

	while( ifs >> code ) {
		if( code == "v" ) {
			ifs >> vx >> vy >> vz;
			m_vertices.push_back( glm::vec3( vx, vy, vz ) );
		} else if( code == "f" ) {
			ifs >> s1 >> s2 >> s3;
			m_faces.push_back( Triangle( s1 - 1, s2 - 1, s3 - 1 ) );
		}
	}
}

Hit Mesh::intersect(const Ray & ray) {
	// Intersection occurs when: e + td = a + β(b − a) + γ(c − a)
	// s.t. a, b and c are the points on the triangle
	double tmin = INFINITY;
	Hit hit;
	for (auto tri : m_faces) {
		vec3 a{m_vertices[tri.v1]};
		vec3 b{m_vertices[tri.v2]};
		vec3 c{m_vertices[tri.v3]};

		vec3 X{b - a};
		vec3 Y{c - a};
		vec3 Z{-ray.direction}; // -(direction - origin) => origin - direction

		float D = determinant(mat3{X, Y, Z});

		if (fabs(D) < 0.0001) {
			continue;
		}

		vec3 M = vec3(ray.origin) - a;

		double DX = determinant(mat3{M, Y, Z});
		double DY = determinant(mat3{X, M, Z});
		double DZ = determinant(mat3{X, Y, M});

		vec3 unknowns{DX/D, DY/D, DZ/D};

		float beta = unknowns.x;
		float gamma = unknowns.y;
		float t = unknowns.z;

		// If this statement is true, then there is no intersection
		if (beta <= 0 || gamma <= 0 || t <= 0 || beta + gamma >= 1)
			continue;

		if (t < tmin) {
			tmin = t;

			hit.t = tmin;
			hit.hit = true;
			// CALCULATE NORMAL
			// TODO: Fix this so the colour displays correctly?
			hit.normal = vec4(normalize(cross(b - a, c - a)), 0);
			vec4 point = ray.origin + ray.direction * t;
		}
	}

	return hit; // If no intersections, this Hit will be built by blank default constructor
}

std::ostream& operator<<(std::ostream& out, const Mesh& mesh)
{
  out << "mesh {";
  /*
  
  for( size_t idx = 0; idx < mesh.m_verts.size(); ++idx ) {
  	const MeshVertex& v = mesh.m_verts[idx];
  	out << glm::to_string( v.m_position );
	if( mesh.m_have_norm ) {
  	  out << " / " << glm::to_string( v.m_normal );
	}
	if( mesh.m_have_uv ) {
  	  out << " / " << glm::to_string( v.m_uv );
	}
  }

*/
  out << "}";
  return out;
}
