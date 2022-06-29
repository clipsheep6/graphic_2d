#include "base_blur_filter.h"

void BaseBlurFilter::SetValue(std::string key, void* value, int size)
{
    if (key.compare("offsets") == 0) {
        if (size != -1) {
            float* temp = (float*)value;
            for (int i = 0; i < size; i++) {
                params_.offsets_[i] = *(temp + i);
            }
        }
    }
    if (key.compare("weights") == 0) {
        if (size != -1) {
            float* temp = (float*)value;
            for (int i = 0; i < size; i++) {
                params_.weights_[i] = *(temp + i);
            }
        }
    }
}

void BaseBlurFilter::CreateProgram(std::string vertexString, std::string fragmentString)
{
    mProgram_ = new Program();
    mProgram_->Compile(vertexString, fragmentString);
    mProgram_->UseProgram();

    weightsID_ = glGetUniformLocation(mProgram_->programID_, "weight");
    offsetID_ = glGetUniformLocation(mProgram_->programID_, "offset");
}

void BaseBlurFilter::LoadFilterParams()
{
    Use();
    glUniform1fv(weightsID_, RADIUS, params_.weights_);
    glUniform1fv(offsetID_, RADIUS, params_.offsets_);
}


void BaseBlurFilter::DoProcess(ProcessDate& data)
{
    Prepare(data);
    LoadFilterParams();
    Draw(data);
}
