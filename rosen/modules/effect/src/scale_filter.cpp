#include "scale_filter.h"

ScaleFilter::ScaleFilter()
{
    CreateProgram(GetVertexShader(), GetFragmentShader());
}

std::string ScaleFilter::GetVertexShader()
{
    return R"SHADER(
        #version 330 core
        layout (location = 0) in vec3 vertexCoord;
        layout (location = 1) in vec2 inputTexCoord;
        out vec2 texCoord;

        void main()
        {
            gl_Position = vec4(vertexCoord, 1.0); 
            texCoord = vec2(inputTexCoord.x, inputTexCoord.y);
        }
    )SHADER";
}

std::string ScaleFilter::GetFragmentShader()
{
	return R"SHADER(
        #version 330 core
        in vec2 texCoord;
        out vec4 fragColor;
        uniform sampler2D uTexture;
        void main()
        {
            fragColor = texture(uTexture, texCoord)* 1.0;
        }
	)SHADER";
}
//
void ScaleFilter::SetScale(float scale)
{
    scale_ = scale;
}
//
void ScaleFilter::Prepare(ProcessDate& data)
{
    data.textureHeight_ = floorf(scale_ * data.textureHeight_);
    data.textureWidth_ = floorf(scale_ * data.textureWidth_);
    glBindTexture(GL_TEXTURE_2D, data.outputTextureID_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, data.textureWidth_, data.textureHeight_, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glBindFramebuffer(GL_FRAMEBUFFER, data.frameBufferID_);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, data.outputTextureID_, 0);
    glViewport(0, 0, data.textureWidth_, data.textureHeight_);
    glClearColor(1.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void ScaleFilter::DoProcess(ProcessDate& data)
{
    Prepare(data);
    Draw(data);
}

