#pragma once
#include <SDL.h>
#include <glm/glm.hpp>
#include <string>
#include "Texture.h"
#include "FontRenderer.h"

// GUI classes
namespace GE {
	
	class GUIText // Use for text
	{
	public:
		
		GUIText(int x, int y, std::string text, std::string ff) // Constructor
		{
			font = ff;
			fr = new FontRenderer();

			fr->init();

			tex = fr->renderText(text, font, 24);

			xPos = x;
			yPos = y;
		}

		
		~GUIText() // Destructor
		{
			
			if (tex) 
			{
				delete tex; // Clean up any texture of text
			}

			
			delete fr; // Clean up font renderer
		}

		// Set the font filename to use to render text
		// Could also create a constructor which is passed
		// the name of a font file
		void setFontFileName(const std::string ffName) 
		{
			font = ffName;
		}

		// Sets the text to display on GUI which
		// results in generating a new texture
		void setText(const std::string& text) 
		{
			// Free any existing texture object
			if (tex) {
				delete tex;

				tex = nullptr;
			}

			// Note, whilst can change the font size is fixed.
			// Left as an exercise to allow size to be changed
			tex = fr->renderText(text, font, 24);
		}

		// Returns the texture of text
		// Note, returns nullptr if no text has been set
		Texture* getTexture() 
		{
			return tex;
		}

		// Set position
		void setX(int newX) 
		{
			xPos = newX;
		}

		void setY(int newY) 
		{
			yPos = newY;
		}

		// Get position
		int getX() 
		{
			return xPos;
		}

		int getY() 
		{
			return yPos;
		}

	private:
		std::string font;

		FontRenderer* fr;

		Texture* tex;

		int xPos, yPos;
	};

	// Thin wrapper around a Texture object for a GUI image
	class GUIImage {
	public:
		GUIImage(int x, int y, Texture* tex) 
		{
			xPos = x;
			yPos = y;

			img = tex;
		}
		~GUIImage() 
		{
			delete img;
		}

		void setTexture(Texture* tex) 
		{
			if (img) {
				delete img;

				img = nullptr;
			}

			img = tex;
		}

		// Note, returns nullptr if no text has been set
		Texture* getTexture() 
		{
			return img;
		}

		// Position setters and getters
		void setX(int newX) 
		{
			xPos = newX;
		}

		void setY(int newY) 
		{
			yPos = newY;
		}

		// Get position
		int getX() 
		{
			return xPos;
		}

		int getY() 
		{
			return yPos;
		}

	private:
		Texture* img;

		int xPos, yPos;
	};

	// Main GUI rendering class
	class GUIRenderer
	{
	public:
		GUIRenderer();

		// Initialises GUI shaders and camera
		void init(int window_width, int window_height);

		// Draw text on GUI
		void drawText(GUIText* guiText) 
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			drawTexture(guiText->getX(), guiText->getY(), guiText->getTexture());
		}

		
		void drawImage(GUIImage* guiImg) 
		{
			drawTexture(guiImg->getX(), guiImg->getY(), guiImg->getTexture()); // Draw image of GUI
		}

		// Shutdown
		void destroy();

	private:
		void drawTexture(int x, int y, Texture* tex);

	private:
		GLuint programId; // stores the program object that contains the shaders

		// This member stores the attribute to select into the pipeline
		// to link the triangle vertices to the pipeline
		GLint vertexLocation;

		
		GLint vertexUVLocation; // where vertex uv is in a model

		// GLSL uniform variables for projection and texture sampler
		GLuint projectionUniformId;
		GLuint samplerId;

		
		glm::mat4 projectionMat; // Project matrix. Calculated in init and reused in draw
	};
}


