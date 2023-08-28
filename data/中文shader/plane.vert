#version 330 core
uniform mat4 view;
uniform mat4 proj;
uniform mat4 view_inv;
uniform mat4 proj_inv;
out vec3 near_point;
out vec3 far_point;
// Grid position are in xy clipped space
vec3 gridPlane[6] = vec3[](
  vec3(1, 1, 0), vec3(-1, -1, 0), vec3(-1, 1, 0),
  vec3(-1, -1, 0), vec3(1, 1, 0), vec3(1, -1, 0)
);
// normal vertice projection

vec3 unproject_point(vec3 p) {
  vec4 unprojected_p = view_inv * proj_inv * vec4(p,1.0);
  return unprojected_p.xyz / unprojected_p.w;
}

void main() {
  // we have to perform screen space conversion
  // gl_Position = proj * view * vec4(gridPlane[gl_VertexID].xyz, 1.0);
  // gl_Position = vec4(gridPlane[gl_VertexID].xyz, 1.0);
  vec3 p = gridPlane[gl_VertexID];  
  near_point = unproject_point(vec3(p.xy,0.0));
  far_point  = unproject_point(vec3(p.xy,1.0));
  gl_Position = vec4(p, 1.0);
}