#include "CubeWorld.h"
#include <SDL_image.h>
#include "InstancedRenderer.h"
#include "GameEngine.h"

namespace GE
{

    CubeWorld::CubeWorld(GE::GameEngine* engine): engine(engine)
	{
      
	}

	CubeWorld::~CubeWorld()
	{
	}

	Colour32 CubeWorld::GetColourAtPixel(int x, int y, SDL_Surface* LayoutSurface)
	{
		if (x < 0 || x >= LayoutSurface->w || y < 0 || y >= LayoutSurface->h)  // Make sure the coordinates are inside the surface
		{
			return Colour32();
		}

		Uint8 r, g, b, a;

		
		Uint32* row = (Uint32*)((Uint8*)LayoutSurface->pixels + y * LayoutSurface->pitch); // Find the row using the surface pitch
		//SDL_Log("pixel type %d:", LayoutSurface->format->BytesPerPixel);
		
		Uint32 pixel = row[x]; // Get the pixel at X within that row

		// Convert the pixel according to SDL's surface format
		SDL_GetRGBA(pixel, LayoutSurface->format, &r, &g, &b, &a);

		return Colour32(r, g, b, a);
	}

	std::vector<std::string> CubeWorld::ParseLine(const std::string& line) // Function to parse a line from the DEF file into parts, respecting quoted strings
	{
		std::vector<std::string> parts; // Create a vector to hold the parts of the line

		bool inQuotes = false; // Flag to track if we are inside quotes
		std::string current; // String to hold the current part being built

		for (char c : line) // Loop through each character in the line
		{
			if (c == '"') // If the character is a quote, toggle the inQuotes flag and add the quote to the current part
			{
				inQuotes = !inQuotes; // Toggle the inQuotes flag
				current += c; // Add the quote to the current part
			}
			else if (c == ' ' && !inQuotes) // If the character is a space and we are not inside quotes, we have reached the end of a part
			{
				if (!current.empty()) // If the current part is not empty, we add it to the parts vector and clear the current part for the next one
				{
					parts.push_back(current); // Add the current part to the parts vector
					current.clear(); // Clear the current part for the next one
				}
			}
			else // If the character is not a space or we are inside quotes, we add it to the current part
			{
				current += c; // Add the character to the current part
			}
		}

		if (!current.empty()) // If there is a remaining part after the loop, we add it to the parts vector
		{
			parts.push_back(current); // Add the last part to the parts vector
		}

		return parts; // Return the vector of parts
	}

	void CubeWorld::init(std::string filename)
	{
		last_four = filename.substr(filename.length() - 4);

		if (last_four == ".PCX")
		{
			engine->display_info_message("PCX file format is not supported. Please use a .txt or .png file for the layout.");
			engine->shutdown();
		}
		else if (last_four == ".txt") 
		{
			LoadLayoutFromTextFile(filename);
			SDL_Log("Loaded layout from text file: %s", filename.c_str());
		}
		else if (last_four == ".png" || last_four == ".PNG")
		{
			Build_Final_Layout(filename);	
			Build_System_Palette();
			SDL_Log("Loaded layout from image file: %s", filename.c_str());

		}
		else if (last_four == ".def" || last_four == ".DEF")
		{
			LoadDEF(filename);

		}
		else 
		{
			SDL_Log("Unsupported file format: %s", filename.c_str());
			engine->display_info_message("file format is not supported. Please use a .txt or .png file for the layout.");
		}

	}

	void CubeWorld::LoadDEF(std::string filename)
	{
		SDL_Log("");

		SDL_Log("Loading DEF file: %s....", filename.c_str());

		DEFReader.open(filename);

		if (!DEFReader.is_open())
		{
			SDL_Log("Failed to load DEF file: %s", filename.c_str());
		}
		else
		{
			SDL_Log("Successfully loaded DEF file: %s", filename.c_str()); //
		}
		SDL_Log("");

		//std::string LayoutFilename;
		bool end_of_file = false; // Flag to indicate if we've reached the end of the DEF file
		
		while(std::getline(DEFReader, CurLine) && end_of_file == false)// while Loop through each line of the DEF file as long as end_of_file rendermode is false
		{
			if (!CurLine.empty()) // If the current line is not empty, we process it
			{
				std::vector<std::string> parts = ParseLine(CurLine); // Parse the current line into parts using the ParseLine function, which splits the line into words while respecting quoted strings

				if (parts[0] == "NEW_VARIABLE")
				{
					SDL_Log("WARNING: NEW_VARIABLE unimplemented! (%s)", parts[1].c_str());
					SDL_Log("");
				}

				else if (parts[0] == "NEW_PALETTE")
				{
					SDL_Log("WARNING: NEW_PALETTE unimplemented! (%s)", parts[1].c_str());
					SDL_Log("");
				}

				else if (parts[0] == "NEW_CAMERA")
				{
					SDL_Log("WARNING: NEW_CAMERA unimplemented! (%s)", parts[1].c_str());
					SDL_Log("");
				}

				else if (parts[0] == "NEW_TEXTURE")
				{
					std::string textureName = parts[1];  // Extract the texture name from the second part of the line

					
					if (textureName.size() >= 2) // Remove quotes from the cube name if it has at least two characters (to avoid out-of-bounds errors)
					{
						textureName = textureName.substr(1, textureName.size() - 2);
					}

					Process_Texture(textureName); // Process the texture by calling the Process_Texture function and passing the textureName as an argument
				}

				else if (parts[0] == "NEW_QUBIX")
				{
					Process_Qubix(DEFReader, CurLine); // Process the Qubix world properties by calling the Process_Qubix function and passing the DEFReader and CurLine as arguments
				}

				else if (parts[0] == "NEW_ACTOR_TYPE")
				{
					SDL_Log("WARNING: NEW_ACTOR_TYPE unimplemented! (%s)", parts[1].c_str());
					SDL_Log("");
				}

				else if (parts[0] == "NEW_PATH")
				{
					SDL_Log("WARNING: NEW_PATH unimplemented! (%s)", parts[1].c_str());
					SDL_Log("");
				}

				else if (parts[0] == "NEW CUBEDEF")
				{
					SDL_Log("WARNING: NEW CUBEDEF unimplemented! (%s)", parts[1].c_str());
					SDL_Log("");
				}

				else if (parts[0] == "NEW_CUBE")
				{
					std::string cubeName = parts[1]; // Extract the cube name from the second part of the line

					if (cubeName.size() >= 2) // Remove quotes from the cube name if it has at least two characters (to avoid out-of-bounds errors)
					{
						cubeName = cubeName.substr(1, cubeName.size() - 2);
					}

					Process_Cube(cubeName); // Process the cube by calling the Process_Cube function and passing the cubeName as an argument
				}

				else if (parts[0] == "}}}}}}}}}}}}}}}}}}}}}}}}}}}}}")
				{
					SDL_Log("End of file");
					end_of_file = true; // Set the end_of_file rendermode to true to exit the outer while loop and stop processing the DEF file
				}
			}
		}
		Build_System_Palette(); // Generate the system palette based on the original Qubix color palette

		Build_Final_Layout(LayoutFilepath); // Build the final layout of the level using the loaded cubes and textures
	}

