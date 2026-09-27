#pragma once
#include <string>
#include "Texture.h"

namespace GE {

	// Class to represent text rendering.  However, would only
	// create one of these in a program as renderText can render
	// any font and size so should this be a singleton?
	class FontRenderer
	{
	public:
		// Constructor
		FontRenderer() { }

		// Destructor
		~FontRenderer() {

		}

		// Initialises TTF font
		void init();

		// Creates a texture object for the text based on font and size
		Texture *renderText(std::string text, std::string fontname, int size);

		// Shuts down TTF font
		void destroy();
	};

}

