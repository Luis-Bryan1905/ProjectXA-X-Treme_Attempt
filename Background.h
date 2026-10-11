#pragma once
#include <GL/glew.h>
#include <SDL_opengl.h>
#include <string>
#include "Camera.h"

namespace GE
{
	class Background
	{

	public:

		Background(std::string fname, int windowWidth, int windowHeight); // Background renderer

		void render(Camera* cam); // Render Background to screen

		void clean(); // Destroy Background and free up resources from memory

		void createBackground(std::string filenames);
		void createBackgroundVBO(float windowAspectRatio, float imageAspectRatio);
		void createBackgroundProgram();

	private:

		GLuint BackgroundCubeMapName;

		GLuint BackgroundProgramId;

		GLuint vertexLocation;
		GLuint vertexUVLocation;

		GLuint vboBackground;

		GLuint samplerId;

	};

}