	void CubeWorld::Process_Qubix(std::istream &DEFReader, std::string &CurLine)
	{
		std::getline(DEFReader, CurLine); // Read the next line from the DEF file, which should be an opening brace indicating the start of the Qubix world properties

		SDL_Log("Processing Qubix at line:");

		if (CurLine != "{") // If the next line after the Qubix keyword is not an opening brace, log an error and return
		{
			SDL_Log("DEFPARSE: No opening brace after keyword line!"); // Log an error indicating that the expected opening brace is missing
			return;
		}

		bool end_of_properties = false; // Flag to indicate if we've reached the end of the Qubix world properties in the DEF file

		while (!end_of_properties) // Loop through each line of the Qubix world properties until we reach the closing brace
		{
			// Read the next property line
			std::getline(DEFReader, CurLine); // Read the next line from the DEF file, which should contain a property of the Qubix world

			std::vector<std::string> parts = ParseLine(CurLine);

			if (parts.empty())
			{
				continue;
			}

			if (parts[0] == "Value") // If the line starts with "Value", we process the value of a property of the Qubix world
			{
				//SDL_Log("Qubix: %s", parts[0]);

				if (parts.size() < 2)
				{
					SDL_Log("DEFPARSE: Invalid Value property!");
					continue;
				}

				std::string valueParts = parts[1]; // Split the value part into two parts: the property ID and the property value

				size_t commaPos = valueParts.find(','); // Split the property value part to extract the value (removing quotes)

				if (commaPos == std::string::npos)
				{
					SDL_Log("DEFPARSE: Invalid Value format:");
					continue;
				}

				std::string propertyID = valueParts.substr(0, commaPos);

				std::string propertyValue = valueParts.substr(commaPos + 1);


				if (propertyValue.size() >= 2 && propertyValue.front() == '"' && propertyValue.back() == '"')// Remove quotation marks
				{
					propertyValue = propertyValue.substr(1, propertyValue.size() - 2);
				}

				if (propertyID == "001") // If the property ID is "001", we process the layout filename of the Qubix world by extracting it from the value part and assigning it to the LayoutFilename variable
				{
					LayoutFilename = propertyValue;
					LayoutFilepath = ".//Assets/PACKAGEX/PCX/" + LayoutFilename + ".png"; // Assign the extracted layout filename to the LayoutFilename variable for later use
					SDL_Log("Layout filename: %s", LayoutFilepath.c_str());
				}

				if (propertyID == "002") // If the property ID is "001", we process the layout filename of the Qubix world by extracting it from the value part and assigning it to the LayoutFilename variable
				{
					BackdropFilename = propertyValue;
					BackdropFilepath = ".//Assets/PACKAGEX/PCX/" + BackdropFilename + ".png";
					SDL_Log("Backdrop filename: %s", BackdropFilepath.c_str());
				}
			}

			else if (parts[0] == "HexDump")
			{
				SDL_Log("HexDump unimplemented!");
			}

			else if (parts[0] == "}")
			{
				end_of_properties = true;
				SDL_Log("");
			}
		}


	}

