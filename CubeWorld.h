#pragma once

#include <fstream>
#include <SDL.h>
#include <iostream>
#include <vector>
#include "InstancedRenderer.h"
#include <string>
#include <sstream>
#include <regex>
#include "Texture.h"
#include <list>
#include "Model.h"
#include <cmath>


namespace GE 
{
	struct Vector3
	{
		float x, y, z;

		Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

		Vector3() : x(0), y(0), z(0) {}
	};

	struct Colour32
	{
		uint8_t r;
		uint8_t g;
		uint8_t b;
		uint8_t a;

		Colour32(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha) // Constructor to initialize the color values
		{
			r = red;
			g = green;
			b = blue;
			a = alpha;
		}

		Colour32() // Default constructor to initialize the color values to 0
		{
			r = 0;
			g = 0;
			b = 0;
			a = 0;
		}
	};

	//=======================================================================================
	//                                 SX structs and enums                                  
	//=======================================================================================

	struct SX_Bitmap // Bitmap struct for storing bitmap data
	{
		std::string name; // Name of the bitmap
		Texture* texture; // Texture object for the bitmap
		Colour32* auto_trans_colour; // Color of the top-left pixel, used for auto transparency
		
		SX_Bitmap() = default;

		SX_Bitmap(const std::string& Name, Texture* TexturePtr, Colour32* AutoTransColour)
			: name(Name), texture(TexturePtr), auto_trans_colour(AutoTransColour)
		{
		}

	};

    struct SX_Texture // Texture struct for storing texture data
    {
       std::string name; // Name of the texture

       uint8_t flags; // Flags for the texture (e.g. TXTR_Bitmap, TXTR_FLAT_FILL, etc.)
       int rendermode; // Render mode for the texture (e.g. Opaque, Transparent, Blend, etc.)

       Texture AITexture; // Texture2D object for the auto-generated texture
       std::pair<float, float> scroll; // Scroll values for the texture (x and y)

       std::list<SX_Bitmap> bitmaps; // List of bitmaps associated with the texture

       // Add a default constructor to resolve the issue
       SX_Texture() : name(""), flags(0), rendermode(0), AITexture(""), scroll({0.0f, 0.0f}) {}
    };

	struct SX_Material
	{
		std::string name; // Name of the material
		uint8_t flags; // Flags for the material
		uint8_t type; // Type of the material
		Colour32 colour; // Colour of the material
		Texture* mainTexture; // Pointer to the texture associated with the material
		SX_Material() : name(""), flags(0), type(0), colour(0, 0, 0, 255), mainTexture(nullptr) {}

	};

	struct SX_Cube // Cube struct for storing cube data
	{
		std::string name; // Name of the cube

		uint8_t  flags; // Flags for the cube (e.g. CDF_Active, CDF_Chkrs, etc.)
		uint8_t  type; // Type of the cube (e.g. 0 = normal, 3 = X-shaped, 4 = diamond-shaped, etc.)
		uint8_t  slope_dir; // Slope direction for the cube (0 = none, 1-12 = various slope directions)
		uint8_t  double_sided_flags; // Flags for double-sided rendering of the cube (e.g. 0 = none, 1 = all faces, etc.)

		Vector3 scale = {1.0f, 1.0f, 1.0f};  // Scale values for the cube (x, y, z)
		Vector3 offset = { 0.0f, 0.0f, 0.0f }; // Offset values for the cube (x, y, z)

		Model mesh; // Mesh object for the cube (used for rendering the cube in the game engine)
		std::list<SX_Material> materials; // Array of materials for the cube (one for each face)
	};


	class GameEngine; // Forward declaration

	class CubeWorld
	{
		public:
			CubeWorld(GE::GameEngine* engine);

			~CubeWorld();

			void init(std::string filename);

			//=======================================================================================
			//                                    Public vars                                       
			//=======================================================================================

			std::string DEFFile; // Name of the DEF file to load
			Texture* BackdropImg; // Texture for the backdrop of the level


		private:
			void LoadLayoutFromTextFile(std::string filename); // Load the layout of the level from a text file

			void Build_Final_Layout(std::string filename); // Reads the layout file and generates the final level layout based on the cube definitions and system palette

			void LoadDEF(std::string filename); // Load and parse the DEF file, creating textures and cubes as specified in the file

			void Process_Qubix(std::istream& DEFReader, std::string& CurLine); // Process a Qubix world definition from the DEF file, extracting layout and backdrop information

			void Process_Texture(std::string& textureName); // Process a texture definition from the DEF file, creating a new SX_Texture object and populating its properties based on the DEF file contents

			void Process_Cube(std::string& cubeName); // Process the cube by loading it and adding it to the system palette

			void Generate_Cube(SX_Cube cube, Vector3 pos); // Generates a cube GameObject in the Unity scene based on the provided SX_Cube definition and position. This function creates a new GameObject, sets its position and scale, assigns the cube's mesh and materials, and handles special cases such as trigger colliders and material swapping for checkerboard patterns.
			
			void Build_System_Palette(); // Generates a 256-color palette based on the original Qubix color palette

			void Process_Material(); //(ref Material cubeMat, string texture_name, int type) // Processes a material for a cube based on the provided texture name and type, adjusting properties such as color, transparency, and texture scaling accordingly

			void Process_Shading_And_Renderflags(); //(ref Material cubeMat, string ValuePart)  // Processes shading and render flags for a material based on the provided value part, adjusting color, transparency, texture scaling, and rotation accordingly

			int Get_Texture_Index(std::string TextureName); // Returns the index of a texture in the SX_Textures list based on its name. If the texture is not found, it logs a warning and returns index 0.

			Colour32 GetColourAtPixel(int x, int y, SDL_Surface* LayoutSurface);
			
			std::vector<std::string> ParseLine(const std::string& line);

			//=======================================================================================
			//                                    Private vars                                       
			//=======================================================================================

			GE::GameEngine* engine; // GameEngine Pointer 

			std::string last_four;

			std::string CurLine; // Current line being read from the DEF file
			std::ifstream DEFReader; // StreamReader for reading the DEF file

			std::string LayoutFilename; // Filename of the layout file for the level, used to build the level geometry
			std::string LayoutFilepath; // Full filepath of the layout file for the level

			std::string BackdropFilename; // Filename of the Backdrop file for the level, used for the background
			std::string BackdropFilepath; // Full filepath of the Backdrop file for the level

			Colour32 SysPalette[256]; // System palette for the level, used for shading and color mapping

			std::list<SX_Texture> SX_Textures; // List of textures loaded from the DEF file, used for creating materials and applying textures to cubes
			std::list<SX_Cube> SX_Cubes; // List of cubes loaded from the DEF file, used for building the level geometry

		};
}


