#ifndef SHADER_H
#define SHADER_H

#include <string>

class Shader {
  public:
    unsigned int ID;
    // constructor generates the shader on the fly
    // ------------------------------------------------------------------------
    Shader(const std::string vertexPath, const std::string fragmentPath);
    // activate the shader
    // ------------------------------------------------------------------------
    void use();
    // utility uniform functions
    // ------------------------------------------------------------------------
    void setInt(const std::string &name, int value) const;

  private:
    // utility function for checking shader compilation/linking errors.
    // ------------------------------------------------------------------------
    void checkCompileErrors(unsigned int shader, std::string type);
};
#endif