	void CubeWorld::Process_Texture(std::string &textureName)
	{
		SDL_Log("NEW_TEXTURE: (%s)", textureName.c_str());

		SX_Texture newTex; // Create a new SX_Texture object to hold the properties of the Texture being processed
		newTex.name = textureName; // Assign the Texture name to the newTex object
		newTex.flags = 0; // Initialize flags to 0
		newTex.rendermode = 0; // Initialize the render mode of the new texture to 0
		// newTex.AITexture = new Texture2D(16, 16); // Initialize the AITexture of the new texture to a new 16x16 Texture object
		newTex.scroll = std::make_pair(0.0f, 0.0f); // Initialize the scroll values of the new texture to a zero vector
		newTex.bitmaps = std::list<SX_Bitmap>();// Initialize the bitmaps list of the new texture to an empty list of SX_Bitmap objects

		SDL_Log("Processing Texture at line: (%s)", CurLine.c_str());

		if (!std::getline(DEFReader, CurLine)) // Read the next line after NEW_CUBE, which should be an opening brace indicating the start of the cube properties
		{
			SDL_Log("DEFPARSE: Unexpected end of file after NEW_CUBE!");
			return;
		}

		if (CurLine != "{") // If the next line after the Qubix keyword is not an opening brace, log an error and return
		{
			SDL_Log("DEFPARSE: No opening brace after keyword line!"); // Log an error indicating that the expected opening brace is missing
			return;
		}

		bool end_of_properties = false; // Flag to indicate if we've reached the end of the Qubix world properties in the DEF file

		while (!end_of_properties) // Loop through each line of the Qubix world properties until we reach the closing brace
		{
			// Read the next property line
			if (!std::getline(DEFReader, CurLine))
			{
				SDL_Log("DEFPARSE: Unexpected end of file while reading cube properties!");
				return;
			}

			std::vector<std::string> parts = ParseLine(CurLine);

			if (parts.empty())
			{
				continue;
			}

			if (parts[0] == "Value") // If the line starts with "Value", we process the value of a property
			{


				if (parts.size() < 2)
				{
					SDL_Log("DEFPARSE: Invalid Value property!");
					continue;
				}

				std::string valueParts = parts[1]; // Split the value part into two parts: the property ID and the property value

				size_t commaPos = valueParts.find(','); // Split the property value part to extract the value (removing quotes)

				if (commaPos == std::string::npos)
				{
					SDL_Log("DEFPARSE: Invalid Value format:");
					continue;
				}

				std::string propertyID = valueParts.substr(0, commaPos);
				std::string propertyValue = valueParts.substr(commaPos + 1);


				if (propertyValue.size() >= 2 && propertyValue.front() == '"' && propertyValue.back() == '"')// Remove quotation marks
				{
					propertyValue = propertyValue.substr(1, propertyValue.size() - 2);
				}

				// -------------------------------------------------// 
				//					   FLAGS						//
				// -------------------------------------------------//

				if (propertyID == "001") // If the property ID is "001", we process the flags of the texture by parsing the value part and setting the flags accordingly
				{
					SDL_Log("%s Flags: %s", textureName.c_str(), propertyValue.c_str());

					try
					{
						newTex.flags =
							static_cast<uint8_t>(
								std::stoi(propertyValue)
								);
					}
					catch (...)
					{
						std::stringstream ss(propertyValue);
						std::string rendermode;

						while (std::getline(ss, rendermode, '+'))
						{
							if (rendermode == "TXTR_Bitmap")
							{
								newTex.flags |= 8;
								SDL_Log("%s Flags = TXTR_Bitmap", textureName.c_str());
							}

							else if (rendermode == "TXTR_FLAT_FILL")
							{
								newTex.flags |= 16;
								SDL_Log("%s Flags = TXTR_FLAT_FILL", textureName.c_str());
							}

							else if (rendermode == "TXTR_AUTO_FILL")
							{
								newTex.flags |= 32;
								SDL_Log("%s Flags = TXTR_AUTO_FILL", textureName.c_str());
							}

							else if (rendermode == "TXTR_FRAME1")
							{
								newTex.flags |= 64;
								SDL_Log("%s Flags = TXTR_FRAME1", textureName.c_str());
							}

							else if (rendermode == "TXTR_FRAME2")
							{
								newTex.flags |= 128;
								SDL_Log("%s Flags = TXTR_FRAME2", textureName.c_str());
							}
						}
					}
				}

				// -------------------------------------------------// 
				//					RENDER MODE						//
				// -------------------------------------------------//

				else if (propertyID == "002") // If the property ID is "002", we process the render mode of the texture by parsing the value part and setting the rendermode accordingly
				{
					SDL_Log("%s Render Mode: %s", textureName.c_str(), propertyValue.c_str());

					try
					{
						newTex.rendermode =
							static_cast<uint8_t>(
								std::stoi(propertyValue)
								);
					}
					catch (...)
					{
						std::stringstream ss(propertyValue);
						std::string rendermode;

						while (std::getline(ss, rendermode, '+'))
						{
							if (rendermode == "Opaque")
							{
								newTex.rendermode |= 1;
								SDL_Log("%s Render Mode = Opaque", textureName.c_str());
							}

							else if (rendermode == "Transparent")
							{
								newTex.rendermode |= 2;
								SDL_Log("%s Render Mode = Transparent", textureName.c_str());
							}

							else if (rendermode == "Blend")
							{
								newTex.rendermode |= 4;
								newTex.rendermode |= 16;
								SDL_Log("%s Render Mode = Blend", textureName.c_str());
							}

							else if (rendermode == "AutoTrans")
							{
								newTex.rendermode |= 2048;
								SDL_Log("%s Render Mode = AutoTrans", textureName.c_str());
							}

							else if (rendermode == "BestQuality")
							{
								newTex.rendermode |= 8192;
								SDL_Log("%s Render Mode = BestQuality", textureName.c_str());
							}
						}
					}
				}

				// -------------------------------------------------// 
				//					 BITMAPS						//
				// -------------------------------------------------//
				else if (propertyID == "008") // If the property ID is "008", we process the bitmap filename of the texture by extracting it from the value part and loading the corresponding bitmap files
				{
					std::string texturePath = propertyValue;

					
					for (char& c : texturePath) // Replace ':', '\' and '\\' to "//"
					{
						if (c == ':' || c == '\\')
						{
							c = '/';
						}
					}

					SDL_Log("%s Bitmap filename: %s", textureName.c_str(), texturePath.c_str());
				}

				// -------------------------------------------------// 
				//					 AI TEXTURE						//
				// -------------------------------------------------//
				else if (propertyID == "017") // If the property ID is "017", we process the fill color of the texture by parsing the value part and setting the fill color keys accordingly
				{

				}

				else if (propertyID == "018") // If the property ID is "018", we process the frame color of the texture by parsing the value part and setting the frame color keys accordingly
				{

				}

				else if (propertyID == "019") // If the property ID is "019", we process the second frame color of the texture by parsing the value part and setting the second frame color keys accordingly
				{

				}

				else if (propertyID == "020") // If the property ID is "020", we process the second fill color of the texture by parsing the value part and setting the second fill color keys accordingly
				{

				}

				else if (propertyID == "021") // If the property ID is "021", we process the auto-fill scaling of the texture by parsing the value part and setting the AIScaleX and AIScaleY values accordingly
				{

				}

				// -------------------------------------------------// 
				//					  SCROLL 						//
				// -------------------------------------------------//
				else if (propertyID == "022") // If the property ID is "022", we process the scroll values of the texture by parsing the value part and setting the scroll.x and scroll.y values accordingly
				{
					SDL_Log("%s Scroll: %s", textureName.c_str(), propertyValue.c_str());
					
					//method to seperate line "Color(000,048,192)" into seperate x, y
					// EXAMPLE: "Color(000,048,192)" = X value = 48, Y value = 192
				}
			}

			else if (parts[0] == "}")
			{
				end_of_properties = true;
				SDL_Log("");
			}
		}
	}

