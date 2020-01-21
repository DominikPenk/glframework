#version 430 core
pass vec4 Position;

// --vertex
layout (location = 0) in vec3 position;

uniform mat4 MV;

flat out int vId;

void main() {
    oPosition = MV * vec4(position, 1);
    vId = gl_VertexID;
}

// --geometry
layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

uniform float size;
uniform mat4 P;

flat in int vId[];
out vec2 uv;
flat out int vertexId;

void main() {
    vec4 c = iPosition[0];
    vertexId = vId[0];
    gl_Position = P * (c - vec4(-size, -size, 0, 0) * c.z * 0.5);
    uv = vec2(-1, -1);
    EmitVertex(); 
    gl_Position = P * (c - vec4(-size, size, 0, 0) * c.z * 0.5);
    uv = vec2(-1, 1);
    EmitVertex(); 
    gl_Position = P * (c - vec4(size, -size, 0, 0) * c.z * 0.5);
    uv = vec2(1, -1);
    EmitVertex(); 
    gl_Position = P * (c - vec4(size, size, 0, 0) * c.z * 0.5);
    uv = vec2(1, 1);
    EmitVertex(); 
    EndPrimitive();
}

// --fragment
uniform vec4 color;
uniform vec4 selectedColor;
uniform int hoveredId;

in vec2 uv;
flat in int vertexId;

out vec4 FragColor;

void main() {
    FragColor = hoveredId == vertexId ? selectedColor : color;
}