#ifndef SH_SAMPLE_H
#define SH_SAMPLE_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class SH_SAMPLE
{
public:
	float phi, theta;
	glm::vec3 direction;
	float* shValues;

	SH_SAMPLE() :shValues(nullptr) {};
	~SH_SAMPLE()
	{
		if (shValues)
			delete[]shValues;
		shValues = nullptr;
	}
};

bool GenerateSHSampes(int sqrtNumSamples, int numBands, SH_SAMPLE* samples);
void GenerateSHSample(int numBands, glm::vec3 direction, SH_SAMPLE& sample);

#endif