	void CubeWorld::Process_Material() // Processes a material for a cube based on the provided texture name and type, adjusting properties such as color, transparency, and texture scaling accordingly
	{

	}

	void CubeWorld::Process_Shading_And_Renderflags()  // Processes shading and render flags for a material based on the provided value part, adjusting color, transparency, texture scaling, and rotation accordingly
	{
	} 

	void CubeWorld::Process_Cube(std::string& cubeName)
	{
		SDL_Log("NEW_CUBE: (%s)", cubeName.c_str());

		SX_Cube newCube; // Create a new SX_Cube object to hold the properties of the cube being processed
		newCube.name = cubeName; // Assign the cube name to the newCube object
		newCube.flags = 0; // Initialize flags to 0
		newCube.type = 0; // Initialize type to 0
		newCube.slope_dir = 0; // Initialize slope_dir to 0
		newCube.double_sided_flags = 0; // Initialize double_sided_flags to 0

		newCube.scale = Vector3(1.0f, 1.0f, 1.0f); // Initialize scale to (1, 1, 1)

		newCube.offset = Vector3(0.0f, 0.0f, 0.0f); // Initialize offset to (0, 0, 0)


		SDL_Log("Processing Cube at line: (%s)", CurLine.c_str());


		if (!std::getline(DEFReader, CurLine)) // Read the next line after NEW_CUBE, which should be an opening brace indicating the start of the cube properties
		{ 
			SDL_Log("DEFPARSE: Unexpected end of file after NEW_CUBE!");
			return; 
		}

		if (CurLine != "{") // If the next line after the Qubix keyword is not an opening brace, log an error and return
		{
			SDL_Log("DEFPARSE: No opening brace after keyword line!"); // Log an error indicating that the expected opening brace is missing
			return;
		}

		bool end_of_properties = false; // Flag to indicate if we've reached the end of the Qubix world properties in the DEF file

		while (!end_of_properties) // Loop through each line of the Qubix world properties until we reach the closing brace
		{
			// Read the next property line
			if (!std::getline(DEFReader, CurLine)) 
			{ 
				SDL_Log("DEFPARSE: Unexpected end of file while reading cube properties!");
				return; 
			}

			std::vector<std::string> parts = ParseLine(CurLine);

			if (parts.empty())
			{
				continue;
			}

			if (parts[0] == "Value") // If the line starts with "Value", we process the value of a property
			{
				

				if (parts.size() < 2)
				{
					SDL_Log("DEFPARSE: Invalid Value property!");
					continue;
				}

				std::string valueParts = parts[1]; // Split the value part into two parts: the property ID and the property value

				size_t commaPos = valueParts.find(','); // Split the property value part to extract the value (removing quotes)

				if (commaPos == std::string::npos)
				{
					SDL_Log("DEFPARSE: Invalid Value format:");
					continue;
				}

				std::string propertyID = valueParts.substr(0, commaPos);
				std::string propertyValue = valueParts.substr(commaPos + 1);


				if (propertyValue.size() >= 2 && propertyValue.front() == '"' && propertyValue.back() == '"')// Remove quotation marks
				{
					propertyValue = propertyValue.substr(1, propertyValue.size() - 2);
				}

				// -------------------------------------------------// 
				//					   FLAGS						//
				// -------------------------------------------------//

				if (propertyID == "001") // If the property ID is "001", we process the flags of the cube
				{

					//newCube.flags = static_cast<uint8_t>(std::stoi(propertyValue)); // Convert the property value to an integer and assign it to the flags variable of the newCube object
					////SDL_Log("%s Cube Flags: %s", cubeName.c_str(), propertyValue.c_str());

					//if (newCube.flags & 1) // If the first bit of the flags is set, we log that the cube is active
					//{
					//	newCube.flags |= 1;
					//	SDL_Log("%s: Active", cubeName.c_str());
					//}
					//else if (newCube.flags & 8) // If the fourth bit of the flags is set, we log that the cube is a checkpoint
					//{
					//	newCube.flags |= 8;
					//	SDL_Log("%s: Checkpoint", cubeName.c_str());
					//}

					try 
					{
						newCube.flags = static_cast<uint8_t>(std::stoi(propertyValue)); // Convert the property value to an integer and assign it to the flags variable of the newCube object

						SDL_Log("%s Flags: %s", cubeName.c_str(), propertyValue.c_str());
					}
					catch (...)
					{ 
						// Otherwise the value is a list such as: 
						// CDF_Active+CDF_Chkrs 

						std::stringstream ss(propertyValue); // Create a stringstream to parse the property value into individual flags
						std::string rendermode; // Variable to hold each individual rendermode as we parse them from the property value

						while (std::getline(ss, rendermode, '+')) 
						{ 
							if (rendermode == "CDF_Active") // If the rendermode name is "CDF_Active", we set the first bit of the cube's flags to indicate that the cube is active
							{ 
								newCube.flags |= 1; 

								SDL_Log("%s: Active", cubeName.c_str());
							} 
							else if (rendermode == "CDF_Chkrs") // If the rendermode name is "CDF_Chkrs", we set the second bit of the cube's flags to indicate that the cube is a checkpoint
							{ 
								newCube.flags |= 8; 
								SDL_Log("%s: Checkpoint", cubeName.c_str());

							} 

							
						} 
					}

				}

				// -------------------------------------------------// 
				//					  CUBE TYPE					    //
				// -------------------------------------------------//

				else if (propertyID == "002") // If the property ID is "002", we process the type of the cube by trying to parse the property value as a byte and assigning it to the cube's type
				{
					newCube.type = static_cast<uint8_t>(std::stoi(propertyValue));

					SDL_Log("%s Cube Type: %s", cubeName.c_str(), propertyValue.c_str());
				}

				// ---------------------------------------------------------// 
				//					   FACE MATERIALS	 				    //
				// ---------------------------------------------------------//

				else if (propertyID == "003") // If the property ID is "003", we process the material for the first face of the cube by calling Process_Material() with the appropriate parameters
				{
					// Call Process_Material() to process the material for the first face of the cube, passing in a reference to the first material in the cube's materials array, the texture name, and the cube's type
				}

				else if (propertyID == "004") // If the property ID is "004", we process the material for the second face of the cube by calling Process_Material() with the appropriate parameters
				{
					// Call Process_Material() to process the material for the second face of the cube, passing in a reference to the second material in the cube's materials array, the texture name, and the cube's type
					// Call Process_Material() again to process the material for the second face of the cube, passing in a reference to the second material in the cube's materials array, the texture name, and the cube's type

				}

				else if (propertyID == "005") // If the property ID is "005", we process the material for the third face of the cube by calling Process_Material() with the appropriate parameters
				{
					//  Process_Material(ref newCube.materials[2], TexNameSplit[1], newCube.type);
					// Add some default shading to certain faces
					//newCube.materials[2].color = new Color( // Set the color of the third material to be half as bright as its original color, effectively darkening it for shading purposes
					//	newCube.materials[2].color.r / 2, // Divide the red component of the color by 2 to darken it
					//	newCube.materials[2].color.g / 2, // Divide the green component of the color by 2 to darken it
					//	newCube.materials[2].color.b / 2, // Divide the blue component of the color by 2 to darken it
					//	newCube.materials[2].color.a);
					//break;
				}

				else if (propertyID == "006") // If the property ID is "006", we process the material for the fourth face of the cube by calling Process_Material() with the appropriate parameters
				{
					//Process_Material(ref newCube.materials[3], TexNameSplit[1], newCube.type); // Call Process_Material() to process the material for the fourth face of the cube, passing in a reference to the fourth material in the cube's materials array, the texture name, and the cube's type
					//newCube.materials[3].color = new Color( // Set the color of the fourth material to be half as bright as its original color, effectively darkening it for shading purposes
					//	newCube.materials[3].color.r / 2,
					//	newCube.materials[3].color.g / 2,
					//	newCube.materials[3].color.b / 2,
					//	newCube.materials[3].color.a);
				}

				else if (propertyID == "007") // If the property ID is "007", we process the material for the fifth face of the cube by calling Process_Material() with the appropriate parameters
				{

				}

				else if (propertyID == "008") // If the property ID is "008", we process the material for the sixth face of the cube by calling Process_Material() with the appropriate parameters
				{
						
				}

				// ---------------------------------------------------------// 
				//					   SLOPE DIRECTION	 				    //
				// ---------------------------------------------------------//

				else if (propertyID == "015") // If the property ID is "015", we process the slope direction of the cube by trying to parse the property value as a byte and assigning it to the cube's slope_dir
				{
					newCube.slope_dir = static_cast<uint8_t>(std::stoi(propertyValue));

					SDL_Log("%s Slope Direction: %s", cubeName.c_str(), propertyValue.c_str());
				}

				// ---------------------------------------------------------// 
				//					   SCALE / OFFSET	 				    //
				// ---------------------------------------------------------//

				else if (propertyID == "016") // If the property ID is "016", we process the scale and offset for the X axis of the cube by parsing the property value as an integer and calculating the scale and offset based on bit manipulation
				{
					int rawValX = std::stoi(propertyValue); 
					newCube.scale.x = static_cast<int16_t>(rawValX) / 128.0f; 
					newCube.offset.x = static_cast<int16_t>(rawValX >> 16) / 256.0f;

					SDL_Log("%s X Size/Offset: %s", cubeName.c_str(), propertyValue.c_str());
					//SDL_Log("%s X Size: %f, X Offset: %f", cubeName.c_str(), newCube.scale.x, newCube.offset.x);
				}

				else if (propertyID == "017") // If the property ID is "017", we process the scale and offset for the Y axis of the cube by parsing the property value as an integer and calculating the scale and offset based on bit manipulation
				{
					int rawValY = std::stoi(propertyValue);
					newCube.scale.y = static_cast<int16_t>(rawValY) / 128.0f;
					newCube.offset.y = static_cast<int16_t>(rawValY >> 16) / 256.0f;

					SDL_Log("%s Y Size/Offset: %s", cubeName.c_str(), propertyValue.c_str());
					//SDL_Log("%s Y Size: %f, Y Offset: %f", cubeName.c_str(), newCube.scale.y, newCube.offset.y);
				}

				else if (propertyID == "018") // If the property ID is "018", we process the scale and offset for the Z axis of the cube by parsing the property value as an integer and calculating the scale and offset based on bit manipulation
				{
					int rawValZ = std::stoi(propertyValue);
					newCube.scale.z = static_cast<int16_t>(rawValZ) / 128.0f;
					newCube.offset.z = static_cast<int16_t>(rawValZ >> 16) / 256.0f;

					SDL_Log("%s Z Size/Offset: %s", cubeName.c_str(), propertyValue.c_str());
					//SDL_Log("%s Z Size: %f, Z Offset: %f", cubeName.c_str(), newCube.scale.z, newCube.offset.z);
				}

				// ---------------------------------------------------------// 
				//					     DOUBLESIDED	 				    //
				// ---------------------------------------------------------//

				else if (propertyID == "020") // If the property ID is "020", we process the double-sided flags of the cube by trying to parse the property value as a byte and assigning it to the cube's double_sided_flags. We also apply a double-sided shader to all faces of the cube if the shader is not "Custom/Blank"
				{
					// HACK: Ignore flags and just apply double sided shader to all faces
					//newCube.double_sided_flags = static_cast<uint8_t>(std::stoi(propertyValue));
					if (std::all_of(propertyValue.begin(), propertyValue.end(), ::isdigit)) {
						newCube.double_sided_flags = static_cast<uint8_t>(std::stoi(propertyValue));
						SDL_Log("%s: Double Sided", cubeName.c_str());
					}
					else {
						SDL_Log("Invalid property value: %s", propertyValue.c_str());
					}

					
				}

				// ---------------------------------------------------------// 
				//					   SHADING & RENDERING	 				//
				// ---------------------------------------------------------//

				else if (propertyID == "029") // If the property ID is "029", we process the shading and render flags for the first face of the cube by calling Process_Shading_And_Renderflags() with the appropriate parameters
				{

				}

				else if (propertyID == "030")  // If the property ID is "030", we process the shading and render flags for the second face of the cube by calling Process_Shading_And_Renderflags() with the appropriate parameters
				{

				}

				else if (propertyID == "031") // If the property ID is "031", we process the shading and render flags for the third face of the cube by calling Process_Shading_And_Renderflags() with the appropriate parameters
				{

				}

				else if (propertyID == "032") //	If the property ID is "032", we process the shading and render flags for the fourth face of the cube by calling Process_Shading_And_Renderflags() with the appropriate parameters
				{

				}

				else if (propertyID == "033") // If the property ID is "033", we process the shading and render flags for the fifth face of the cube by calling Process_Shading_And_Renderflags() with the appropriate parameters
				{

				}

				else if (propertyID == "034") // If the property ID is "034", we process the shading and render flags for the sixth face of the cube by calling Process_Shading_And_Renderflags() with the appropriate parameters
				{

				}
			}

			else if (parts[0] == "HexDump")
			{
				SDL_Log("HexDump unimplemented!");
			}

			else if (parts[0] == "}")
			{
				end_of_properties = true;
				SDL_Log("");
			}
		}

		Vector3 newCubeVerts[8] = 
		{ 
			{ -0.5f,  0.5f, -0.5f }, 
			{  0.5f,  0.5f, -0.5f }, 
			{  0.5f, -0.5f, -0.5f }, 
			{ -0.5f, -0.5f, -0.5f }, 
			{  0.5f,  0.5f,  0.5f }, 
			{ -0.5f,  0.5f,  0.5f }, 
			{ -0.5f, -0.5f,  0.5f }, 
			{  0.5f, -0.5f,  0.5f } 
		};


		// Store the cube 
		SX_Cubes.push_back(newCube);
	}

