#ifndef ALGO_FILTER_H
#define ALGO_FILTER_H

#include "filter.h"
#include "mesh.h"
#include "cJSON.h"
class AlgoFilter :public Filter
{
public:
	AlgoFilter();
	virtual ~AlgoFilter();

	virtual filter_type GetFilterType()override;

	virtual void SetValue(std::string key, void* value, int size = -1);
protected:
	void Use();
	virtual std::string GetVertexShader() = 0;
	virtual std::string GetFragmentShader() = 0;
	virtual void Prepare(ProcessDate& data);
	virtual void Draw(ProcessDate& data);
	virtual void CreateProgram(std::string vertexString, std::string fragmentString);
	Program* mProgram_ = nullptr;
	Mesh* myMesh_ = nullptr;
private:
	friend class ImageChain;
};

#endif // !ALGO_FILTER_H



