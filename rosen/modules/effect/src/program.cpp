#include "program.h"
#include <iostream>

using namespace std;

void Program::Compile(string vertexShader, string fragmentShader)
{
	vertexID_ = CreateShader(GL_VERTEX_SHADER, vertexShader);
	fragmentID_ = CreateShader(GL_FRAGMENT_SHADER, fragmentShader);
	if (!initFlag_) {
		initFlag_ = true;
		programID_ = glCreateProgram();
	}
	glAttachShader(programID_, vertexID_);
	glAttachShader(programID_, fragmentID_);
	glLinkProgram(programID_);
	CheckProgramCompileErrors(programID_);
	glDeleteShader(vertexID_);
	glDeleteShader(fragmentID_);
}

void Program::UseProgram() const
{
	glUseProgram(programID_);
}

GLuint Program::CreateShader(GLuint type, const string& shaderCode)
{
	const GLchar* charShaderCode = (const GLchar*)shaderCode.c_str();
	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, &charShaderCode, nullptr);
	glCompileShader(shader);
	CheckShaderCompileErrors(shader);
	return shader;
}

void Program::CheckShaderCompileErrors(GLuint shader)
{
	GLint complete = 0;
	GLchar infoLog[1024];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &complete);
	if (!complete) {
		glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
		//print infoLog
		cout << "shader: " << infoLog << endl;
	}
}

void Program::CheckProgramCompileErrors(GLuint program)
{
	GLint complete = 0;
	GLchar infoLog[1024];
	glGetProgramiv(program, GL_COMPILE_STATUS, &complete);
	if (!complete) {
		glGetProgramInfoLog(program, 1024, nullptr, infoLog);
		//print infoLog
		cout << "program: " << infoLog << endl;
	}
}
