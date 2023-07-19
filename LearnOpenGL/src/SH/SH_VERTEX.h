#ifndef SH_VERTEX_H
#define SH_VERTEX_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class SH_VERTEX
{
public:
	glm::vec3 position;
	glm::vec3 color;
	glm::vec3 glossy;
	glm::vec3 normal;
	glm::vec3 diffuse;
	glm::vec2 texcoord;
	float* shCoeffs_r=nullptr;
	float* shCoeffs_g = nullptr;
	float* shCoeffs_b = nullptr;

	float* shCoeffs_temp_r = nullptr;
	float* shCoeffs_temp_g = nullptr;
	float* shCoeffs_temp_b = nullptr;

	float* shGlossyCoeffs_r = nullptr;
	float* shGlossyCoeffs_g = nullptr;
	float* shGlossyCoeffs_b = nullptr;

	float* shGlossyCoeffs_temp_r = nullptr;
	float* shGlossyCoeffs_temp_g = nullptr;
	float* shGlossyCoeffs_temp_b = nullptr;

	SH_VERTEX() 
		:shCoeffs_r(nullptr), shCoeffs_g(nullptr), shCoeffs_b(nullptr), shCoeffs_temp_r(nullptr), shCoeffs_temp_g(nullptr), shCoeffs_temp_b(nullptr) {};
	~SH_VERTEX()
	{
		if (shCoeffs_r)
		{
			delete[]shCoeffs_r;
		}
		shCoeffs_r = nullptr;

		if (shCoeffs_g)
		{
			delete[]shCoeffs_g;
		}
		shCoeffs_g = nullptr;

		if (shCoeffs_b)
		{
			delete[]shCoeffs_b;
		}
		shCoeffs_b = nullptr;

		if (shCoeffs_temp_r)
		{
			delete[]shCoeffs_temp_r;
		}
		shCoeffs_temp_r = nullptr;

		if (shCoeffs_temp_g)
		{
			delete[]shCoeffs_temp_g;
		}
		shCoeffs_temp_g = nullptr;

		if (shCoeffs_temp_b)
		{
			delete[]shCoeffs_temp_b;
		}
		shCoeffs_temp_b = nullptr;

		if (shGlossyCoeffs_r)
		{
			delete[]shGlossyCoeffs_r;
		}
		shGlossyCoeffs_r = nullptr;

		if (shGlossyCoeffs_g)
		{
			delete[]shGlossyCoeffs_g;
		}
		shGlossyCoeffs_g = nullptr;

		if (shGlossyCoeffs_b)
		{
			delete[]shGlossyCoeffs_b;
		}
		shGlossyCoeffs_b = nullptr;

		if (shGlossyCoeffs_temp_r)
		{
			delete[]shGlossyCoeffs_temp_r;
		}
		shGlossyCoeffs_temp_r = nullptr;

		if (shGlossyCoeffs_temp_g)
		{
			delete[]shGlossyCoeffs_temp_g;
		}
		shGlossyCoeffs_temp_g = nullptr;

		if (shGlossyCoeffs_temp_b)
		{
			delete[]shGlossyCoeffs_temp_b;
		}
		shGlossyCoeffs_temp_b = nullptr;
	}
};

#endif
