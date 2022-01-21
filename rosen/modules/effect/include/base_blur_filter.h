#ifndef BASE_BLUR_FILTER_H
#define BASE_BLUR_FILTER_H

#include "algo_filter.h"

struct GaussianBlurFilterParams {
	float weights_[3];
	float offsets_[3];
};

class BaseBlurFilter :
	public AlgoFilter
{
public:
	void SetValue(std::string key, void* value, int size = -1) override;
protected:
	virtual void CreateProgram(std::string vertexString, std::string fragmentString) override;
	GaussianBlurFilterParams params_;
	void LoadFilterParams();
	void DoProcess(ProcessDate& data);
	GLuint weightsID_;
	GLuint offsetID_;
	const int RADIUS = 3;
	friend class GaussianBlurFilter;
};

#endif



