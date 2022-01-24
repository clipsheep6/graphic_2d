/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "program.h"

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
        int buffSize = 1024;
        glGetShaderInfoLog(shader, buffSize, nullptr, infoLog);
    }
}

void Program::CheckProgramCompileErrors(GLuint program)
{
    GLint complete = 0;
    GLchar infoLog[1024];
    glGetProgramiv(program, GL_COMPILE_STATUS, &complete);
    if (!complete) {
        int buffSize = 1024;
        glGetProgramInfoLog(program, buffSize, nullptr, infoLog);
    }
}
