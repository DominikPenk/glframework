#version 330 core

// --vertex 
layout(location = 0) in vec3 position;
out vec4 vertexColor;


uniform mat4 MVP;
uniform vec4 color;

void main() {
	gl_Position = MVP * vec4(position, 1.0f);
	vertexColor = color;
}


// --fragment
in vec4 vertexColor;
out vec4 FragColor;

void main() {
	FragColor = vertexColor;
}