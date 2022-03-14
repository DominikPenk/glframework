#pragma once

static const char* CATMULL_ROM_SPLINE_SHADER = R"(#version 430 core
pass vec4 ControlPoint;

// --vertex
layout (location = 0) in vec3 cp;

uniform mat4 MV;

void main() {
    oControlPoint = MV * vec4(cp, 1.0);
}

// --tesscontrol
layout (vertices = 4) out;

uniform int subdivisions;

void main() {
    oControlPoint[gl_InvocationID] = iControlPoint[gl_InvocationID];

    gl_TessLevelOuter[0] = 1;
    gl_TessLevelOuter[1] = subdivisions;
}

// --tesseval
layout(isolines, equal_spacing) in;

uniform int nPoints;
// 0: Natural, 1: Periodic, 2: Clamped, 3: knot-a-knot
uniform int endpointCondition;
uniform vec3 q0;
uniform vec3 qk;
uniform mat4 MV;

out vec4 normal;

void main() {
    float t0 = 0;
    float t1 = t0 + length(iControlPoint[0] - iControlPoint[1]);
    float t2 = t1 + length(iControlPoint[1] - iControlPoint[2]);
    float t3 = t2 + length(iControlPoint[2] - iControlPoint[3]);

    vec4 d0 = iControlPoint[1];
    vec4 d1 = iControlPoint[1] + (t2 - t1) / (3.0 * (t2 - t0)) * (iControlPoint[2] - iControlPoint[0]);
    vec4 d2 = iControlPoint[2] - (t2 - t1) / (3.0 * (t3 - t1)) * (iControlPoint[3] - iControlPoint[1]);
    vec4 d3 = iControlPoint[2];


    if(gl_PrimitiveID == 0 && endpointCondition != 1) { // Not periodic
        if(endpointCondition == 0)                      // Natural
            d1 = 0.5 * (d0 + d2);
        else if(endpointCondition == 2)                 // Clamped
            d1 = d0 + (t2 - t1) / 3.0 * MV * vec4(q0, 0);
        else                                            // knot-a-knot
            d1 = d2 - (d3 - d0) / 3.0;
    }
    else if(gl_PrimitiveID == nPoints - 2 && endpointCondition != 1) {
        if(endpointCondition == 0)
            d2 = 0.5 * (d1 + d3);
        else if(endpointCondition == 2)
            d2 = d3 + (t2 - t1) / 3.0 * MV * vec4(qk, 0);
        else 
            d2 = d1 + (d3 - d0) / 3.0;
    }

    float u = gl_TessCoord.x;
    float b0 = (1.-u) * (1.-u) * (1.-u);
    float b1 = 3. * u * (1.-u) * (1.-u);
    float b2 = 3. * u * u * (1.-u);
    float b3 = u * u * u;

    float b0u = -3. * (1. - u) * (1. - u);
    float b1u = 3. * (u - 1.0) * (3 * u - 1.);
    float b2u = (6.0 - 9.0 * u) * u;
    float b3u = 3. * u * u;

    gl_Position = b0*d0 + b1*d1 + b2*d2 + b3*d3;
    
    normal = b0u*d0 + b1u*d1 + b2u*d2 + b3u*d3;
    normal = normalize(vec4(-normal.y, normal.x, 0, 0));
}

// --geometry
layout(lines) in;
layout(triangle_strip, max_vertices = 4) out;

uniform float width;
uniform mat4 P;

in vec4 normal[];
out float v;

void main() {
    vec4 p0 = gl_in[0].gl_Position;
    vec4 p1 = gl_in[1].gl_Position;
    gl_Position = P * (p0 + width * normal[0] * p0.z);
	v = 1.;
    EmitVertex();
    gl_Position = P * (p0 - width * normal[0] * p0.z);
    v = -1.;
	EmitVertex();
    gl_Position = P * (p1 + width * normal[1] * p0.z);
    v = 1.;
	EmitVertex();
	gl_Position = P * (p1 - width * normal[1] * p0.z);
    v = -1.;
	EmitVertex();
    EndPrimitive();
}


// --fragment
uniform vec4 color;

in float v;

out vec4 FragColor;

void main() {
	// Antia aliasing
	float dist = (1- abs(v));
	float d = fwidth(dist);
    FragColor = vec4(color.xyz, smoothstep(0, d, dist));
})";