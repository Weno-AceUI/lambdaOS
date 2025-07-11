// Shader.cpp
#include "Shader.h"
#include <fstream>
#include <sstream>
#include <glad/glad.h> // or Vulkan equivalent

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath) {
    // 1. Load GLSL source from file
    // 2. Compile and link into m_RendererID
}

void Shader::bind() const { glUseProgram(m_RendererID); }
void Shader::unbind() const { glUseProgram(0); }

void Shader::setUniform(const std::string& name, float value) {
    glUniform1f(getUniformLocation(name), value);
}
// ... more overloads