// Fall 2019

#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"
#include "cs488-framework/GlErrorCheck.hpp"

class Sphere
{
public:
	Sphere( int x, int z );
	~Sphere();

	void reset();
    void uploadBufferData();
    void draw();
    void genBuffers();
    void translate(float x, float z);

    std::vector<glm::vec3> verts;
    std::vector<GLuint> indices;

    unsigned short int xPos;
    unsigned short int zPos;
    bool disabled;
private:
    GLuint SphereVao;
    GLuint SphereVbo;
};
