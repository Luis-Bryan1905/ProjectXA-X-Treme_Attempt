#pragma once
#include <GL/glew.h>
#include <SDL.h>
#include <SDL_image.h>
#include <string>

namespace GE {
	class Texture {
	public:
		// Constructor
		Texture(std::string filename) {
			width = 0;
			height = 0;
			textureName = 0;
			format = 0;

			// Load the texture into OpenGL
			loadTexture(filename);
		}

		// New constructor for GUI.  Allows a texture object to be created
		// from an existing OpenGL texture object.  Does make assumptions
		// about the texture object
		Texture(GLuint name) {
			textureName = name;

			glBindTexture(GL_TEXTURE_2D, textureName);

			glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
			glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);

			glBindTexture(GL_TEXTURE_2D, 0);

			format = GL_RGBA;
		}

		~Texture() {
			if (textureName != 0)
			{
				glDeleteTextures(1, &textureName);
				textureName = 0;
			}
		}

		// Accessor methods
		int getWidth() {
			return width;
		}
		int getHeight() {
			return height;
		}

		Uint32 getFormat() {
			return format;
		}

		GLuint getTextureName() {
			return textureName;
		}

	private:
		// Helper function to load the texture
		void loadTexture(std::string filename);

	private:
		// Dimensions
		int width;
		int height;

		// Format
		Uint32 format;

		// OpenGL name for texture object
		GLuint textureName;
	};
}

