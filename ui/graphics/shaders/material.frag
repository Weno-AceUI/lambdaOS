#version 450

layout(location = 0) in vec2 v_TexCoord;
layout(location = 0) out vec4 fragColor;

layout(binding = 0) uniform sampler2D u_BlurTex;

layout(push_constant) uniform PushConstants {
    vec4 u_Color;       // Base opaque color
    float u_Alpha;      // Blend alpha between blur and base color
    float u_UseBlur;    // Toggle blur effect
    float u_ShadowSize; // Controls inner shadow depth
    float u_Radius;     // (Optional for future rounded corner support)
} pc;

void main() {
    vec2 uv = v_TexCoord;

    // ----- INNER SHADOW -----
    float edgeDist = min(min(uv.x, 1.0 - uv.x), min(uv.y, 1.0 - uv.y));
    float shadowIntensity = smoothstep(0.0, pc.u_ShadowSize, edgeDist);
    vec4 innerShadow = vec4(0.0, 0.0, 0.0, (1.0 - shadowIntensity) * 0.2);

    // ----- BACKGROUND BLUR -----
    vec4 blurred = texture(u_BlurTex, uv);

    // ----- BASE COLOR COMPOSITE -----
    vec4 base = mix(blurred, pc.u_Color, pc.u_Alpha);

    // ----- FINAL OUTPUT -----
    fragColor = base + innerShadow;
}