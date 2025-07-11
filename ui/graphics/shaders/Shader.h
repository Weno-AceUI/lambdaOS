// Shader.h
#pragma once
#include <string>
#include <glm/glm.hpp>

class Shader {
public:
    Shader(const std::string& vertexPath, const std::string& fragmentPath);
    void bind() const;
    void unbind() const;

    // Uniform setters
    void setUniform(const std::string& name, float value);
    void setUniform(const std::string& name, const glm::vec2& value);
    void setUniform(const std::string& name, const glm::vec4& value);
private:
    unsigned int m_RendererID;
    int getUniformLocation(const std::string& name) const;
};