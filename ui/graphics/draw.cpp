Shader materialShader("shaders/ui_material.vert", "shaders/ui_material.frag");
Material::ApplyPanelStyle(materialShader);
materialShader.bind();
drawQuad(); // your API call