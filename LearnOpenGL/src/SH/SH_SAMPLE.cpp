#include "SH_SAMPLE.h"
#include "SH.h"
#include <iostream>

const float PI = 3.14159265359;

bool GenerateSHSampes(int sqrtNumSamples, int numBands, SH_SAMPLE* samples)
{
	int numSamples = sqrtNumSamples * sqrtNumSamples;
	int numFunctions = numBands * numBands;

	for (int i = 0; i != numSamples; i++)
	{
		samples[i].shValues = new float[numFunctions];
		if (!samples[i].shValues)
		{
			std::cout << "SH Values of Samples Are Failed to Allocate!" << std::endl;
			return false;
		}
	}

	int index = 0;
	for (int i = 0; i != sqrtNumSamples; i++)
	{
		for (int j = 0; j != sqrtNumSamples; j++)
		{
			float x = (i + (float)rand() / RAND_MAX) / sqrtNumSamples;
			float y = (j + (float)rand() / RAND_MAX) / sqrtNumSamples;

			float theta = 2.0 * acos(sqrt(1.0 - x));
			float phi = 2.0 * PI * y;
			
			samples[index].theta = theta;
			samples[index].phi = phi;
			samples[index].direction = glm::vec3(cos(phi) * sin(theta), cos(theta), sin(phi) * sin(theta));

			for (int l = 0; l < numBands; l++)
			{
				for (int m = -l; m <= l; m++)
				{
					int id = l * (l + 1) + m;
					samples[index].shValues[id] = SH(l, m, theta, phi);
				}
			}

			index++;
		}
	}

	return true;
}

void GenerateSHSample(int numBands, glm::vec3 direction, SH_SAMPLE& sample)
{
	int numFunctions = numBands * numBands;

	sample.direction = direction;
	sample.theta = acos(direction.y);
	sample.phi = atan2(direction.z , direction.x)+PI;
	sample.shValues = new float[numFunctions];
	if (!sample.shValues)
	{
		std::cout << "SH Values of Samples Are Failed to Allocate!" << std::endl;
		return;
	}

	for (int l = 0; l < numBands; l++)
	{
		for (int m = -l; m <= l; m++)
		{
			int id = l * (l + 1) + m;
			sample.shValues[id] = SH(l, m, sample.theta, sample.phi);
		}
	}
	return;
}
