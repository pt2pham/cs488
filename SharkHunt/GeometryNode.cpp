// Fall 2019

#include "GeometryNode.hpp"

using namespace std;
using namespace glm;

//---------------------------------------------------------------------------------------
GeometryNode::GeometryNode(
		const std::string & meshId,
		const std::string & name
)
	: SceneNode(name),
	  meshId(meshId)
{
	m_nodeType = NodeType::GeometryNode;
}

void GeometryNode::render(
        const ShaderProgram & m_shader,
        const mat4 & m_view,
        BatchInfoMap & m_batchInfoMap, 
        deque<mat4> & stack
) 
{
	stack.push_back(trans);

	// render all children.
	for (auto node : children)
		node->render(m_shader, m_view, m_batchInfoMap, stack);
	
	updateShaderUniforms(m_shader, m_view, stack);

	// Get the BatchInfo corresponding to the GeometryNode's unique MeshId.
	BatchInfo batchInfo = m_batchInfoMap[meshId];

	//-- Now render the mesh:
	m_shader.enable();
	glDrawArrays(GL_TRIANGLES, batchInfo.startIndex, batchInfo.numIndices);
	m_shader.disable();

	stack.pop_back();
}

//----------------------------------------------------------------------------------------
// Update mesh specific shader uniforms:
void GeometryNode::updateShaderUniforms(
		const ShaderProgram & m_shader,
        const mat4 & m_view,
		deque<mat4> & stack
) 
{
	m_shader.enable();
	{
		//-- Set ModelView matrix:
		GLint location = m_shader.getUniformLocation("ModelView");
		
		// Build up transformations on stack that also include this nodes Trans matrix
		mat4 modelView;
		for (auto stack_trans = stack.rbegin(); stack_trans != stack.rend(); ++stack_trans)
			modelView = (*stack_trans) * modelView;
		modelView = m_view * modelView;
		
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(modelView));
		CHECK_GL_ERRORS;
		if( do_picking ) {
			float r = float(m_nodeId&0xff) / 255.0f;
			float g = float((m_nodeId>>8)&0xff) / 255.0f;
			float b = float((m_nodeId>>16)&0xff) / 255.0f;

			location = m_shader.getUniformLocation("material.kd");
			glUniform3f( location, r, g, b );
			CHECK_GL_ERRORS;
		} else {
			//-- Set NormMatrix:
			location = m_shader.getUniformLocation("NormalMatrix");
			mat3 normalMatrix = glm::transpose(glm::inverse(mat3(modelView)));
			glUniformMatrix3fv(location, 1, GL_FALSE, value_ptr(normalMatrix));
			CHECK_GL_ERRORS;

			//-- Set Material values:
			location = m_shader.getUniformLocation("material.kd");
			glUniform3fv(location, 1, value_ptr(isSelected ? vec3(1,1,0) : material.kd));
			CHECK_GL_ERRORS;
		}
	}
	m_shader.disable();
}
