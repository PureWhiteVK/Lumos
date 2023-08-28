#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 vertexNormal;
layout (location = 2) in vec2 textureCoord;
uniform mat4 mvp;
out vec3 normal;
void main() {
  gl_Position = mvp * vec4(position, 1.0);
  normal = vertexNormal;
}