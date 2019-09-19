// Fall 2019

#include <algorithm>
#include <cstdio>
#include <iostream>

#include "cube.hpp"

static const float CUBE_SIZE = 1.0f;
Cube::Cube( int x, int height, int z ) : disabled(false), xPos(x), zPos(z) {
    genBuffers();

    verts = {
		// Top 
		glm::vec3(1.0f + CUBE_SIZE*x, CUBE_SIZE*height, 1.0f + CUBE_SIZE*z), // back right
		glm::vec3(CUBE_SIZE*x, CUBE_SIZE*height, 1.0f + CUBE_SIZE*z), // back left
		glm::vec3(1.0f + CUBE_SIZE*x, CUBE_SIZE*height, CUBE_SIZE*z), // front right
		glm::vec3(CUBE_SIZE*x, CUBE_SIZE*height, CUBE_SIZE*z), // front left
		// Bottom
		glm::vec3(1.0f + CUBE_SIZE*x, 0, 1.0f + CUBE_SIZE*z), // back right
		glm::vec3(CUBE_SIZE*x, 0, 1.0f + CUBE_SIZE*z), // back left
		glm::vec3(1.0f + CUBE_SIZE*x, 0, CUBE_SIZE*z), // front right
		glm::vec3(CUBE_SIZE*x, 0, CUBE_SIZE*z), // front left
	};

	indices = {
		0, 1, 2, 2, 1, 3, // top
		3, 7, 2, 2, 6, 7, // front
		1, 5, 3, 3, 5, 7, // left side
		0, 4, 2, 2, 4, 6, // right side
		5, 4, 1, 1, 4, 0, // back
		5, 4, 7, 7, 4, 6 // bottom
	};
    uploadBufferData();
}

Cube::~Cube() {
}

void Cube::genBuffers() {
    glGenVertexArrays( 1, &cubeVao );
	glGenBuffers( 1, &cubeVbo );
    glGenBuffers( 1, &indicesEbo );
}

void Cube::uploadBufferData() {
    // Cube VAO
    glBindVertexArray( cubeVao );

    // Cube VBO
    glBindBuffer( GL_ARRAY_BUFFER, cubeVbo );
	glBufferData( GL_ARRAY_BUFFER, sizeof(glm::vec3)*verts.size(), verts.data(), GL_STATIC_DRAW );
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);	
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (char*)(sizeof(float) * 3));	

    // Indices EBO
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, indicesEbo );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), indices.data(), GL_STATIC_DRAW );
}

void Cube::draw() {
    // Rebind
    glBindVertexArray( cubeVao );
    glBindBuffer(GL_ARRAY_BUFFER, cubeVbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesEbo);
    // Draw
    glDrawElements(GL_TRIANGLES, (GLsizei)36*3, GL_UNSIGNED_INT, 0);
    // Reset binded buffers
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// The first 4 vertices are for the top square
void Cube::growHeight(unsigned short int delta) {
    for (int i=0;i<4;i++){
        verts[i].y += delta;
    }
    uploadBufferData();
}

void Cube::shrinkHeight(unsigned short int delta) {
    for (int i=0;i<4;i++){
        verts[i].y = std::max(0.0f, verts[i].y - delta);
    }
    uploadBufferData();
}

void Cube::translate(float x, float z) {
    xPos += x; // Tracked to compare position to walls in maze
    zPos += z;

    for (int i = 0; i < 8; i++) {
        verts[i].x += x;
        verts[i].z += z;
    }
    uploadBufferData();
}