	void CubeWorld::Generate_Cube(SX_Cube cube, Vector3 pos) 
	{
		SDL_Log("Generated Cube: %s at Position: (%f, %f, %f)", cube.name.c_str(), pos.x, pos.y, pos.z);
		engine->PushMeshInstance(pos.x, pos.y, pos.z, 0, 0, 0); // Add a cube instance to the scene at the specified position (X, Y, Z)

		
	}

	void CubeWorld::Build_System_Palette()
	{
		SDL_Log("Building System Palette...");

		for (int pal_index = 0; pal_index < 256;) // Iterate through the palette indices
		{
			for (float r = 31; r >= -1; r -= 6.2f)  // Iterate through the red color values from 31 to 0 in steps of 6.2
			{
				for (float g = 31; g >= -1; g -= 6.2f) // Iterate through the green color values from 31 to 0 in steps of 6.2
				{
					for (float b = 31; b >= -1; b -= 6.2f) // Iterate through the blue color values from 31 to 0 in steps of 6.2
					{
						if (pal_index >= 256) // If the palette index exceeds 255, exit the function
							return;

						//SysPalette[pal_index] = new Colour32( // Create a new Colour32 object for the current palette index
						//	(uint8_t)(roundf(r) * 8), // Convert the red color value to a byte and multiply by 8 to scale it to the 0-255 range
						//	(uint8_t)(roundf(g) * 8), // Convert the green color value to a byte and multiply by 8 to scale it to the 0-255 range
						//	(uint8_t)(roundf(b) * 8), 255 // Set the alpha value to 255 (fully opaque)
						//);

						uint8_t red =
							static_cast<uint8_t>(std::round(r) * 8.0f);

						uint8_t green =
							static_cast<uint8_t>(std::round(g) * 8.0f);

						uint8_t blue =
							static_cast<uint8_t>(std::round(b) * 8.0f);

						SysPalette[pal_index] =
							Colour32(red, green, blue, 255);


						if (pal_index < 256) // If the palette index is less than 256, log the current palette color and index to the console for debugging purposes
						{
							//SDL_Log("Palette: " + SysPalette[pal_index] + " index: " + pal_index); // Log the current palette color and index to the console for debugging purposes
							SDL_Log("System Palette: (%d, %d, %d, %d) index: %d", SysPalette[pal_index].r, SysPalette[pal_index].g, SysPalette[pal_index].b, SysPalette[pal_index].a, pal_index);
						}

						pal_index++; // Increment the palette index for the next iteration
					}
				}
			}
		}
	}

