#ifndef FILTER_H
#define FILTER_H

#include "program.h"

#include <string>
#include <vector>
#include <iostream>
#include <glad/glad.h>
#include <fstream>
#include <sstream>

enum class filter_type { INPUT, ALGOFILTER, MEGERFILTER, OUTPUT };

struct ProcessDate
{
	GLuint srcTextureID_;
	GLuint outputTextureID_;
	GLuint frameBufferID_;
	int textureWidth_;
	int textureHeight_;
};

class Filter {
public:
	virtual filter_type GetFilterType() = 0;

	virtual void Process(ProcessDate& data);

	virtual void DoProcess(ProcessDate& data) = 0;

	virtual void AddNextFilter(Filter* next);

	virtual void AddPreviousFilter(Filter* previous);

	virtual Filter* GetNextFilter(void);

	virtual Filter* GetPreviousFilter(void);

	virtual int GetInputNumber();

	virtual int GetOutputNumber();

	virtual int GetMaxInputNumber();

	virtual int GetMaxOutputNumber();
protected:
	int nextNum_ = 0;
	int preNum_ = 0;
	int nextPtrMax_ = 1;
	int prePtrMax_ = 1;
	Filter* previous_ = nullptr;
	Filter* next_ = nullptr;
};
#endif
