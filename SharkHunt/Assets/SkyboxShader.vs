#version 330

layout (location = 0) in vec3 aPos;

out vec3 texCoords;

uniform mat4 P;
uniform mat4 V;

void main()
{
    texCoords = aPos;
    gl_Position = P * V * vec4(aPos, 1.0);
}  