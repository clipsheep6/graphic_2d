#ifndef PROGRAM_H
#define PROGRAM_H

#include <string>
#include <glad/glad.h>

class Program
{
public:
	void Compile(std::string vertexShader, std::string fragmentShader);
	void UseProgram() const;
	unsigned int programID_;
private:
	bool initFlag_ = false;
	unsigned int vertexID_;
	unsigned int fragmentID_;
	GLuint CreateShader(GLuint type, const std::string& shaderCode);
	void CheckShaderCompileErrors(GLuint shader);
	void CheckProgramCompileErrors(GLuint program);
};

#endif

