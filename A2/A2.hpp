// Fall 2019

#pragma once

#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"

#include <glm/glm.hpp>

#include <vector>

// Set a global maximum number of vertices in order to pre-allocate VBO data
// in one shot, rather than reallocating each frame.
const GLsizei kMaxVertices = 1000;


// Convenience class for storing vertex data in CPU memory.
// Data should be copied over to GPU memory via VBO storage before rendering.
class VertexData {
public:
	VertexData();

	std::vector<glm::vec2> positions;
	std::vector<glm::vec3> colours;
	GLuint index;
	GLsizei numVertices;
};


class A2 : public CS488Window {
public:
	A2();
	virtual ~A2();

protected:
	virtual void init() override;
	virtual void appLogic() override;
	virtual void guiLogic() override;
	virtual void draw() override;
	virtual void cleanup() override;

	virtual bool cursorEnterWindowEvent(int entered) override;
	virtual bool mouseMoveEvent(double xPos, double yPos) override;
	virtual bool mouseButtonInputEvent(int button, int actions, int mods) override;
	virtual bool mouseScrollEvent(double xOffSet, double yOffSet) override;
	virtual bool windowResizeEvent(int width, int height) override;
	virtual bool keyInputEvent(int key, int action, int mods) override;

	void createShaderProgram();
	void enableVertexAttribIndices();
	void generateVertexBuffers();
	void mapVboDataToVertexAttributeLocation();
	void uploadVertexDataToVbos();

	void initLineData();

	void setLineColour(const glm::vec3 & colour);

	void drawLine (
			const glm::vec2 & v0,
			const glm::vec2 & v1
	);
	void reset();
	void initV();
	void initP();
	void drawGnomons();
	void rotateModel(double xDifference);
	void translateModel(double xDifference);
	void scaleModel(double xDifference);
	void rotateView(double xDifference);
	void translateView(double xDifference);
	void changePerspective(double xDifference);
	void resizeViewport(double xPos, double yPos);

	void drawViewport();
	void drawCube();

	bool clip(glm::vec4 & v1, glm::vec4 & v2);
	glm::vec2 scaleToViewport(const glm::vec2 & v);

	ShaderProgram m_shader;

	GLuint m_vao;            // Vertex Array Object
	GLuint m_vbo_positions;  // Vertex Buffer Object
	GLuint m_vbo_colours;    // Vertex Buffer Object

	VertexData m_vertexData;

	int transformation_mode;
	double old_x_pos;
	
	// Perspective Projection
	float fieldOfView = 30.0f;
	float near = 1.0f; // Based on course notes
	float far = 1000.0f; 

	// const float initM[16] {
	// 	0.1f, 0, 0, 0,
	// 	0, 0.1f, 0, 0,
	// 	0, 0, 0.1f, 0,
	// 	0, 0, 0, 1,
	// };
	const float initM[16] {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		1, 1, 1, 1,
	};

	float viewportHeight;
	float viewportWidth;
	float viewport_x_vl; // X-coord of the viewport window (bottom left)
	float viewport_y_vb; // Y-coord of the viewport window (bottom left)
	float viewport_x_topright; // Y-coord of the viewport window (top right)
	float viewport_y_topright; // Y-coord of the viewport window (top right)
	float selected_viewport_x;
	float selected_viewport_y;
	float changing_viewport_x;
	float changing_viewport_y;
	bool storeViewportCorner = false;
	bool calcViewportSize = false;

	glm::vec3 m_currentLineColour;

	glm::mat4 P; // View-to-Projection matrix
	glm::mat4 V; // World-to-View matrix
	glm::mat4 M; // Model-to-World matrix
	glm::mat4 M_gnomon; // Model-to-World matrix for gnomon

	// TODO: These don't need to be vectors I think ... I never change them
	std::vector<std::pair<glm::vec4, glm::vec4>> world_gnomon; 
	std::vector<std::pair<glm::vec4, glm::vec4>> model_gnomon;
	std::vector<std::pair<glm::vec4, glm::vec4>> cube;

};
