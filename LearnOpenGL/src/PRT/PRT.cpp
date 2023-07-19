#include "PRT.h"

struct hit_object
{
	unsigned int index;
	glm::vec3 weight;
	glm::vec3 indices;
};

bool findTri(const SH_Object &object, glm::vec3 p, hit_object & obj)
{
	const float threshold = 0.1;
	unsigned int numTri = object.Indices.size()/3;
	for (unsigned int i = 0; i != numTri; i++)
	{
		glm::vec3 p1 = object.Vertices[object.Indices[3 * i]].position;
		glm::vec3 p2 = object.Vertices[object.Indices[3 * i+1]].position;
		glm::vec3 p3 = object.Vertices[object.Indices[3 * i+2]].position;

		glm::vec3 c1 = glm::cross(p - p2, p - p3);
		glm::vec3 c2 = glm::cross(p - p3, p - p1);
		glm::vec3 c3 = glm::cross(p - p1, p - p2);

		if (glm::dot(c1, c2) < threshold && glm::dot(c2, c3) < threshold && glm::dot(c3, c1) < threshold)
		{
			float l1 = glm::length(c1);
			float l2 = glm::length(c2);
			float l3 = glm::length(c3);
			float lsum = l1 + l2 + l3;
			obj.weight = glm::vec3(l1 / lsum, l2 / lsum, l3 / lsum);
			obj.indices = glm::vec3(3 * i, 3 * i + 1, 3 * i + 2);
			return true;
		}
	}
	return false;
}

bool hit(std::vector<SH_Object>& objects, const ray& r, float t_min, float t_max, hit_record& rec, hit_object& obj) {
	hit_record temp_rec;
	hit_object temp_obj;
	bool hit_anything = false;
	auto closest_so_far = t_max;

	for(unsigned int i=0;i!=objects.size();++i){
		const auto& object = objects[i];
		if (object.Object->hit(r, t_min, closest_so_far, temp_rec)) {
			hit_anything = true;
			closest_so_far = temp_rec.t;
			rec = temp_rec;

			temp_obj.index = i;
			if (!findTri(object, temp_rec.p, temp_obj))
				std::cout << "Find Tri is Failed!" << std::endl;
			obj = temp_obj;
		}
	}

	return hit_anything;
}

