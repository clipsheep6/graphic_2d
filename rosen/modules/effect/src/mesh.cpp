#include "mesh.h"
Mesh::Mesh() {
	float vertices[] = {
	 1.0f,  1.0f, 0.0f,     1.0f, 1.0f, // top right
	 1.0f, -1.0f, 0.0f,     1.0f, 0.0f, // bottom right
	-1.0f, -1.0f, 0.0f,     0.0f, 0.0f, // bottom left
	-1.0f,  1.0f, 0.0f,     0.0f, 1.0f  // top left 
	};
	unsigned int indices[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};
	verticesSize_ = sizeof(vertices);
	indicesSize_ = sizeof(indices);
	verticesNumber_ = sizeof(vertices) / sizeof(float);
	indicesNumber_ = sizeof(indices) / sizeof(unsigned int);

	mVertices_ = new float[verticesNumber_];
	mIndices_ = new unsigned int[indicesNumber_];
	for (int i = 0; i < verticesNumber_; i++) {
		mVertices_[i] = vertices[i];
	}
	for (int i = 0; i < indicesNumber_; i++) {
		mIndices_[i] = indices[i];
	}
	glGenVertexArrays(1, &mVAO_);
	glGenBuffers(1, &mVBO_);
	glGenBuffers(1, &mEBO_);
}

Mesh::~Mesh()
{
	delete[] mVertices_;
	delete[] mIndices_;
}


void Mesh::Use() {
	glBindVertexArray(mVAO_);

	glBindBuffer(GL_ARRAY_BUFFER, mVBO_);
	glBufferData(GL_ARRAY_BUFFER, verticesSize_, mVertices_, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO_);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize_, mIndices_, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
}

void Mesh::SetVertices(float* newVertices, int size) {
	if (newVertices) {
		verticesSize_ = size * sizeof(float);
		if (mVertices_) {
			delete[] mVertices_;
			mVertices_ = nullptr;
		}
		mVertices_ = new float[size];
		for (int i = 0; i < size; i++) {
			mVertices_[i] = newVertices[i];
		}
	}
	Use();
}
void Mesh::SetIndices(unsigned int* newIndices, int size) {
	if (newIndices) {
		indicesSize_ = size * sizeof(unsigned int);
		if (mIndices_) {
			delete[] mIndices_;
			mIndices_ = nullptr;
		}
		mIndices_ = new unsigned int[size];
		for (int i = 0; i < size; i++) {
			mIndices_[i] = newIndices[i];
		}
		Use();
	}
}

void Mesh::Delete()
{
	glDeleteVertexArrays(1, &mVAO_);
	glDeleteBuffers(1, &mVBO_);
	glDeleteBuffers(1, &mEBO_);
}
