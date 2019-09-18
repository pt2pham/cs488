// Fall 2019

#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"
#include "cs488-framework/GlErrorCheck.hpp"

class Cube
{
public:
	Cube( int x, int height, int z );
	~Cube();

	void reset();
    void uploadBufferData();
    void draw();
    void genBuffers();

    std::vector<glm::vec3> verts;
    std::vector<GLuint> indices;
private:
    GLuint cubeVao;
    GLuint cubeVbo;
    GLuint indicesEbo;
};
