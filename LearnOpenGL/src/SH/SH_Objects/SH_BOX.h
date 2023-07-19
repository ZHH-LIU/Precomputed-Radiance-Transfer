#ifndef SH_BOX_H
#define SH_BOX_H

#include "../SH_Object.h"
#include "../../RAY/hittables/box.h"
using std::shared_ptr;
using std::make_shared;

class SH_BOX :public SH_Object
{
public:
	SH_BOX(glm::vec3 _boxmin, glm::vec3 _boxmax, shared_ptr<texture>ptr, float _roughness, float _metalness, unsigned int x_steps, unsigned int y_steps, unsigned int z_steps)
	{
		roughness = _roughness;
		metalness = _metalness;
		Object = make_shared<box>(_boxmin, _boxmax, ptr);
		GetSHVertices(_boxmin, _boxmax, x_steps, y_steps, z_steps);
	}

	void GetSHVertices(glm::vec3 boxmin, glm::vec3 boxmax, unsigned int x_steps, unsigned int y_steps, unsigned int z_steps)
	{
		//Z
		float height = boxmax.y - boxmin.y;
		float width = boxmax.x - boxmin.x;

		for (int i = 0; i <= y_steps; i++)
		{
			for (int j = 0; j <= x_steps; j++)
			{
				SH_VERTEX newVertex;
				newVertex.position = glm::vec3(boxmin.x + (float)j / x_steps * width, boxmin.y + (float)i / y_steps * height, boxmax.z);
				newVertex.normal = glm::vec3(0.0, 0.0, 1.0);
				newVertex.texcoord = glm::vec2((float)j / x_steps, (float)i / y_steps);
				newVertex.diffuse = Object->color(newVertex.texcoord);

				Vertices.push_back(newVertex);
			}
		}

		for (int i = 0; i != y_steps; i++)
		{
			for (int j = 0; j != x_steps; j++)
			{
				Indices.push_back(i * (x_steps +1) + j);
				Indices.push_back((i + 1) * (x_steps + 1) + j);
				Indices.push_back((i + 1) * (x_steps + 1) + j + 1);

				Indices.push_back(i * (x_steps + 1) + j);
				Indices.push_back((i + 1) * (x_steps + 1) + j + 1);
				Indices.push_back(i * (x_steps + 1) + j + 1);
			}
		}

		//Z-
		int base = Vertices.size();

		height = boxmax.y - boxmin.y;
		width = boxmax.x - boxmin.x;

		for (int i = 0; i <= y_steps; i++)
		{
			for (int j = 0; j <= x_steps; j++)
			{
				SH_VERTEX newVertex;
				newVertex.position = glm::vec3(boxmin.x + (float)j / x_steps * width, boxmin.y + (float)i / y_steps * height, boxmin.z);
				newVertex.normal = glm::vec3(0.0, 0.0, -1.0);
				newVertex.texcoord = glm::vec2((float)j / x_steps, (float)i / y_steps);
				newVertex.diffuse = Object->color(newVertex.texcoord);

				Vertices.push_back(newVertex);
			}
		}

		for (int i = 0; i != y_steps; i++)
		{
			for (int j = 0; j != x_steps; j++)
			{
				Indices.push_back(base + i * (x_steps +1) + j);
				Indices.push_back(base + (i + 1) * (x_steps +1) + j);
				Indices.push_back(base + (i + 1) * (x_steps +1) + j + 1);

				Indices.push_back(base + i * (x_steps +1) + j);
				Indices.push_back(base + (i + 1) * (x_steps +1) + j + 1);
				Indices.push_back(base + i * (x_steps +1) + j + 1);
			}
		}

		//X
		base = Vertices.size();

		height = boxmax.y - boxmin.y;
		width = boxmax.z - boxmin.z;

		for (int i = 0; i <= y_steps; i++)
		{
			for (int j = 0; j <= z_steps; j++)
			{
				SH_VERTEX newVertex;
				newVertex.position = glm::vec3(boxmax.x, boxmin.y + (float)i / y_steps * height, boxmin.z + (float)j / z_steps * width);
				newVertex.normal = glm::vec3(1.0, 0.0, 0.0);
				newVertex.texcoord = glm::vec2((float)j / z_steps, (float)i / y_steps);
				newVertex.diffuse = Object->color(newVertex.texcoord);

				Vertices.push_back(newVertex);
			}
		}

		for (int i = 0; i != y_steps; i++)
		{
			for (int j = 0; j != z_steps; j++)
			{
				Indices.push_back(base + i * (z_steps +1) + j);
				Indices.push_back(base + (i + 1) * (z_steps +1) + j);
				Indices.push_back(base + (i + 1) * (z_steps +1) + j + 1);

				Indices.push_back(base + i * (z_steps +1) + j);
				Indices.push_back(base + (i + 1) * (z_steps +1) + j + 1);
				Indices.push_back(base + i * (z_steps +1) + j + 1);
			}
		}

		//X-
		base = Vertices.size();

		height = boxmax.y - boxmin.y;
		width = boxmax.z - boxmin.z;

		for (int i = 0; i <= y_steps; i++)
		{
			for (int j = 0; j <= z_steps; j++)
			{
				SH_VERTEX newVertex;
				newVertex.position = glm::vec3(boxmin.x, boxmin.y + (float)i / y_steps * height, boxmin.z + (float)j / z_steps * width);
				newVertex.normal = glm::vec3(-1.0, 0.0, 0.0);
				newVertex.texcoord = glm::vec2((float)j / z_steps, (float)i / y_steps);
				newVertex.diffuse = Object->color(newVertex.texcoord);

				Vertices.push_back(newVertex);
			}
		}

		for (int i = 0; i != y_steps; i++)
		{
			for (int j = 0; j != z_steps; j++)
			{
				Indices.push_back(base + i * (z_steps + 1) + j);
				Indices.push_back(base + (i + 1) * (z_steps + 1) + j);
				Indices.push_back(base + (i + 1) * (z_steps + 1) + j + 1);

				Indices.push_back(base + i * (z_steps + 1) + j);
				Indices.push_back(base + (i + 1) * (z_steps + 1) + j + 1);
				Indices.push_back(base + i * (z_steps + 1) + j + 1);
			}
		}


		//Y
		base = Vertices.size();

		height = boxmax.z - boxmin.z;
		width = boxmax.x - boxmin.x;

		for (int i = 0; i <= z_steps; i++)
		{
			for (int j = 0; j <= x_steps; j++)
			{
				SH_VERTEX newVertex;
				newVertex.position = glm::vec3(boxmin.x + (float)j / x_steps * width, boxmax.y, boxmin.z + (float)i / z_steps * height);
				newVertex.normal = glm::vec3(0.0, 1.0, 0.0);
				newVertex.texcoord = glm::vec2((float)j / z_steps, (float)i / x_steps);
				newVertex.diffuse = Object->color(newVertex.texcoord);

				Vertices.push_back(newVertex);
			}
		}

		for (int i = 0; i != z_steps; i++)
		{
			for (int j = 0; j != x_steps; j++)
			{
				Indices.push_back(base + i * (x_steps +1) + j);
				Indices.push_back(base + (i + 1) * (x_steps +1) + j);
				Indices.push_back(base + (i + 1) * (x_steps +1) + j + 1);

				Indices.push_back(base + i * (x_steps +1) + j);
				Indices.push_back(base + (i + 1) * (x_steps +1) + j + 1);
				Indices.push_back(base + i * (x_steps +1) + j + 1);
			}
		}

		//Y-
		base = Vertices.size();

		height = boxmax.z - boxmin.z;
		width = boxmax.x - boxmin.x;

		for (int i = 0; i <= z_steps; i++)
		{
			for (int j = 0; j <= x_steps; j++)
			{
				SH_VERTEX newVertex;
				newVertex.position = glm::vec3(boxmin.x + (float)j / x_steps * width, boxmin.y, boxmin.z + (float)i / z_steps * height);
				newVertex.normal = glm::vec3(0.0, -1.0, 0.0);
				newVertex.texcoord = glm::vec2((float)j / x_steps, (float)i / z_steps);
				newVertex.diffuse = Object->color(newVertex.texcoord);

				Vertices.push_back(newVertex);
			}
		}

		for (int i = 0; i != z_steps; i++)
		{
			for (int j = 0; j != x_steps; j++)
			{
				Indices.push_back(base + i * (x_steps + 1) + j);
				Indices.push_back(base + (i + 1) * (x_steps + 1) + j);
				Indices.push_back(base + (i + 1) * (x_steps + 1) + j + 1);

				Indices.push_back(base + i * (x_steps + 1) + j);
				Indices.push_back(base + (i + 1) * (x_steps + 1) + j + 1);
				Indices.push_back(base + i * (x_steps + 1) + j + 1);
			}
		}

	}
};


#endif
