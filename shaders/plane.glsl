#version 330
// --vertex
uniform mat4 MVP;
uniform vec2 dimensions;


void main() {
    if(gl_VertexID == 0) {
        gl_Position = MVP * (vec4(0, 0, 0, 1));
    }
    if (gl_VertexID == 1) {
        gl_Position = MVP * (vec4(-1 * dimensions.x, 0, 0, 1));
    }
    if (gl_VertexID == 2) {
        gl_Position = MVP * (vec4(0, 0, -1 * dimensions.y, 1));
    }
}

// --fragment
uniform vec4 color;
out vec4 FragColor;

void main() {
    FragColor = vec4(1, 0, 0, 1);
}