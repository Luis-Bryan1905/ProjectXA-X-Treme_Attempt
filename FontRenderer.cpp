#include <SDL.h>
#include <SDL_ttf.h>
#include <GL/glew.h>
#include <SDL_opengl.h>
#include "FontRenderer.h"
#include <iostream>

namespace GE {

	// Initialise TTF engine
	void FontRenderer::init()
	{
		TTF_Init();
	}

	Texture *FontRenderer::renderText(std::string text, std::string fontname, int font_size)
	{
		// Create text using SDL_ttf
		// First open the font
		TTF_Font* font = TTF_OpenFont(fontname.c_str(), font_size); 

		// Check font was created okay
		if (font == nullptr) 
		{
			std::cerr << "Error creating font " << text << "! " << TTF_GetError() << std::endl;
			return nullptr;
		}

		// Now render text into a surface object
		SDL_Color White = { 255, 255, 255 };

		SDL_Surface* surfaceImage = TTF_RenderText_Solid(font, text.c_str(), White);

		// Convert the format into RGBA for OpenGL
		SDL_Surface* converted = SDL_ConvertSurfaceFormat(surfaceImage, SDL_PIXELFORMAT_RGBA32, 0);

		// Check conversion was okay
		if (converted != nullptr) {
			SDL_FreeSurface(surfaceImage);

			surfaceImage = converted;
		}
		else {
			std::cerr << "Error converting font " << text << "! " << SDL_GetError() << std::endl;

			return nullptr;
		}

		// Get the dimensions, need for OpenGL
		int width = surfaceImage->w;
		int height = surfaceImage->h;

		// Flip surface pixels
		// First check if surface needs locking
		if (SDL_MUSTLOCK(surfaceImage)) {
			SDL_LockSurface(surfaceImage);
		}

		// Calculate number of bytes in a row
		int bytesPerRow = width * surfaceImage->format->BytesPerPixel;
		
		// Create a temporary buffer for a row of pixels
		char* temp = new char[bytesPerRow];
		
		// Exchange rows of pixels from top to bottom
		for (int y = 0; y < height / 2; y++) 
		{
			
			memcpy(temp, ((char*)surfaceImage->pixels) + ((height - y - 1) * bytesPerRow), bytesPerRow); // Copy bottom row into temp
		
			
			memcpy(((char*)surfaceImage->pixels) + ((height - y - 1) * bytesPerRow), (char*)surfaceImage->pixels + (y * bytesPerRow), bytesPerRow); // Copy top row to bottom row
		
			
			memcpy((char*)surfaceImage->pixels + (y * bytesPerRow), temp, bytesPerRow); // Copy temp to top row
		}
		
		// Delete temp buffer
		delete[] temp;
		
		// Unlock surface if it was a lockable surface
		if (SDL_MUSTLOCK(surfaceImage)) {
			SDL_UnlockSurface(surfaceImage);
		}

		Uint32 format = GL_RGBA;

		// Create the texture in OpenGL.
		GLuint textureName;

		// Create a texture name for the texture
		glGenTextures(1, &textureName);

		// Select created texture for subsequent texture operations
		// to setup the texture for OpenGL
		glBindTexture(GL_TEXTURE_2D, textureName);

		// Copy the pixel data from the SDL_Surface object to the OpenGL texture
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
			GL_UNSIGNED_BYTE, surfaceImage->pixels);

		// Configure how the texture will be manipulated when it needs to be
		// reduced or increased (magnified) when rendering onto an object.
		// GL_LINEAR is weighted average of the colours around the texture co-ords
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Release the surface object and memory associated with it
		// as no longer need it
		SDL_FreeSurface(surfaceImage);

		// Release the font created for the text too
		TTF_CloseFont(font);

		// Got this far so have a valid texture object
		return new Texture(textureName);
	}

	// Release the TTF engine
	void FontRenderer::destroy()
	{
		// Shutdown TTF down
		TTF_Quit();
	}

}

