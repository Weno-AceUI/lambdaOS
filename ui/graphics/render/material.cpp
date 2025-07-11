// Material.cpp
void Material::ApplyPanelStyle(Shader& shader) {
    shader.setUniform("u_Color", glm::vec4(0.1, 0.1, 0.1, 1.0));
    shader.setUniform("u_ShadowOffset", glm::vec2(0.01f, 0.01f));
    shader.setUniform("u_ShadowColor", glm::vec4(0.0, 0.0, 0.0, 0.25));
    shader.setUniform("u_ShadowBlur", 0.02f);
    shader.setUniform("u_ShadowSpread", 0.015f);
}