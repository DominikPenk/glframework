#version 430

// --vertex
layout(location = 0) in vec4 vPosition;
layout(location = 1) in vec2 vUV;

uniform vec2  center;
uniform vec2  canvasSize;
uniform vec2  size;
uniform float z;
uniform float handleSize;

out vec2 uv;

void main() 
{
    vec2 clipDelta = 2.0f * (3 * handleSize * vPosition.xy + size * vPosition.zw) / canvasSize;
    vec2 clipPos   = -1.0f + 2.0f * center / canvasSize;
    clipPos        = clipPos + clipDelta; 
    gl_Position = vec4(clipPos, z, 1);
    uv = vUV;
}

// --fragment
in vec2 uv;

uniform vec4 color;
uniform vec2 canvasSize;

layout(location = 0) out vec4 FragColor;

void main() 
{
    FragColor = vec4(color.rgb, uv.x);
}