	void CubeWorld::LoadLayoutFromTextFile(std::string filename)
	{

		std::ifstream layout_stream; // load level layout
		layout_stream.open(filename);

		if (!layout_stream.is_open()) 
		{
			SDL_Log("Failed to open Text: %s", filename.c_str());
		}
		else 
		{
			SDL_Log("Successfully opened Text: %s", filename.c_str());
		}

		int mapWidth = 110 / 2;
		int mapHeight = 10;
		int Y = 0;

		for (int rowIdx = 0; rowIdx < mapHeight; ++rowIdx) // Loop through each row of the tilemap
		{
			for (int colIdx = 0; colIdx < mapWidth; ++colIdx) // Loop through each column of the tilemap
			{
				std::string Value; // Read the layout value from the file

				layout_stream >> Value; // Read the layout value from the file

				if (layout_stream.fail())  // If reading the layout value failed
				{
					//SDL_Log("Failed to read layout value at row %d, col %d", rowIdx, colIdx); // Log the error to the console
					break; // Break out of the inner loop to stop reading the tilemap
				}

				else 
				{

					//SDL_Log("layout value at row %d, col %d: %s", rowIdx, colIdx, Value.c_str()); // Log the layout value to the console

					switch(Value[0]) // Switch statement to handle different layout values
					{
						case '1':
							engine->PushMeshInstance(colIdx, Y, rowIdx, 0, 0, 0); // Add a cube instance to the scene at the specified position
							break;

						case '#':
							Y++; // Increment the Z value to move to the next layer of the tilemap
							rowIdx = 0;
							colIdx = -1;
							break;

						default:
							// Handle other layout values if needed
							break;
					}
				}
			}
		}
		
		layout_stream.close();
	}

