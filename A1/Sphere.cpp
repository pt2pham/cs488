// Fall 2019

#include <algorithm>
#include <cstdio>
#include <iostream>
#include <cmath>

#include "Sphere.hpp"

static const float RADIUS = 0.5f;
Sphere::Sphere( int x, int z ) {
    genBuffers();
    xPos = x;
    zPos = z;
    float latitude_spacing = 360.0f / (30);
    float longitude_spacing = 180.0f / (30);

    float theta, phi;
    for (int latitude = 0; latitude < 360.f; latitude += latitude_spacing) {
        for (int longitude = 0; longitude < 180.0f; longitude += longitude_spacing) {
            
            verts.push_back(glm::vec3(
                (GLfloat) (RADIUS * std::sin((latitude * 3.14159f) / 180) * std::sin((longitude * 3.14159f) / 180)), 
                (GLfloat) (RADIUS * std::cos((latitude * 3.14159f) / 180)),
                (GLfloat) (RADIUS * std::sin((latitude * 3.14159f) / 180) * std::cos((longitude * 3.14159f) / 180))
            ));
        }
    }
    uploadBufferData();
}

Sphere::~Sphere() {
}

void Sphere::genBuffers() {
    glGenVertexArrays( 1, &SphereVao );
	glGenBuffers( 1, &SphereVbo );
}

void Sphere::uploadBufferData() {
    // Sphere VAO
    glBindVertexArray( SphereVao );

    // Sphere VBO
    glBindBuffer( GL_ARRAY_BUFFER, SphereVbo );
	glBufferData( GL_ARRAY_BUFFER, sizeof(glm::vec3)*verts.size(), verts.data(), GL_STATIC_DRAW );
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);	
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (char*)(sizeof(float) * 3));	
}

void Sphere::draw() {
    // Rebind
    glBindVertexArray( SphereVao );
    glBindBuffer(GL_ARRAY_BUFFER, SphereVbo);
    // Draw
    glDrawElements(GL_TRIANGLES, (GLsizei)36*3, GL_UNSIGNED_INT, 0);
    // Reset binded buffers
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}