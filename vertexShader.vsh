
attribute vec3 vertex;
in vec4 color;
out vec4 fragColor;
uniform mat4 matrix;

void main(void) {

   fragColor = color;
   gl_Position = matrix * vec4(vertex.xyz, 1.0);
}