void GetPTRCoffes(std::vector<SH_Object>& objects, int numSamples, int numBands, SH_SAMPLE* samples, GLFWwindow* window, Font* theFont, const char* filename)
{
	const float PI = 3.1415926539;

	int numFunctions = numBands * numBands;
	int squareNumFunctions = numFunctions * numFunctions;
	int numObjects = objects.size();

	for (int i = 0; i < numObjects; ++i)
	{
		SH_Object* currentObject = &objects[i];
		const int numVertices = currentObject->Vertices.size();

		for (int j = 0; j < numVertices; ++j)
		{
			SH_VERTEX& currentVertex = currentObject->Vertices[j];

			currentVertex.shCoeffs_r = new float[numFunctions];
			currentVertex.shCoeffs_g = new float[numFunctions];
			currentVertex.shCoeffs_b = new float[numFunctions];
			currentVertex.shCoeffs_temp_r = new float[numFunctions];
			currentVertex.shCoeffs_temp_g = new float[numFunctions];
			currentVertex.shCoeffs_temp_b = new float[numFunctions];

			currentVertex.shGlossyCoeffs_r = new float[squareNumFunctions];
			currentVertex.shGlossyCoeffs_g = new float[squareNumFunctions];
			currentVertex.shGlossyCoeffs_b = new float[squareNumFunctions];
			currentVertex.shGlossyCoeffs_temp_r = new float[squareNumFunctions];
			currentVertex.shGlossyCoeffs_temp_g = new float[squareNumFunctions];
			currentVertex.shGlossyCoeffs_temp_b = new float[squareNumFunctions];

			if (!(currentVertex.shCoeffs_r && currentVertex.shCoeffs_g && currentVertex.shCoeffs_b && currentVertex.shCoeffs_temp_r && currentVertex.shCoeffs_temp_g && currentVertex.shCoeffs_temp_b))
			{
				std::cout << "Unable to create space for vertex SH coefficients";
			}
		}
	}

	//Is there a file containing the coefficients, or do they need to be regenerated?
	bool regenerateCoeffs = false;

	//Does the file exist?
	std::ifstream inFile(filename, std::ios::in | std::ios::binary);
	if (!inFile.is_open())
	{
		std::cout<<"Unable to open directcoeffs.dat, regenerating coefficients...";
		regenerateCoeffs = true;
	}

	//Are the number of bands and samples in the file correct?
	if (!regenerateCoeffs)
	{
		int numFileBands, numFileSamples;

		inFile.read((char*)&numFileBands, sizeof(int));
		inFile.read((char*)&numFileSamples, sizeof(int));

		if (numFileBands != numBands || numFileSamples != numSamples)
		{
			std::cout<<"directcoeffs.dat has different number of bands/samples, regenerating coefficients...";
			regenerateCoeffs = true;
			inFile.close();
		}
	}

	//If the file is good, read in the coefficients
	if (!regenerateCoeffs)
	{
		for (int i = 0; i < numObjects; ++i)
		{
			SH_Object* currentObject = &objects[i];

			const int numVertices = currentObject->Vertices.size();
			for (int j = 0; j < numVertices; ++j)
			{
				SH_VERTEX& currentVertex = currentObject->Vertices[j];

				inFile.read((char*)currentVertex.shCoeffs_r,
					numFunctions * sizeof(float));
				inFile.read((char*)currentVertex.shCoeffs_g,
					numFunctions * sizeof(float));
				inFile.read((char*)currentVertex.shCoeffs_b,
					numFunctions * sizeof(float));

				inFile.read((char*)currentVertex.shGlossyCoeffs_r,
					squareNumFunctions * sizeof(float));
				inFile.read((char*)currentVertex.shGlossyCoeffs_g,
					squareNumFunctions * sizeof(float));
				inFile.read((char*)currentVertex.shGlossyCoeffs_b,
					squareNumFunctions * sizeof(float));
			}
		}

		inFile.close();
		return;
	}
	
	//diffuse 1
	for (int i = 0; i < objects.size(); ++i)
	{
		for (int j = 0; j < objects[i].Vertices.size(); ++j)
		{
			SH_VERTEX& currentVertex = objects[i].Vertices[j];

			//Clear the coefficients for this vertex
			for (int k = 0; k < numFunctions; ++k)
			{
				currentVertex.shCoeffs_r[k] = 0.0;
				currentVertex.shCoeffs_g[k] = 0.0;
				currentVertex.shCoeffs_b[k] = 0.0;
				currentVertex.shCoeffs_temp_r[k] = 0.0;
				currentVertex.shCoeffs_temp_g[k] = 0.0;
				currentVertex.shCoeffs_temp_b[k] = 0.0;
			}

			//Loop through samples
			for (int k = 0; k < numSamples; ++k)
			{
				//hit test
				ray hitRay(currentVertex.position, samples[k].direction);
				hit_record hitRecord;
				hit_object hitObect;
				bool isHit = hit(objects, hitRay, 0.01f, 1000.0f, hitRecord, hitObect);

				if (isHit)
					continue;

				//Calculate cosine term for this sample
				double dot = glm::dot(samples[k].direction, currentVertex.normal);

				//Clamp to [0, 1]
				if (dot > 0.0)
				{
					//Add the contribution of this sample to the coefficients
					for (int l = 0; l < numFunctions; ++l)
					{
						double contribution = dot * samples[k].shValues[l]; //currentVertex.diffuse.x * dot * samples[k].shValues[l];

						currentVertex.shCoeffs_r[l] += contribution;
						currentVertex.shCoeffs_g[l] += contribution;
						currentVertex.shCoeffs_b[l] += contribution;
					}
				}
			}

			//Rescale the coefficients AND insert diffuse
			for (int k = 0; k < numFunctions; ++k)
			{
				currentVertex.shCoeffs_r[k] *= 4 * PI / numSamples * currentVertex.diffuse.r;
				currentVertex.shCoeffs_g[k] *= 4 * PI / numSamples * currentVertex.diffuse.g;
				currentVertex.shCoeffs_b[k] *= 4 * PI / numSamples * currentVertex.diffuse.b;
			}

			glClearColor(0.1f, 0.0f, 0.0f, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			std::stringstream ss;
			ss << "PRT(Diffuse) is calculated(1/2).";
			theFont->RenderText(ss.str(), 150.0f, 380.0f, 0.8f, glm::vec3(1.0, 1.0, 1.0));

			std::stringstream ss1;
			ss1 << "Object: No." << i + 1 << " of " << objects.size();
			theFont->RenderText(ss1.str(), 290.0f, 300.0f, 0.8f, glm::vec3(1.0, 1.0, 1.0));

			std::stringstream ss2;
			ss2 << "Vertex: No." << j + 1 << " of " << objects[i].Vertices.size();
			theFont->RenderText(ss2.str(), 270.0f, 250.0f, 0.8f, glm::vec3(1.0, 1.0, 1.0));

			//事件检查、缓冲交换
			glfwSwapBuffers(window);
			glfwPollEvents();

		}
	}
	//diffuse 2
	for (int i = 0; i < objects.size(); ++i)
	{
		for (int j = 0; j < objects[i].Vertices.size(); ++j)
		{
			SH_VERTEX& currentVertex = objects[i].Vertices[j];

			//Loop through samples
			for (int k = 0; k < numSamples; ++k)
			{
				//hit test
				ray hitRay(currentVertex.position, samples[k].direction);
				hit_record hitRecord;
				hit_object hitObect;
				bool isHit = hit(objects, hitRay, 0.01f, 1000.0f, hitRecord, hitObect);
				int visibility = isHit ? 0.0 : 1.0;

				if (!isHit)
					continue;

				//Calculate cosine term for this sample
				double dot = glm::dot(samples[k].direction, currentVertex.normal);

				//Clamp to [0, 1]
				if (dot > 0.0)
				{
					//Add the contribution of this sample to the coefficients
					for (int l = 0; l < numFunctions; ++l)
					{
						double contribution = dot * (1.0-visibility);

						if (isHit)
						{
							currentVertex.shCoeffs_temp_r[l] += contribution * (hitObect.weight.x * objects[hitObect.index].Vertices[hitObect.indices.x].shCoeffs_r[l] + hitObect.weight.y * objects[hitObect.index].Vertices[hitObect.indices.y].shCoeffs_r[l] + hitObect.weight.z * objects[hitObect.index].Vertices[hitObect.indices.z].shCoeffs_r[l]);
							currentVertex.shCoeffs_temp_g[l] += contribution * (hitObect.weight.x * objects[hitObect.index].Vertices[hitObect.indices.x].shCoeffs_g[l] + hitObect.weight.y * objects[hitObect.index].Vertices[hitObect.indices.y].shCoeffs_g[l] + hitObect.weight.z * objects[hitObect.index].Vertices[hitObect.indices.z].shCoeffs_g[l]);
							currentVertex.shCoeffs_temp_b[l] += contribution * (hitObect.weight.x * objects[hitObect.index].Vertices[hitObect.indices.x].shCoeffs_b[l] + hitObect.weight.y * objects[hitObect.index].Vertices[hitObect.indices.y].shCoeffs_b[l] + hitObect.weight.z * objects[hitObect.index].Vertices[hitObect.indices.z].shCoeffs_b[l]);

						}
					}
				}

			}

			//Rescale the coefficients AND insert diffuse
			for (int k = 0; k < numFunctions; ++k)
			{
				currentVertex.shCoeffs_temp_r[k] *= 4 * PI / numSamples * currentVertex.diffuse.r;
				currentVertex.shCoeffs_temp_g[k] *= 4 * PI / numSamples * currentVertex.diffuse.g;
				currentVertex.shCoeffs_temp_b[k] *= 4 * PI / numSamples * currentVertex.diffuse.b;

				currentVertex.shCoeffs_r[k] += currentVertex.shCoeffs_temp_r[k];
				currentVertex.shCoeffs_g[k] += currentVertex.shCoeffs_temp_g[k];
				currentVertex.shCoeffs_b[k] += currentVertex.shCoeffs_temp_b[k];
			}

			//std::cout << "AA" << std::endl;
			glClearColor(0.1f, 0.0f, 0.0f, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			std::stringstream ss;
			ss << "PRT(Diffuse) is calculated(2/2).";
			theFont->RenderText(ss.str(), 150.0f, 380.0f, 0.8f, glm::vec3(1.0, 1.0, 1.0));

			std::stringstream ss1;
			ss1 << "Object: No." << i + 1 << " of " << objects.size();
			theFont->RenderText(ss1.str(), 290.0f, 300.0f, 0.8f, glm::vec3(1.0, 1.0, 1.0));

			std::stringstream ss2;
			ss2 << "Vertex: No." << j + 1 << " of " << objects[i].Vertices.size();
			theFont->RenderText(ss2.str(), 270.0f, 250.0f, 0.8f, glm::vec3(1.0, 1.0, 1.0));

			//事件检查、缓冲交换
			glfwSwapBuffers(window);
			glfwPollEvents();

		}
	}
	
	//glossy 1
	for (int i = 0; i < objects.size(); ++i)
	{
		for (int j = 0; j < objects[i].Vertices.size(); ++j)
		{
			SH_VERTEX& currentVertex = objects[i].Vertices[j];

			//Clear the coefficients for this vertex
			for (int k = 0; k < squareNumFunctions; ++k)
			{
				currentVertex.shGlossyCoeffs_r[k] = 0.0;
				currentVertex.shGlossyCoeffs_g[k] = 0.0;
				currentVertex.shGlossyCoeffs_b[k] = 0.0;
				currentVertex.shGlossyCoeffs_temp_r[k] = 0.0;
				currentVertex.shGlossyCoeffs_temp_g[k] = 0.0;
				currentVertex.shGlossyCoeffs_temp_b[k] = 0.0;
			}

			//Loop through samples
			for (int k = 0; k < numSamples; ++k)
			{
				//hit test
				ray hitRay(currentVertex.position, samples[k].direction);
				hit_record hitRecord;
				hit_object hitObect;
				bool isHit = hit(objects, hitRay, 0.01f, 1000.0f, hitRecord, hitObect);

				if (isHit)
					continue;

				//Calculate cosine term for this sample
				double dot = glm::dot(samples[k].direction, currentVertex.normal);

				//Add the contribution of this sample to the coefficients
				for (int l = 0; l < numFunctions; ++l)
				{

					for (int lc = 0; lc < numFunctions; ++lc)
					{
						int ind = l * numFunctions + lc;

						float contribution = samples[k].shValues[l] * samples[k].shValues[lc];

						currentVertex.shGlossyCoeffs_r[ind] += contribution;
						currentVertex.shGlossyCoeffs_g[ind] += contribution;
						currentVertex.shGlossyCoeffs_b[ind] += contribution;

					}
				}
			}

			//Rescale the coefficients AND insert diffuse
			for (int k = 0; k < squareNumFunctions; ++k)
			{
				currentVertex.shGlossyCoeffs_r[k] *= 4 * PI / numSamples;
				currentVertex.shGlossyCoeffs_g[k] *= 4 * PI / numSamples;
				currentVertex.shGlossyCoeffs_b[k] *= 4 * PI / numSamples;
			}

			glClearColor(0.1f, 0.0f, 0.0f, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			std::stringstream ss;
			ss << "PRT(Glossy) is calculated(1/2).";
			theFont->RenderText(ss.str(), 150.0f, 380.0f, 0.8f, glm::vec3(1.0, 1.0, 1.0));

			std::stringstream ss1;
			ss1 << "Object: No." << i + 1 << " of " << objects.size();
			theFont->RenderText(ss1.str(), 290.0f, 300.0f, 0.8f, glm::vec3(1.0, 1.0, 1.0));

			std::stringstream ss2;
			ss2 << "Vertex: No." << j + 1 << " of " << objects[i].Vertices.size();
			theFont->RenderText(ss2.str(), 270.0f, 250.0f, 0.8f, glm::vec3(1.0, 1.0, 1.0));

			//事件检查、缓冲交换
			glfwSwapBuffers(window);
			glfwPollEvents();

		}
	}
	
	//glossy 2
	for (int i = 0; i < objects.size(); ++i)
	{
		SH_Object* currentObject = &objects[i];
		for (int j = 0; j < objects[i].Vertices.size(); ++j)
		{
			SH_VERTEX& currentVertex = currentObject->Vertices[j];

			//Loop through samples
			for (int k = 0; k < numSamples; ++k)
			{
				//hit test
				ray hitRay(currentVertex.position, samples[k].direction);
				hit_record hitRecord;
				hit_object hitObect;
				bool isHit = hit(objects, hitRay, 0.01f, 1000.0f, hitRecord, hitObect);
				int visibility = isHit ? 0.0 : 1.0;
				if (!isHit)
					continue;

				glm::vec3 R = -glm::reflect(-samples[k].direction, hitRecord.normal);
				SH_SAMPLE currentSample;
				GenerateSHSample(numBands, R, currentSample);

				float* rotateG_r = new float[numFunctions];
				float* rotateG_g = new float[numFunctions];
				float* rotateG_b = new float[numFunctions];
				RotateSHCoefficients(numBands, objects[hitObect.index].G_r, rotateG_r, currentSample.theta * 180.0 / PI, currentSample.phi * 180.0 / PI);
				RotateSHCoefficients(numBands, objects[hitObect.index].G_g, rotateG_g, currentSample.theta * 180.0 / PI, currentSample.phi * 180.0 / PI);
				RotateSHCoefficients(numBands, objects[hitObect.index].G_b, rotateG_b, currentSample.theta * 180.0 / PI, currentSample.phi * 180.0 / PI);

				//Calculate cosine term for this sample
				float dot = glm::dot(samples[k].direction, currentVertex.normal);
				
				//Add the contribution of this sample to the coefficients
				for (int l = 0; l < numFunctions; ++l)
				{
					for (int lc = 0; lc != numFunctions; lc++)
					{
						int id = l * numFunctions + lc;

						glm::vec3 contribution = glm::vec3(0.0f);
						for (int kid = 0; kid != numFunctions; kid++)
						{
							int id2 = kid * numFunctions + lc;
							glm::vec3 currentCoeff;
							currentCoeff.r = (hitObect.weight.x * objects[hitObect.index].Vertices[hitObect.indices.x].shGlossyCoeffs_r[id2] + hitObect.weight.y * objects[hitObect.index].Vertices[hitObect.indices.y].shGlossyCoeffs_r[id2] + hitObect.weight.z * objects[hitObect.index].Vertices[hitObect.indices.z].shGlossyCoeffs_r[id2]);
							currentCoeff.g = (hitObect.weight.x * objects[hitObect.index].Vertices[hitObect.indices.x].shGlossyCoeffs_g[id2] + hitObect.weight.y * objects[hitObect.index].Vertices[hitObect.indices.y].shGlossyCoeffs_g[id2] + hitObect.weight.z * objects[hitObect.index].Vertices[hitObect.indices.z].shGlossyCoeffs_g[id2]);
							currentCoeff.b = (hitObect.weight.x * objects[hitObect.index].Vertices[hitObect.indices.x].shGlossyCoeffs_b[id2] + hitObect.weight.y * objects[hitObect.index].Vertices[hitObect.indices.y].shGlossyCoeffs_b[id2] + hitObect.weight.z * objects[hitObect.index].Vertices[hitObect.indices.z].shGlossyCoeffs_b[id2]);

							contribution.r += rotateG_r[kid] * currentCoeff.r * currentSample.shValues[kid];
							contribution.g += rotateG_g[kid] * currentCoeff.g * currentSample.shValues[kid];
							contribution.b += rotateG_b[kid] * currentCoeff.b * currentSample.shValues[kid];
						}

						currentVertex.shGlossyCoeffs_temp_r[id] += contribution.r * samples[k].shValues[i];
						currentVertex.shGlossyCoeffs_temp_g[id] += contribution.g * samples[k].shValues[i];
						currentVertex.shGlossyCoeffs_temp_b[id] += contribution.b * samples[k].shValues[i];
					}
				}

				if (rotateG_r)
				{
					delete[]rotateG_r;
				}
				rotateG_r = nullptr;

				if (rotateG_g)
				{
					delete[]rotateG_g;
				}
				rotateG_g = nullptr;

				if (rotateG_b)
				{
					delete[]rotateG_b;
				}
				rotateG_b = nullptr;

			}

			//Rescale the coefficients AND insert diffuse
			for (int k = 0; k < squareNumFunctions; ++k)
			{
				currentVertex.shGlossyCoeffs_temp_r[k] *= 4 * PI / numSamples;
				currentVertex.shGlossyCoeffs_temp_g[k] *= 4 * PI / numSamples;
				currentVertex.shGlossyCoeffs_temp_b[k] *= 4 * PI / numSamples;

				currentVertex.shGlossyCoeffs_r[k] += currentVertex.shGlossyCoeffs_temp_r[k];
				currentVertex.shGlossyCoeffs_g[k] += currentVertex.shGlossyCoeffs_temp_g[k];
				currentVertex.shGlossyCoeffs_b[k] += currentVertex.shGlossyCoeffs_temp_b[k];
			}

			//std::cout << "AA" << std::endl;
			glClearColor(0.1f, 0.0f, 0.0f, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			std::stringstream ss;
			ss << "PRT(Glossy) is calculated(2/2).";
			theFont->RenderText(ss.str(), 150.0f, 380.0f, 0.8f, glm::vec3(1.0, 1.0, 1.0));

			std::stringstream ss1;
			ss1 << "Object: No." << i + 1 << " of " << objects.size();
			theFont->RenderText(ss1.str(), 290.0f, 300.0f, 0.8f, glm::vec3(1.0, 1.0, 1.0));

			std::stringstream ss2;
			ss2 << "Vertex: No." << j + 1 << " of " << objects[i].Vertices.size();
			theFont->RenderText(ss2.str(), 270.0f, 250.0f, 0.8f, glm::vec3(1.0, 1.0, 1.0));

			//事件检查、缓冲交换
			glfwSwapBuffers(window);
			glfwPollEvents();

		}
	}
	
	//Save the coefficients to a file
	std::ofstream outFile(filename, std::ios::out | std::ios::binary | std::ios::trunc);

	//First save the number of bands and samples
	outFile.write((const char*)&numBands, sizeof(int));
	outFile.write((const char*)&numSamples, sizeof(int));

	//Loop through the vertices and save the coefficients for each
	for (int i = 0; i < numObjects; ++i)
	{
		SH_Object* currentObject = &objects[i];

		const int numVertices = currentObject->Vertices.size();
		for (int j = 0; j < numVertices; ++j)
		{
			SH_VERTEX& currentVertex = currentObject->Vertices[j];

			outFile.write((const char*)currentVertex.shCoeffs_r,
				numFunctions * sizeof(float));
			outFile.write((const char*)currentVertex.shCoeffs_g,
				numFunctions * sizeof(float));
			outFile.write((const char*)currentVertex.shCoeffs_b,
				numFunctions * sizeof(float));

			outFile.write((const char*)currentVertex.shGlossyCoeffs_r,
				squareNumFunctions * sizeof(float));
			outFile.write((const char*)currentVertex.shGlossyCoeffs_g,
				squareNumFunctions * sizeof(float));
			outFile.write((const char*)currentVertex.shGlossyCoeffs_b,
				squareNumFunctions * sizeof(float));
		}
	}

	outFile.close();

	return;

};

void GetLightCoeffs(int numSamples, int numBands, SH_SAMPLE* samples, float* light)
{
	int numFunctions = numBands * numBands;
	const float PI = 3.14159265359;

	for (int l = 0; l != numFunctions; l++)
	{
		light[l] = 0.0;

		for (int i = 0; i != numSamples; i++)
		{
			light[l] += (samples[i].theta < 1.0 / 6.0 * PI) ? samples[i].shValues[l] : 0.0;

		}

		light[l] *= 4 * PI / numSamples;
	}
};

void GetOutcCoeffs(std::vector<SH_Object>& objects, float* light, glm::vec3 lightColor, glm::vec3 viewPos, int numSamples, int numBands)
{
	const float PI = 3.14159265359;
	int numFunctions = numBands * numBands;
	for (int i = 0; i < objects.size(); ++i)
	{
		SH_Object* currentObject = &objects[i];
		const int numVertices = currentObject->Vertices.size();
		for (int j = 0; j < numVertices; ++j)
		{
			SH_VERTEX& currentVertex = currentObject->Vertices[j];

			//glossy
			glm::vec3 viewDirection = glm::normalize(currentVertex.position - viewPos);
			glm::vec3 R = glm::reflect(viewDirection, currentVertex.normal);
			SH_SAMPLE currentSample;
			GenerateSHSample(numBands, R, currentSample);

			float* rotateG_r = new float[numFunctions];
			float* rotateG_g = new float[numFunctions];
			float* rotateG_b = new float[numFunctions];
			RotateSHCoefficients(numBands, currentObject->G_r, rotateG_r, currentSample.theta * 180.0 / PI, currentSample.phi * 180.0 / PI);
			RotateSHCoefficients(numBands, currentObject->G_g, rotateG_g, currentSample.theta * 180.0 / PI, currentSample.phi * 180.0 / PI);
			RotateSHCoefficients(numBands, currentObject->G_b, rotateG_b, currentSample.theta * 180.0 / PI, currentSample.phi * 180.0 / PI);

			glm::vec3 contribution = glm::vec3(0.0f);
			for (int l = 0; l != numFunctions; ++l)
			{
				glm::vec3 glossyContribute = glm::vec3(0.0f);
				for (int lc = 0; lc != numFunctions; ++lc)
				{
					int id = l * numFunctions + lc;
					glossyContribute += glm::vec3(currentVertex.shGlossyCoeffs_r[id], currentVertex.shGlossyCoeffs_g[id], currentVertex.shGlossyCoeffs_b[id]) * light[lc] * lightColor;
				}
				contribution += glm::vec3(rotateG_r[l], rotateG_g[l], rotateG_b[l]) * glossyContribute * currentSample.shValues[l];
			}
			currentVertex.glossy= glm::vec3(glm::max(contribution.r, 0.0f), glm::max(contribution.g, 0.0f), glm::max(contribution.b, 0.0f));

			if (rotateG_r)
			{
				delete[]rotateG_r;
			}
			rotateG_r = nullptr;

			if (rotateG_g)
			{
				delete[]rotateG_g;
			}
			rotateG_g = nullptr;

			if (rotateG_b)
			{
				delete[]rotateG_b;
			}
			rotateG_b = nullptr;

			//diffuse
			float cosine = glm::dot(-viewDirection,currentVertex.normal);
			float a = currentObject->roughness * currentObject->roughness;
			float a2 = a * a;
			if (cosine > 0.0f)
			{
				float item = cosine * cosine * (a2 - 1.0f) + 1.0f;
				float pdf = a2 / (PI * item * item) * cosine;
				float item2 = glm::pow((1.0 - cosine), 5.0f);
				glm::vec3 F0 = currentObject->metalness * currentObject->Object->color(glm::vec2(0.0f)) + (1.0f - currentObject->metalness) * glm::vec3(0.04f);
				glm::vec3 fernel = F0 * (1.0f - item2) + item2;
				glm::vec3 Kd = (glm::vec3(1.0f) - fernel) * (1.0f - currentObject->metalness);

				glm::vec3 contribute = glm::vec3(0.0);
				for (int k = 0; k < numFunctions; ++k)
				{
					contribute += glm::vec3(currentVertex.shCoeffs_r[k], currentVertex.shCoeffs_g[k], currentVertex.shCoeffs_b[k]) * light[k];
				}
				currentVertex.color = Kd * glm::vec3(glm::max(contribute.r, 0.0f), glm::max(contribute.g, 0.0f), glm::max(contribute.b, 0.0f));
			}
		}
		
		glBindVertexArray(currentObject->VAO);
		glBindBuffer(GL_ARRAY_BUFFER, currentObject->VBO);
		glBufferData(GL_ARRAY_BUFFER, currentObject->Vertices.size()*sizeof(SH_VERTEX), &(currentObject->Vertices[0]), GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, currentObject->EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, currentObject->Indices.size()*sizeof(unsigned int), &(currentObject->Indices[0]), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SH_VERTEX), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(SH_VERTEX), (void*)(offsetof(SH_VERTEX, color)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(SH_VERTEX), (void*)(offsetof(SH_VERTEX, glossy)));
		glEnableVertexAttribArray(2);

	}
};

void DrawPrtObjects(Shader prtShader, std::vector<SH_Object> &objects, glm::mat4 view, glm::mat4 projection, unsigned int FBO, unsigned int SCR_WIDTH, unsigned int SCR_HEIGHT)
{
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);

	glDisable(GL_CULL_FACE);
	glDisable(GL_BLEND);

	prtShader.use();

	prtShader.setMat4("view", glm::value_ptr(view));
	prtShader.setMat4("projection", glm::value_ptr(projection));

	for (unsigned int i = 0; i != objects.size(); i++)
	{
		prtShader.setMat4("model", glm::value_ptr(objects[i].model));

		glBindVertexArray(objects[i].VAO);
		glDrawElements(GL_TRIANGLES, objects[i].Indices.size(), GL_UNSIGNED_INT, 0);
	}
}

void GetObjectGlossy(std::vector<SH_Object>& objects, int numSamples, int numBands, SH_SAMPLE* samples)
{
	int numFunctions = numBands * numBands;
	int numObjects = objects.size();
	const float PI = 3.14159265359;
	for (int i = 0; i != numObjects; i++)
	{
		SH_Object* currentObject = &objects[i];

		currentObject->G_r = new float[numFunctions];
		currentObject->G_g = new float[numFunctions];
		currentObject->G_b = new float[numFunctions];

		float a = currentObject->roughness * currentObject->roughness;
		float a2 = a * a;
		for (int j = 0; j != numFunctions; j++)
		{
			currentObject->G_r[j] = 0.0f;
			currentObject->G_g[j] = 0.0f;
			currentObject->G_b[j] = 0.0f;

			for (int k = 0; k != numSamples; ++k)
			{
				float cosine = samples[k].direction.y;
				//float sine = sqrt(1.0f - cosine * cosine);

				float item = cosine * cosine * (a2 - 1.0f) + 1.0f;
				float pdf = a2 / (PI * item * item) * cosine;
				float item2 = glm::pow((1.0 - cosine), 5.0f);
				glm::vec3 F0 = currentObject->metalness * currentObject->Object->color(glm::vec2(0.0f)) + (1.0f - currentObject->metalness) * glm::vec3(0.04f);
				glm::vec3 fernel = F0;// *(1.0f - item2) + item2;

				float item3 = pdf * samples[k].shValues[j];
				currentObject->G_r[j] += item3 * fernel.r;
				currentObject->G_g[j] += item3 * fernel.g;
				currentObject->G_b[j] += item3 * fernel.b;
			}
			currentObject->G_r[j] *= 4.0 * PI / numSamples;
			currentObject->G_g[j] *= 4.0 * PI / numSamples;
			currentObject->G_b[j] *= 4.0 * PI / numSamples;
		}

	}
	return;
}

