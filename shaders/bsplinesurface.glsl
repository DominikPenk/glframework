#version 430 core
pass vec4 Position;
pass vec2 UV;

// --vertex
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;

void main() 
{
	oPosition = vec4(position, 1.0);
	oUV = uv;
}

// --tesscontrol
layout (vertices = 16) out;

void main() 
{
	oPosition[gl_InvocationID] = iPosition[gl_InvocationID];
	oUV[gl_InvocationID] = iUV[gl_InvocationID];

	gl_TessLevelOuter[0] = 32;
    gl_TessLevelOuter[1] = 32;
    gl_TessLevelOuter[2] = 32;
    gl_TessLevelOuter[3] = 32;
    gl_TessLevelInner[0] = 32;
    gl_TessLevelInner[1] = 32;
	
}

// --tesseval
layout(quads) in;

uniform mat4 MVP;
uniform vec4 plane;

out float diopter;

int idx(int j, int i) 
{
	return j * 4 + i;
}
void main() 
{	
	oUV = mix(
		mix(iUV[idx(0, 0)], iUV[idx(0, 1)], gl_TessCoord.x),
		mix(iUV[idx(1, 0)], iUV[idx(1, 1)], gl_TessCoord.x),
		gl_TessCoord.y);
		
	float u = gl_TessCoord.x;
	float v = gl_TessCoord.y;
	vec4 wu = vec4((1.0-u)*(1.0-u)*(1.0-u)/6.0, (3.0*u*u*u - 6.0*u*u + 4.0) / 6.0, -(3.0*u*u*u - 3.0*u*u - 3.0*u - 1.0) / 6.0, u*u*u/6.0);
	vec4 wv = vec4((1.0-v)*(1.0-v)*(1.0-v)/6.0, (3.0*v*v*v - 6.0*v*v + 4.0) / 6.0, -(3.0*v*v*v - 3.0*v*v - 3.0*v - 1.0) / 6.0, v*v*v/6.0);

	vec4 wdu = vec4(-0.5*(1.0-u)*(1.0-u), u*(1.5*u - 2), -1.5*u*u + u + 0.5, 0.5*u*u);
	vec4 wduu = vec4(1.0-u, 3.0*u - 2.0, 1.0 - 3.0*u, u);

	vec4 wdv = vec4(-0.5*(1.0-v)*(1.0-v), v*(1.5*v - 2), -1.5*v*v + v + 0.5, 0.5*v*v);
	vec4 wdvv = vec4(1.0-v, 3.0*v - 2.0, 1.0 - 3.0*v, v);

	float hu = 0;
	float hv = 0;
	float huu = 0;
	float huv = 0;
	float hvv = 0;
	vec4 p = vec4(0, 0, 0, 0);
	for(int j = 0; j < 4; ++j) {
		for(int i = 0; i < 4; ++i) {
			vec4 cp = iPosition[idx(j, i)];
			p += wu[i]*wv[j] * cp;
			hu += wdu[i]*wv[j]*cp.z;
			hv += wu[i]*wdv[j]*cp.z;
			huu += wduu[i]*wv[j]*cp.z;
			huv += wdu[i]*wdv[j]*cp.z;
			hvv += wu[i]*wdvv[j]*cp.z;
		}
	}

	// Direct mean curvature computation (see http://mathworld.wolfram.com/MeanCurvature.html) 
	float num = (1.0 + hv*hv)*huu
		- 2.0 * hu*hv*huv
		+ (1.0 + hu*hu)*hvv;
	float denom = 2.0 * pow((1.0 + hu*hu + hv*hv), 1.5);
	diopter = abs(0.523 * num / denom * 1000);
	
	gl_Position = MVP * p;
	oPosition = p;
}

// --fragment
uniform vec3 color;

uniform int displayStyle;
uniform int regionType;
uniform vec2 ab;
uniform vec2 center;
uniform float alpha; 

in float diopter;
out vec4 FragColor;

float linearstep(float e0, float e1, float u) {
	return clamp((u - e0) / (e1 - e0), 0, 1);
}

vec3 valueToRGB(float value, float min, float max) {
	float val = clamp((max - value) / (max - min), 0, 1);
	float h = 240.0*val;

	float r, g, b;

	float s = 0.9;
	float v = 0.8;

	int hi = int(floor(h / 60.0));
	float f = (h / 60.0) - hi;
	float p = v*(1 - s);
	float q = v*(1 - s*f);
	float t = v*(1 - s*(1 - f));


	if (hi == 0 || hi == 6) { r = v; g = t; b = p; }
	else if (hi == 1) { r = q; g = v; b = p; }
	else if (hi == 2) { r = p; g = v; b = t; }
	else if (hi == 3) { r = p; g = q; b = v; }
	else if (hi == 4) { r = t; g = p; b = v; }
	else { r = v; g = p; b = q; }
	return vec3(b, g, r);
}

bool inCircle(vec2 uv, vec2 center, float radius) {
	return dot(uv - center, uv-center) / (radius*radius) < 1.0;
}

bool inRectangle(vec2 uv, vec2 bl, vec2 tr) {
	return uv.x > bl.x && uv.x < tr.x && uv.y > bl.y && uv.y < tr.y;
}

bool inEllipse(vec2 uv, vec2 center, vec2 radii, float alpha) {
	float c = cos(alpha);
	float s = sin(alpha);
	float x = c * uv.x - s * uv.y;
	float y = s * uv.x + c * uv.y;
	return (x - center.x)*(x - center.x) / (radii.x*radii.x) + (y - center.y)*(y-center.y) / (radii.y*radii.y) < 1.0;
}

void main()
{
	if(
		(regionType == 1 && !inEllipse(iPosition.xy, center, ab, alpha))
		||
		(regionType == 2 && !inCircle(iPosition.xy, center, ab.x))
		||
		(regionType == 3 && !inRectangle(iPosition.xy, center, ab))
	) {	// Ellipse
		discard;
	}

	if(displayStyle == 0) {
		FragColor = vec4(color, 1);
	}
	else if(displayStyle == 1) {
		FragColor = vec4(iPosition.xyz, 1);
	}
	else if(displayStyle == 2) {
		FragColor = vec4(iUV, 0, 1);
	}
	else if(displayStyle ==3) {
		vec3 col = mix(color, vec3(1, 1, 1), linearstep(2.0, 6.0, diopter));
		FragColor = vec4(col, 1);
	} 
	else {
		FragColor = vec4(valueToRGB(diopter, 1.0, 5.0), 1);
	}
}