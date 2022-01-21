#ifndef MESH_H
#define MESH_H

#include <glad/glad.h>
#include <iostream>

class Mesh {
public:
	Mesh();
	~Mesh();
	void Use();
	void SetVertices(float* newVertices, int size = 20);
	void SetIndices(unsigned int* newIndices, int size = 6);

	void Delete();
	unsigned int mVAO_;
private:
	unsigned int mVBO_;
	unsigned int mEBO_;
	int verticesSize_;
	int indicesSize_;
	int verticesNumber_;
	int indicesNumber_;
	float* mVertices_ = nullptr;
	unsigned int* mIndices_ = nullptr;
};
#endif

