#version 330 core
uniform mat4 view;
uniform mat4 proj;
// can we just use camera ray ?
in vec3 near_point;
in vec3 far_point; 
out vec4 frag_color;

float checkers(vec2 p) {
  vec2 q = floor(p);
  return mod(q.x+q.y,2.);
}

vec4 grid(vec3 fragPos3D, float scale) {
  vec2 coord = fragPos3D.xz * scale; // use the scale variable to set the distance between the lines
  vec2 derivative = fwidth(coord);
  vec2 grid = abs(fract(coord - 0.5) - 0.5) / derivative;
  float line = min(grid.x, grid.y);
  float minimumz = min(derivative.y, 1);
  float minimumx = min(derivative.x, 1);
  vec4 color = vec4(0.2, 0.2, 0.2, 1.0 - min(line, 1.0));
  // z axis
  if(fragPos3D.x > -0.1 * minimumx && fragPos3D.x < 0.1 * minimumx)
      color.z = 1.0;
  // x axis
  if(fragPos3D.z > -0.1 * minimumz && fragPos3D.z < 0.1 * minimumz)
      color.x = 1.0;
  return color;
}

void main() {
  // screen space reconstruction
  // the normal for xz plane is y (0,1,0)
  // 此处计算直线和平面的交点？
  float t = -near_point.y / (far_point.y - near_point.y);
  // 带入直线参数方程求出交点？
  vec3 p = near_point + t * (far_point - near_point);
  // 此时的 p 就对应于 xz 平面上的一点
  vec4 ndc_p = proj * view * vec4(p,1.0);
  gl_FragDepth = ndc_p.z / ndc_p.w;
  frag_color = vec4( vec3(0.2,0.4,0.8) * checkers(p.xz)  ,t > 0.0 ? 1.0 : 0.0);
  // frag_color = grid(p, 5) * float(t > 0);
  // assume depth value in 0~1
  // float ndc = gl_FragDepth * 2.0 - 1.0;
  // float near = 0.1;
  // float far = 100.0;
  // float linear_depth = (2.0 * near * far) / (far + near - ndc * (far - near));
  // frag_color = vec4(vec3(linear_depth), 1.0);
}