// Fall 2019

#include <glm/ext.hpp>

#include "A4.hpp"
#include "GeometryNode.hpp"
#include "Debug.hpp"

using namespace glm;
using namespace std;

void A4_Render(
		// What to render  
		SceneNode * root,

		// Image to write to, set to a given width and height  
		Image & image,

		// Viewing parameters  
		const glm::vec3 & eye,
		const glm::vec3 & view,
		const glm::vec3 & up,
		double fovy,

		// Lighting parameters  
		const glm::vec3 & ambient,
		const std::list<Light *> & lights
) {

  // Fill in raytracing code here...  

  std::cout << "Calling A4_Render(\n" <<
		  "\t" << *root <<
          "\t" << "Image(width:" << image.width() << ", height:" << image.height() << ")\n"
          "\t" << "eye:  " << glm::to_string(eye) << std::endl <<
		  "\t" << "view: " << glm::to_string(view) << std::endl <<
		  "\t" << "up:   " << glm::to_string(up) << std::endl <<
		  "\t" << "fovy: " << fovy << std::endl <<
          "\t" << "ambient: " << glm::to_string(ambient) << std::endl <<
		  "\t" << "lights{" << std::endl;

	for(const Light * light : lights) {
		std::cout << "\t\t" <<  *light << std::endl;
	}
	std::cout << "\t}" << std::endl;
	std:: cout <<")" << std::endl;

	const double width = image.width();
	const double height = image.height();
	const double d = 1;

	const double windowHeight = 2 * d * glm::tan(glm::radians(fovy/2));
	const double windowWidth = (width / height) * windowHeight;

	std::cerr << "width: " << width << std::endl;
	std::cerr << "height: " << height << std::endl;
	std::cerr << "windowHeight: " << windowHeight << std::endl;
	std::cerr << "windowWidth: " << windowWidth << std::endl;
	std::cerr << "d: " << d << std::endl;

	glm::mat4 T;

	T = glm::translate(T, glm::vec3(-width / 2, -height / 2, d));
	T = glm::scale(glm::vec3(-windowWidth / width, -windowHeight / height, 1)) * T;
	
	const glm::vec3 w = glm::normalize(view);
	const glm::vec3 u = glm::normalize(glm::cross(up, w));
	const glm::vec3 v = glm::normalize(glm::cross(w, u));

	glm::mat4 ViewToWorld {
		glm::vec4(u, 0),
		glm::vec4(v, 0),
		glm::vec4(w, 0),
		glm::vec4(eye, 1),
	};

  	T = ViewToWorld * T;	
	for (uint y = 0; y < height; ++y) {
		for (uint x = 0; x < width; ++x) {
			vec4 pixel = T * vec4(x, y, 0, 1);
			Ray ray(vec4(eye, 1), pixel);

			Hit closest_hit = closestObjectFromRay(root, ray);
			vec3 colour = closest_hit.hit ? evaluateLighting(ambient, lights, root, closest_hit, ray) : 
											drawBackground(y, height);
			// vec3 colour = closest_hit.hit ? vec3(closest_hit.mat->m_kd.r, closest_hit.mat->m_kd.g, closest_hit.mat->m_kd.b) : 
			// vec3 colour = closest_hit.hit ? ambient : 
			// 								drawBackground(y, height);
			image(x, y, 0) = colour.r; 
			image(x, y, 1) = colour.g;
			image(x, y, 2) = colour.b;
		}
	}
}

vec3 drawBackground(double y, const double height) {
	return vec3(0, 0, y / height);
}

// ------------------------------------------------------------- 
// Evaluate lighting and set pixel to that colour
// using the Blinn-Phong Lighting Model
vec3 evaluateLighting(
	const glm::vec3 & ambient,
	const std::list<Light *> & lights,
	SceneNode * node, // Root
	Hit & hit, 
	const Ray & ray)
{
	vec3 colour{ambient};

	for (auto light : lights) {
		glm::vec4 shadow_start = ray.origin + ray.direction * hit.t;

		Ray shadow(shadow_start, vec4(light->position, 1));

		Hit shadow_hit = closestObjectFromRay(node, shadow);

		if (!shadow_hit.hit) { // If shadow ray does NOT intersect with a light source
			// Direction from fragment to light source.
			vec4 lp = normalize(shadow.direction);
			vec3 l = vec3(lp.x, lp.y, lp.z);
			// Direction from fragment to viewer (origin - fragPosition).
			vec4 vp = normalize(ray.origin - ray.direction);
			vec3 v = vec3(vp.x, vp.y, vp.z);
			// DIFFUSE COEFFICIENT
			vec4 normal = hit.normal;
			float n_dot_l = std::max(0.0f, dot(vec3(normal.x, normal.y, normal.z), l));
			vec3 diffuse = hit.mat->m_kd * n_dot_l;
			// SPECULAR COEFFICIENT
			vec3 specular;
			if (n_dot_l > 0) {
				vec3 h = normalize(v + l);
				float n_dot_h = std::max(0.0f, dot(vec3(normal.x, normal.y, normal.z), h));
				specular = hit.mat->m_ks * pow(n_dot_h, hit.mat->m_shininess);
			}
			colour += light->colour * (diffuse + specular);
		} // Else just apply the Ambient colour
	}
	return colour;
}

// Find the closest object that the ray hits from the eye
Hit closestObjectFromRay(SceneNode * root, const Ray & ray) {
	return root->intersect(ray);
}
