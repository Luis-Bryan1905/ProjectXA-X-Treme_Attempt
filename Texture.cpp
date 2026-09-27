#include "Texture.h"
#include <iostream>

namespace GE
{

	void Texture::loadTexture(std::string filename)
	{
		SDL_Surface* surfaceImage = IMG_Load(filename.c_str()); // Load the image file into an SDL_Surface

		if (surfaceImage == nullptr) // If the image failed to load
		{
			std::cout << "Failed to load texture: " << filename << std::endl;
			std::cout << "SDL_image Error: " << IMG_GetError() << std::endl;
			return;
		}

		width = surfaceImage->w; // Get the width and height of the image from the SDL_Surface
		height = surfaceImage->h;

		// Determine GL format from surface pixel format (robust to RGB/BGR, RGBA/BGRA)
		format = surfaceImage->format->format;

		switch (format)
		{
			case SDL_PIXELFORMAT_RGBA32:
				format = GL_RGBA;
				break;

			case SDL_PIXELFORMAT_RGB24:
				format = GL_RGB;
				break;

			default:
				format = GL_RGBA;
				break;
		}

		// Create and upload texture
		glGenTextures(1, &textureName);
		glBindTexture(GL_TEXTURE_2D, textureName);

		// alignment for odd row sizes
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, surfaceImage->pixels);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glGenerateMipmap(GL_TEXTURE_2D);

		// unbind
		glBindTexture(GL_TEXTURE_2D, 0);

		SDL_FreeSurface(surfaceImage); // Free the SDL_Surface now that the texture has been uploaded to OpenGL
	}

}