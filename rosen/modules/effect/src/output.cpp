#include "output.h"

std::string Output::GetVertexShader()
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

std::string Output::GetFragmentShader()
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

filter_type Output::GetFilterType()
{
    return filter_type::OUTPUT;
}

void Output::SaveRenderResultToImage(std::string name, int width, int height)
{
	saveResult_ = true;
	imageName_ = name;
    imageWidth_ = width;
    imageHeight_ = height;
}

void Output::RenderOnScreen(GLuint& RenderTextureID)
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);//¿ØÖÆ±³¾°ÑÕÉ«
	glClear(GL_COLOR_BUFFER_BIT);
	Use();
	glBindVertexArray(myMesh_->mVAO_);
	glBindTexture(GL_TEXTURE_2D, RenderTextureID);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

Output::Output()
{
    CreateProgram(GetVertexShader(), GetFragmentShader());
}