	void CubeWorld::Build_Final_Layout(std::string filename)
	{
		SDL_Surface* LayoutSurface = IMG_Load(filename.c_str());

		if (LayoutSurface == nullptr) {
			SDL_Log("Failed to load layout image: %s", SDL_GetError());
			return;
		}
		else
		{
			SDL_Log("Successfully loaded layout image: %s", filename.c_str());
		}

		SDL_Log("Layout image width: %s", std::to_string(LayoutSurface->w).c_str());
		SDL_Log("Layout image height: %s", std::to_string(LayoutSurface->h).c_str());
		SDL_Log("");

		// get first colour of first top left pixel
		Colour32 EmptyCol = GetColourAtPixel(0, 0, LayoutSurface);
		
		// get second colour of second top left pixel 
		Colour32 BorderCol = GetColourAtPixel(0, 1, LayoutSurface);

		int layerHeight = 1;
		int layerWidth = 1;

		// Calculate width of a layer in layout image
		for (int i = 1; i < LayoutSurface->w; i++) // Loop through the width of the layout texture starting from the second pixel (index 1)
		{
			layerWidth++;

			Colour32 CurrentColour = GetColourAtPixel(i, 0, LayoutSurface);

			if (CurrentColour.r == EmptyCol.r && CurrentColour.g == EmptyCol.g && CurrentColour.b == EmptyCol.b)
			{
				// If the current pixel is different from the border colour and empty colour, break the loop
				break;
			}
		}

		// Calculate height of a layer in PCX layout image
		for (int i = 1; i < LayoutSurface->h; i++) // Loop through the width of the layout texture starting from the second pixel (index 1)
		{
			layerHeight++;

			Colour32 CurrentColour = GetColourAtPixel(0, i, LayoutSurface);

			if (CurrentColour.r == EmptyCol.r && CurrentColour.g == EmptyCol.g && CurrentColour.b == EmptyCol.b)
			{
				// If the current pixel is different from the border colour and empty colour, break the loop
				break;
			}
		}

		int Z = 0;
		// Loop through each pixel in the PNG
		//for (int layerY = LayoutSurface->h - layerHeight; layerY >= 0; layerY -= layerHeight) // Loop through the layout texture from the top layer to the bottom layer, decrementing by the height of a layer each time
		for (int layerY = 0; layerY < LayoutSurface->h; layerY += layerHeight)
		{
			for (int layerX = 0; layerX < LayoutSurface->w; layerX += layerWidth) // Loop through the layout texture from the top layer to the bottom layer, decrementing by the height of a layer each time
			{
			  //for (int y = layerHeight - 1; y >= 0; y--) // Loop through the layout texture from the top layer to the bottom layer, decrementing by the height of a layer each time
				for (int y = 0; y < layerHeight; y++)
				{
					for (int x = 0; x < layerWidth; x++) // Loop through each horizontal row of the tilemap
					{
						Colour32 CurrentColour = GetColourAtPixel(layerX + x, layerY + y, LayoutSurface);


						if ((CurrentColour.r == EmptyCol.r && CurrentColour.g == EmptyCol.g && CurrentColour.b == EmptyCol.b) || (CurrentColour.r == BorderCol.r && CurrentColour.g == BorderCol.g && CurrentColour.b == BorderCol.b))
						{
							//engine->PushMeshInstance(x, 0, y); // Add a cube instance to the scene at the specified position (X, Y, Z)
						}
						else
						{
							SDL_Log("Pixel CurrentColour at Layer %d row %d, col %d: R=%d G=%d B=%d A=%d", Z, x, y, CurrentColour.r, CurrentColour.g, CurrentColour.b, CurrentColour.a); // Log the pixel CurrentColour to the console
							
							if (last_four == ".png" || last_four == ".PNG")
							{
								engine->PushMeshInstance(x, Z, y, 0, 0, 0); // Add a cube instance to the scene at the specified position (X, Y, Z)
							}
							else
							{
								for (int i = 0; i < 256; i++) // Cycle through system palette for corresponding cube
								{
									if (CurrentColour.r == SysPalette[i].r && CurrentColour.g == SysPalette[i].g && CurrentColour.b == SysPalette[i].b) // If the current pixel color matches a color in the system palette, it indicates that a cube should be generated using the corresponding cube definition
									{
										SDL_Log("Pixel CurrentColour matches System Palette index %d: R=%d G=%d B=%d A=%d", i, SysPalette[i].r, SysPalette[i].g, SysPalette[i].b, SysPalette[i].a); // Log the pixel CurrentColour to the console
										SDL_Log("");
										
										if (i > 0 && static_cast<size_t>(i - 1) < SX_Cubes.size())
										{
											auto it = SX_Cubes.begin();
											std::advance(it, i - 1); // Move the iterator to the (i-1)th position
											Generate_Cube(*it, Vector3(x, Z, y)); // Generate a cube using the corresponding cube definition
										}
										else
										{
											SDL_Log("Invalid cube index: %d (SX_Cubes.size() = %zu)", i - 1, SX_Cubes.size());
										}

										break; // Break out of the loop once a matching color is found in the system palette, as we only need to generate one cube for each pixel color
									}
								}
							}

						}
					}
				}
				Z++;
				
			}
		}

		SDL_Log("");

		SDL_Log("Horizontal Border Colour Count: %d", layerWidth); // Log the border colour count to the console
		//SDL_Log("Horizontal Layer Count: %d", layerWidth / 2); // Log the border colour count to the console

		SDL_Log("Vertical Border Colour Count: %d", layerHeight); // Log the border colour count to the console
		//SDL_Log("Vertical Layer Count: %d", (layerHeight / 2)); // Log the border colour count to the console

		//SDL_Log("Total Layers: %d", (layerHeight / 2) * (layerWidth / 2)); // Log the border colour count to the console
	}

	//=======================================================================================
	//                           H E L P E R    F U N C T I O N S                            
	//=======================================================================================
	int Get_Texture_Index(std::string TextureName) // Returns the index of a texture in the SX_Textures list based on its name. If the texture is not found, it logs a warning and returns index 0.
	{
		//for (int i = 0; i < SX_Textures.Count; i++) // Loop through the SX_Textures list to find the texture with the specified name
		//{
		//	if (SX_Textures[i].name == TextureName) // If the name of the current texture matches the specified TextureName, return its index
		//	{
		//		return i;
		//  }
		//}

		SDL_Log("DEFPARSER: Failed to find texture %s", TextureName.c_str(), " - Falling back to index 0!"); // Log a warning if the texture is not found in the SX_Textures list, indicating that the code will fall back to using index 0 as a default texture
		return 0; // Return index 0 as a fallback if the specified texture is not found in the SX_Textures list
	}
}