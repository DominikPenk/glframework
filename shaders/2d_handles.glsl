#version 430

// --vertex
layout(location = 0) in vec2 vPosition;
layout(location = 1) in vec4 vColor;

uniform vec2 canvasSize;
uniform float z;

out vec4 color;

void main()
{
    vec2 screenPos = vPosition;
    vec2 clipPos   = -1.0f + 2.0f * screenPos / canvasSize;

    gl_Position = vec4(clipPos, z, 1.0f);
    color = vColor;
}

// --fragment
in vec4 color;
layout(location = 0) out vec4 FragColor;
uniform vec4 hue;

void main() 
{
    FragColor = hue * color;
}