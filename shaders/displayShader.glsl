#version 430
// --vertex
out vec2 texCoord;
 
void main()
{
    float x = -1.0 + float((gl_VertexID & 1) << 2);
    float y = -1.0 + float((gl_VertexID & 2) << 1);
    texCoord.x = (x+1.0)*0.5;
    texCoord.y = (y+1.0)*0.5;
    gl_Position = vec4(x, y, 0, 1);
}

// --fragment
in vec2 texCoord;

uniform sampler2D renderTexture;
uniform float gamma;
bool hdr;

layout(location = 0) out vec4 FragColor;

#if HDR_MAPPING_TYPE == 4
float A = 0.15;
float B = 0.50;
float C = 0.10;
float D = 0.20;
float E = 0.02;
float F = 0.30;
float W = 11.2;

vec3 Uncharted2Tonemap(vec3 x)
{
   return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
}
#endif

// HDR Mapping like http://filmicworlds.com/blog/filmic-tonemapping-operators/
void main()
{
    vec3 texColor  = texture(renderTexture, texCoord).rgb; 
    // texColor *= 16;  // Hardcoded Exposure Adjustment
#if HDR_MAPPING_TYPE == 1   
    // Reinhard mapping
    vec3 color = texColor / (texColor + vec3(1.0));
    color = pow(color, vec3(1.0 / gamma));
#elif HDR_MAPPING_TYPE == 2
    // Haarm-Peter Duiker
    // Missing TGA
    // vec3 ld = 0.002;
    // float linReference = 0.18;
    // float logReference = 444;
    // float logGamma = 0.45;
    //    
    // vec3 LogColor;
    // LogColor.rgb = (log10(0.4*texColor.rgb/linReference)/ld*logGamma + logReference)/1023.f;
    // LogColor.rgb = saturate(LogColor.rgb);
    //    
    // float FilmLutWidth = 256;
    // float Padding = .5/FilmLutWidth;
    //    
    // //  apply response lookup and color grading for target display
    // float3 retColor;
    // retColor.r = tex2D(FilmLut, float2( lerp(Padding,1-Padding,LogColor.r), .5)).r;
    // retColor.g = tex2D(FilmLut, float2( lerp(Padding,1-Padding,LogColor.g), .5)).r;
    // retColor.b = tex2D(FilmLut, float2( lerp(Padding,1-Padding,LogColor.b), .5)).r;
    vec3 color = texColor;
    color = pow(color, vec3(1.0 / gamma));
#elif HDR_MAPPING_TYPE == 3
    // Jim Hejl and Richard Burgess-Dawson
    vec3 x = max(vec3(0.),texColor-0.004);
    vec3 color = (x*(6.2*x+.5))/(x*(6.2*x+1.7)+0.06);
#elif HDR_MAPPING_TYPE == 4
    // Unchartered 2
    float ExposureBias = 2.0f;
    vec3 curr = Uncharted2Tonemap(ExposureBias*texColor);

    vec3 whiteScale = vec3(1.0f/Uncharted2Tonemap(vec3(W)));
    vec3 color = curr*whiteScale;
    color = pow(color, vec3(1.0 / gamma));
#else
    vec3 color = texColor;
    color = pow(color, vec3(1.0 / gamma));
#endif    
    FragColor = vec4(color, 1.0);
}