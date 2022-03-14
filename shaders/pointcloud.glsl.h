#pragma once

static const char* POINT_CLOUD_SHADER = R"(#version 430 core
pass vec4 Position;
pass vec4 Color;

// --vertex
layout (location = 0) in vec3  position;
layout (location = 1) in vec3  color;

uniform mat4 MV;

void main() {
	oPosition = MV * vec4(position, 1);
	oColor = vec4(color, 1);
}

// --geometry
layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

uniform float pointsize;
uniform mat4 P;

out vec2 uv;

void main() {
    vec4 c = iPosition[0];
    oColor = iColor[0];
    gl_Position = P * (c - vec4(-pointsize, -pointsize, 0, 0) * c.z * 0.5);
    uv = vec2(-1, -1);
    EmitVertex(); 
    gl_Position = P * (c - vec4(-pointsize, pointsize, 0, 0) * c.z * 0.5);
    uv = vec2(-1, 1);
    EmitVertex(); 
    gl_Position = P * (c - vec4(pointsize, -pointsize, 0, 0) * c.z * 0.5);
    uv = vec2(1, -1);
    EmitVertex(); 
    gl_Position = P * (c - vec4(pointsize, pointsize, 0, 0) * c.z * 0.5);
    uv = vec2(1, 1);
    EmitVertex(); 
    EndPrimitive();
}

// --fragment
in vec2 uv;

out vec4 FragColor;

void main() {
	
	if(dot(uv, uv) > 1) discard;
	FragColor = iColor;
})";