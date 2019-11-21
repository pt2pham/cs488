// Fall 2019

#pragma once

#include "SceneNode.hpp"

class GeometryNode : public SceneNode {
public:
	GeometryNode(
		const std::string & meshId,
		const std::string & name
	);

	Material material;

	// Mesh Identifier. This must correspond to an object name of
	// a loaded .obj file.
	std::string meshId;

	void render(
        const ShaderProgram & m_shader,
        const glm::mat4 & m_view,
        BatchInfoMap & m_batchInfoMap, 
        std::deque<glm::mat4> & stack
    );

	void updateShaderUniforms(
		const ShaderProgram & m_shader,
		const glm::mat4 & m_view,
		std::deque<glm::mat4> & stack
	);
};
