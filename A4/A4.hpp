// Fall 2019

#pragma once

#include <glm/glm.hpp>

#include "SceneNode.hpp"
#include "Light.hpp"
#include "Image.hpp"

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
);

glm::vec3 drawBackground(double y, const double height);

glm::vec3 evaluateLighting(
	const glm::vec3 & ambient,
	const std::list<Light *> & lights, 
	SceneNode * node,
	Hit & hit, 
	const Ray & ray
);

Hit closestObjectFromRay(SceneNode * root, const Ray & ray);
