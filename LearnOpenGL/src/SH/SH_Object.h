#ifndef SH_OBJECT_H
#define SH_OBJECT_H

#include <memory>
#include <vector>
#include "../RAY/hittable.h"
#include "SH_VERTEX.h"
#include <glad/glad.h> 
#include <GLFW/glfw3.h>

class SH_Object
{
public:
	SH_Object()
	{
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);
		glGenVertexArrays(1, &VAO);
	}
	~SH_Object()
	{
		if (G_r)
		{
			delete[]G_r;
		}
		G_r = nullptr;

		if (G_g)
		{
			delete[]G_g;
		}
		G_g = nullptr;

		if (G_b)
		{
			delete[]G_b;
		}
		G_b = nullptr;
	}
	std::vector<SH_VERTEX> Vertices;
	std::vector<unsigned int> Indices;
	std::shared_ptr<hittable> Object;
	unsigned int VAO, VBO, EBO;
	glm::mat4 model = glm::mat4(1.0);
	float roughness;
	float metalness;
	float* G_r;
	float* G_g;
	float* G_b;
	virtual glm::vec3 getColor(int u, int v);
};

#endif
