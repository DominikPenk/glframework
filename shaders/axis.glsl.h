#pragma once

static const char * AXIS_SHADER = R"(#version 330 core
pass vec4 Position;
// --vertex
layout (location = 0) in vec3 position;

void main()
{
    oPosition = vec4(position, 1.0);
}

// --geometry
layout (points) in;
layout (line_strip, max_vertices = 6) out;

uniform mat4 MVP;
uniform float length;

out vec3 fColor;

void main() {    
	fColor = vec3(1, 0, 0);
    gl_Position = MVP * (iPosition[0] + length * vec4(fColor, 0.0)); 
    EmitVertex();
	gl_Position = MVP * iPosition[0]; 
    EmitVertex();
	EndPrimitive();
	
	fColor = vec3(0, 1, 0);
    gl_Position = MVP * (iPosition[0] + length * vec4(fColor, 0.0)); 
    EmitVertex();
	gl_Position = MVP * iPosition[0];
    EmitVertex();
	EndPrimitive();
	
	fColor = vec3(0, 0, 1);
    gl_Position = MVP * (iPosition[0] + length * vec4(fColor, 0.0)); 
    EmitVertex();
	gl_Position = MVP * iPosition[0]; 
    EmitVertex();
	EndPrimitive();
}  

// --fragment
layout(location = 0) out vec4 FragColor;

in vec3 fColor;

void main()
{
    FragColor = vec4(fColor, 1.0);   
})";