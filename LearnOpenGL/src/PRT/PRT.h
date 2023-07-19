#ifndef PRT_H
#define PRT_H

#include <vector>
#include "../SH/SH_Object.h"
#include "../SH/SH_SAMPLE.h"
#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include "../font.h"
#include "../shader.h"
#include "../RAY/ray.h"
#include "../RAY/hittables/hittable_list.h"
#include "../SH_Rotate/SH_ROTATION.h"

void GetPTRCoffes(std::vector<SH_Object>& objects, int numSamples, int numBands, SH_SAMPLE* samples, GLFWwindow* window, Font* theFont, const char* filename);
void GetLightCoeffs(int numSamples, int numBands, SH_SAMPLE* samples, float* light);
void GetOutcCoeffs(std::vector<SH_Object>& objects, float* light, glm::vec3 lightColor, glm::vec3 viewPos, int numSamples, int numBands);
void DrawPrtObjects(Shader prtShader, std::vector<SH_Object>& objects, glm::mat4 view, glm::mat4 projection, unsigned int FBO, unsigned int SCR_WIDTH, unsigned int SCR_HEIGHT);
void GetObjectGlossy(std::vector<SH_Object>& objects, int numSamples, int numBands, SH_SAMPLE* samples);

#endif