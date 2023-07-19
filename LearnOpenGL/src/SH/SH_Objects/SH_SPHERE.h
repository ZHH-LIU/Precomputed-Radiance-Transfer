#ifndef SH_SPHERE_H
#define SH_SPHERE_H

#include "../SH_Object.h"
#include "../../RAY/hittables/sphere.h"
using std::shared_ptr;
using std::make_shared;

class SH_SPHERE :public SH_Object
{
public:
	SH_SPHERE(glm::vec3 cen, float r, shared_ptr<texture> m, float _roughness, float _metalness, unsigned int X_SEGMENTS, unsigned int Y_SEGMENTS)
	{
		roughness = _roughness;
		metalness = _metalness;
		Object = make_shared<sphere>(cen, r, m);
		GetSHVertices(cen, r, m, X_SEGMENTS, Y_SEGMENTS);
	}

	void GetSHVertices(glm::vec3 cen, float r, shared_ptr<texture> m, unsigned int X_SEGMENTS, unsigned int Y_SEGMENTS)
	{
		float PI = 3.1415926f;

		for (unsigned int y = 0; y <= Y_SEGMENTS; y++)
		{
			for (unsigned int x = 0; x <= X_SEGMENTS; x++)
			{
				float xSegment = (float)x / (float)X_SEGMENTS;
				float ySegment = (float)y / (float)Y_SEGMENTS;
				float xPos = cos(xSegment * 2.0f * PI) * sin(ySegment * PI);
				float yPos = cos(ySegment * PI);
				float zPos = sin(xSegment * 2.0f * PI) * sin(ySegment * PI);

				SH_VERTEX newVertex;
				newVertex.position = glm::vec3(xPos * r + cen.x, yPos * r + cen.y, zPos * r + cen.z);
				newVertex.normal = glm::vec3(xPos, yPos, zPos);
				newVertex.texcoord = glm::vec2(xSegment, ySegment);
				newVertex.diffuse = Object->color(newVertex.texcoord);
				Vertices.push_back(newVertex);
			}
		}

		for (unsigned int i = 0; i < Y_SEGMENTS; i++)
		{
			for (unsigned int j = 0; j < X_SEGMENTS; j++)
			{
				Indices.push_back(i * (X_SEGMENTS + 1) + j);
				Indices.push_back((i + 1) * (X_SEGMENTS + 1) + j + 1);
				Indices.push_back((i + 1) * (X_SEGMENTS + 1) + j);
				
				Indices.push_back(i * (X_SEGMENTS + 1) + j);
				Indices.push_back(i * (X_SEGMENTS + 1) + j + 1);
				Indices.push_back((i + 1) * (X_SEGMENTS + 1) + j + 1);
			}
		}
	}

};

void GetCosineCoeffs(int numSamples, int numBands, SH_SAMPLE* samples, float* out)
{
	int numFunctions = numBands * numBands;
	const float PI = 3.14159265359;

	

	for (int j = 0; j != numFunctions; j++)
	{
		out[j] = 0.0f;

		for (int k = 0; k != numSamples; ++k)
		{
			float item = glm::max(cos(samples[k].theta),0.0f) * samples[k].shValues[j];
			out[j] += item;
		}
		out[j] *= 4.0 * PI / numSamples;
	}
	return;
}

#endif