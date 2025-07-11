#version 450

layout(location = 0) in vec2 v_TexCoord;
layout(location = 0) out vec4 fragColor;

layout(binding = 0) uniform sampler2D u_BlurTex;

layout(push_constant) uniform PushConstants {
    vec4 u_Color;       // Opaque fill color
    float u_Alpha;      // Base color blend with blur
    float u_UseBlur;    // Enable background blur
    vec2 u_ShadowOffset; // (X, Y) offset direction
    float u_ShadowBlur;  // Blur softness
    float u_ShadowSpread; // Thickness of shadow area
    vec4 u_ShadowColor;  // RGBA shadow color
} pc;

void main() {
    vec2 uv = v_TexCoord;
    vec4 blurred = texture(u_BlurTex, uv);
vec4 base = mix(blurred, pc.u_Color, pc.u_Alpha);
fragColor = base + innerShadow;

    // ----- INNER SHADOW -----
    // Convert UV into distance from edges
vec2 edgeDist = min(uv, 1.0 - uv); // distance to left/top and right/bottom
float innerEdge = min(edgeDist.x, edgeDist.y);

// Move edge inward by spread
float shiftedEdge = innerEdge - pc.u_ShadowSpread;

// Compute shadow based on blur
float shadowMask = 1.0 - smoothstep(0.0, pc.u_ShadowBlur, shiftedEdge);

// Fade shadow based on offset direction
vec2 offsetUV = uv + pc.u_ShadowOffset;
float offsetEdge = min(min(offsetUV.x, 1.0 - offsetUV.x), min(offsetUV.y, 1.0 - offsetUV.y));
float offsetMask = 1.0 - smoothstep(0.0, pc.u_ShadowBlur, offsetEdge);

// Combine masks
float combinedShadow = shadowMask * offsetMask;

// Apply shadow color
vec4 innerShadow = pc.u_ShadowColor * combinedShadow;

    // ----- BACKGROUND BLUR -----
    vec4 blurred = texture(u_BlurTex, uv);

    // ----- BASE COLOR COMPOSITE -----
    vec4 base = mix(blurred, pc.u_Color, pc.u_Alpha);

    // ----- FINAL OUTPUT -----
    fragColor = base + innerShadow;
}