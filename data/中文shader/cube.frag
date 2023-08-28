#version 330 core
in vec3 normal;
out vec4 fragColor;
vec4 normal_fragment_shader(vec3 n){
  return vec4((n + vec3(1.0)) / 2.0,1.0);
}
void main() {
  fragColor = normal_fragment_shader(normal);
}