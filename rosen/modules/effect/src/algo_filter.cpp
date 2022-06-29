#include "algo_filter.h"

AlgoFilter::AlgoFilter()
{
		myMesh_ = new Mesh();
		myMesh_->Use();
}
void AlgoFilter::Prepare(ProcessDate& data)
{
	glBindTexture(GL_TEXTURE_2D, data.outputTextureID_);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, data.textureWidth_, data.textureHeight_, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glBindFramebuffer(GL_FRAMEBUFFER, data.frameBufferID_);//°ó¶¨Ö¡»º³å  
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, data.outputTextureID_, 0);
	glViewport(0, 0, data.textureWidth_, data.textureHeight_);
	// make sure we clear the framebuffer's content
	glClearColor(1.0f, 0.0f, 0.0f, 0.0f);//¿ØÖÆ±³¾°ÑÕÉ«
	glClear(GL_COLOR_BUFFER_BIT);
}

void AlgoFilter::Draw(ProcessDate& data)
{
	Use();
	glBindVertexArray(myMesh_->mVAO_);
	glBindTexture(GL_TEXTURE_2D, data.srcTextureID_);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void AlgoFilter::CreateProgram(std::string vertexString, std::string fragmentString)
{
	mProgram_ = new Program();
	mProgram_->Compile(vertexString, fragmentString);
}


AlgoFilter::~AlgoFilter()
{
	delete mProgram_;
	delete myMesh_;
}

void AlgoFilter::Use()
{
	if (mProgram_ != nullptr) {
		mProgram_->UseProgram();
	}
}

filter_type AlgoFilter::GetFilterType()
{
	return filter_type::ALGOFILTER;
}

void AlgoFilter::SetValue(std::string key, void* value, int size)
{
}