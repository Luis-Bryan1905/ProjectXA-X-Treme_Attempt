#pragma once
#include <GL/glew.h>
#include <SDL_opengl.h>
#include <vector>
#include <string>
#include "Camera.h"

namespace GE
{

	class SkyboxRenderer
	{
	public:
		SkyboxRenderer(std::string front_fname, std::string back_fname, std::string right_fname, std::string left_fname, std::string top_fname, std::string bottom_fname)
		{

			std::vector<std::string> filenames;

			filenames.push_back(right_fname);
			filenames.push_back(left_fname);
			filenames.push_back(top_fname);
			filenames.push_back(bottom_fname);
			filenames.push_back(front_fname);
			filenames.push_back(back_fname);

			createCubeMap(filenames);
			createCubeVBO();
			createSkyboxProgram();

		}

		~SkyboxRenderer();

		void draw(Camera* cam);

		void destroy();

	private:
		void createCubeMap(std::vector<std::string> filenames);
		void createCubeVBO();
		void createSkyboxProgram();

	private:

		GLuint skyboxCubeMapName;

		GLuint skyboxProgramId;

		GLuint vertexLocation;

		GLuint vboSkybox;

		GLuint viewUniformID;
		GLuint projectionUniformID;
		GLuint samplerId;

	};

}
