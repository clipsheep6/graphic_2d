#include "vertical_blur_filter.h"
VerticalBlurFilter::VerticalBlurFilter()
{
    CreateProgram(GetVertexShader(), GetFragmentShader());
}

std::string VerticalBlurFilter::GetFragmentShader()
{
    return R"SHADER(
        #version 330 core
        const int RADIUS = 3;
        in vec2 texCoord;
        out vec4 fragColor;
        
        uniform sampler2D uTexture;
        uniform float weight[3];
        uniform float offset[3];

        void main()
        {
            vec2 tex_offset = 1.0 / textureSize(uTexture, 0); // gets size of single texel
            vec3 result = texture(uTexture, texCoord).rgb * weight[0]; // current fragment's contribution

            for(int i = 1; i < RADIUS; ++i)
            {
                result += texture(uTexture, texCoord + vec2(0.0, tex_offset.y) * offset[i]).rgb * weight[i];
                result += texture(uTexture, texCoord - vec2(0.0, tex_offset.y) * offset[i]).rgb * weight[i];
            }

            fragColor = vec4(result, 1.0);
        }
	)SHADER";
}

std::string VerticalBlurFilter::GetVertexShader()
{
    return R"SHADER(
        #version 330 core
        layout (location = 0) in vec3 vertexCoord;
        layout (location = 1) in vec2 inputTexCoord;
        out vec2 texCoord;

        void main()
        {
            gl_Position = vec4(vertexCoord, 1.0); 
            texCoord = inputTexCoord;
        }
    )SHADER";
}
